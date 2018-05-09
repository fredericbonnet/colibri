/*                                                                              *//*!   @cond PRIVATE @file \
 * colInternal.h
 *
 *  This header file defines internal structures and macros.
 *
 *  @private
 */

#ifndef _COLIBRI_INTERNAL
#define _COLIBRI_INTERNAL

#include "colConf.h"

/*
================================================================================*//*!   @addtogroup strings \
Strings                                                                         *//*!   @{ *//*
================================================================================
*/

/*---------------------------------------------------------------------------   *//*!   @def \
 * CHAR_WIDTH
 *
 *  Get character width in buffer from format policy (numeric values are
 *  chosen so that the lower 3 bits give the character width).
 *
 *  @param format    Format policy.
 *
 *  @result
 *      Character width in buffer for given string format.
 *
 *  @see Col_StringFormat
 *//*-----------------------------------------------------------------------*/

#define CHAR_WIDTH(format) \
    ((format) & 0x7)

/*---------------------------------------------------------------------------   *//*!   @def \
 * FORMAT_UTF
 *
 *  Whether format uses variable-width UTF format.
 *
 *  @param format    Format policy
 *
 *  @retval nonzero     for UTF formats.
 *  @retval zero        for non-UTF formats.
 *
 *  @see Col_StringFormat
 *//*-----------------------------------------------------------------------*/

#define FORMAT_UTF(format) \
    ((format) & 0x10)
                                                                                /*!     @} */

/*
================================================================================*//*!   @addtogroup error \
Error Handling & Debugging                                                      *//*!   @{ *//*
================================================================================
*/

/*---------------------------------------------------------------------------   *//*!   @def \
 * ASSERT
 *
 *  Check condition at runtime in debug mode. If failed, generate a fatal
 *  error.
 *
 *  In non-debug mode this macro does nothing.
 *
 *  @param condition     Boolean condition.
 *
 *  @sideeffect
 *      **condition** is evaluated in debug mode only.
 *
 *  @see COL_DEBUGCHECK
 *  @see REQUIRE
 *//*-----------------------------------------------------------------------*/

