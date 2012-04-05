/*
 * Header: colStrBuf.h
 *
 *	This header file defines the string buffer handling features of Colibri.
 *
 *	String buffers are used to build strings incrementally in an efficient
 *	manner, by appending individual characters or whole ropes. The current
 *	accumulated rope can be retrieved at any time.
 *
 * See also:
 *	<colStrBuf.c>
 */

#ifndef _COLIBRI_STRBUF
#define _COLIBRI_STRBUF

#include <stddef.h> /* For size_t */
#include <stdarg.h> /* For variadic procs */


/*
================================================================================
Section: String Buffers
================================================================================
*/

/****************************************************************************
 * Group: String Buffer Creation
 *
 * Declarations:
 *	<Col_GetMaxStringBufferLength>, <Col_NewStringBuffer>
 ****************************************************************************/

EXTERN size_t		Col_GetMaxStringBufferLength(Col_StringFormat format);
EXTERN Col_Word		Col_NewStringBuffer(size_t maxLength, 
			    Col_StringFormat format);


/****************************************************************************
 * Group: String Buffer Access
 *
 * Declarations:
 *	<Col_StringBufferFormat>, <Col_StringBufferMaxLength>,
 *	<Col_StringBufferLength>, <Col_StringBufferValue>
 ****************************************************************************/

EXTERN Col_StringFormat	Col_StringBufferFormat(Col_Word strbuf);
EXTERN size_t		Col_StringBufferMaxLength(Col_Word strbuf);
EXTERN size_t		Col_StringBufferLength(Col_Word strbuf);
EXTERN Col_Word		Col_StringBufferValue(Col_Word strbuf);


/****************************************************************************
 * Group: String Buffer Operations
 *
 * Declarations:
 *	<Col_StringBufferAppendChar>, <Col_StringBufferAppendRope>,
 *	<Col_StringBufferAppendSequence>, <Col_StringBufferReserve>, 
 *	<Col_StringBufferRelease>, <Col_StringBufferReset>, 
 *	<Col_StringBufferFreeze>
 ****************************************************************************/

EXTERN int		Col_StringBufferAppendChar(Col_Word strbuf,
			    Col_Char c);
EXTERN int		Col_StringBufferAppendRope(Col_Word strbuf,
			    Col_Word rope);
EXTERN int		Col_StringBufferAppendSequence(Col_Word strbuf,
			    Col_RopeIterator *begin, Col_RopeIterator *end);//TODO
EXTERN void *		Col_StringBufferReserve(Col_Word strbuf, size_t length);
EXTERN void		Col_StringBufferRelease(Col_Word strbuf, size_t length);
EXTERN void		Col_StringBufferReset(Col_Word strbuf);
EXTERN Col_Word		Col_StringBufferFreeze(Col_Word strbuf);

#endif /* _COLIBRI_STRBUF */
