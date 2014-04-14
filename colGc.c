/*                                                                              *//*!   @file \
 * colGc.c
 *
 *  This file implements the mark-and-sweep, generational, exact GC that is
 *  at the heart of Colibri.
 *
 *  Newer cells are born in "Eden", i.e. generation 1 pool, and are promoted
 *  to older pools when they survive more than one collection.
 *
 *  A GC is triggered after a number of page allocations have been made
 *  resulting from cell allocation failures. They are only triggered outside
 *  of GC-protected sections to avoid collecting the newly allocated cells.
 *
 *  Younger pools are collected more often than older ones. Completing a
 *  given number of GCs on a pool will include the previous generation to
 *  the next GC. For example, if this generational factor is 10, then
 *  generation x will be collected every 10 collections of generation x-1.
 *  So gen-x collections will occur every 10 gen-(x-1) collection, i.e.
 *  every 10^x gen-0 collection.
 */

#include "include/colibri.h"
#include "colInternal.h"
#include "colPlatform.h"

#include "colWordInt.h"
#include "colRopeInt.h"
#include "colVectorInt.h"
#include "colListInt.h"
#include "colMapInt.h"
#include "colHashInt.h"
#include "colTrieInt.h"
#include "colStrBufInt.h"

#include <memory.h>
#include <limits.h>
#include <malloc.h>
                                                                                #       ifndef DOXYGEN
/*
 * Prototypes for functions used only in this file.
 */

static size_t           GetNbCells(Col_Word word);
static void             ClearPoolBitmasks(MemoryPool *pool);
static void             MarkReachableCellsFromRoots(GroupData *data);
static void             MarkReachableCellsFromParents(GroupData *data);
static void             PurgeParents(GroupData *data);
static void             MarkWord(GroupData *data, Col_Word *wordPtr,
                            Page *parentPage);
static void             SweepUnreachableCells(GroupData *data,
                            MemoryPool *pool);
static void             PromotePages(GroupData *data, MemoryPool *pool);
static void             ResetPool(MemoryPool *pool);
static Col_CustomWordChildEnumProc MarkWordChild;
                                                                                #       endif DOXYGEN

/*
================================================================================*//*!   @addtogroup gc \
Garbage Collection                                                              *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * GC Exceptions                                                                *//*!   @{ *//*
 ******************************************************************************/
                                                                                /*!     @cond PRIVATE */
/*---------------------------------------------------------------------------   *//*!   @def \
 * PRECONDITION_GCPROTECTED
 *                                                                                      @hideinitializer
 *  Precondition macro checking that the call is performed within a
 *  GC-protected section (i.e.\ between Col_PauseGC() / Col_TryPauseGC() and
 *  Col_ResumeGC()).
 *
 *  May be followed by a return block.
 *
 *  @param data     #ThreadData
 *
 *  @error{COL_ERROR_GCPROTECT}
 *
 *  @see Col_Error
 *  @see ThreadData
 *
 *  @private
 *
 *  @todo rewrite with #COL_RUNTIMECHECK
 *//*-----------------------------------------------------------------------*/

#define PRECONDITION_GCPROTECTED(data) \
    if (!(data)->pauseGC) { \
        Col_Error(COL_ERROR, ColibriDomain, COL_ERROR_GCPROTECT); \
        goto PRECONDITION_GCPROTECTED_FAILED; \
    } \
    if (0) \
PRECONDITION_GCPROTECTED_FAILED:
                                                                                /*!     @endcond */
                                                                                /*!     @} */
/********************************************************************************//*!   @name \
 * Process & Threads                                                            *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * GcInitThread
 *                                                                              *//*!
 *  Per-thread GC-related initialization.
 *
 *  @sideeffect
 *      Initialize the eden pool (which is always thread-specific).
 *
 *  @see ThreadData
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

void
GcInitThread(
    ThreadData *data)   /*!< Thread-specific data. */
{
    PoolInit(&data->eden, 1);
}

/*---------------------------------------------------------------------------
 * GcInitGroup
 *                                                                              *//*!
 *  Per-group GC-related initialization.
 *
 *  @sideeffect
 *      Initialize all memory pools but eden.
 *
 *  @see GroupData
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

void
GcInitGroup(
    GroupData *data)    /*!< Group-specific data. */
{
    unsigned int generation;
    PoolInit(&data->rootPool, 0);
    for (generation = 2; generation < GC_MAX_GENERATIONS; generation++) {
        PoolInit(&data->pools[generation-2], generation);
    }
}

/*---------------------------------------------------------------------------
 * GcCleanupThread
 *                                                                              *//*!
 *  Per-thread GC-related cleanup.
 *
 *  @sideeffect
 *      Cleanup the eden pool (which is always thread-specific).
 *
 *  @see ThreadData
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

void
GcCleanupThread(
    ThreadData *data)   /*!< Thread-specific data. */
{
    PoolCleanup(&data->eden);
}

/*---------------------------------------------------------------------------
 * GcCleanupGroup
 *                                                                              *//*!
 *  Per-group GC-related cleanup.
 *
 *  @sideeffect
 *      Cleanup all memory pools but eden.
 *
 *  @see GroupData
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

void
GcCleanupGroup(
    GroupData *data)    /*!< Group-specific data. */
{
    unsigned int generation;
    PoolCleanup(&data->rootPool);
    for (generation = 2; generation < GC_MAX_GENERATIONS; generation++) {
        PoolCleanup(&data->pools[generation-2]);
    }
}
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Word Lifetime Management                                                     *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * GetNbCells
 *                                                                              *//*!
 *  Get the number of cells taken by a word.
 *
 *  @return
 *      The number of cells.
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