#define ASSERT(condition) \
    COL_DEBUGCHECK(condition, COL_FATAL, ColibriDomain, COL_ERROR_ASSERTION, __FILE__, __LINE__, #condition);

/*---------------------------------------------------------------------------   *//*!   @def \
 * REQUIRE
 *
 *  In debug mode, same as #ASSERT.
 *
 *  In non-debug mode, this simply evaluates the given condition.
 *
 *  @param condition     Boolean condition.
 *
 *  @sideeffect
 *      **condition** is always evaluated.
 *
 *  @see ASSERT
 *//*-----------------------------------------------------------------------*/

#ifdef _DEBUG
#   define REQUIRE ASSERT
#else
#   define REQUIRE(condition) (condition);
#endif

/*---------------------------------------------------------------------------   *//*!   @def \
 * TYPECHECK
 *
 *  Debug-mode runtime type checking.
 *
 *  @param condition     Boolean condition
 *  @param code          Error code
 *  @param ...           Remaining arguments passed to #COL_DEBUGCHECK
 *
 *  @see COL_DEBUGCHECK
 *  @see COL_TYPECHECK
 *//*-----------------------------------------------------------------------*/

#define TYPECHECK(condition, code, ...) \
    COL_DEBUGCHECK(condition, COL_TYPECHECK, ColibriDomain, code, ## __VA_ARGS__)

/*---------------------------------------------------------------------------   *//*!   @def \
 * VALUECHECK
 *
 *  Runtime value checking. Works in both debug and non-debug mode.
 *
 *  @param condition     Boolean condition.
 *  @param code          Error code.
 *  @param ...           Remaining arguments passed to #COL_DEBUGCHECK.
 *
 *  @see COL_RUNTIMECHECK
 *  @see COL_VALUECHECK
 *//*-----------------------------------------------------------------------*/

#define VALUECHECK(condition, code, ...) \
    COL_RUNTIMECHECK(condition, COL_VALUECHECK, ColibriDomain, code, ## __VA_ARGS__)

/*---------------------------------------------------------------------------   *//*!   @def \
 * TRACE
 *
 *  Output traces to stderr if macro **DO_TRACE** is defined.
 *
 *  @param format   **fprintf** format string.
 *  @param ...      Remaining arguments passed to **fprintf**.
 *//*-----------------------------------------------------------------------*/

#ifdef DO_TRACE
#   include <stdio.h>
#   define TRACE(format, ...) (fprintf(stderr, format, __VA_ARGS__), fflush(stderr))
#else
#   define TRACE(format, ...)
#endif

/*
 * Remaining declarations.
 */
                                                                                #       ifndef DOXYGEN
extern const char * const ColibriDomain[];
                                                                                #       endif /* DOXYGEN */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup pages_cells \
Pages and Cells
                                                                                        @ingroup alloc
  Pages are divided into cells. On a 32-bit system with 1024-byte logical
  pages, each page stores 64 16-byte cells. On a 64-bit system with
  4096-byte logical pages, each page stores 128 32-byte cells.

  Each page has reserved cells that store information about the page. The
  remaining cells store word info.

  @par First Cell Layout
    On all architectures, the first cell is reserved and is formatted as
    follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        first_cell [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="20" align="left">0</td><td sides="B" width="20" align="right">3</td>
                <td sides="B" width="20" align="left">4</td><td sides="B" width="20" align="right">7</td>
                <td sides="B" width="120" align="left">8</td><td sides="B" width="120" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref PAGE_GENERATION" title="PAGE_GENERATION" colspan="2">Gen</td>
                <td href="@ref PAGE_FLAGS" title="PAGE_FLAGS" colspan="2">Flags</td>
                <td href="@ref PAGE_NEXT" title="PAGE_NEXT" colspan="2">Next</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref PAGE_GROUPDATA" title="PAGE_GROUPDATA" colspan="6">Group Data</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref PAGE_BITMASK" title="PAGE_BITMASK" colspan="6" rowspan="2">Bitmask</td>
            </tr>
            <tr><td sides="R">3</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0     3 4     7 8                                             n
          +-------+-------+-----------------------------------------------+
        0 |  Gen  | Flags |                     Next                      |
          +-------+-------+-----------------------------------------------+
        1 |                           Group Data                          |
          +---------------------------------------------------------------+
        2 |                                                               |
          +                            Bitmask                            +
        3 |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
                                                                                *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Page and Cell Types & Constants                                              *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * Page
 *                                                                              *//*!
 *  Page-sized byte array.
 *
 *  @see PAGE_SIZE
 *//*-----------------------------------------------------------------------*/

typedef char Page[PAGE_SIZE];

/*---------------------------------------------------------------------------
 * Cell
 *                                                                              *//*!
 *  Cell-sized byte array.
 *
 *  @see CELL_SIZE
 *//*-----------------------------------------------------------------------*/

typedef char Cell[CELL_SIZE];

/*---------------------------------------------------------------------------   *//*!   @def \
 * RESERVED_CELLS
 *  Number of reserved cells in page.
 *
 *  @see AVAILABLE_CELLS
 *//*-----------------------------------------------------------------------*/

#define RESERVED_CELLS      1

/*---------------------------------------------------------------------------   *//*!   @def \
 * AVAILABLE_CELLS
 *  Number of available cells in page, i.e.\ number of cells in page minus
 *  number of reserved cells.
 *
 *  @see CELLS_PER_PAGE
 *  @see RESERVED_CELLS
 *//*-----------------------------------------------------------------------*/

#define AVAILABLE_CELLS     (CELLS_PER_PAGE-RESERVED_CELLS)

/*---------------------------------------------------------------------------   *//*!   @def \
 * NB_CELLS
 *  Number of cells needed to store a given number of bytes.
 *
 *  @param size  - Number of raw bytes to store.
 *
 *  @see CELL_SIZE
 *//*-----------------------------------------------------------------------*/

#define NB_CELLS(size) \
    (((size)+CELL_SIZE-1)/CELL_SIZE)

/*---------------------------------------------------------------------------
 * Page and Cell Flags
 *                                                                              *//*!   @def \
 * PAGE_FLAG_FIRST
 *  Marks first page in group.
 *
 *  @see PAGE_FLAG
 *  @see PAGE_SET_FLAG
 *  @see PAGE_CLEAR_FLAG
 *
 *                                                                              *//*!   @def \
 * PAGE_FLAG_LAST
 *  Marks last page in group.
 *
 *  @see PAGE_FLAG
 *  @see PAGE_SET_FLAG
 *  @see PAGE_CLEAR_FLAG
 *
 *                                                                              *//*!   @def \
 * PAGE_FLAG_PARENT
 *  Marks pages having parent cells.
 *
 *  @see PAGE_FLAG
 *  @see PAGE_SET_FLAG
 *  @see PAGE_CLEAR_FLAG
 *//*-----------------------------------------------------------------------*/

#define PAGE_FLAG_FIRST         0x10
#define PAGE_FLAG_LAST          0x20
#define PAGE_FLAG_PARENT        0x40
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Page and Cell Accessors                                                      *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * PAGE_NEXT
 *  Get next page in pool.
 *
 *  @param page     #Page to access.
 *
 *  @return
 *      Next page in pool.
 *
 *  @see PAGE_SET_NEXT
 *
 *                                                                              *//*!   @def \
 * PAGE_SET_NEXT
 *  Set next page in pool.
 *
 *  @param page     #Page to access.
 *  @param next     Next #Page to link to.
 *
 *  @see PAGE_NEXT
 *
 *                                                                              *//*!   @def \
 * PAGE_GENERATION
 *  Get generation of pool containing this page.
 *
 *  @param page     #Page to access.
 *
 *  @return
 *      The page generation.
 *
 *  @see PAGE_SET_GENERATION
 *
 *                                                                              *//*!   @def \
 * PAGE_SET_GENERATION
 *  Set generation of pool containing this page.
 *
 *  @param page     #Page to access.
 *  @param gen      Generation.
 *
 *  @see PAGE_GENERATION
 *
 *                                                                              *//*!   @def \
 * PAGE_FLAGS
 *  Get page flags.
 *
 *  @param page     #Page to access.
 *
 *  @see PAGE_FLAG
 *  @see PAGE_SET_FLAG
 *  @see PAGE_CLEAR_FLAG
 *
 *                                                                              *//*!   @def \
 * PAGE_FLAG
 *  Get page flag.
 *
 *  @param page     #Page to access.
 *  @param flag     Flag to get.
 *                                                                              *//*!   @def \
 * PAGE_SET_FLAG
 *  Set page flag.
 *
 *  @param page     #Page to access.
 *  @param flag     Flag to set.
 *
 *  @see PAGE_FLAG
 *
 *                                                                              *//*!   @def \
 * PAGE_CLEAR_FLAG
 *  Clear page flag.
 *
 *  @param page     #Page to access.
 *  @param flag     Flag to clear.
 *
 *  @see PAGE_FLAG
 *  @see PAGE_SET_FLAG
 *
 *                                                                              *//*!   @def \
 * PAGE_GROUPDATA
 *  Get/set data for the thread group the page belongs to.
 *
 *  @param page     #Page to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *                                                                              *//*!   @def \
 * PAGE_BITMASK
 *  Get/set bitmask for allocated cells in page.
 *
 *  @param page     #Page to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *//*-----------------------------------------------------------------------*/
                                                                                #       ifndef DOXYGEN
#define PAGE_NEXT_MASK                  (~(PAGE_GENERATION_MASK|PAGE_FLAGS_MASK))
#define PAGE_GENERATION_MASK            0x0F
#define PAGE_FLAGS_MASK                 0xF0
                                                                                #       endif /* DOXYGEN */
#define PAGE_NEXT(page)                 ((Page *)((*(uintptr_t *)(page)) & PAGE_NEXT_MASK))
#define PAGE_SET_NEXT(page, next)       (*(uintptr_t *)(page) &= ~PAGE_NEXT_MASK, *(uintptr_t *)(page) |= ((uintptr_t)(next)) & PAGE_NEXT_MASK)
#define PAGE_GENERATION(page)           ((*(uintptr_t *)(page)) & PAGE_GENERATION_MASK)
#define PAGE_SET_GENERATION(page, gen)  (*(uintptr_t *)(page) &= ~PAGE_GENERATION_MASK, *(uintptr_t *)(page) |= (gen) & PAGE_GENERATION_MASK)
#define PAGE_FLAGS(page)                ((*(uintptr_t *)(page)) & PAGE_FLAGS_MASK)
#define PAGE_FLAG(page, flag)           ((*(uintptr_t *)(page)) & (flag))
#define PAGE_SET_FLAG(page, flag)       ((*(uintptr_t *)(page)) |= (flag))
#define PAGE_CLEAR_FLAG(page, flag)     ((*(uintptr_t *)(page)) &= ~(flag))
#define PAGE_GROUPDATA(page)            (*((GroupData **)(page)+1))
#define PAGE_BITMASK(page)              ((uint8_t *)(page)+sizeof(Page *)*2)

/*---------------------------------------------------------------------------   *//*!   @def \
 * PAGE_CELL
 *  Get **index**-th cell in page.
 *
 *  @param page     #Page containing cell.
 *  @param index    Index of cell in **page**.
 *
 *  @return
 *      **index**-th #Cell in page.
 *
 *  @see Cell
 *//*-----------------------------------------------------------------------*/

#define PAGE_CELL(page, index) \
    ((Cell *)(page)+(index))

/*---------------------------------------------------------------------------   *//*!   @def \
 * CELL_PAGE
 *  Get page containing the cell.
 *
 *  @param cell  - Cell to get page for.
 *
 *  @return
 *      #Page containing **cell**.
 *
 *  @see Page
 *  @see PAGE_SIZE
 *//*-----------------------------------------------------------------------*/

#define CELL_PAGE(cell) \
    ((Page *)((uintptr_t)(cell) & ~(PAGE_SIZE-1)))

/*---------------------------------------------------------------------------   *//*!   @def \
 * CELL_INDEX
 *  Get index of cell in page.
 *
 *  @param cell  - Cell to get index for.
 *
 *  @return
 *      Index of **cell** in its containing page.
 *
 *  @see PAGE_SIZE
 *  @see CELL_SIZE
 *//*-----------------------------------------------------------------------*/

#define CELL_INDEX(cell) \
    (((uintptr_t)(cell) % PAGE_SIZE) / CELL_SIZE)
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup alloc \
Memory Allocation                                                               *//*!   @{ *//*
================================================================================*//*!   @} *//*FIXME: mismatched markup somewhere???
*/

/********************************************************************************//*!   @name \
 * Memory Pools                                                                 *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * MemoryPool
 *                                                                              *//*!
 *  Memory pools are a set of pages storing the cells of a given generation.
 *
 *  @see Page
 *  @see Cell
 *//*-----------------------------------------------------------------------*/

typedef struct MemoryPool {
    unsigned int generation;            /*!< Generation level; 0 = younger,
                                             1 = eden, 2+ = older. */
    Page *pages;                        /*!< Pages form a singly-linked list. */
    Page *lastPage;                     /*!< Last page in pool. */
    Cell *lastFreeCell[AVAILABLE_CELLS];/*!< Last cell sequence of a given size
                                             found. */
    size_t nbPages;                     /*!< Number of pages in pool. */
    size_t nbAlloc;                     /*!< Number of pages alloc'd since last
                                             GC. */
    size_t nbSetCells;                  /*!< Number of set cells in pool. */
    size_t gc;                          /*!< GC counter. Used for generational
                                             GC. */
    Col_Word sweepables;                /*!< List of cells that need sweeping
                                             when unreachable after a GC. */
} MemoryPool;

/*
 * Remaining declarations.
 */
                                                                                #       ifndef DOXYGEN
void                    PoolInit(MemoryPool *pool, unsigned int generation);
void                    PoolCleanup(MemoryPool *pool);
                                                                                #       endif /* DOXYGEN */
                                                                                /*!     @} */
/********************************************************************************//*!   @name \
 * System Page Allocation                                                       *//*!   @{ *//*
 ******************************************************************************/
                                                                                #       ifndef DOXYGEN
void                    SysPageProtect(void *page, int protect);
                                                                                #       endif /* DOXYGEN */
                                                                                /*!     @} */
/********************************************************************************//*!   @name \
 * Page Allocation                                                              *//*!   @{ *//*
 ******************************************************************************/
                                                                                #       ifndef DOXYGEN
void                    PoolAllocPages(MemoryPool *pool, size_t number);
void                    PoolFreeEmptyPages(MemoryPool *pool);
                                                                                #       endif /* DOXYGEN */
                                                                                /*!     @} */
/********************************************************************************//*!   @name \
 * Cell Allocation                                                              *//*!   @{ *//*
 ******************************************************************************/
                                                                                #       ifndef DOXYGEN
Cell *                  PoolAllocCells(MemoryPool *pool, size_t number);
void                    SetCells(Page *page, size_t first, size_t number);
void                    ClearCells(Page *page, size_t first, size_t number);
void                    ClearAllCells(Page *page);
int                     TestCell(Page *page, size_t index);
size_t                  NbSetCells(Page *page);
Cell *                  AllocCells(size_t number);
                                                                                #       endif /* DOXYGEN */
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup gc \
Garbage Collection                                                              *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Process & Threads                                                            *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------
 * GroupData
 *                                                                              *//*!
 *  Group-local data.
 *
 *  @see @ref threading_models "Threading Model Constants"
 *  @see MemoryPool
 *  @see ThreadData
 *  @see PlatEnter
 *  @see PlatLeave
 *  @see AllocGroupData
 *  @see FreeGroupData
 *//*-----------------------------------------------------------------------*/

typedef struct GroupData {
    unsigned int model;             /*!< Threading model. */
    MemoryPool rootPool;            /*!< Memory pool used to store root and
                                         parent page descriptor cells. */
    Cell *roots;                    /*!< Root descriptors are stored in a trie
                                         indexed by the root cell address. */
    Cell *parents;                  /*!< Parent descriptors are stored in a
                                         linked list. */
    MemoryPool
        pools[GC_MAX_GENERATIONS-2];/*!< Memory pools used to store words for
                                         generations older than eden (1 <
                                         generation < #GC_MAX_GENERATIONS).*/
    unsigned int
        maxCollectedGeneration;     /*!< Oldest collected generation during
                                         current GC. */
#ifdef PROMOTE_COMPACT
    unsigned int compactGeneration; /*!< Generation on which to perform
                                         compaction during promotion (see
                                         #PROMOTE_COMPACT). */
#endif
    struct ThreadData *first;       /*!< Group member threads form a circular
                                         list. */
} GroupData;

/*---------------------------------------------------------------------------
 * ThreadData
 *                                                                              *//*!
 *  Thread-local data.
 *
 *  @see @ref threading_models "Threading Model Constants"
 *  @see MemoryPool
 *  @see GroupData
 *  @see PlatEnter
 *  @see PlatLeave
 *//*-----------------------------------------------------------------------*/

typedef struct ThreadData {
    struct ThreadData *next;    /*!< Next in thread group member circular
                                     list. */
    GroupData *groupData;       /*!< Group-local data. */
    size_t nestCount;           /*!< Number of nested calls to Col_Init(). Clear
                                     structures once it drops to zero. */
    Col_ErrorProc *errorProc;   /*!< Error procs are thread-local. */
    size_t pauseGC;             /*!< GC-protected section counter, i.e.\ nb of
                                     nested pause calls. When positive, we are
                                     in a GC-protected section. */
    MemoryPool eden;            /*!< Eden, i.e.\ memory pool for generation 1.
                                     New cells are always created in
                                     thread-local eden pools for minimum
                                     contention. */
} ThreadData;

/*
 * Remaining declarations.
 */
                                                                                #       ifndef DOXYGEN
void                    GcInitThread(ThreadData *data);
void                    GcInitGroup(GroupData *data);
void                    GcCleanupThread(ThreadData *data);
void                    GcCleanupGroup(GroupData *data);
                                                                                #       endif /* DOXYGEN */
                                                                                /*!     @} */
/********************************************************************************//*!   @name \
 * Mark & Sweep Algorithm                                                       *//*!   @{ *//*
 ******************************************************************************/
                                                                                #       ifndef DOXYGEN
void                    PerformGC(GroupData *data);
void                    RememberSweepable(Col_Word word,
                            Col_CustomWordType *type);
void                    CleanupSweepables(MemoryPool *pool);
                                                                                #       endif /* DOXYGEN */
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup gc_roots \
Roots
                                                                                        @ingroup gc
  Roots are explicitly preserved words, using a reference count.

  Roots are stored in a trie indexed by the root source addresses. This
  forms an ordered collection that is traversed upon GC and is indexable
  and modifiable in O(log(k)) worst-case, k being the bit size of a cell
  pointer.

  There are two kinds of cells that form the root trie: nodes and leaves.
  Nodes are intermediate cells pointing to two children. Leaves are
  terminal cells pointing to the preserved word.

  @par Common Requirements
  - Root trie cells use one single cell.

  - Root trie cells must know their parent cell for bottom-up traversal.

  - There must be a way to tell nodes and leaves apart.

  @param Parent     Parent node in trie. NULL for trie root.


  ### Root Trie Nodes

  @par Requirements
  - Root trie nodes must know their critical bit, which is the highest bit where
    left and right subtrie keys differ. They store this information as a bitmask
    with this single bit set.

  - Root trie nodes must know their left and right subtries.

  @param Mask       Bitmask where only the critical bit is set.
  @param Left       Left subtrie. Keys have their critical bit cleared.
  @param Right      Right subtrie. Keys have their critical bit set.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        root_node [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="160" align="left">0</td><td sides="B" width="160" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref ROOT_NODE_MASK" title="ROOT_NODE_MASK" colspan="2" width="320">Mask</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref ROOT_PARENT" title="ROOT_PARENT" colspan="2">Parent</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref ROOT_NODE_LEFT" title="ROOT_NODE_LEFT" colspan="2">Left</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref ROOT_NODE_RIGHT" title="ROOT_NODE_RIGHT" colspan="4">Right</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0                                                             n
          +---------------------------------------------------------------+
        0 |                             Mask                              |
          +---------------------------------------------------------------+
        1 |                            Parent                             |
          +---------------------------------------------------------------+
        2 |                             Left                              |
          +---------------------------------------------------------------+
        3 |                             Right                             |
          +---------------------------------------------------------------+
                                                                                        @endif

  ### Root Trie Leaves

  @par Requirements
  - Root trie leaves must know the root's source word.

  - Root trie leaves must know the root's reference count. Once it drops to zero
    the root is removed, however the source may survive if it is referenced
    elsewhere.

  @param Generation     Generation of the source page. Storing it here saves a
                        pointer dereference.
  @param Refcount       Reference count.
  @param Source         Preserved word.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        root_leaf [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="160" align="left">0</td><td sides="B" width="160" align="right">31</td>
                <td sides="B" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref ROOT_LEAF_GENERATION" title="ROOT_LEAF_GENERATION" colspan="2">Generation</td>
                <td bgcolor="grey75"> Unused (n &gt; 32) </td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref ROOT_PARENT" title="ROOT_PARENT" colspan="3">Parent</td>
            </tr>
            <tr><td sides="R">2</td>
                <td href="@ref ROOT_LEAF_REFCOUNT" title="ROOT_LEAF_REFCOUNT" colspan="3">Refcount</td>
            </tr>
            <tr><td sides="R">3</td>
                <td href="@ref ROOT_LEAF_SOURCE" title="ROOT_LEAF_SOURCE" colspan="3">Source</td>
            </tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0                            31                               n
          +-------------------------------+-------------------------------+
        0 |          Generation           |        Unused (n > 32)        |
          +-------------------------------+-------------------------------+
        1 |                            Parent                             |
          +---------------------------------------------------------------+
        2 |                       Reference count                         |
          +---------------------------------------------------------------+
        3 |                            Source                             |
          +---------------------------------------------------------------+
                                                                                        @endif

  @see Col_WordPreserve
  @see Col_WordRelease                                                          *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Root Trie Creation                                                           *//*!   @{ *//*
 ******************************************************************************/

 /*---------------------------------------------------------------------------   *//*!   @def \
 * ROOT_NODE_INIT
 *
 *  Root trie node initializer.
 *
 *  @param cell     Cell to initialize.
 *  @param parent   #ROOT_PARENT.
 *  @param mask     #ROOT_NODE_MASK.
 *  @param left     #ROOT_NODE_LEFT.
 *  @param right    #ROOT_NODE_RIGHT.
 *
 *  @warning
 *      Argument **cell** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *//*-----------------------------------------------------------------------*/

#define ROOT_NODE_INIT(cell, parent, mask, left, right) \
    ROOT_PARENT(cell) = parent; \
    ROOT_NODE_MASK(cell) = mask; \
    ROOT_NODE_LEFT(cell) = left; \
    ROOT_NODE_RIGHT(cell) = right;

/*---------------------------------------------------------------------------   *//*!   @def \
 * ROOT_LEAF_INIT
 *
 *  Root trie leaf initializer.
 *
 *  @param cell         Cell to initialize.
 *  @param parent       #ROOT_PARENT.
 *  @param generation   #ROOT_LEAF_GENERATION.
 *  @param refcount     #ROOT_LEAF_REFCOUNT.
 *  @param source       #ROOT_LEAF_SOURCE.
 *
 *  @warning
 *      Argument **cell** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *//*-----------------------------------------------------------------------*/

#define ROOT_LEAF_INIT(cell, parent, generation, refcount, source) \
    ROOT_PARENT(cell) = parent; \
    ROOT_LEAF_GENERATION(cell) = generation; \
    ROOT_LEAF_REFCOUNT(cell) = refcount; \
    ROOT_LEAF_SOURCE(cell) = source;
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Root Trie Accessors                                                          *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * ROOT_IS_LEAF
 *  Whether pointed cell is a root trie node or leaf. To distinguish between
 *  both types the least significant bit of leaf pointers is set.
 *
 *  @param cell     Cell to access.
 *
 *  @see ROOT_GET_NODE
 *  @see ROOT_GET_LEAF
 *
 *                                                                              *//*!   @def \
 * ROOT_GET_NODE
 *  Get actual pointer to node.
 *
 *  @param cell     Cell to access.
 *
 *  @see ROOT_IS_LEAF
 *
 *                                                                              *//*!   @def \
 * ROOT_GET_LEAF
 *  Get actual pointer to leaf.
 *
 *  @param cell     Cell to access.
 *
 *  @see ROOT_IS_LEAF
 *
 *                                                                              *//*!   @def \
 * ROOT_PARENT
 *  Get/set parent node in trie.
 *
 *  @param cell     Cell to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see ROOT_NODE_INIT
 *  @see ROOT_LEAF_INIT
 *
 *                                                                              *//*!   @def \
 * ROOT_NODE_MASK
 *  Get/set bitmask where only the critical bit is set, i.e.\ the highest bit
 *  where left and right subtries differ.
 *
 *  @param cell     Cell to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see ROOT_NODE_INIT
 *
 *                                                                              *//*!   @def \
 * ROOT_NODE_LEFT
 *  Get/set left subtrie.
 *
 *  @param cell     Cell to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see ROOT_NODE_INIT
 *
 *                                                                              *//*!   @def \
 * ROOT_NODE_RIGHT
 *  Get/set right subtrie.
 *
 *  @param cell     Cell to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see ROOT_NODE_INIT
 *
 *                                                                              *//*!   @def \
 * ROOT_LEAF_GENERATION
 *  Get/set generation of the source page. Storing it here saves a pointer
 *  dereference.
 *
 *  @param cell     Cell to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see ROOT_LEAF_INIT
 *
 *                                                                              *//*!   @def \
 * ROOT_LEAF_REFCOUNT
 *  Get/set the root's reference count.
 *
 *  @param cell     Cell to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see ROOT_LEAF_INIT
 *
 *                                                                              *//*!   @def \
 * ROOT_LEAF_SOURCE
 *  Get/set the root's preserved word.
 *
 *  @param cell     Cell to access.
 *
 *  @note
 *      Macro is L-Value and suitable for both read/write operations.
 *
 *  @see ROOT_LEAF_INIT
 *//*-----------------------------------------------------------------------*/

#define ROOT_IS_LEAF(cell)      ((uintptr_t)(cell) & 1)
#define ROOT_GET_NODE(cell)     ((Cell *)((uintptr_t)(cell) & ~1))
#define ROOT_GET_LEAF(cell)     ((Cell *)((uintptr_t)(cell) | 1))

#define ROOT_PARENT(cell)       (((Cell **)(cell))[1])

#define ROOT_NODE_MASK(cell)        (((uintptr_t *)(cell))[0])
#define ROOT_NODE_LEFT(cell)        (((Cell **)(cell))[2])
#define ROOT_NODE_RIGHT(cell)       (((Cell **)(cell))[3])

#define ROOT_LEAF_GENERATION(cell)  (((unsigned int *)(cell))[0])
#define ROOT_LEAF_REFCOUNT(cell)    (((size_t *)(cell))[2])
#define ROOT_LEAF_SOURCE(cell)      (((Col_Word *)(cell))[3])
                                                                                /*!     @} */
                                                                                /*!     @} */
/*
================================================================================*//*!   @addtogroup gc_parents \
Parents
                                                                                        @ingroup gc
  Parents are cells pointing to other cells of a newer generation. During
  GC, parents from uncollected generations are traversed in addition to
  roots from collected generations, and the parent list is updated.

  When a page of an older generation is written, this means that it may
  contain parents. Such "dirty" pages are added to the parent list at each
  GC, and each page is checked for potential parents.

  @par Requirements
  - Parent cells use one single cell.

  - Parent cells are linked together for traversal.

  - Parent cells don't point to parent words directly but instead point to the
    parent's page.

  @param Next   Parent nodes are linked together in a singly-linked list for
                traversal during GC.
  @param Page   Pointer to source page.

  @par Cell Layout
    On all architectures the single-cell layout is as follows:

    @dot
    digraph {
        node [fontname="Lucida Console,Courier" fontsize=14];
        parent_cell [shape=none, label=<
            <table border="0" cellborder="1" cellspacing="0">
            <tr><td border="0"></td>
                <td sides="B" width="160" align="left">0</td><td sides="B" width="160" align="right">n</td>
            </tr>
            <tr><td sides="R">0</td>
                <td href="@ref PARENT_NEXT" title="PARENT_NEXT" colspan="2">Next</td>
            </tr>
            <tr><td sides="R">1</td>
                <td href="@ref PARENT_PAGE" title="PARENT_PAGE" colspan="2">Page</td>
            </tr>
            <tr><td sides="R">2</td>
                <td colspan="2" rowspan="2" bgcolor="grey75">Unused</td>
            </tr>
            <tr><td sides="R">3</td></tr>
            </table>
        >]
    }
    @enddot
                                                                                        @if IGNORE
           0                                                             n
          +---------------------------------------------------------------+
        0 |                             Next                              |
          +---------------------------------------------------------------+
        1 |                             Page                              |
          +---------------------------------------------------------------+
        2 |                                                               |
          +                            Unused                             +
        3 |                                                               |
          +---------------------------------------------------------------+
                                                                                        @endif
                                                                                *//*!   @{ *//*
================================================================================
*/

/********************************************************************************//*!   @name \
 * Parent Cell Creation                                                         *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * PARENT_INIT
 *
 *  Parent cell initializer.
 *
 *  @param cell     Cell to initialize.
 *  @param next     #PARENT_NEXT.
 *  @param page     #PARENT_PAGE.
 *
 *  @warning
 *      Argument **cell** is referenced several times by the macro. Make sure to
 *      avoid any side effect.
 *
 *  @see MarkReachableCellsFromParents
 *//*-----------------------------------------------------------------------*/

#define PARENT_INIT(cell, next, page) \
    PARENT_NEXT(cell) = next; \
    PARENT_PAGE(cell) = page;
                                                                                /*!     @} */

/********************************************************************************//*!   @name \
 * Parent Cell Accessors                                                        *//*!   @{ *//*
 ******************************************************************************/

/*---------------------------------------------------------------------------   *//*!   @def \
 * PARENT_NEXT
 *  Parent nodes are linked together in a singly-linked list for traversal
 *  during GC.
 *
 *  @param cell     Cell to access.
 *
 *  @see PARENT_INIT
 *
 *                                                                              *//*!   @def \
 * PARENT_PAGE
 *  Pointer to source page.
 *
 *  @param cell     Cell to access.
 *
 *  @see PARENT_INIT
 *//*-----------------------------------------------------------------------*/

#define PARENT_NEXT(cell)   (((Cell **)(cell))[1])
#define PARENT_PAGE(cell)   (((Page **)(cell))[2])
                                                                                /*!     @} */

/*
 * Remaining declarations.
 */

void                    UpdateParents(GroupData *data);
                                                                                /*!     @} */

#endif /* _COLIBRI_INTERNAL */
                                                                                /*!     @endcond */