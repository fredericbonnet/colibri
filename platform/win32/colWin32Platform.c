#include "../../colibri.h"
#include "../../colInternal.h"
#include "../../colPlatform.h"

#include <windows.h>
#include <sys/types.h>

/*
 * Thread-local data.
 */

static DWORD tlsToken;
typedef struct {
    size_t nestCount;
    GcMemInfo gcMemInfo; 
    Col_ErrorProc *errorProc;
} ThreadData;

/*
 * Bit twiddling hack for computing the log2 of a power of 2.
 * See: http://www-graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
 */

static const int MultiplyDeBruijnBitPosition2[32] = 
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};
#define LOG2(v) MultiplyDeBruijnBitPosition2[(uint32_t)(v * 0x077CB531U) >> 27]

/*
 * Prototypes for functions used only in this file.
 */

static BOOL Init(void);


/*
 *----------------------------------------------------------------
 * System page allocator.
 *----------------------------------------------------------------
 */

/*
 * Contrary to most other platforms where individual pages can be allocated and
 * freed (for example using mmap/munmap), the Windows VirtualAlloc API restricts 
 * address reservation to a granularity of SYSTEM_INFO.dwAllocationGranularity 
 * (65536 on IA32), although it can commit and decommit individual pages with a 
 * size of SYSTEM_INFO.dwPageSize (4096 on IA32). Besides, behaving as if 
 * system pages had a size of SYSTEM_INFO.dwAllocationGranularity proved to be
 * too CPU and memory intensive. So if we want to keep the system page small, 
 * this implies that each page is itself part of a larger address range.
 *
 * Achieving single page granularity involves a lot of bookkeeping, as Windows 
 * doesn't provide an efficient query API (VirtualQuery is very CPU intensive 
 * and is known to be even slower on Win64). So large address ranges are 
 * reserved, and individual pages are committed within these ranges.
 *
 * When the number of pages to allocate exceeds a certain size (defined as 
 * LARGE_PAGE_SIZE), a dedicated address range is reserved and committed, which 
 * must be freed all at once. Else,pages are allocated within larger address 
 * ranges using the following scheme:
 *
 * Address ranges are reserved in geometrically increasing sizes up to a 
 * maximum size (the first being a multiple of the allocation granularity). 
 * Ranges form a singly-linked list in allocation order (so that search times 
 * are geometrically increasing too). A descriptor structure is malloc'd along 
 * with the range and consists of a pointer to the next descrptor, the base 
 * address of the range, the total and free numbers of pages and the index of 
 * the first free page in range, and a bitmask table for allocated pages (much 
 * alike regular Colibri pages).
 *
 * To allocate a group of pages in an address range, the bitmask is scanned 
 * until a large enough group of free pages is found. To free a group of
 * pages, the containing address range is found by scanning all ranges (even
 * VirtualQuery is too slow for this simple purpose!). In either case the 
 * descriptor is updated accordingly. If a containing range is not found we 
 * assume that it was a dedicated range and we attempt to release it at once.
 *
 * The allocation scheme may not look optimal at first sight (especially the
 * bitmask scanning step), but keep in mind that the typical use case only 
 * involves single page allocations. Multiple page allocations only occur when 
 * allocating large, multiple cell-based objects, and most objects are single 
 * cell sized. And very large pages will end up in their own dedicated range
 * with no page management. Moreover stress tests have shown that this scheme 
 * yielded similar or better performances than the previous scheme that was 
 * limited to granularity-sized address ranges.
 */

static SYSTEM_INFO systemInfo;
static size_t shiftPage;

typedef struct AddressRange {
    struct AddressRange *next;
    void *base;
    size_t size;
    size_t free;
    size_t first;
    unsigned char bitmask[0];
} AddressRange;

static AddressRange *ranges;
static AddressRange *dedicatedRanges;
static CRITICAL_SECTION protect;

#define FIRST_RANGE_SIZE	256 /* 1 MB */
#define MAX_RANGE_SIZE		32768 /* 128 MB */