static size_t
GetNbCells(
    Col_Word word)  /*!< The word. */
{
    /*
     * Get number of cells taken by word.
     * @note word is not immediate.
     */

    switch (WORD_TYPE(word)) {
    case WORD_TYPE_UCSSTR:
        return UCSSTR_SIZE(WORD_UCSSTR_LENGTH(word)
                * CHAR_WIDTH(WORD_UCSSTR_FORMAT(word)));

    case WORD_TYPE_UTFSTR:
        return UTFSTR_SIZE(WORD_UTFSTR_BYTELENGTH(word));

    case WORD_TYPE_VECTOR:
        return VECTOR_SIZE(WORD_VECTOR_LENGTH(word));

    case WORD_TYPE_MVECTOR:
        return WORD_MVECTOR_SIZE(word);

    case WORD_TYPE_CUSTOM: {
        Col_CustomWordType *typeInfo = WORD_TYPEINFO(word);
        size_t headerSize;
        switch (typeInfo->type) {
        case COL_HASHMAP: headerSize = CUSTOMHASHMAP_HEADER_SIZE;
        case COL_TRIEMAP: headerSize = CUSTOMTRIEMAP_HEADER_SIZE;
        default:          headerSize = CUSTOM_HEADER_SIZE;
        }
        return WORD_CUSTOM_SIZE(typeInfo, headerSize,
                typeInfo->sizeProc(word));
        }

    case WORD_TYPE_STRHASHMAP:
    case WORD_TYPE_INTHASHMAP:
        return HASHMAP_NBCELLS;

    case WORD_TYPE_STRBUF:
        return WORD_STRBUF_SIZE(word);

    /* WORD_TYPE_UNKNOWN */

    default:
        return 1;
    }
}

/*---------------------------------------------------------------------------
 * Col_WordPreserve
 *                                                                              *//*!
 *  Preserve a persistent reference to a word, making it a root. This allows
 *  words to be safely stored in external structures regardless of memory
 *  management cycles. More specifically, they can't be collected and their
 *  address remains constant.
 *
 *  Calls can be nested. A reference count is updated accordingly.
 *
 *  Roots are stored in a trie indexed by the root source addresses.
 *
 *  @pre
 *      Must be called within a GC-protected section.
 *
 *  @sideeffect
 *      May allocate memory cells. Marks word as pinned.
 *
 *  @see Col_WordRelease
 *//*-----------------------------------------------------------------------*/

void
Col_WordPreserve(
    Col_Word word)  /*!< The word to preserve. */
{
    ThreadData *data = PlatGetThreadData();
    Cell *node, *leaf, *parent, *newParent;
    Col_Word leafSource;
    uintptr_t mask;

    /*
     * Check preconditions.
     */

    PRECONDITION_GCPROTECTED(data) return;                                      /*!     @error{COL_ERROR_GCPROTECT} */

    /*
     * Rule out immediate values.
     */

    switch (WORD_TYPE(word)) {
    case WORD_TYPE_NIL:
    case WORD_TYPE_SMALLINT:
    case WORD_TYPE_SMALLFP:
    case WORD_TYPE_CHARBOOL:
    case WORD_TYPE_SMALLSTR:
    case WORD_TYPE_VOIDLIST:
        /*
         * Immediate values.
         */

        return;

    case WORD_TYPE_CIRCLIST:
        /*
         * Preserve core list.
         */

        Col_WordPreserve(WORD_CIRCLIST_CORE(word));
        return;

        /* WORD_TYPE_UNKNOWN */
    }

    /*
     * Search for matching entry in root trie.
     */

    EnterProtectRoots(data->groupData);
    {
        node = data->groupData->roots;
        while (node) {
            if (!ROOT_IS_LEAF(node)) {
                if ((uintptr_t) word & ROOT_NODE_MASK(node)) {
                    /*
                     * Recurse on right.
                     */

                    node = ROOT_NODE_RIGHT(node);
                } else {
                    /*
                     * Recurse on left.
                     */

                    node = ROOT_NODE_LEFT(node);
                }
                continue;
            }

            /*
             * Leaf node.
             */

            ASSERT(ROOT_IS_LEAF(node));
            node = ROOT_GET_NODE(node);
            leafSource = ROOT_LEAF_SOURCE(node);
            if (word == leafSource) {
                /*
                 * Found! Increment reference count.
                 */

                ASSERT(WORD_PINNED(word));
                ROOT_LEAF_REFCOUNT(node)++;
                goto end;
            }
            break;
        }

        /*
         * Not found, insert. Pin the word so that its address doesn't change
         * during compaction.
         */

        WORD_SET_PINNED(word);

        leaf = PoolAllocCells(&data->groupData->rootPool, 1);
        ROOT_LEAF_INIT(leaf, NULL, PAGE_GENERATION(CELL_PAGE(word)), 1, word);

        if (!data->groupData->roots) {
            /*
             * First leaf.
             */

            data->groupData->roots = ROOT_GET_LEAF(leaf);
            goto end;
        }

        /*
         * Get diff mask between inserted key and existing leaf key, i.e. only
         * keep the highest bit set.
         * See: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
         */

        mask = (uintptr_t) word ^ (uintptr_t) leafSource;
        ASSERT(mask);
        mask |= mask >> 1;
        mask |= mask >> 2;
        mask |= mask >> 4;
        mask |= mask >> 8;
        mask |= mask >> 16;
        mask >>= 1;
        mask++;
        ASSERT(mask);

        /*
         * Find insertion point.
         */

        node = data->groupData->roots;
        while (node) {
            if (!ROOT_IS_LEAF(node) && mask < ROOT_NODE_MASK(node)) {
                if ((uintptr_t) word & ROOT_NODE_MASK(node)) {
                    /*
                     * Recurse on right.
                     */

                    node = ROOT_NODE_RIGHT(node);
                } else {
                    /*
                     * Recurse on left.
                     */

                    node = ROOT_NODE_LEFT(node);
                }
                continue;
            }
            break;
        }
        ASSERT(node);

        /*
         * Insert leaf here.
         */

        parent = ROOT_PARENT(ROOT_GET_NODE(node));
        newParent = PoolAllocCells(&data->groupData->rootPool, 1);
        if ((uintptr_t) word & mask) {
            /*
             * Leaf is right.
             */

            ROOT_NODE_INIT(newParent, parent, mask, node, ROOT_GET_LEAF(leaf));
        } else {
            /*
             * Leaf is left.
             */

            ROOT_NODE_INIT(newParent, parent, mask, ROOT_GET_LEAF(leaf), node);
        }
        ROOT_PARENT(leaf) = newParent;
        ROOT_PARENT(ROOT_GET_NODE(node)) = newParent;
        if (!parent) {
            /*
             * Leaf was trie root.
             */

            data->groupData->roots = newParent;
        } else if ((uintptr_t) word & ROOT_NODE_MASK(parent)) {
            ROOT_NODE_RIGHT(parent) = newParent;
        } else {
            ROOT_NODE_LEFT(parent) = newParent;
        }
    }
end:
    LeaveProtectRoots(data->groupData);
}

