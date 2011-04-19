#ifndef _COLIBRI_TRIE
#define _COLIBRI_TRIE


/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/*
 * Crit-bit trie maps (string based).
 */

EXTERN Col_Word		Col_NewStringTrieMap();
EXTERN int		Col_StringTrieMapGet(Col_Word map, Col_Word key,
			    Col_Word *valuePtr);
EXTERN int		Col_StringTrieMapSet(Col_Word map, Col_Word key, 
			    Col_Word value);
EXTERN int		Col_StringTrieMapUnset(Col_Word map, Col_Word key);
EXTERN Col_Word		Col_StringTrieMapFindEntry(Col_Word map, Col_Word key, 
			    int *createPtr);

/*
 * Integer crit-bit trie maps.
 */

EXTERN Col_Word		Col_NewIntTrieMap();
EXTERN int		Col_IntTrieMapGet(Col_Word map, int key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntTrieMapSet(Col_Word map, int key, 
			    Col_Word value);
EXTERN int		Col_IntTrieMapUnset(Col_Word map, int key);
EXTERN Col_Word		Col_IntTrieMapFindEntry(Col_Word map, int key, 
			    int *createPtr);


/* 
 *----------------------------------------------------------------
 * Iteration and traversal. 
 *----------------------------------------------------------------
 */

/*
 * Iteration over individual trie map elements.
 */

EXTERN void		Col_TrieMapIterBegin(Col_Word map, 
			    Col_MapIterator *it);
EXTERN void		Col_TrieMapIterNext(Col_MapIterator *it);

#endif /* _COLIBRI_TRIE */
