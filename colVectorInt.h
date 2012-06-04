/*
 * Internal Header: colVectorInt.h
 *
 *	This header file defines the vector word internals of Colibri.
 *
 *	Vectors are arrays of words that are directly accessible through a
 *	pointer value. 
 *
 *	They come in both immutable and mutable forms :
 *
 *	- Immutable vectors are flat arrays of fixed length.
 *
 *	- Mutable vectors are flat arrays that can grow up to a maximum length, 
 *	whose content is directly modifiable through a C pointer. They can be
 *	"frozen" and turned into immutable versions.
 *
 * See also:
 *	<colVector.c>, <colVector.h>, <WORD_TYPE_VECTOR>, <WORD_TYPE_MVECTOR>
 */

#ifndef _COLIBRI_VECTOR_INT
#define _COLIBRI_VECTOR_INT


/*
================================================================================
Internal Section: Immutable Vectors
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Immutable Vector Word
 *
 *	Immutable vectors are constant arrays of words that are directly 
 *	accessible through a pointer value. 
 *
 * Requirements:
 *	Vector words must know their length, i.e. the number of elements they
 *	contain.
 *
 *	Elements are stored within the word cells following the header.
 *
 *	Vector words use as much cells as needed to store their elements.
 *
 * Fields:
 *	Length		- Size of element array.
 *	Elements	- Array of element words.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                        Unused                         |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Length                             |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                           Elements                            .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<WORD_TYPEINFO>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_VECTOR_* Accessors
 *
 *	Vector word field accessor macros. Both immutable and mutable versions
 *	use these fields.
 *
 *  WORD_VECTOR_LENGTH		- Size of element array.
 *  WORD_VECTOR_ELEMENTS	- Array of words.
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Immutable Vector Word>, <WORD_VECTOR_INIT>, <WORD_MVECTOR_INIT>
 *---------------------------------------------------------------------------*/

#define WORD_VECTOR_LENGTH(word)	(((size_t *)(word))[1])
#define WORD_VECTOR_ELEMENTS(word)	((Col_Word *)(word)+2)

/*---------------------------------------------------------------------------
 * Internal Constant: VECTOR_HEADER_SIZE
 *
 *	Byte size of vector header.
 *
 * See also:
 *	<Immutable Vector Word>
 *---------------------------------------------------------------------------*/

#define VECTOR_HEADER_SIZE		(sizeof(size_t)*2)

/*---------------------------------------------------------------------------
 * Internal Macro: VECTOR_SIZE
 *
 *	Get number of cells for a vector of a given length.
 *
 * Argument:
 *	length	- Vector length.
 *
 * Result:
 *	Number of cells taken by word.
 *
 * See also:
 *	<Immutable Vector Word>
 *---------------------------------------------------------------------------*/

#define VECTOR_SIZE(length) \
    (NB_CELLS(VECTOR_HEADER_SIZE+(length)*sizeof(Col_Word)))

/*---------------------------------------------------------------------------
 * Internal Macro: VECTOR_MAX_LENGTH
 *
 *	Get maximum vector length for a given byte size.
 *
 * Argument:
 *	byteSize	- Available size.
 *
 * Result:
 *	Vector length fitting the given size.
 *
 * See also:
 *	<Immutable Vector Word>, <Col_MaxVectorLength>
 *---------------------------------------------------------------------------*/

#define VECTOR_MAX_LENGTH(byteSize) \
    (((byteSize)-VECTOR_HEADER_SIZE)/sizeof(Col_Word))

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_VECTOR_INIT
 *
 *	Immutable vector word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	length	- <WORD_VECTOR_LENGTH>
 *
 * See also:
 *	<Immutable Vector Word>, <WORD_TYPE_VECTOR>, <Col_NewVector>, 
 *	<Col_NewVectorNV>
 *---------------------------------------------------------------------------*/

#define WORD_VECTOR_INIT(word, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_VECTOR); \
    WORD_VECTOR_LENGTH(word) = (length);


/*
================================================================================
Internal Section: Mutable Vectors
================================================================================
*/

/*---------------------------------------------------------------------------
 * Data Structure: Mutable Vector Word
 *
 *	Mutable vectors are arrays of words that are directly accessible and
 *	modifiable through a pointer value, and whose length can vary up to a
 *	given capacity set at creation time. 
 *
 * Requirements:
 *	Mutable vector words use the same structure as immutable vector words
 *	so that they can be turned immutable inplace ("frozen", see 
 *	<Col_MVectorFreeze>).
 *
 *	In addition to immutable vector fields, mutable vectors must know their
 *	maximum capacity. It is deduced from the word size in cells minus the
 *	header. So we just have to know the word cell size. When frozen, unused
 *	trailing cells are freed.
 *
 * Fields:
 *	Size		- Number of allocated cells. This determines the maximum
 *			  length of the element array. Given the storage 
 *			  capacity, the maximum size of a mutable vector is
 *			  smaller than that of immutable vectors (see 
 *			  <MVECTOR_MAX_SIZE>, <Col_MaxMVectorLength>).
 *	Length		- Generic <Immutable Vector Word> Length field.
 *	Elements	- Generic <Immutable Vector Word> Elements array.
 *
 * Layout:
 *	On all architectures the cell layout is as follows:
 *
 * (start table)
 *      0     7                                                       n
 *     +-------+-------------------------------------------------------+
 *   0 | Type  |                         Size                          |
 *     +-------+-------------------------------------------------------+
 *   1 |                            Length                             |
 *     +---------------------------------------------------------------+
 *     .                                                               .
 *     .                           Elements                            .
 *     .                                                               .
 *     +---------------------------------------------------------------+
 * (end)
 *
 * See also:
 *	<Immutable Vector Word>, <WORD_TYPEINFO>
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Internal Macros: WORD_MVECTOR_* Accessors
 *
 *	Mutable vector word field accessor macros.
 *
 *  WORD_MVECTOR_SIZE		- Get the number of allocated cells.
 *  WORD_MVECTOR_SET_SIZE	- Set the number of allocated cells. (Caution: 
 *				  word is evaluated several times during macro 
 *				  expansion)
 *
 * Note:
 *	Macros are L-values and side effect-free unless specified (i.e. 
 *	accessible for both read/write operations).
 *
 * See also:
 *	<Mutable Vector Word>, <WORD_MVECTOR_INIT>
 *---------------------------------------------------------------------------*/