static size_t 
FindClearedBits(
    unsigned char *mask,	/* Bitmask table. */
    size_t size,		/* Size of bitmask in bits. */
    size_t number,		/* Number of consecutive bits to find. */
    size_t index)		/* First bit to consider. */
{
    size_t i, first = (size_t)-1, remaining;
    char seq;

    /* 
     * Iterate over bytes of the bitmask to find a chain of <number> zero bits. 
     */

    remaining = number;
    for (i = (index>>3); i < (size>>3); i++) {
	if (remaining <= 7) {
	    /* 
	     * End of sequence, or whole sequence for number < 8. 
	     * Find sequence in byte. 
	     */

	    seq = firstZeroBitSequence[remaining-1][mask[i]];
	    if (seq != -1) {
		if (remaining == number) {
		    /* 
		     * Sequence fits into one byte. 
		     */

		    return (i<<3) + seq;
		}

		/* 
		 * Sequence spans over several bytes. 
		 */

		if (seq == 0) {
		    /* 
		     * Sequence is contiguous. 
		     */

		    return first;
		}
	    }

	    /* 
	     * Sequence interrupted, restart. 
	     */

	    remaining = number;
	}

	/* 
	 * Sequence may span over several bytes. 
	 */

	if (remaining == number) {
	    /* 
	     * Beginning of sequence. 
	     *
	     * Note: leading bits actually denote trailing cells: bit 0 (LSB) is
	     * first cell, bit 7 (MSB) is last.
	     */

	    seq = longestLeadingZeroBitSequence[mask[i]];
	    if (seq) {
		first = (i<<3) + (8-seq);
		remaining -= seq;
	    }
	} else {
	    /* 
	     * Middle of sequence, look for 8 bits cleared = zero byte. 
	     */

	    if (mask[i] == 0) {
		/* 
		 * Continue sequence. 
		 */

		remaining -= 8;
	    } else {
		/* 
		 * Sequence interrupted, restart. 
		 */

		remaining = number;
	    }
	}
	if (remaining == 0) {
	    /*
	     * End of sequence reached.
	     */

	    return first;
	}
    }

    /*
     * None found.
     */

    return (size_t) -1;
}

static void
SetBit(
    unsigned char *mask,	/* Bitmask table. */
    size_t i)			/* Index of first bit to set. */
{
    mask[i>>3] |= 1<<(i&7);
}

static void 
SetBits(
    unsigned char *mask,	/* Bitmask table. */
    size_t first,		/* Index of first bit to set. */
    size_t number)		/* Number of bits to set. */
{
    size_t i;
    for (i=first; i < first+number; i++) {
	mask[i>>3] |= 1<<(i&7);
    }
}

static void 
ClearBits(
    unsigned char *mask,	/* Bitmask table. */
    size_t first,		/* Index of first bit to clear. */
    size_t number)		/* Number of bits to clear. */
{
    size_t i;
    for (i=first; i < first+number; i++) {
	mask[i>>3] &= ~(1<<(i&7));
    }
}

