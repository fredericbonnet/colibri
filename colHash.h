#ifndef _COLIBRI_HASH
#define _COLIBRI_HASH


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/*
 * String hash maps are specialized hash maps where keys are ropes.
 */

EXTERN Col_Word		Col_NewStringHashMap(size_t capacity);
EXTERN int		Col_StringHashMapGet(Col_Word map, Col_Word key, 
			    Col_Word *valuePtr);
EXTERN int		Col_StringHashMapSet(Col_Word map, Col_Word key, 
			    Col_Word value);
EXTERN int		Col_StringHashMapUnset(Col_Word map, Col_Word key);
EXTERN Col_Word		Col_StringHashMapFindEntry(Col_Word map, Col_Word key, 
			    int *createPtr);

/*
 * Integer hash maps are specialized hash maps where keys are integers.
 */

EXTERN Col_Word		Col_NewIntHashMap(size_t capacity);
EXTERN int		Col_IntHashMapGet(Col_Word map, int key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntHashMapSet(Col_Word map, int key, 
			    Col_Word value);
EXTERN int		Col_IntHashMapUnset(Col_Word map, int key);
EXTERN Col_Word		Col_IntHashMapFindEntry(Col_Word map, int key, 
			    int *createPtr);


/* 
 *----------------------------------------------------------------
 * Custom hash maps. 
 *----------------------------------------------------------------
 */

/*
 * Function types and structures for custom hash map types.
 */

typedef unsigned int (Col_HashProc) (Col_Word key);
typedef int (Col_CompareProc) (Col_Word key1, Col_Word key2);

typedef struct Col_CustomHashMapType {
    Col_CustomWordType type;	/* Must be first, and type field must be 
				 * COL_HASHMAP. */

    Col_HashProc *hashProc;	/* Called to get the hash value of the key. */
    Col_CompareProc *compareProc;	
				/* Called to compare both keys. */
} Col_CustomHashMapType;


/* 
 *----------------------------------------------------------------
 * Iteration and traversal. 
 *----------------------------------------------------------------
 */

/*
 * Iteration over individual hash map elements.
 */

EXTERN void		Col_HashMapIterBegin(Col_Word map, 
			    Col_MapIterator *it);
EXTERN void		Col_HashMapIterNext(Col_MapIterator *it);

#endif /* _COLIBRI_HASH */