/*---------------------------------------------------------------------------
 * Col_WordRelease
 *                                                                              *//*!
 *  Release a root word previously made by Col_WordPreserve().
 *
 *  Calls can be nested. A reference count is updated accordingly. Once the
 *  count drops below 1, the root becomes stale.
 *
 *  @pre
 *      Must be called within a GC-protected section.
 *
 *  @sideeffect
 *      May release memory cells. Unpin word.
 *
 *  @see Col_WordPreserve
 *//*-----------------------------------------------------------------------*/

void
Col_WordRelease(
    Col_Word word)  /*!< The root word to release. */
{
    ThreadData *data = PlatGetThreadData();
    Cell *node, *sibling, *parent, *grandParent;

    /*
     * Check preconditions.
     */

    PRECONDITION_GCPROTECTED(data) return;                                      /*!     @error{COL_ERROR_GCPROTECT} */

    /*
     * Rule out immediate values.
     */

    switch (WORD_TYPE(word)) {
    case WORD_TYPE_NIL:
    case WORD_TYPE_SMALLINT:
    case WORD_TYPE_SMALLFP:
    case WORD_TYPE_CHARBOOL:
    case WORD_TYPE_SMALLSTR:
    case WORD_TYPE_VOIDLIST:
        /*
         * Immediate values.
         */

        return;

    case WORD_TYPE_CIRCLIST:
        /*
         * Release core list.
         */

        Col_WordRelease(WORD_CIRCLIST_CORE(word));
        return;

        /* WORD_TYPE_UNKNOWN */
    }

    EnterProtectRoots(data->groupData);
    {
        if (!WORD_PINNED(word)) {
            /*
             * Roots are always pinned.
             */

            goto end;
        }

        /*
         * Search for matching entry.
         */

        if (!data->groupData->roots) {
            goto end;
        }
        node = data->groupData->roots;
        while (node) {
            if (!ROOT_IS_LEAF(node)) {
                if ((uintptr_t) word & ROOT_NODE_MASK(node)) {
                    /*
                     * Recurse on right.
                     */

                    node = ROOT_NODE_RIGHT(node);
                } else {
                    /*
                     * Recurse on left.
                     */

                    node = ROOT_NODE_LEFT(node);
                }
                continue;
            }

            /*
             * Leaf node.
             */

            ASSERT(ROOT_IS_LEAF(node));
            node = ROOT_GET_NODE(node);
            if (word != ROOT_LEAF_SOURCE(node)) {
                /*
                 * Source doesn't match.
                 */

                goto end;
            }
            break;
        }
        ASSERT(node);

        /*
         * Decrement reference count.
         */

        if (--ROOT_LEAF_REFCOUNT(node)) {
            /*
             * Root is still valid.
             */

            goto end;
        }

        /*
         * Remove leaf. Unpin word as well.
         */

        ASSERT(TestCell(CELL_PAGE(node), CELL_INDEX(node)));
        ClearCells(CELL_PAGE(node), CELL_INDEX(node), 1);
        WORD_CLEAR_PINNED(word);

        parent = ROOT_PARENT(node);
        if (!parent) {
            /*
             * Leaf was trie root.
             */

            data->groupData->roots = NULL;
            goto end;
        }

        /*
         * Remove parent and replace by sibling.
         */

        ASSERT(TestCell(CELL_PAGE(parent), CELL_INDEX(parent)));
        ClearCells(CELL_PAGE(parent), CELL_INDEX(parent), 1);
        if ((uintptr_t) word & ROOT_NODE_MASK(parent)) {
            sibling = ROOT_NODE_LEFT(parent);
        } else {
            sibling = ROOT_NODE_RIGHT(parent);
        }

        grandParent = ROOT_PARENT(parent);
        if (!grandParent) {
            /*
             * Parent was trie root.
             */

            data->groupData->roots = sibling;
        } else if ((uintptr_t) word & ROOT_NODE_MASK(grandParent)) {
            ROOT_NODE_RIGHT(grandParent) = sibling;
        } else {
            ROOT_NODE_LEFT(grandParent) = sibling;
        }
        ROOT_PARENT(ROOT_GET_NODE(sibling)) = grandParent;
    }
end:
    LeaveProtectRoots(data->groupData);
}
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Cell Allocation                                                              *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * AllocCells
 *                                                                              *//*!
 *  Allocate cells in the eden pool.
 *
 *  @pre
 *      Must be called within a GC-protected section.
 *
 *  @retval pointer     to the first allocated cell if successful.
 *  @retval NULL        otherwise.
 *
 *  @see PoolAllocCells
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

Cell *
AllocCells(
    size_t number)  /*!< Number of cells to allocate. */
{
    ThreadData *data = PlatGetThreadData();

    /*
     * Check preconditions.
     */

    PRECONDITION_GCPROTECTED(data) return NULL;                                 /*!     @error{COL_ERROR_GCPROTECT} */

    /*
     * Alloc cells; alloc pages if needed.
     */

    return PoolAllocCells(&data->eden, number);
}
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * GC-Protected Sections                                                        *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Col_PauseGC
 *                                                                              *//*!
 *  Pause the automatic garbage collection. Calls can be nested. Code
 *  between the outermost pause and resume calls define a GC-protected
 *  section.
 *
 *  When the threading model isn't #COL_SINGLE, blocks as long as a GC is
 *  underway.
 *
 *  @see Col_TryPauseGC
 *  @see Col_ResumeGC
 *//*-----------------------------------------------------------------------*/

void
Col_PauseGC()
{
    ThreadData *data = PlatGetThreadData();
    if (data->pauseGC == 0) {
        SyncPauseGC(data->groupData);
    }
    data->pauseGC++;
}

/*---------------------------------------------------------------------------
 * Col_TryPauseGC
 *                                                                              *//*!
 *  Try to pause the automatic garbage collection. Calls can be nested.
 *
 *  @retval 1   if successful
 *  @retval 0   if a GC is underway (this implies the threading model isn't
 *              #COL_SINGLE). In this case the caller must try again later
 *              or use the blocking version.
 *
 *  @see Col_PauseGC
 *  @see Col_ResumeGC
 *//*-----------------------------------------------------------------------*/

int
Col_TryPauseGC()
{
    ThreadData *data = PlatGetThreadData();
    if (data->pauseGC || TrySyncPauseGC(data->groupData)) {
        data->pauseGC++;
        return 1;
    } else {
        return 0;
    }
}