static size_t 
TestBit(
    unsigned char *mask,	/* Bitmask table. */
    size_t index)		/* Index of bit to set. */
{
    return mask[index>>3] & (1<<(index&7));
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatSysPageAlloc --
 *
 *	Allocate system pages.
 *
 * Results:
 *	The allocated system pages' base address.
 *
 * Side effects:
 *	New pages are allocated.
 *
 *---------------------------------------------------------------------------
 */

void * 
PlatSysPageAlloc(
    size_t number)		/* Number of pages to alloc. */
{
    void *addr = NULL;
    AddressRange *range, **prevPtr;
    size_t first, size;

    if (number > LARGE_PAGE_SIZE || !(number 
	    & ((systemInfo.dwAllocationGranularity >> shiftPage)-1))) {
	/*
	 * Length exceeds a certain threshold or is a multiple of the allocation
	 * granularity, allocate dedicated address range.
	 */

	addr = VirtualAlloc(NULL, number << shiftPage, MEM_RESERVE|MEM_COMMIT, 
		PAGE_READWRITE);
	if (!addr) {
	    /*
	     * Fatal error!
	     */

	    Col_Error(COL_FATAL, "VirtualAlloc: Error %u", GetLastError());
	}

	/*
	 * Create descriptor without bitmask and insert at head.
	 */

	EnterCriticalSection(&protect);
	range = (AddressRange *) (malloc(sizeof(AddressRange)));
	range->next = dedicatedRanges;
	range->base = addr;
	range->size = number;
	range->free = 0;
	range->first = number;
	dedicatedRanges = range;
	LeaveCriticalSection(&protect);

	return addr;
    }

    /*
     * Try to find address range with enough consecutive pages.
     */

    EnterCriticalSection(&protect);

    first = (size_t)-1;
    prevPtr = &ranges;
    range = ranges;
    while (range) {
	size = range->size;
	if (range->free >= number) {
	    /*
	     * Range has the required number of pages.
	     */

	    unsigned char *mask = range->bitmask;
	    first = range->first;

	    if (number == 1 && !TestBit(mask, first)) {
		/*
		 * Fast-track the most common case: allocate the first free 
		 * page.
		 */

		break;
	    }
	    
	    /*
	     * Find the first available bit sequence.
	     */

	    first = FindClearedBits(mask, size, number, first);
	    if (first != (size_t)-1) {
		break;
	    }
	}

	/*
	 * Not found, try next range.
	 */

	prevPtr = &range->next;
	range = range->next;
    }

    if (!range) {
	/*
	 * No range was found with available pages. Create a new one.
	 */

	if (!ranges) {
	    size = FIRST_RANGE_SIZE;
	} else {
	    /*
	     * New range size is double that of the previous one.
	     */

	    size <<= 1;
	    if (size > MAX_RANGE_SIZE) size = MAX_RANGE_SIZE;
	}
	ASSERT(number <= size-1);

	/*
	 * Reserve address range.
	 */

	addr = VirtualAlloc(NULL, size << shiftPage, MEM_RESERVE, 
		PAGE_READWRITE);
	if (!addr) {
	    /*
	     * Fatal error!
	     */

	    LeaveCriticalSection(&protect);
	    Col_Error(COL_FATAL, "VirtualAlloc: Error %u", GetLastError());
	    return NULL;
	}

	/*
	 * Create descriptor.
	 */

	range = (AddressRange *) (malloc(sizeof(AddressRange) + (size >> 3)));
	*prevPtr = range;
	range->base = addr;
	range->next = NULL;
	range->size = size;
	range->free = size;
	range->first = 0;
	memset(range->bitmask, 0, (size >> 3));
	first = 0;
    }

    /*
     * Commit pages. 
     */

    ASSERT(first+number <= size);
    ASSERT(number <= range->free);
    addr = VirtualAlloc((char *) range->base + (first << shiftPage), 
	    number << shiftPage, MEM_COMMIT, PAGE_READWRITE);
    if (!addr) {
	/*
	 * Fatal error!
	 */

	LeaveCriticalSection(&protect);
	Col_Error(COL_FATAL, "VirtualAlloc: Error %u", GetLastError());
	return NULL;
    }

    /*
     * Update metadata and return.
     */

    range->free -= number;
    if (first == range->first) {
	/* 
	 * Simply increase the first free page index, the actual index will be 
	 * updated on next allocation.
	 */

	range->first += number;
    }
    SetBits(range->bitmask, first, number);

    LeaveCriticalSection(&protect);
    return addr;
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatSysPageFree --
 *
 *	Free system pages.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The pages are freed.
 *
 *---------------------------------------------------------------------------
 */

void 
PlatSysPageFree(
    void * page,		/* Base address of the pages to free. */
    size_t number)		/* Number of pages to free. */
{
    size_t index;
    AddressRange * range;
    unsigned char *mask;

    EnterCriticalSection(&protect);

    /*
     * Get range info for page. 
     */

    range = ranges;
    while (range) {
	if (page >= range->base && (char *) page < (char *) range->base 
		+ (range->size << shiftPage)) {
	    break;
	}
	range = range->next;
    }
    if (!range) {
	/*
	 * Likely dedicated address range. 
	 */

	AddressRange **prevPtr = &dedicatedRanges;
	range = dedicatedRanges;
	while (range) {
	    if (page >= range->base && (char *) page < (char *) range->base 
		    + (range->size << shiftPage)) {
		break;
	    }
	    prevPtr = &range->next;
	    range = range->next;
	}
	if (!range) {
	    /*
	     * Not found.
	     */

	    LeaveCriticalSection(&protect);
	    Col_Error(COL_FATAL, "Page not found %p", page);
	    return;
	}

	/*
	 * Release whole range.
	 */

	if (!VirtualFree(range->base, 0, MEM_RELEASE)) {
	    /*
	     * Fatal error!
	     */

	    LeaveCriticalSection(&protect);
	    Col_Error(COL_FATAL, "VirtualFree: Error %u", GetLastError());
	    return;
	}

	/*
	 * Remove from dedicated range list.
	 */

	*prevPtr = range->next;
	free(range);
	LeaveCriticalSection(&protect);
	return;
    }

    mask = range->bitmask;
    index = ((char *) page - (char *) range->base) >> shiftPage;
    ASSERT(index+number <= range->size);
    ASSERT(TestBit(mask, index));

    /*
     * Decommit pages.
     */

    if (!VirtualFree(page, number << shiftPage, MEM_DECOMMIT)) {
	/*
	 * Fatal error!
	 */

	LeaveCriticalSection(&protect);
	Col_Error(COL_FATAL, "VirtualFree: Error %u", GetLastError());
	return;
    }
    ClearBits(mask, index, number);
    range->free += number;
    ASSERT(range->free <= range->size);
    if (range->first > index) {
	/*
	 * Old first free page is beyond the one we just freed, update.
	 */
	 
	range->first = index;
    }
    LeaveCriticalSection(&protect);
}

/*
 *----------------------------------------------------------------
 * Mark-and-sweep, generational, exact GC.
 *----------------------------------------------------------------
 */

#ifdef COL_THREADS

/*
 *---------------------------------------------------------------------------
 *
 * PlatGetGcMemInfo --
 * PlatGetErrorProcPtr --
 *
 *	Get thread-local info.
 *
 * Results:
 *	Pointer to thread-local info.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

GcMemInfo *
PlatGetGcMemInfo() {
    return &((ThreadData *) TlsGetValue(tlsToken))->gcMemInfo;
}

Col_ErrorProc **
PlatGetErrorProcPtr() {
    return &((ThreadData *) TlsGetValue(tlsToken))->errorProc;
}

#endif /* COL_THREADS */


/*
 *----------------------------------------------------------------
 * Process & threads.
 *----------------------------------------------------------------
 */

/*
 *---------------------------------------------------------------------------
 *
 * DllMain --
 *
 *	Windows DLL entry point.
 *
 * Results:
 *	True.
 *
 * Side effects:
 *	Create thread-local storage token.
 *
 *---------------------------------------------------------------------------
 */

BOOL APIENTRY 
DllMain( 
    HMODULE hModule,
    DWORD dwReason,
    LPVOID lpReserved)
{
    switch (dwReason) {
	case DLL_PROCESS_ATTACH:
	    return Init();

	case DLL_PROCESS_DETACH:
	    TlsFree(tlsToken);
	    break;
    }
    return TRUE;
}

/*
 *---------------------------------------------------------------------------
 *
 * Init --
 *
 *	Initialization routine. Called through DllMain.
 *
 * Results:
 *	True.
 *
 * Side effects:
 *	Create thread-local storage key. Note: key is never freed.
 *
 *---------------------------------------------------------------------------
 */

static BOOL
Init()
{
    if ((tlsToken = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
	return FALSE;
    }

    GetSystemInfo(&systemInfo);
    systemPageSize = systemInfo.dwPageSize;
    shiftPage = LOG2(systemPageSize);

    ranges = NULL;
    dedicatedRanges = NULL;
    InitializeCriticalSection(&protect);

    return TRUE;
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatEnter --
 *
 *	Enter the thread.
 *
 * Results:
 *	Non-zero if this is the first nested call, else 0.
 *
 * Side effects:
 *	Global data is initialized; decrement nesting count.
 *
 *---------------------------------------------------------------------------
 */

int
PlatEnter()
{
    ThreadData *info = (ThreadData *) TlsGetValue(tlsToken);
    if (!info) {
	/*
	 * Not yet initialized.
	*/

	info = (ThreadData *) malloc(sizeof(ThreadData));
	info->nestCount = 0;
	TlsSetValue(tlsToken, info);
    }

    /* 
     * Increment nest count. 
     */

    return ((++info->nestCount) == 1);
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatLeave --
 *
 *	Leave the thread.
 *
 * Results:
 *	Non-zero if this is the last nested call, else 0.
 *
 * Side effects:
 *	Decrement nesting count.
 *
 *---------------------------------------------------------------------------
 */

int
PlatLeave()
{
    ThreadData *info = (ThreadData *) TlsGetValue(tlsToken);
    if (!info) {
	/* TODO: exception ? */
	return 0;
    }

    /*
     * Decrement nest count.
     */

    return ((--info->nestCount) == 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * PlatCleanup --
 *
 *	Final cleanup.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Cleanup global structures.
 *
 *---------------------------------------------------------------------------
 */

void
PlatCleanup()
{
    ThreadData *info = (ThreadData *) TlsGetValue(tlsToken);
    if (!info || info->nestCount != 0) {
	/* TODO: exception ? */
	return;
    }

    free(info);
    TlsSetValue(tlsToken, 0);
}
