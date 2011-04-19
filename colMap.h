#ifndef _COLIBRI_MAP
#define _COLIBRI_MAP

#include <stddef.h> /* For size_t */


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/*
 * Generic map interface.
 */

EXTERN size_t		Col_MapSize(Col_Word map);
/*
EXTERN int		Col_MapGet(Col_Word map, Col_Word key, 
			    Col_Word *valuePtr);
EXTERN int		Col_MapSet(Col_Word map, Col_Word key, Col_Word value);
EXTERN int		Col_MapUnset(Col_Word map, Col_Word key);
*/

EXTERN void		Col_MapEntryGet(Col_Word entry, Col_Word *keyPtr,
			    Col_Word *valuePtr);
EXTERN Col_Word		Col_MapEntryGetKey(Col_Word entry);
EXTERN Col_Word		Col_MapEntryGetValue(Col_Word entry);
EXTERN void		Col_MapEntrySetValue(Col_Word entry, Col_Word value);

/*
 * Generic string map interface.
 */

EXTERN int		Col_StringMapGet(Col_Word map, Col_Word key, 
			    Col_Word *valuePtr);
EXTERN int		Col_StringMapSet(Col_Word map, Col_Word key, 
			    Col_Word value);
EXTERN int		Col_StringMapUnset(Col_Word map, Col_Word key);

/*
 * Generic integer map interface.
 */

EXTERN int		Col_IntMapGet(Col_Word map, int key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntMapSet(Col_Word map, int key, Col_Word value);
EXTERN int		Col_IntMapUnset(Col_Word map, int key);

EXTERN void		Col_IntMapEntryGet(Col_Word entry, int *keyPtr,
			    Col_Word *valuePtr);
EXTERN int		Col_IntMapEntryGetKey(Col_Word entry);


/* 
 *----------------------------------------------------------------
 * Iteration and traversal. 
 *----------------------------------------------------------------
 */

/*
 * Generic iteration interface.
 */

typedef struct Col_MapIterator {
    Col_Word map;		/* Map being iterated. */
    Col_Word entry;		/* Current entry. */
    size_t bucket;		/* Current bucket (for hash maps). */
} Col_MapIterator;

#define Col_MapIterEnd(it)	(!(it)->map)
#define Col_MapIterMap(it)	((it)->map)
#define Col_MapIterEntry(it)	((it)->entry)

/* TODO */

typedef int (*Col_HashKeyProc) (Col_Word key);
typedef int (*Col_CompareKeysProc) (Col_Word key1, Col_Word key2);

#endif /* _COLIBRI_MAP */