/*---------------------------------------------------------------------------
 * Col_ResumeGC
 *                                                                              *//*!
 *  Resume the automatic garbage collection. Calls can be nested.
 *
 *  Leaving a GC-protected section potentially triggers a GC.
 *
 *  @pre
 *      Must be called within a GC-protected section.
 *
 *  @sideeffect
 *      May trigger the garbage collection.
 *
 *  @see Col_PauseGC
 *  @see Col_TryPauseGC
 *//*-----------------------------------------------------------------------*/

void
Col_ResumeGC()
{
    ThreadData *data = PlatGetThreadData();

    /*
     * Check preconditions.
     */

    PRECONDITION_GCPROTECTED(data) return;                                      /*!     @error{COL_ERROR_GCPROTECT} */

    if (data->pauseGC > 1) {
        /*
         * Within nested sections.
         */

        data->pauseGC--;
        return;
    } else {
        /*
         * Leaving protected section.
         */

        size_t threshold = data->groupData->pools[0].nbPages / GC_GEN_FACTOR;

        /*
         * GC is needed when the number of pages allocated since the last GC
         * exceed a given threshold.
         */

        int performGc = (data->eden.nbAlloc >= GC_THRESHOLD(threshold));
        ASSERT(data->pauseGC == 1);
        SyncResumeGC(data->groupData, performGc);
        data->pauseGC = 0;
    }
}
                                                                                /*!     @} */
                                                                                
/********************************************************************************//*!   @name \
 * Mark & Sweep Algorithm                                                       *//*!   @{ *//*
 ******************************************************************************/

 /*---------------------------------------------------------------------------
 * PerformGC
 *                                                                              *//*!
 *  Perform a garbage collection.
 *
 *  @sideeffect
 *      May free cells or pages, promote words across pools, or allocate new
 *      pages during promotion.
 *
 *  @see ClearPoolBitmasks
 *  @see MarkReachableCellsFromRoots
 *  @see MarkReachableCellsFromParents
 *  @see SweepUnreachableCells
 *  @see PromotePages
 *  @see ResetPool
  *
 *  @private
 *//*-----------------------------------------------------------------------*/


void
PerformGC(
    GroupData *data)    /*!< Group-specific data. */
{
    unsigned int generation;
    ThreadData *threadData;

    /*
     * Traverse pools to check whether they must be collected. Eden pool is
     * always collected. Root pool uses explicit lifetime management.
     */

    threadData = data->first;
    do {
        ClearPoolBitmasks(&threadData->eden);
        threadData = threadData->next;
    } while (threadData != data->first);
    data->maxCollectedGeneration = 1;

    for (generation = 2; generation < GC_MAX_GENERATIONS; generation++) {
        MemoryPool *pool = &data->pools[generation-2];

        if (generation+1 < GC_MAX_GENERATIONS) {
            /*
             * Intermediary generations.
             */

            size_t  threshold = data->pools[generation-1].nbPages
                    / GC_GEN_FACTOR;
            if (pool->nbAlloc < GC_THRESHOLD(threshold)) {
                /*
                 * Stop if number of allocations is less than the threshold.
                 */

                break;
            }
            if (++pool->gc < GC_GEN_FACTOR) {
                /*
                 * Collection frequency is logarithmic.
                 */

                break;
            }
        } else {
            /*
             * Ultimate generation.
             */

            if (pool->nbAlloc < GC_MAX_PAGE_ALLOC) {
                break;
            }
        }

        data->maxCollectedGeneration = generation;

        /*
         * Clear bitmasks on pool. Reachable words will be marked again in the
         * next step.
         */

        ClearPoolBitmasks(pool);
    }

#ifdef PROMOTE_COMPACT
    if (data->maxCollectedGeneration+1 < GC_MAX_GENERATIONS
            && data->pools[data->maxCollectedGeneration-1].nbPages > 0
            && data->pools[data->maxCollectedGeneration-1].nbSetCells
                    < (data->pools[data->maxCollectedGeneration-1].nbPages
                            * CELLS_PER_PAGE)
                    * PROMOTE_PAGE_FILL_RATIO) {
        data->compactGeneration = data->maxCollectedGeneration;
    } else {
        data->compactGeneration = UINT_MAX;
    }
#endif

    /*
     * Refresh parent list by adding pages written since the last GC.
     */

    UpdateParents(data);

    /*
     * Mark all cells that are reachable from roots of collected pools, and from
     * parent pages of uncollected pools. This also marks still valid roots and
     * parents in the process.
     */

    MarkReachableCellsFromRoots(data);
    MarkReachableCellsFromParents(data);

    /*
     * Purge stale parents.
     */

    PurgeParents(data);

    /*
     * Perform cleanup on all custom words that need sweeping.
     */

    for (generation = data->maxCollectedGeneration; generation >= 2;
            generation--) {
        SweepUnreachableCells(data, &data->pools[generation-2]);
    }
    threadData = data->first;
    do {
        SweepUnreachableCells(data, &threadData->eden);
        threadData = threadData->next;
    } while (threadData != data->first);

    /*
     * Free empty pages from collected pools before promoting them.
     */

    PoolFreeEmptyPages(&data->rootPool);
    threadData = data->first;
    do {
        PoolFreeEmptyPages(&threadData->eden);
        threadData = threadData->next;
    } while (threadData != data->first);
    for (generation = 2; generation <= data->maxCollectedGeneration;
            generation++) {
        PoolFreeEmptyPages(&data->pools[generation-2]);
    }

    /*
     * At this point all reachable cells are set, and unreachable cells are
     * cleared. Now promote whole pages the next generation. Older pools are
     * promoted first so that newer ones are correctly merged when promoted.
     */

    for (generation = data->maxCollectedGeneration; generation >= 2;
            generation--) {
        PromotePages(data, &data->pools[generation-2]);
    }
    threadData = data->first;
    do {
        PromotePages(data, &threadData->eden);
        threadData = threadData->next;
    } while (threadData != data->first);

    /*
     * Finally, reset pools (GC counters, etc.).
     */

    ResetPool(&data->rootPool);
    threadData = data->first;
    do {
        ResetPool(&threadData->eden);
        threadData = threadData->next;
    } while (threadData != data->first);
    for (generation = 2; generation <= data->maxCollectedGeneration;
            generation++) {
        ResetPool(&data->pools[generation-2]);
    }
}

