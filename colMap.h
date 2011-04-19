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

/*
 * Generic integer map interface.
 */

EXTERN int		Col_IntMapGet(Col_Word map, int key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntMapSet(Col_Word map, int key, Col_Word value);
EXTERN int		Col_IntMapUnset(Col_Word map, int key);


/* 
 *----------------------------------------------------------------
 * Iteration and traversal. 
 *----------------------------------------------------------------
 */

/*
 * Iteration over individual list map elements. TODO
 */

typedef struct Col_MapIterator {
    Col_Word map;		/* Map being iterated. */
    /* TODO */
} Col_MapIterator;

#define Col_MapIterEnd(it)	(!(it)->map)
#define Col_MapIterMap(it)	((it)->map)
	
EXTERN void		Col_MapIterBegin(Col_Word map, Col_MapIterator *it);
EXTERN void		Col_MapIterAt(Col_MapIterator *it, Col_Word *keyPtr,
			    Col_Word *valuePtr);
EXTERN void		Col_MapIterNext(Col_MapIterator *it);

#endif /* _COLIBRI_MAP */
