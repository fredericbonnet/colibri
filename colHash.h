#ifndef _COLIBRI_HASH
#define _COLIBRI_HASH

typedef int (*Col_HashKeyProc)(Col_Word key);
typedef int (*Col_CompareKeysProc)(Col_Word key1, Col_Word key2);


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/*
 * Hash maps.
 */

/*
EXTERN Col_Word		Col_NewHashMap(Col_HashKeyProc *haskKeyProc,
			    Col_CompareKeysProc *compareKeysProc);/*TODO: capacity/load factor*//*
*/

/*
 * Integer hash maps are specialized hash maps where keys are integers.
 */

EXTERN Col_Word		Col_NewIntHashMap();/*TODO: capacity/load factor?*/
EXTERN int		Col_IntHashMapGet(Col_Word map, int key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntHashMapSet(Col_Word map, int key, 
			    Col_Word value);
EXTERN int		Col_IntHashMapUnset(Col_Word map, int key);

#endif /* _COLIBRI_HASH */