/*---------------------------------------------------------------------------
 * ClearPoolBitmasks
 *                                                                              *//*!
 *  Unprotect and clear all bitmasks in the pool's pages.
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

static void
ClearPoolBitmasks(
    MemoryPool *pool)   /*!< The pool to traverse. */
{
    void * page;

    for (page = pool->pages; page; page = PAGE_NEXT(page)) {
        ASSERT(PAGE_GENERATION(page) == pool->generation);
        if (pool->generation > 1 && PAGE_FLAG(page, PAGE_FLAG_FIRST)) {
            SysPageProtect(page, 0);
        }
        ClearAllCells(page);
    }
}

/*---------------------------------------------------------------------------
 * MarkReachableCellsFromRoots
 *                                                                              *//*!
 *  Mark all cells reachable from the valid roots.
 *
 *  Traversal will stop at cells from uncollected pools. Cells from these
 *  pools having children in collected pools will be traversed in the
 *  next phase (MarkReachableCellsFromParents()).
 *
 *  @see MarkWord
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

static void
MarkReachableCellsFromRoots(
    GroupData *data)    /*!< Group-specific data. */
{
    Cell *node, *leaf, *parent;
    Col_Word source;

    node = data->roots;
    while (node) {
        ASSERT(TestCell(CELL_PAGE(node), CELL_INDEX(node)));
        if (!ROOT_IS_LEAF(node)) {
            /*
             * Descend into left subtrie.
             */

            node = ROOT_NODE_LEFT(node);
            continue;
        }

        leaf = ROOT_GET_NODE(node);
        source = ROOT_LEAF_SOURCE(leaf);
        if (ROOT_LEAF_GENERATION(leaf) <= data->maxCollectedGeneration) {
            /*
             * Follow root from collected generation.
             */

            MarkWord(data, &source, CELL_PAGE(source));
            if (ROOT_LEAF_GENERATION(leaf)+1 < GC_MAX_GENERATIONS) {
                ROOT_LEAF_GENERATION(leaf)++;
            }
        }

        /*
         * Find next branch.
         */

        parent = ROOT_PARENT(leaf);
        while (parent && ((uintptr_t) source & ROOT_NODE_MASK(parent))) {
            parent = ROOT_PARENT(parent);
        }
        if (!parent) {
            /*
             * Reached end.
             */

            break;
        }
        node = ROOT_NODE_RIGHT(parent);
    }
}

/*---------------------------------------------------------------------------
 * MarkReachableCellsFromParents
 *                                                                              *//*!
 *  Mark all cells reachable from cells in pages with potentially younger
 *  children.
 *
 *  @see MarkWord
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

static void
MarkReachableCellsFromParents(
    GroupData *data)    /*!< Group-specific data. */
{
    Col_Word word;
    Page *page;
    Cell *cell;
    size_t index, nbCells;

    /*
     * Follow cells in parent pages from uncollected generations.
     */

    for (cell = data->parents; cell; cell = PARENT_NEXT(cell)) {
        ASSERT(TestCell(CELL_PAGE(cell), CELL_INDEX(cell)));

        page = PARENT_PAGE(cell);
        if (PAGE_GENERATION(page) <= data->maxCollectedGeneration) {
            continue;
        }

        /*
         * Iterate over logical pages in page group.
         */

        ASSERT(PAGE_FLAG(page, PAGE_FLAG_FIRST));
        for (; page; page = PAGE_NEXT(page)) {
            /*
             * Iterate over cells in page.
             */

            for (index = RESERVED_CELLS; index < CELLS_PER_PAGE;
                    index += nbCells) {
                if (!TestCell(page, index)) {
                    nbCells = 1;
                    continue;
                }

                word = (Col_Word) PAGE_CELL(page, index);
                nbCells = GetNbCells(word);

                /*
                 * Follow word. Clearing the first cell is sufficient, as it
                 * will be set again during the mark phase.
                 */

                ClearCells(page, index, 1);
                MarkWord(data, &word, page);
            }

            if (PAGE_FLAG(page, PAGE_FLAG_LAST)) break;
        }
    }
}

/*---------------------------------------------------------------------------
 * PurgeParents
 *                                                                              *//*!
 *  Purge all parent nodes whose page is not marked as parent.
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

static void
PurgeParents(
    GroupData *data)    /*!< Group-specific data. */
{
    Page *page;
    Cell *cell, *next, *stale;
    int parent;

    for (cell = data->parents, data->parents = NULL, stale = NULL; cell;
            cell = next) {
        ASSERT(TestCell(CELL_PAGE(cell), CELL_INDEX(cell)));

        next = PARENT_NEXT(cell);
        parent = 0;
        ASSERT(PAGE_FLAG(PARENT_PAGE(cell), PAGE_FLAG_FIRST));
        for (page = PARENT_PAGE(cell); page; page = PAGE_NEXT(page)) {
            parent |= PAGE_FLAG(page, PAGE_FLAG_PARENT);
            if (PAGE_FLAG(page, PAGE_FLAG_LAST)) break;
        }
        if (parent) {
            /*
             * Keep parent.
             */

            PARENT_NEXT(cell) = data->parents;
            data->parents = cell;
        } else {
            /*
             * Forget parent.
             */

            PARENT_NEXT(cell) = stale;
            stale = cell;
        }
    }

    /*
     * Now drop stale parents.
     */

    for (cell = stale; cell; cell = PARENT_NEXT(cell)) {
        ASSERT(TestCell(CELL_PAGE(cell), CELL_INDEX(cell)));
        ClearCells(CELL_PAGE(cell), CELL_INDEX(cell), 1);

        /*
         * Protect pages from uncollected generations. Those from collected
         * generations will be protected when promoted.
         */

        page = PARENT_PAGE(cell);
        ASSERT(PAGE_FLAG(page, PAGE_FLAG_FIRST));
        if (PAGE_GENERATION(page) > data->maxCollectedGeneration) {
            SysPageProtect(page, 1);
        }
    }
}

/*---------------------------------------------------------------------------
 * MarkWordChild
 *                                                                              *//*!
 *  Word children enumeration function used to follow all children of a
 *  reachable word during GC. Follows Col_CustomWordChildEnumProc()
 *  signature.
 *
 *  @see Col_CustomWordType
 *  @see Col_CustomWordChildrenProc
 *  @see MarkWord
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

static void
MarkWordChild(
    Col_Word word,              /*!< Custom word whose child is being
                                     followed. */
    Col_Word *childPtr,         /*!< Pointer to child, may be overwritten if
                                     moved. */
    Col_ClientData clientData)  /*!< Points to #GroupData */
{
    MarkWord((GroupData *) clientData, childPtr, CELL_PAGE(word));
}

