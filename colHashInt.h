/*
 * Internal Header: colHashInt.h
 *
 *	This header file defines the hash map word internals of Colibri.
 *
 *	Hash maps are an implementation of generic maps that use key hashing 
 *	and flat bucket arrays for string, integer and custom keys.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colHash.c>, <colHash.h>, <colMapInt.h>, <WORD_TYPE_STRHASHMAP>,
 *	<WORD_TYPE_INTHASHMAP>, <WORD_TYPE_HASHENTRY>, <WORD_TYPE_MHASHENTRY>,
 *	<WORD_TYPE_INTHASHENTRY>, <WORD_TYPE_MINTHASHENTRY>
 */

#ifndef _COLIBRI_HASH_INT
#define _COLIBRI_HASH_INT


/*
================================================================================
Internal Section: Hash Maps
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Hash Map Word
 *
 *	Hash maps are an implementation of generic maps that use key hashing 
 *	and flat bucket arrays for string, integer and custom keys.
 *
 *	They are always mutable.
 *
 * Requirements:
 *	Hash maps can use string or integer keys, but also custom keys. In the
 *	latter case they need to point to custom word type information. So 
 *	string and integer keyed hash maps use predefined numeric types, and
 *	custom keyed hash maps use a custom word type.
 *
 *	As hash maps are containers they must know their size, i.e. the number
 *	of entries they contain.
 *
 *	Hash maps store entries in buckets according to their hash value. 
 *	Buckets are flat arrays of entries (themselves forming a linked
 *	list). So hash maps must know the location of their bucket container. 
 *	For small sizes, buckets are stored inline within the word cells. For 
 *	larger sizes we use a mutable <Vector Word> for container.
 *
 *	Hash map words use one cell for the header plus several extra cells for
 *	static buckets (see <HASHMAP_STATICBUCKETS_NBCELLS>).
 *
 * Fields:
 *	Type info	- Generic <Word> Type info field.
 *	Synonym		- Generic <Word> Synonym field.
 *	Size		- Number of elements in map.
 *	Buckets		- Bucket container for larger sizes. If nil, buckets 
 *			  are stored in the inline static bucket array. Else
 *			  they use a mutable vector.
 *	Static buckets	- Buckets for smaller sizes.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0                                                             n
 *     +---------------------------------------------------------------+
 *   0 |                           Type info                           |
 *     +---------------------------------------------------------------+
 *   1 |                            Synonym                            |
 *     +---------------------------------------------------------------+
 *   2 |                             Size                              |
 *     +---------------------------------------------------------------+
 *   3 |                            Buckets                            |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                        Static buckets                         .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPEINFO>, <WORD_SYNONYM>, <Hash Entry Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_HASHMAP_* Accessors
 *
 *	Hash map field accessor macros.
 *
 *  WORD_HASHMAP_SIZE		- Number of elements in map.
 *  WORD_HASHMAP_BUCKETS	- Bucket container.
 *  WORD_HASHMAP_STATICBUCKETS	- Inline bucket array for small sized maps.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Hash Map Word>, <WORD_HASHMAP_INIT>, <WORD_INTHASHMAP_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_HASHMAP_SIZE(word)		(((size_t *)(word))[2])
#define WORD_HASHMAP_BUCKETS(word)	(((Col_Word *)(word))[3])
#define WORD_HASHMAP_STATICBUCKETS(word) ((Col_Word *)((char *)(word)+WORD_HASHMAP_HEADER_SIZE))

/*---------------------------------------------------------------------------
 * Internal Constants: WORD_HASHMAP_* Constants
 *
 *	Hash map size related constants.
 *
 *  WORD_HASHMAP_HEADER_SIZE		- Byte size of hash map header.
 *  HASHMAP_STATICBUCKETS_NBCELLS	- Number of cells allocated for static 
 *					  bucket array.
 *  HASHMAP_STATICBUCKETS_SIZE		- Number of elements in static bucket
 *					  array.
 *  HASHMAP_NBCELLS			- Number of cells taken by hash maps.
 *
 * See also:
 *	<Hash Map Word>
 *---------------------------------------------------------------------------*/

