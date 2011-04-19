#ifndef _COLIBRI_TRIE
#define _COLIBRI_TRIE

/* 
 *----------------------------------------------------------------
 * Basic operations. 
 *----------------------------------------------------------------
 */

/*
 * Integer cirt-bit trie maps.
 */

EXTERN Col_Word		Col_NewIntTrieMap();
EXTERN int		Col_IntTrieMapGet(Col_Word map, int key, 
			    Col_Word *valuePtr);
EXTERN int		Col_IntTrieMapSet(Col_Word map, int key, 
			    Col_Word value);
EXTERN int		Col_IntTrieMapUnset(Col_Word map, int key);

#endif /* _COLIBRI_TRIE */