/*---------------------------------------------------------------------------
 * MarkWord
 *                                                                              *//*!
 *  Mark word and all its children as reachable.
 *
 *  The algorithm is recursive and stops when it reaches an already set
 *  cell. This handles loops and references to older pools, where cells are
 *  already set.
 *
 *  To limit stack growth, tail recurses when possible using an infinite
 *  loop with conditional return.
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

static void
MarkWord(
    GroupData *data,    /*!< Group-specific data. */
    Col_Word *wordPtr,  /*!< Word to mark and follow, overwritten if
                             promoted. */
    Page *parentPage)   /*!< Page containing wordPtr, will be set as modified
                             if overwritten. */
{
    int type;
    size_t nbCells, index;
    Page *page;

    /*
     * Entry point for tail recursive calls.
     */
                                                                                #       ifndef DOXYGEN
#define TAIL_RECURSE(_wordPtr, _parentPage) \
    wordPtr = (_wordPtr); parentPage = (_parentPage); goto start;
                                                                                #       endif DOXYGEN
start:

    type = WORD_TYPE(*wordPtr);
    switch (type) {
    case WORD_TYPE_NIL:
    case WORD_TYPE_SMALLINT:
    case WORD_TYPE_SMALLFP:
    case WORD_TYPE_CHARBOOL:
    case WORD_TYPE_SMALLSTR:
    case WORD_TYPE_VOIDLIST:
        /*
         * No cell to mark nor child to follow.
         */

        return;

    case WORD_TYPE_CIRCLIST: {
        /*
         * Recurse on core list and update immediate word if needed.
         */

        Col_Word core = WORD_CIRCLIST_CORE(*wordPtr);
        MarkWord(data, &core, parentPage);
        *wordPtr = WORD_CIRCLIST_NEW(core);
        return;
    }

#ifdef PROMOTE_COMPACT
    case WORD_TYPE_REDIRECT:
        /*
         * Overwrite reference and tail recurse on source.
         */

        *wordPtr = WORD_REDIRECT_SOURCE(*wordPtr);
        TAIL_RECURSE(wordPtr, parentPage);
#endif /* PROMOTE_COMPACT */

    /* WORD_TYPE_UNKNOWN */
    }

    page = CELL_PAGE(*wordPtr);
    if (PAGE_GENERATION(page) < PAGE_GENERATION(parentPage)
            && !PAGE_FLAG(parentPage, PAGE_FLAG_PARENT)) {
        /*
         * Mark page as parent.
         */

        PAGE_SET_FLAG(parentPage, PAGE_FLAG_PARENT);
    }

    /*
     * Stop if cell is already marked.
     */

    index = CELL_INDEX(*wordPtr);
    if (TestCell(page, index)) {
        return;
    }

    nbCells = GetNbCells(*wordPtr);

#ifdef PROMOTE_COMPACT
    if (PAGE_GENERATION(page) == data->compactGeneration
            && !WORD_PINNED(*wordPtr)) {
        /*
         * Unpinned words are moved to the next generation.
         */

        Col_Word promoted = (Col_Word) PoolAllocCells(
                &data->pools[data->compactGeneration-1], nbCells);
        memcpy((void *) promoted, (const void *) *wordPtr, nbCells * CELL_SIZE);

        /*
         * Replace original by redirect.
         */

    WORD_REDIRECT_INIT(*wordPtr, promoted);

        /*
         * Tail recurse on promoted. Clearing the first cell is sufficient, as
         * it will be set again during the mark phase.
         */

        ClearCells(CELL_PAGE(promoted), CELL_INDEX(promoted), 1);
        ASSERT(!TestCell(CELL_PAGE(promoted), CELL_INDEX(promoted)));
        *wordPtr = promoted;
        TAIL_RECURSE(wordPtr, parentPage);
    }
#endif

    ASSERT(!TestCell(page, index));
    SetCells(page, index, (index+nbCells < CELLS_PER_PAGE ? nbCells
            : CELLS_PER_PAGE-index));
    ASSERT(TestCell(page, index));

    /*
     * Follow children.
     */

    switch (type) {
    case WORD_TYPE_WRAP:
        if (!(WORD_WRAP_TYPE(*wordPtr) & (COL_INT | COL_FLOAT))) {
            /*
             * Follow source word.
             */

            MarkWord(data, &WORD_WRAP_SOURCE(*wordPtr), page);
        }

        /*
         * Tail recurse on synonym.
         */

        TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);

    case WORD_TYPE_UCSSTR:
    case WORD_TYPE_UTFSTR:
        return;

    case WORD_TYPE_SUBROPE:
        /*
         * Tail recurse on source.
         */

        TAIL_RECURSE(&WORD_SUBROPE_SOURCE(*wordPtr), page);

    case WORD_TYPE_CONCATROPE:
        /*
         * Follow left arm and tail recurse on right.
         */

        MarkWord(data, &WORD_CONCATROPE_LEFT(*wordPtr), page);
        TAIL_RECURSE(&WORD_CONCATROPE_RIGHT(*wordPtr), page);

    case WORD_TYPE_VECTOR:
    case WORD_TYPE_MVECTOR: {
        /*
         * Follow elements.
         */

        size_t i, length = WORD_VECTOR_LENGTH(*wordPtr);
        Col_Word *elements = WORD_VECTOR_ELEMENTS(*wordPtr);
        for (i = 0; i < length; i++) {
            MarkWord(data, elements+i, page);
        }
        return;
        }

    case WORD_TYPE_SUBLIST:
        /*
         * Tail recurse on source.
         */

        TAIL_RECURSE(&WORD_SUBLIST_SOURCE(*wordPtr), page);

    case WORD_TYPE_CONCATLIST:
    case WORD_TYPE_MCONCATLIST:
        /*
         * Follow left arm and tail recurse on right.
         */

        MarkWord(data, &WORD_CONCATLIST_LEFT(*wordPtr), page);
        TAIL_RECURSE(&WORD_CONCATLIST_RIGHT(*wordPtr), page);

    case WORD_TYPE_STRHASHMAP:
    case WORD_TYPE_INTHASHMAP:
        if (WORD_HASHMAP_BUCKETS(*wordPtr)) {
            /*
             * Buckets are stored in a separate word.
             */

            MarkWord(data, &WORD_HASHMAP_BUCKETS(*wordPtr), page);
        } else {
            /*
             * Buckets are stored inline.
             */

            size_t i;
            Col_Word *buckets = WORD_HASHMAP_STATICBUCKETS(*wordPtr);
            for (i = 0; i < HASHMAP_STATICBUCKETS_SIZE; i++) {
                MarkWord(data, buckets+i, page);
            }
        }

        /*
         * Tail recurse on synonym.
         */

        TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);

    case WORD_TYPE_HASHENTRY:
    case WORD_TYPE_MHASHENTRY:
        /*
         * Follow key.
         */

        MarkWord(data, &WORD_MAPENTRY_KEY(*wordPtr), page);
        /* continued. */
    case WORD_TYPE_INTHASHENTRY:
    case WORD_TYPE_MINTHASHENTRY:
        /*
         * Follow value and tail recurse on next.
         */

        MarkWord(data, &WORD_MAPENTRY_VALUE(*wordPtr), page);
        TAIL_RECURSE(&WORD_HASHENTRY_NEXT(*wordPtr), page);

    case WORD_TYPE_STRTRIEMAP:
    case WORD_TYPE_INTTRIEMAP:
        /*
         * Follow trie root and tail recurse on synonym.
         */

        MarkWord(data, &WORD_TRIEMAP_ROOT(*wordPtr), page);
        TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);

    case WORD_TYPE_STRTRIENODE:
    case WORD_TYPE_MSTRTRIENODE:
    case WORD_TYPE_INTTRIENODE:
    case WORD_TYPE_MINTTRIENODE:
    case WORD_TYPE_TRIENODE:
    case WORD_TYPE_MTRIENODE:
        /*
         * Follow left arm and tail recurse on right.
         */

        MarkWord(data, &WORD_TRIENODE_LEFT(*wordPtr), page);
        TAIL_RECURSE(&WORD_TRIENODE_RIGHT(*wordPtr), page);

    case WORD_TYPE_TRIELEAF:
    case WORD_TYPE_MTRIELEAF:
        /*
         * Follow key.
         */

        MarkWord(data, &WORD_MAPENTRY_KEY(*wordPtr), page);
        /* continued. */
    case WORD_TYPE_INTTRIELEAF:
    case WORD_TYPE_MINTTRIELEAF:
        /*
         * Tail recurse on value.
         */

        TAIL_RECURSE(&WORD_MAPENTRY_VALUE(*wordPtr), page);

    case WORD_TYPE_STRBUF:
        /*
         * Tail recurse on rope.
         */

        TAIL_RECURSE(&WORD_STRBUF_ROPE(*wordPtr), page);

    case WORD_TYPE_CUSTOM: {
        Col_CustomWordType *typeInfo = WORD_TYPEINFO(*wordPtr);

        /*
         * Handle type-specific children.
         */

        switch (typeInfo->type) {
        case COL_HASHMAP:
            if (WORD_HASHMAP_BUCKETS(*wordPtr)) {
                /*
                 * Buckets are stored in a separate word.
                 */

                MarkWord(data, &WORD_HASHMAP_BUCKETS(*wordPtr), page);
            } else {
                /*
                 * Buckets are stored inline.
                 */

                size_t i;
                Col_Word *buckets = WORD_HASHMAP_STATICBUCKETS(*wordPtr);
                for (i = 0; i < HASHMAP_STATICBUCKETS_SIZE; i++) {
                    MarkWord(data, buckets+i, page);
                }
            }
            break;

        case COL_TRIEMAP:
            /*
             * Follow trie root.
             */

            MarkWord(data, &WORD_TRIEMAP_ROOT(*wordPtr), page);
            break;
        }

        if (typeInfo->childrenProc) {
            /*
             * Follow children.
             */

            typeInfo->childrenProc(*wordPtr, MarkWordChild, data);
        }

        /*
         * Tail recurse on synonym.
         */

        TAIL_RECURSE(&WORD_SYNONYM(*wordPtr), page);
        }

    /* WORD_TYPE_UNKNOWN */

    default:
        /* CANTHAPPEN */
        ASSERT(0);
        return;
    }
}