#define WORD_HASHMAP_HEADER_SIZE	(sizeof(Col_CustomWordType*)+sizeof(Col_Word)*2+sizeof(size_t))
#define HASHMAP_STATICBUCKETS_NBCELLS	2
#define HASHMAP_STATICBUCKETS_SIZE	(HASHMAP_STATICBUCKETS_NBCELLS*CELL_SIZE/sizeof(Col_Word))
#define HASHMAP_NBCELLS			(HASHMAP_STATICBUCKETS_NBCELLS+1)

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_HASHMAP_INIT
 *
 *	Generic hash map word initializer.
 *
 *	Generic hash maps are custom word types of type <COL_HASHMAP>.
 *
 * Arguments:
 *	word		- Word to initialize. (Caution: evaluated several times 
 *			  during macro expansion)
 *	typeInfo	- <WORD_SET_TYPEINFO>
 *
 * See also:
 *	<Hash Map Word>, <Col_NewCustomWord>, <Col_CustomHashMapType>,
 *	<COL_HASHMAP>
 *---------------------------------------------------------------------------*/

#define WORD_HASHMAP_INIT(word, typeInfo) \
    WORD_SET_TYPEINFO((word), (typeInfo)); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_STRHASHMAP_INIT
 *
 *	String hash map word initializer.
 *
 *	String hash maps are specialized hash maps using built-in compare and 
 *	hash procs on string keys.
 *
 * Argument:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *
 * See also:
 *	<Hash Map Word>, <WORD_TYPE_STRHASHMAP>, <Col_NewStringHashMap>
 *---------------------------------------------------------------------------*/

#define WORD_STRHASHMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_STRHASHMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTHASHMAP_INIT
 *
 *	Integer hash map word initializer.
 *
 *	Integer hash maps are specialized hash maps where the hash value is the 
 *	randomized integer key. 
 *
 * Argument:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *
 * See also:
 *	<Hash Map Word>, <WORD_TYPE_INTHASHMAP>, <Col_NewIntHashMap>
 *---------------------------------------------------------------------------*/

#define WORD_INTHASHMAP_INIT(word) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTHASHMAP); \
    WORD_SYNONYM(word) = WORD_NIL; \
    WORD_HASHMAP_SIZE(word) = 0; \
    WORD_HASHMAP_BUCKETS(word) = WORD_NIL;


/****************************************************************************
 *
 * Internal Section: Hash Entries
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Data Structure: Hash Entry Word
 *
 *	Hash entries are map entries with hash-specific data.
 *
 *	They are usually mutable but can become immutable as a result of a copy
 *	(see <Col_CopyHashMap>): in this case the copy and the original would
 *	share most of their internals (rendered immutable in the process).
 *	Likewise, mutable operations on the copies would use copy-on-write
 *	semantics: immutable structures are copied to new mutable ones before 
 *	modification.
 *
 * Requirements:
 *	Hash entry words, like generic map entry words, use one single cell.
 *
 *	Hash entries sharing the same bucket are chained using a linked list, so
 *	each entry store must store a pointer to the next entry in chain. 
 *
 *	In addition we cache part of the hash value in the remaining bytes to 
 *	avoid recomputing in many cases.
 *
 * Fields:
 *	Key	- Generic <Map Entry Word> Key field.
 *	Value	- Generic <Map Entry Word> Value field.
 *	Next	- Next entry in bucket. Also used during iteration.
 *	Hash	- Higher order bits of the hash value for fast negative test 
 *		  and rehashing. The full value is computed by combining these 
 *		  high order bits and the bucket index whenever possible, else 
 *		  the value is recomputed from the key.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7 8                                                     n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                         Hash                          |
 *     +-------+-------------------------------------------------------+
 *   1 |                             Next                              |
 *     +---------------------------------------------------------------+
 *   2 |                          Key (Generic)                        |
 *     +---------------------------------------------------------------+
 *   3 |                         Value (Generic)                       |
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<Map Entry Word>, <Hash Map Word>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_HASHENTRY_* Accessors
 *
 *	Hash entry field accessor macros. Both immutable and mutable versions
 *	use these fields.
 *
 *  WORD_HASHENTRY_HASH		- Higher order bits of the hash value.
 *  WORD_HASHENTRY_SET_HASH	- Set hash value. Only retain the high order 
 *				  bits.
 *  WORD_HASHENTRY_NEXT		- Pointer to next entry in bucket.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Hash Entry Word>, <WORD_( MAPENTRY | INTMAPENTRY )_* Accessors>, 
 *	<WORD_HASHENTRY_INIT>, <WORD_MHASHENTRY_INIT>, <WORD_INTHASHENTRY_INIT>,
 *	<WORD_MINTHASHENTRY_INIT>
 *---------------------------------------------------------------------------*/

