/*
 * Threading support.
 */

#define COL_THREADS

/*
 * Max number of generations. With a generational factor of 10, and 6 
 * generations, the oldest generation would be collected 10^5 = 1,000,000 times 
 * less frequently than the youngest generation. With one GC every 1/10th of
 * second, that would mean about 1 major GC a day.
 */

#define GC_MAX_GENERATIONS	6

/* 
 * Generational factor, i.e. nb of GCs between generations. 
 */

#define GC_GEN_FACTOR		10

/* 
 * Number of page allocations on a pool before triggering a GC.
 */

#define GC_MIN_PAGE_ALLOC	64
#define GC_MAX_PAGE_ALLOC	1024
#define GC_THRESHOLD(threshold)	\
    ((threshold)<GC_MIN_PAGE_ALLOC ?	GC_MIN_PAGE_ALLOC \
    :(threshold)>GC_MAX_PAGE_ALLOC ?	GC_MAX_PAGE_ALLOC \
    :					(threshold))

/*
 * Threshold value on a pool's fragmentation to decide whether to promote 
 * individual cells vs. whole pages. In practice this performs a compaction of 
 * this pool, which limits fragmentation and memory overhead. 
 */

#define PROMOTE_COMPACT
#define PROMOTE_PAGE_FILL_RATIO 0.90

/*
 * Number of system pages to allocate at once (see PoolAllocPages).
 */

#define NB_SYSPAGE_ALLOC	4

/* 
 * Allocator page size. System page size should be a multiple of it. 
 */

#define PAGE_SIZE		1024
typedef char Page[PAGE_SIZE];

/*
 * Cell size. Each cell can store 4 pointers (=> 16 bytes on 32-bit systems).
 */

#define CELL_SIZE		(sizeof(void *)*4)
typedef char Cell[CELL_SIZE];