/*---------------------------------------------------------------------------
 * RememberSweepable
 *                                                                              *//*!
 *  Remember custom words needing cleanup upon deletion. Such words are
 *  chained in their order of creation, latest being inserted at the head of
 *  the list. This implies that cleanup can stop traversing the list at the
 *  first custom word that belongs to a non GC'd pool.
 *
 *  @see Col_CustomWordType
 *  @see Col_CustomWordFreeProc
 *  @see Col_NewCustomWord
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

void
RememberSweepable(
    Col_Word word,              /*!< The word to declare. */
    Col_CustomWordType *type)   /*!< The word type. */
{
    ThreadData *data = PlatGetThreadData();
    size_t headerSize;

    ASSERT(PAGE_GENERATION(CELL_PAGE(word)) == 1);
    ASSERT(WORD_TYPE(word) == WORD_TYPE_CUSTOM);
    ASSERT(type->freeProc);
    switch (type->type) {
    case COL_HASHMAP: headerSize = CUSTOMHASHMAP_HEADER_SIZE;
    case COL_TRIEMAP: headerSize = CUSTOMTRIEMAP_HEADER_SIZE;
    default:          headerSize = CUSTOM_HEADER_SIZE;
    }
    WORD_CUSTOM_NEXT(word, type, headerSize) = data->eden.sweepables;
    data->eden.sweepables = word;
}