#define HASHENTRY_HASH_MASK		(UINTPTR_MAX^UCHAR_MAX)
#ifdef COL_BIGENDIAN
#   define WORD_HASHENTRY_HASH(word)	((((uintptr_t *)(word))[0]<<CHAR_BIT)&HASHENTRY_HASH_MASK)
#   define WORD_HASHENTRY_SET_HASH(word, hash) ((((uintptr_t *)(word))[0])&=~(HASHENTRY_HASH_MASK>>CHAR_BIT),(((uintptr_t *)(word))[0])|=(((hash)&HASHENTRY_HASH_MASK)>>CHAR_BIT))
#else
#   define WORD_HASHENTRY_HASH(word)	((((uintptr_t *)(word))[0])&HASHENTRY_HASH_MASK)
#   define WORD_HASHENTRY_SET_HASH(word, hash) ((((uintptr_t *)(word))[0])&=~HASHENTRY_HASH_MASK,(((uintptr_t *)(word))[0])|=((hash)&HASHENTRY_HASH_MASK))
#endif
#define WORD_HASHENTRY_NEXT(word)	(((Col_Word *)(word))[1])

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_HASHENTRY_INIT
 *
 *	Immutable hash entry word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_MAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *	next	- <WORD_HASHENTRY_NEXT>
 *	hash	- <WORD_HASHENTRY_SET_HASH> (Caution: evaluated several times
 *		  during macro expansion)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Hash Entry Word>, <WORD_TYPE_HASHENTRY>
 *---------------------------------------------------------------------------*/

#define WORD_HASHENTRY_INIT(word, key, value, next, hash) \
    WORD_SET_TYPEID((word), WORD_TYPE_HASHENTRY); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next); \
    WORD_HASHENTRY_SET_HASH(word, hash);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MHASHENTRY_INIT
 *
 *	Mutable hash entry word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_MAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *	next	- <WORD_HASHENTRY_NEXT>
 *	hash	- <WORD_HASHENTRY_SET_HASH> (Caution: evaluated several times
 *		  during macro expansion)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Hash Entry Word>, <WORD_TYPE_MHASHENTRY>
 *---------------------------------------------------------------------------*/

#define WORD_MHASHENTRY_INIT(word, key, value, next, hash) \
    WORD_SET_TYPEID((word), WORD_TYPE_MHASHENTRY); \
    WORD_MAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next); \
    WORD_HASHENTRY_SET_HASH(word, hash);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_INTHASHENTRY_INIT
 *
 *	Immutable integer hash entry word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_INTMAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *	next	- <WORD_HASHENTRY_NEXT>
 *
 * See also:
 *	<Hash Entry Word>, <WORD_TYPE_INTHASHENTRY>
 *---------------------------------------------------------------------------*/

#define WORD_INTHASHENTRY_INIT(word, key, value, next) \
    WORD_SET_TYPEID((word), WORD_TYPE_INTHASHENTRY); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next);

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MINTHASHENTRY_INIT
 *
 *	Mutable integer hash entry word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	key	- <WORD_INTMAPENTRY_KEY>
 *	value	- <WORD_MAPENTRY_VALUE>
 *	next	- <WORD_HASHENTRY_NEXT>
 *
 * See also:
 *	<Hash Entry Word>, <WORD_TYPE_MINTHASHENTRY>
 *---------------------------------------------------------------------------*/

#define WORD_MINTHASHENTRY_INIT(word, key, value, next) \
    WORD_SET_TYPEID((word), WORD_TYPE_MINTHASHENTRY); \
    WORD_INTMAPENTRY_KEY(word) = (key); \
    WORD_MAPENTRY_VALUE(word) = (value); \
    WORD_HASHENTRY_NEXT(word) = (next);


/*
================================================================================
Internal Section: Type Checking
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_HASHMAP
 *
 *	Type checking macro for hash maps.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a hash map.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsHashMap>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_HASHMAP(word) \
    if (!(Col_WordType(word) & COL_HASHMAP)) { \
	Col_Error(COL_TYPECHECK, "%x is not a hash map", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_STRHASHMAP
 *
 *	Type checking macro for string hash maps.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a string hash map.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsStringHashMap>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_STRHASHMAP(word) \
    if ((Col_WordType(word) & (COL_STRMAP | COL_HASHMAP)) \
	    != (COL_STRMAP | COL_HASHMAP)) { \
	Col_Error(COL_TYPECHECK, "%x is not a string hash map", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_INTHASHMAP
 *
 *	Type checking macro for integer hash maps.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not an integer hash map.
 *
 * See also:
 *	<Col_Error>, <Col_WordIsIntHashMap>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_INTHASHMAP(word) \
    if ((Col_WordType(word) & (COL_INTMAP | COL_HASHMAP)) \
	    != (COL_INTMAP | COL_HASHMAP)) { \
	Col_Error(COL_TYPECHECK, "%x is not an integer hash map", (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

#endif /* _COLIBRI_HASH_INT */
