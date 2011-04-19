#ifndef _COLIBRI_MAP
#define _COLIBRI_MAP

#include <stddef.h> /* For size_t */


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
EXTERN Col_Word		Col_MapIterKeyAt(Col_MapIterator *it);
EXTERN Col_Word		Col_MapIterValueAt(Col_MapIterator *it);
EXTERN void		Col_MapIterNext(Col_MapIterator *it);

#endif /* _COLIBRI_MAP */