/*---------------------------------------------------------------------------
 * SweepUnreachableCells
 *                                                                              *//*!
 *  Perform cleanup for all collected custom words that need sweeping.
 *
 *  @sideeffect
 *      Calls each cleaned word's freeProc.
 *
 *  @see Col_CustomWordType
 *  @see Col_CustomWordFreeProc
 *  @see WORD_CUSTOM_NEXT
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

static void
SweepUnreachableCells(
    GroupData *data,    /*!< Group-specific data. */
    MemoryPool *pool)   /*!< The pool to sweep. */
{
    Col_Word word, *previousPtr;
    Col_CustomWordType *type;
    size_t headerSize;

    ASSERT(pool->generation <= data->maxCollectedGeneration);
    for (previousPtr = &pool->sweepables, word = pool->sweepables; word;
            word = *previousPtr) {
        /*
         * @note by construction, freeable words are custom words (i.e.
         * with a type pointer).
         */

#ifdef PROMOTE_COMPACT
        if (pool->generation == data->compactGeneration
                && WORD_TYPE(word) == WORD_TYPE_REDIRECT) {
            /*
             * Word was redirected.
             */

            word = WORD_REDIRECT_SOURCE(word);
        }
#endif
        ASSERT(WORD_TYPE(word) == WORD_TYPE_CUSTOM);
        type = WORD_TYPEINFO(word);
        ASSERT(type->freeProc);
        switch (type->type) {
        case COL_HASHMAP: headerSize = CUSTOMHASHMAP_HEADER_SIZE;
        case COL_TRIEMAP: headerSize = CUSTOMTRIEMAP_HEADER_SIZE;
        default:          headerSize = CUSTOM_HEADER_SIZE;
        }

        if (!TestCell(CELL_PAGE(word), CELL_INDEX(word))) {
            /*
             * Cleanup.
             */

            type->freeProc(word);

            /*
             * Remove from list.
             */

            *previousPtr = WORD_CUSTOM_NEXT(word, type, headerSize);
        } else {
            /*
             * Keep, updating reference in case of redirection.
             */

            *previousPtr = word;

            previousPtr = &WORD_CUSTOM_NEXT(word, type, headerSize);
        }
    }

    if (pool->generation+1 < GC_MAX_GENERATIONS) {
        /*
         * Move still valid freeable words to the next generation: add to front.
         */

        MemoryPool *nextPool = &data->pools[pool->generation-1];
        *previousPtr = nextPool->sweepables;
        nextPool->sweepables = pool->sweepables;
        pool->sweepables = WORD_NIL;
    }
}

/*---------------------------------------------------------------------------
 * CleanupSweepables
 *                                                                              *//*!
 *  Perform cleanup for all custom words that need sweeping. Called during
 *  global cleanup.
 *
 *  @sideeffect
 *      Calls each cleaned word's freeProc.
 *
 *  @see Col_CustomWordType
 *  @see Col_CustomWordFreeProc
 *  @see WORD_CUSTOM_NEXT
 *  @see PoolCleanup
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

void
CleanupSweepables(
    MemoryPool *pool)   /*!< The pool to cleanup. */
{
    Col_Word word;
    Col_CustomWordType *type;
    size_t headerSize;

    /*
     * Perform cleanup on all custom words that need sweeping.
     */

    for (word = pool->sweepables; word;
            word = WORD_CUSTOM_NEXT(word, type, headerSize)) {
        ASSERT(WORD_TYPE(word) == WORD_TYPE_CUSTOM);
        type = WORD_TYPEINFO(word);
        ASSERT(type->freeProc);
        switch (type->type) {
        case COL_HASHMAP: headerSize = CUSTOMHASHMAP_HEADER_SIZE;
        case COL_TRIEMAP: headerSize = CUSTOMTRIEMAP_HEADER_SIZE;
        default:          headerSize = CUSTOM_HEADER_SIZE;
        }
        type->freeProc(word);
    }
}

/*---------------------------------------------------------------------------
 * PromotePages
 *                                                                              *//*!
 *  Promote non-empty pages to the next pool. This simply move the pool's
 *  pages to the target pool.
 *
 *  @private
 *//*-----------------------------------------------------------------------*/

static void
PromotePages(
    GroupData *data,    /*!< Group-specific data. */
    MemoryPool *pool)   /*!< The pool to promote page for. */
{
    Page *page;
    size_t i;
    MemoryPool *nextPool;
    int parent;

    ASSERT(pool->generation <= data->maxCollectedGeneration);
    if (pool->generation+1 >= GC_MAX_GENERATIONS) {
        /*
         * Can't promote past the last possible generation.
         */

        return;
    }

    if (!pool->pages) {
        /*
         * Nothing to promote.
         */

        return;
    }

    /*
     * Update promoted pages' generation, and protect pages without parents.
     */

    parent = 0;
    for (page = pool->pages; page; page = PAGE_NEXT(page)) {
        ASSERT(PAGE_GENERATION(page) == pool->generation);
        PAGE_SET_GENERATION(page, pool->generation+1);
        parent |= PAGE_FLAG(page, PAGE_FLAG_PARENT);
        if (PAGE_FLAG(page, PAGE_FLAG_LAST)) {
            if (!PAGE_NEXT(page)) {
                /*
                 * Stop there. This avoids protecting the last page before
                 * updating its next pointer (see below).
                 */

                ASSERT(page == pool->lastPage);
                break;
            }
            if (!parent) SysPageProtect(page, 1);
            parent = 0;
        }
    }

    /*
     * Move the promoted pages to the head of the target pool's page list.
     * Note that root & parent lists are already updated at this point.
     */

    ASSERT(pool->lastPage);
    nextPool = &data->pools[pool->generation-1];
    PAGE_SET_NEXT(pool->lastPage, nextPool->pages);
    if (!parent) SysPageProtect(pool->lastPage, 1);
    nextPool->pages = pool->pages;
    if (!nextPool->lastPage) {
        nextPool->lastPage = pool->lastPage;
    }
    nextPool->nbPages += pool->nbPages;
    nextPool->nbAlloc += pool->nbPages;
    nextPool->nbSetCells += pool->nbSetCells;
    for (i = 0; i < AVAILABLE_CELLS; i++) {
        nextPool->lastFreeCell[i] = PAGE_CELL(nextPool->pages, 0);
    }

    pool->pages = NULL;
    pool->lastPage = NULL;
    pool->nbPages = 0;
    pool->nbAlloc = 0;
    pool->nbSetCells = 0;
}

/*---------------------------------------------------------------------------
 * ResetPool
 *                                                                              *//*!
 *  Reset GC-related info and fast cell pointers.
 *  
 *  @private
 *//*-----------------------------------------------------------------------*/

static void
ResetPool(
    MemoryPool *pool)   /*!< The pool to reset fields for. */
{
    size_t i;

    pool->nbAlloc = 0;
    pool->gc = 0;
    for (i = 0; i < AVAILABLE_CELLS; i++) {
        pool->lastFreeCell[i] = PAGE_CELL(pool->pages, 0);
    }
}
                                                                                /*!     @} */
                                                                                /*!     @} */