#define MVECTOR_SIZE_MASK		MVECTOR_MAX_SIZE
#ifdef COL_BIGENDIAN
#   define WORD_MVECTOR_SIZE(word)	((((size_t *)(word))[0])&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~MVECTOR_SIZE_MASK,(((size_t *)(word))[0])|=((size)&MVECTOR_SIZE_MASK))
#else
#   define WORD_MVECTOR_SIZE(word)	((((size_t *)(word))[0]>>CHAR_BIT)&MVECTOR_SIZE_MASK)
#   define WORD_MVECTOR_SET_SIZE(word, size) ((((size_t *)(word))[0])&=~(MVECTOR_SIZE_MASK<<CHAR_BIT),(((size_t *)(word))[0])|=(((size)&MVECTOR_SIZE_MASK)<<CHAR_BIT))
#endif

/*---------------------------------------------------------------------------
 * Internal Constant: MVECTOR_MAX_SIZE
 *
 *	Maximum cell size taken by mutable vectors.
 *
 * See also:
 *	<Mutable Vector Word>
 *---------------------------------------------------------------------------*/

#define MVECTOR_MAX_SIZE	(SIZE_MAX>>CHAR_BIT)

/*---------------------------------------------------------------------------
 * Internal Macro: WORD_MVECTOR_INIT
 *
 *	Mutable vector word initializer.
 *
 * Arguments:
 *	word	- Word to initialize. (Caution: evaluated several times during 
 *		  macro expansion)
 *	size	- <WORD_MVECTOR_SET_SIZE>
 *	length	- <WORD_VECTOR_LENGTH>
 *
 * See also:
 *	<Mutable Vector Word>, <WORD_TYPE_MVECTOR>, <Col_NewMVector>
 *---------------------------------------------------------------------------*/

#define WORD_MVECTOR_INIT(word, size, length) \
    WORD_SET_TYPEID((word), WORD_TYPE_MVECTOR); \
    WORD_MVECTOR_SET_SIZE((word), (size)); \
    WORD_VECTOR_LENGTH(word) = (length);


/*
================================================================================
Internal Section: Type Checking
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_VECTOR
 *
 *	Type checking macro for vectors.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a vector.
 *
 * See also:
 *	<Col_Error>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_VECTOR(word) \
    if (!(Col_WordType(word) & COL_VECTOR)) { \
	Col_Error(COL_TYPECHECK, ColibriDomain, COL_ERROR_VECTOR, (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

/*---------------------------------------------------------------------------
 * Internal Macro: TYPECHECK_MVECTOR
 *
 *	Type checking macro for mutable vectors.
 *
 * Argument:
 *	word	- Checked word.
 *
 * Side effects:
 *	Generate <COL_TYPECHECK> error when *word* is not a vector.
 *
 * See also:
 *	<Col_Error>
 *---------------------------------------------------------------------------*/

#define TYPECHECK_MVECTOR(word) \
    if (!(Col_WordType(word) & COL_MVECTOR)) { \
	Col_Error(COL_TYPECHECK, ColibriDomain, COL_ERROR_MVECTOR, (word)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 


/*
================================================================================
Internal Section: Range Checking
================================================================================
*/

/*---------------------------------------------------------------------------
 * Internal Macro: RANGECHECK_VECTORLENGTH
 *
 *	Range checking macro for vectors, ensures that length does not exceed 
 *	the maximum value.
 *
 * Argument:
 *	length		- Checked length.
 *	maxLength	- Maximum allowed value.
 *
 * Side effects:
 *	Generate <COL_RANGECHECK> error when *length* exceeds *maxLength*.
 *
 * See also:
 *	<Col_Error>, <VECTOR_MAX_LENGTH>
 *---------------------------------------------------------------------------*/

#define RANGECHECK_VECTORLENGTH(length, maxLength) \
    if ((length) > (maxLength)) { \
	Col_Error(COL_RANGECHECK, ColibriDomain, COL_ERROR_VECTORLENGTH, \
		(length), (maxLength)); \
	goto COL_CONCATENATE(FAILED,__LINE__); \
    } \
    if (0) \
COL_CONCATENATE(FAILED,__LINE__): 

#endif /* _COLIBRI_VECTOR_INT */
