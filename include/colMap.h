/*
 * Header: colMap.h
 *
 *	This header file defines the generic map handling features of Colibri.
 *
 *	Maps are a collection datatype that associates keys to values. Keys can
 *	be integers, strings or generic words.
 *
 *	They are always mutable.
 *
 * See also:
 *	<colMap.c>
 */

#ifndef _COLIBRI_MAP
#define _COLIBRI_MAP

#include <stddef.h> /* For size_t */


/*
================================================================================
Section: Maps
================================================================================
*/

/****************************************************************************
 * Group: Map Accessors
 *
 * Declarations:
 *	<Col_MapSize>, <Col_MapGet>, <Col_IntMapGet>, <Col_MapSet>,
 *	<Col_IntMapSet>, <Col_MapUnset>, <Col_IntMapUnset>
 ****************************************************************************/

EXTERN size_t		Col_MapSize(Col_Word map);
EXTERN int		Col_MapGet(Col_Word map, Col_Word key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntMapGet(Col_Word map, intptr_t key, 
			    Col_Word *valuePtr);
EXTERN int		Col_MapSet(Col_Word map, Col_Word key, Col_Word value);
EXTERN int		Col_IntMapSet(Col_Word map, intptr_t key, Col_Word value);
EXTERN int		Col_MapUnset(Col_Word map, Col_Word key);
EXTERN int		Col_IntMapUnset(Col_Word map, intptr_t key);


/****************************************************************************
 * Group: Map Iteration
 *
 * Declarations:
 *	<Col_MapIterBegin>, <Col_MapIterFind>, <Col_IntMapIterFind>, 
 *	<Col_MapIterGet>, <Col_IntMapIterGet>, <Col_MapIterGetKey>, 
 *	<Col_IntMapIterGetKey>, <Col_MapIterGetValue>, <Col_MapIterSetValue>, 
 *	<Col_MapIterNext>
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Internal Typedef: ColMapIterator
 *
 *	Internal implementation of map iterators.
 *
 * Fields:
 *	map		- Map being iterated.
 *	entry		- Current entry.
 *
 * Hash-specific fields:
 *	hash.bucket	- Current bucket index.
 *
 * Trie-specific fields:
 *	trie.prev	- Subtrie whose rightmost leaf is the previous entry.
 *	trie.next	- Subtrie whose leftmost leaf is the next entry.
 *
 * Custom-specific fields:
 *	custom		- Opaque client data. A pair of values is available to
 *			  custom map types for iteration purpose.
 *
 * See also:
 *	<Col_MapIterator>
 *---------------------------------------------------------------------------*/

typedef struct ColMapIterator {
    Col_Word map;
    Col_Word entry;
    union {
	struct {
	    size_t bucket;
	} hash;
	struct {
	    Col_Word prev;
	    Col_Word next;
	} trie;
	Col_ClientData custom[2];
    } traversal;
} ColMapIterator;

/*---------------------------------------------------------------------------
 * Typedef: Col_MapIterator
 *
 *	Map iterator. Encapsulates the necessary info to iterate & access map
 *	data transparently.
 *
 * Note:
 *	Datatype is opaque. Fields should not be accessed by client code.
 *
 *	Each iterator takes 4 words on the stack.
 *---------------------------------------------------------------------------*/

typedef ColMapIterator Col_MapIterator;

/*---------------------------------------------------------------------------
 * Internal Variable: colMapIterNull
 *
 *	Static variable for map iterator initialization.
 *
 * See also:
 *	<COL_MAPITER_NULL>
 *---------------------------------------------------------------------------*/

static const Col_MapIterator colMapIterNull = {WORD_NIL};

/*---------------------------------------------------------------------------
 * Constant: COL_MAPITER_NULL
 *
 *	Static initializer for null map iterators.
 *
 * See also: 
 *	<Col_MapIterator>, <Col_MapIterNull>
 *---------------------------------------------------------------------------*/

#define COL_MAPITER_NULL	colMapIterNull

/*---------------------------------------------------------------------------
 * Macro: Col_MapIterNull
 *
 *	Test whether iterator is null (e.g. it has been set to 
 *	<COL_MAPITER_NULL>). This uninitialized states renders it unusable for
 *	any call. Use with caution.
 *
 * Argument:
 *	it	- The iterator to test.
 *
 * Result:
 *	Non-zero if iterator is null.
 *
 * See also: 
 *	<Col_MapIterator>, <COL_MAPITER_NULL>
 *---------------------------------------------------------------------------*/

#define Col_MapIterNull(it) \
    ((it)->map == WORD_NIL)

/*---------------------------------------------------------------------------
 * Macro: Col_MapIterMap
 *
 *	Get map for iterator.
 *
 * Argument:
 *	it	- The iterator to get map for.
 *
 * Result:
 *	The map, or <WORD_NIL> if at end.
 *
 * See also: 
 *	<Col_MapIterator>, <Col_MapIterEnd>
 *---------------------------------------------------------------------------*/

#define Col_MapIterMap(it) \
    ((it)->map)

/*---------------------------------------------------------------------------
 * Macro: Col_MapIterEnd
 *
 *	Test whether iterator reached end of map.
 *
 * Argument:
 *	it	- The iterator to test.
 *
 * Result:
 *	Non-zero if iterator is at end.
 *
 * See also: 
 *	<Col_MapIterator>
 *---------------------------------------------------------------------------*/

#define Col_MapIterEnd(it) \
    (!(it)->entry)

/*
 * Remaining declarations.
 */

EXTERN void		Col_MapIterBegin(Col_Word map, Col_MapIterator *it);
EXTERN void		Col_MapIterFind(Col_Word map, Col_Word key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_IntMapIterFind(Col_Word map, intptr_t key, 
			    int *createPtr, Col_MapIterator *it);
EXTERN void		Col_MapIterGet(Col_MapIterator *it, Col_Word *keyPtr,
			    Col_Word *valuePtr);
EXTERN void		Col_IntMapIterGet(Col_MapIterator *it, intptr_t *keyPtr,
			    Col_Word *valuePtr);
EXTERN Col_Word		Col_MapIterGetKey(Col_MapIterator *it);
EXTERN intptr_t		Col_IntMapIterGetKey(Col_MapIterator *it);
EXTERN Col_Word		Col_MapIterGetValue(Col_MapIterator *it);
EXTERN void		Col_MapIterSetValue(Col_MapIterator *it, 
			    Col_Word value);
EXTERN void		Col_MapIterNext(Col_MapIterator *it);


/*
================================================================================
Section: Custom Maps
================================================================================
*/

/*---------------------------------------------------------------------------
 * Typedef: Col_MapSizeProc
 *
 *	Function signature of custom map size procs.
 *
 * Argument:
 *	map	- Custom map to get size for.
 *
 * Result:
 *	The custom map size.
 *
 * See also: 
 *	<Col_CustomMapType>, <Col_CustomIntMapType>
 *---------------------------------------------------------------------------*/

typedef size_t (Col_MapSizeProc) (Col_Word map);

/*---------------------------------------------------------------------------
 * Typedef: Col_MapGetProc
 *
 *	Function signature of custom map get procs.
 *
 * Arguments:
 *	map		- Custom map to get entry from.
 *	key		- Entry key. Can be any word type, including string, 
 *			  however it must match the actual type used by the map.
 *	valuePtr	- Returned entry value, if found.
 *
 * Results:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_MapGetProc)(Col_Word map, Col_Word key, Col_Word *valuePtr);

/*---------------------------------------------------------------------------
 * Typedef: Col_IntMapGetProc
 *
 *	Function signature of custom integer map get procs.
 *
 * Arguments:
 *	map		- Custom integer map to get entry from.
 *	key		- Integer entry key.
 *	valuePtr	- Returned entry value, if found.
 *
 * Results:
 *	Whether the key was found in the map. In this case the value is returned
 *	through valuePtr.
 *
 * See also: 
 *	<Col_CustomIntMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_IntMapGetProc)(Col_Word map, intptr_t key, Col_Word *valuePtr);

/*---------------------------------------------------------------------------
 * Typedef: Col_MapSetProc
 *
 *	Function signature of custom map set procs.
 *
 * Arguments:
 *	map	- Custom map to insert entry into.
 *	key	- Entry key. Can be any word type, including string, however it
 *		  must match the actual type used by the map.
 *	value	- Entry value.
 *
 * Result:
 *	Whether a new entry was created.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_MapSetProc)(Col_Word map, Col_Word key, Col_Word value);

/*---------------------------------------------------------------------------
 * Typedef: Col_IntMapSetProc
 *
 *	Function signature of custom integer map set procs.
 *
 * Arguments:
 *	map	- Custom integer map to insert entry into.
 *	key	- Integer entry key.
 *	value	- Entry value.
 *
 * Result:
 *	Whether a new entry was created.
 *
 * See also: 
 *	<Col_CustomIntMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_IntMapSetProc)(Col_Word map, intptr_t key, Col_Word value);

/*---------------------------------------------------------------------------
 * Typedef: Col_MapUnsetProc
 *
 *	Function signature of custom map unset procs.
 *
 * Arguments:
 *	map	- Custom map to remove entry from.
 *	key	- Entry key. Can be any word type, including string, however it
 *		  must match the actual type used by the map.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_MapUnsetProc)(Col_Word map, Col_Word key);
    
/*---------------------------------------------------------------------------
 * Typedef: Col_IntMapUnsetProc
 *
 *	Function signature of custom integer map unset procs.
 *
 * Arguments:
 *	map	- Custom integer map to remove entry from.
 *	key	- Integer entry key.
 *
 * See also: 
 *	<Col_CustomIntMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_IntMapUnsetProc)(Col_Word map, intptr_t key);
    
/*---------------------------------------------------------------------------
 * Typedef: Col_MapIterBeginProc
 *
 *	Function signature of custom map iter begin procs.
 *
 * Arguments:
 *	map		- Custom map to begin iteration for.
 *	clientData	- Opaque client data. A pair of values is available to
 *			  custom map types for iteration purpose.
 *
 * Result:
 *	Nonzero if iteration began, zero if at end.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_MapIterBeginProc)(Col_Word map, 
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Typedef: Col_MapIterFindProc
 *
 *	Function signature of custom map iter find procs.
 *
 * Arguments:
 *	map		- Custom map to find or create entry into.
 *	key		- Entry key. Can be any word type, including string,
 *			  however it must match the actual type used by the map.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *	clientData	- Opaque client data. A pair of values is available to
 *			  custom map types for iteration purpose.
 *
 * Result:
 *	Nonzero if iteration began, zero if at end.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_MapIterFindProc)(Col_Word map, Col_Word key, int *createPtr,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Typedef: Col_IntMapIterFindProc
 *
 *	Function signature of custom integer map iter find procs.
 *
 * Arguments:
 *	map		- Custom integer map to find or create entry into.
 *	key		- Integer entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *	clientData	- Opaque client data. A pair of values is available to
 *			  custom map types for iteration purpose.
 *
 * Result:
 *	Nonzero if iteration began, zero if at end.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_IntMapIterFindProc)(Col_Word map, intptr_t key, int *createPtr,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Typedef: Col_MapIterNextProc
 *
 *	Function signature of custom map iter next procs.
 *
 * Arguments:
 *	map		- Custom map to continue iteration for.
 *	clientData	- Opaque client data. A pair of values is available to
 *			  custom map types for iteration purpose.
 *
 * Result:
 *	Nonzero if iteration continued, zero if at end.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef int (Col_MapIterNextProc)(Col_Word map, 
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Typedef: Col_MapIterGetKeyProc
 *
 *	Function signature of custom map iter key get procs.
 *
 * Arguments:
 *	map		- Custom map to get iterator key from.
 *	clientData	- Opaque client data. A pair of values is available to
 *			  custom map types for iteration purpose.
 *
 * Result:
 *	Key word.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef Col_Word (Col_MapIterGetKeyProc)(Col_Word map, 
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Typedef: Col_IntMapIterGetKeyProc
 *
 *	Function signature of custom integer map iter key get procs.
 *
 * Arguments:
 *	map		- Custom integer map to get iterator key from.
 *	key		- Integer entry key.
 *	createPtr	- (in) If non-NULL, whether to create entry if absent.
 *	clientData	- Opaque client data. A pair of values is available to
 *			  custom map types for iteration purpose.
 *
 * Result:
 *	Integer key.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef intptr_t (Col_IntMapIterGetKeyProc)(Col_Word map,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Typedef: Col_MapIterGetValueProc
 *
 *	Function signature of custom map iter value get procs.
 *
 * Arguments:
 *	map		- Custom map to get iterator value from.
 *	clientData	- Opaque client data. A pair of values is available to
 *			  custom map types for iteration purpose.
 *
 * Result:
 *	Value word.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef Col_Word (Col_MapIterGetValueProc)(Col_Word map, 
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Typedef: Col_MapIterSetValueProc
 *
 *	Function signature of custom map iter value set procs.
 *
 * Arguments:
 *	map		- Custom map to set iterator value for.
 *	value		- Value to set.
 *	clientData	- Opaque client data. A pair of values is available to
 *			  custom map types for iteration purpose.
 *
 * Result:
 *	Value word.
 *
 * See also: 
 *	<Col_CustomMapType>
 *---------------------------------------------------------------------------*/

typedef Col_Word (Col_MapIterSetValueProc)(Col_Word map, Col_Word value,
    Col_ClientData (*clientData)[2]);

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomMapType
 *
 *	Custom map type descriptor. Inherits from <Col_CustomWordType>.
 *
 * Fields:
 *	type			- Generic word type descriptor. Type field must 
 *				  be equal to <COL_MAP>.
 *	sizeProc		- Called to get map size.
 *	getProc			- Called to get entry value.
 *	setProc			- Called to set entry value.
 *	unsetProc		- Called to unset entry.
 *	iterBeginProc		- Called to begin iteration.
 *	iterFindProc		- Called to begin iteration at given key.
 *	iterNextProc		- Called to continue iteration.
 *	iterGetKeyProc		- Called to get iterator key.
 *	iterGetValueProc	- Called to get iterator value.
 *	iterSetValueProc	- Called to set iterator value.
 *
 * See also:
 *	<Col_CustomWordType>, <Col_MapSizeProc>, <Col_MapGetProc>, 
 *	<Col_MapSetProc>, <Col_MapUnsetProc>, <Col_MapIterBeginProc>, 
 *	<Col_MapIterFindProc>, <Col_MapIterNextProc>, <Col_MapIterGetKeyProc>, 
 *	<Col_MapIterGetValueProc>, <Col_MapIterSetValueProc>
 *---------------------------------------------------------------------------*/

typedef struct Col_CustomMapType {
    Col_CustomWordType type;
    Col_MapSizeProc *sizeProc;
    Col_MapGetProc *getProc;
    Col_MapSetProc *setProc;
    Col_MapUnsetProc *unsetProc;
    Col_MapIterBeginProc *iterBeginProc;
    Col_MapIterFindProc *iterFindProc;
    Col_MapIterNextProc *iterNextProc;
    Col_MapIterGetKeyProc *iterGetKeyProc;
    Col_MapIterGetValueProc *iterGetValueProc;
    Col_MapIterSetValueProc *iterSetValueProc;
} Col_CustomMapType;

/*---------------------------------------------------------------------------
 * Typedef: Col_CustomIntMapType
 *
 *	Custom integer map type descriptor. Inherits from <Col_CustomWordType>.
 *
 * Fields:
 *	type			- Generic word type descriptor. Type field must 
 *				  be equal to <COL_INTMAP>.
 *	sizeProc		- Called to get map size.
 *	getProc			- Called to get entry value.
 *	setProc			- Called to set entry value.
 *	unsetProc		- Called to unset entry.
 *	iterBeginProc		- Called to begin iteration.
 *	iterFindProc		- Called to begin iteration at given key.
 *	iterNextProc		- Called to continue iteration.
 *	iterGetKeyProc		- Called to get iterator key.
 *	iterGetValueProc	- Called to get iterator value.
 *	iterSetValueProc	- Called to set iterator value.
 *
 * See also:
 *	<Col_CustomWordType>, <Col_MapSizeProc>, <Col_IntMapGetProc>, 
 *	<Col_IntMapSetProc>, <Col_IntMapUnsetProc>, <Col_MapIterBeginProc>, 
 *	<Col_IntMapIterFindProc>, <Col_MapIterNextProc>, 
 *	<Col_IntMapIterGetKeyProc>, <Col_MapIterGetValueProc>, 
 *	<Col_MapIterSetValueProc>
 *---------------------------------------------------------------------------*/

typedef struct Col_CustomIntMapType {
    Col_CustomWordType type;
    Col_MapSizeProc *sizeProc;
    Col_IntMapGetProc *getProc;
    Col_IntMapSetProc *setProc;
    Col_IntMapUnsetProc *unsetProc;
    Col_MapIterBeginProc *iterBeginProc;
    Col_IntMapIterFindProc *iterFindProc;
    Col_MapIterNextProc *iterNextProc;
    Col_IntMapIterGetKeyProc *iterGetKeyProc;
    Col_MapIterGetValueProc *iterGetValueProc;
    Col_MapIterSetValueProc *iterSetValueProc;
} Col_CustomIntMapType;

/****************************************************************************
 * Group: Custom Map Creation
 *
 * Declarations:
 *	<Col_NewCustomMap>, <Col_NewCustomIntMap>
 ****************************************************************************/

EXTERN Col_Word		Col_NewCustomMapMap(Col_CustomMapType *type, 
			    size_t size, void **dataPtr);
EXTERN Col_Word		Col_NewCustomIntMapMap(Col_CustomIntMapType *type, 
			    size_t size, void **dataPtr);

#endif /* _COLIBRI_MAP */
