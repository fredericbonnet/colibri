/*
 * Header: colibri.h
 *
 *	This header file describes the public API of the Colibri library.
 */

#ifndef _COLIBRI
#define _COLIBRI

/*
 * For C++ compilers, use extern "C"
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The following definitions set up the proper options for Windows compilers.
 * We use this method because there is no autoconf equivalent.
 */

#ifndef __WIN32__
#   if defined(_WIN32) || defined(WIN32) || defined(__MINGW32__) || defined(__BORLANDC__) || (defined(__WATCOMC__) && defined(__WINDOWS_386__))
#	define __WIN32__
#	ifndef WIN32
#	    define WIN32
#	endif
#	ifndef _WIN32
#	    define _WIN32
#	endif
#   endif
#endif

/*
 * STRICT: See MSDN Article Q83456
 */

#ifdef __WIN32__
#   ifndef STRICT
#	define STRICT
#   endif
#endif /* __WIN32__ */

/*
 * Macros used to declare a function to be exported by a DLL. Used by Windows,
 * maps to no-op declarations on non-Windows systems. The default build on
 * windows is for a DLL, which causes the DLLIMPORT and DLLEXPORT macros to be
 * nonempty. To build a static library, the macro STATIC_BUILD should be
 * defined.
 *
 * Note: when building static but linking dynamically to MSVCRT we must still
 *       correctly decorate the C library imported function.  Use CRTIMPORT
 *       for this purpose.  _DLL is defined by the compiler when linking to
 *       MSVCRT.  
 */

#if (defined(__WIN32__) && (defined(_MSC_VER) || (__BORLANDC__ >= 0x0550) || defined(__LCC__) || defined(__WATCOMC__) || (defined(__GNUC__) && defined(__declspec))))
#   define HAVE_DECLSPEC 1
#   ifdef STATIC_BUILD
#       define DLLIMPORT
#       define DLLEXPORT
#       ifdef _DLL
#           define CRTIMPORT __declspec(dllimport)
#       else
#           define CRTIMPORT
#       endif
#   else
#       define DLLIMPORT __declspec(dllimport)
#       define DLLEXPORT __declspec(dllexport)
#       define CRTIMPORT __declspec(dllimport)
#   endif
#else
#   define DLLIMPORT
#   if defined(__GNUC__) && __GNUC__ > 3
#       define DLLEXPORT __attribute__ ((visibility("default")))
#   else
#       define DLLEXPORT
#   endif
#   define CRTIMPORT
#endif

/*
 * These macros are used to control whether functions are being declared for
 * import or export. If a function is being declared while it is being built
 * to be included in a shared library, then it should have the DLLEXPORT
 * storage class. If is being declared for use by a module that is going to
 * link against the shared library, then it should have the DLLIMPORT storage
 * class. If the symbol is beind declared for a static build or for use from a
 * stub library, then the storage class should be empty.
 *
 * The convention is that a macro called BUILD_xxxx, where xxxx is the name of
 * a library we are building, is set on the compile line for sources that are
 * to be placed in the library. When this macro is set, the storage class will
 * be set to DLLEXPORT. At the end of the header file, the storage class will
 * be reset to DLLIMPORT.
 */

#undef COL_STORAGE_CLASS
#ifdef BUILD_colibri
#   define COL_STORAGE_CLASS DLLEXPORT
#else
#   define COL_STORAGE_CLASS DLLIMPORT
#endif

/*
 * Make sure EXTERN isn't defined elsewhere
 */

#ifdef EXTERN
#   undef EXTERN
#endif /* EXTERN */

#ifdef __cplusplus
#   define EXTERN extern "C" COL_STORAGE_CLASS
#else
#   define EXTERN extern COL_STORAGE_CLASS
#endif

/*
 * Integral types.
 */

#if defined(HAVE_STDINT_H)
    /* C99 types.*/
#   include <stdint.h>
#elif defined(_MSC_VER)
    /* Microsoft specific extensions. */
#   define int8_t __int8
#   define uint8_t unsigned __int8
#   define int16_t __int16
#   define uint16_t unsigned __int16
#   define int32_t __int32
#   define uint32_t unsigned __int32
#   define int64_t __int64
#   define uint64_t unsigned __int64
#   define UINT16_MAX (65535)
#   define INTPTR_MIN (-2147483647-1)
#   define INTPTR_MAX (2147483647)
#   define UINTPTR_MAX (4294967295U)
#else
    /* Sensible fallback. */
#   define int8_t char
#   define uint8_t unsigned char
#   define int16_t short
#   define uint16_t unsigned short
#   define int32_t int
#   define uint32_t unsigned int
#   define int64_t long long
#   define uint64_t unsigned long long
#endif

#include <stddef.h> /* For size_t */


/*
================================================================================
Section: Basic Types
================================================================================
*/

/*---------------------------------------------------------------------------
 * Typedef: Col_ClientData
 *
 *	Opaque token used to hold private data. Typically used with callbacks.
 *---------------------------------------------------------------------------*/

typedef void * Col_ClientData;


/*
================================================================================
Section: Strings

Declarations:
	<Col_Utf8Addr>, <Col_Utf8Get>, <Col_Utf8Set>, <Col_Utf8Next>, 
	<Col_Utf8Prev>, <Col_Utf16Addr>, <Col_Utf16Get>, <Col_Utf16Set>,
	<Col_Utf16Next>, <Col_Utf16Prev>
================================================================================
*/

/*---------------------------------------------------------------------------
 * Typedef: Col_Char
 *
 *	String characters use the 32-bit Unicode encoding.
 *---------------------------------------------------------------------------*/

typedef uint32_t Col_Char;

/*---------------------------------------------------------------------------
 * Constant: COL_CHAR_INVALID
 *
 *	Invalid Unicode character. Used in error cases.
 *---------------------------------------------------------------------------*/

#define COL_CHAR_INVALID	((Col_Char) -1)

/*---------------------------------------------------------------------------
 * Typedefs: Character Types
 *
 *	Characters use the Universal Character Set (UCS) encoding.
 *
 *  Col_Char1	- 1-byte UCS codepoint truncated to the lower 8 bits, i.e.
 *		  Latin-1.
 *  Col_Char2	- 2-byte UCS-2 codepoint.
 *  Col_Char4	- 4-byte UCS-4/UTF-32 codepoint, same as <Col_Char>.
 *
 * See also:
 *	<Col_Char>
 *---------------------------------------------------------------------------*/

typedef uint8_t Col_Char1;
typedef uint16_t Col_Char2;
typedef Col_Char Col_Char4;

/*---------------------------------------------------------------------------
 * Constants: COL_CHAR( * )_MAX
 *
 *	Maximum character codepoints.
 *
 *  COL_CHAR_MAX	- Maximum Universal Character Set (UCS) codepoint.
 *  COL_CHAR1_MAX	- Maximum UCS-1 codepoint.
 *  COL_CHAR2_MAX	- Maximum UCS-2 codepoint.
 *  COL_CHAR4_MAX	- Maximum UCS-4 codepoint, same as <COL_CHAR_MAX>.
 *---------------------------------------------------------------------------*/

#define COL_CHAR_MAX		((Col_Char) 0x10FFFF)
#define COL_CHAR1_MAX		0xFF
#define COL_CHAR2_MAX		0xFFFF
#define COL_CHAR4_MAX		COL_CHAR_MAX

/*---------------------------------------------------------------------------
 * Enum: Col_StringFormat
 *
 *	Strings can use various formats. 
 *
 *  COL_UCS1	- Fixed-width array of <Col_Char1>.
 *  COL_UCS2	- Fixed-width array of <Col_Char2>.
 *  COL_UCS4	- Fixed-width array of <Col_Char4>.
 *  COL_UTF8	- UTF-8 variable width encoding.
 *  COL_UTF16	- UTF-16 variable width encoding.
 *
 * Note: 
 *	We assume that UTF-8/16 data is always well-formed. It is the caller 
 *	responsibility to validate and ensure well-formedness of UTF-8/16 data,
 *	notably for security reasons. 
 *
 *	Numeric values are chosen so that the lower 3 bits give the character
 *	width in the data chunk. 
 *---------------------------------------------------------------------------*/

typedef enum Col_StringFormat {
    COL_UCS1=0x01,
    COL_UCS2=0x02,
    COL_UCS4=0x04,
    COL_UTF8=0x11,
    COL_UTF16=0x12
} Col_StringFormat;

/*---------------------------------------------------------------------------
 * Constant: COL_UTF8_MAX_WIDTH
 *
 *	Maximum width of character in UTF-8 code units.
 *---------------------------------------------------------------------------*/

#define COL_UTF8_MAX_WIDTH	4

/*---------------------------------------------------------------------------
 * Macro: COL_UTF8_WIDTH
 *
 *	Get width of character in UTF-8 code units.
 *
 * Argument:
 *	c	- The character.
 *
 * Result:
 *	Number of UTF-8 codepoints needed for the character (0 if invalid).
 *---------------------------------------------------------------------------*/

#define COL_UTF8_WIDTH(c) \
    (  (c) <= 0x7F	? 1 \
     : (c) <= 0x7FF	? 2 \
     : (c) <= 0xD7FF	? 3 \
     : (c) <= 0xDFFF	? 0 \
     : (c) <= 0xFFFF	? 3 \
     : (c) <= 0x10FFFF	? 4 \
     : 			  0 )

/*---------------------------------------------------------------------------
 * Constant: COL_UTF16_MAX_WIDTH
 *
 *	Maximum width of character in UTF-16 code units.
 *---------------------------------------------------------------------------*/

#define COL_UTF16_MAX_WIDTH	2

/*---------------------------------------------------------------------------
 * Macro: COL_UTF16_WIDTH
 *
 *	Get width of character in UTF-16 code units.
 *
 * Argument:
 *	c	- The character.
 *
 * Result:
 *	Number of UTF-16 codepoints needed for the character (0 if invalid).
 *---------------------------------------------------------------------------*/

#define COL_UTF16_WIDTH(c) \
    (  (c) <= 0xD7FF	? 1 \
     : (c) <= 0xDFFF	? 0 \
     : (c) <= 0xFFFF	? 1 \
     : (c) <= 0x10FFFF	? 2 \
     : 			  0 )

/*---------------------------------------------------------------------------
 * Macro: COL_CHAR_NEXT
 *
 *	Get next character in data chunk.
 *
 * Arguments:
 *	format	- Data format.
 *	data	- Pointer to next character.
 *
 * Result:
 *	c	- Character codepoint.
 *
 * Side effects:
 *	The data pointer is moved just past the character.
 *---------------------------------------------------------------------------*/

#define COL_CHAR_NEXT(format, data, c) \
    switch (format) { \
	case COL_UCS1:  (c) =            *((const Col_Char1 *) (data)); (data) = (const char *) (data) + 1;                                break; \
	case COL_UCS2:  (c) =            *((const Col_Char2 *) (data)); (data) = (const char *) (data) + 2;                                break; \
	case COL_UCS4:  (c) =            *((const Col_Char4 *) (data)); (data) = (const char *) (data) + 4;                                break; \
	case COL_UTF8:  (c) = Col_Utf8Get ((const Col_Char1 *) data);   (data) = (const char *) Col_Utf8Next ((const Col_Char1 *) (data)); break; \
	case COL_UTF16: (c) = Col_Utf16Get((const Col_Char2 *) data);   (data) = (const char *) Col_Utf16Next((const Col_Char2 *) (data)); break; \
	default: (c) = COL_CHAR_INVALID; data = NULL; /* Keep compilers happy. */ \
    }

/*---------------------------------------------------------------------------
 * Macro: COL_CHAR_PREVIOUS
 *
 *	Get previous character in data chunk.
 *
 * Arguments:
 *	format	- Data format.
 *	data	- Pointer just past the next character.
 *
 * Result:
 *	c	- Character codepoint.
 *
 * Side effects:
 *	The data pointer is moved to the character.
 *---------------------------------------------------------------------------*/

#define COL_CHAR_PREVIOUS(format, data, c) \
    switch (format) { \
	case COL_UCS1:  (data) = (const char *) (data) - 1;                                (c) =            *((const Col_Char1 *) (data)); break; \
	case COL_UCS2:  (data) = (const char *) (data) - 2;                                (c) =            *((const Col_Char2 *) (data)); break; \
	case COL_UCS4:  (data) = (const char *) (data) - 4;                                (c) =            *((const Col_Char4 *) (data)); break; \
	case COL_UTF8:  (data) = (const char *) Col_Utf8Prev ((const Col_Char1 *) (data)); (c) = Col_Utf8Get ((const Col_Char1 *) data);   break; \
	case COL_UTF16: (data) = (const char *) Col_Utf16Prev((const Col_Char2 *) (data)); (c) = Col_Utf16Get((const Col_Char2 *) data);   break; \
	default: (c) = COL_CHAR_INVALID; data = NULL; /* Keep compilers happy. */ \
    }

/*
 * Remaining declarations.
 */

EXTERN const Col_Char1 * Col_Utf8Addr(const Col_Char1 * data, size_t index, 
			    size_t length, size_t byteLength);
EXTERN Col_Char		Col_Utf8Get(const Col_Char1 * data);
EXTERN Col_Char1 * 	Col_Utf8Set(Col_Char1 * data, Col_Char c);
EXTERN const Col_Char1 * Col_Utf8Next(const Col_Char1 * data);
EXTERN const Col_Char1 * Col_Utf8Prev(const Col_Char1 * data);
EXTERN const Col_Char2 * Col_Utf16Addr(const Col_Char2 * data, size_t index, 
			    size_t length, size_t byteLength);
EXTERN Col_Char		Col_Utf16Get(const Col_Char2 * data);
EXTERN Col_Char2 * 	Col_Utf16Set(Col_Char2 * data, Col_Char c);
EXTERN const Col_Char2 * Col_Utf16Next(const Col_Char2 * data);
EXTERN const Col_Char2 * Col_Utf16Prev(const Col_Char2 * data);


/*
 * Include other files.
 */

#include "colUtils.h"

#include "colWord.h"

#include "colRope.h"
#include "colStrBuf.h"

#include "colVector.h"
#include "colList.h"

#include "colMap.h"
#include "colHash.h"
#include "colTrie.h"


/*
================================================================================
Section: Initialization/Cleanup

Declarations:
	<Col_Init>, <Col_Cleanup>
================================================================================
*/

/*---------------------------------------------------------------------------
 * Constants: Threading Model Constants
 *
 *	Threading models chosen at initialization time.
 *
 *  COL_SINGLE	- Strict appartment + stop-the-world model. GC is done 
 *		  synchronously when client thread resumes GC (<Col_ResumeGC>).
 *
 *  COL_ASYNC   - Strict appartment model with asynchronous GC. GC uses a 
 *		  dedicated thread for asynchronous processing, the client 
 *		  thread cannot pause a running GC and is blocked until 
 *		  completion.
 *
 *  COL_SHARED  - Shared multithreaded model with GC-preference. Data can be 
 *		  shared across client threads of the same group (COL_SHARED is 
 *		  the base index value). GC uses a dedicated thread for 
 *		  asynchronous processing.; GC process starts once all client 
 *		  threads get out of pause, no client thread can pause a 
 *		  scheduled GC.
 *
 * See also: 
 *	<Col_Init>, <Col_PauseGC>, <Col_ResumeGC>
 *---------------------------------------------------------------------------*/

#define COL_SINGLE		0
#define COL_ASYNC		1
#define COL_SHARED		2

EXTERN void		Col_Init(unsigned int model);
EXTERN void		Col_Cleanup(void);


/*
================================================================================
Section: Error Handling

Declarations:
	<Col_Error>, <Col_SetErrorProc>
================================================================================
*/

/*---------------------------------------------------------------------------
 * Enum: Col_ErrorLevel
 *
 *	Error levels.
 *
 *  COL_FATAL	- Fatal error within Colibri, forces process termination.
 *  COL_ERROR	- Error typically caused by the application (bad input...).
 *
 * See also: 
 *	<Col_Error>
 *---------------------------------------------------------------------------*/

typedef enum {
    COL_FATAL,
    COL_ERROR
} Col_ErrorLevel;

/*---------------------------------------------------------------------------
 * Typedef: Col_ErrorProc
 *
 *	Function signature of custom error handlers.
 *
 * Arguments:
 *	level	- Error level.
 *	format	- Format string fed to fprintf().
 *	args	- Remaining arguments fed to fprintf().
 *
 * See also: 
 *	<Col_SetErrorProc>, <Col_ErrorLevel>
 *---------------------------------------------------------------------------*/

typedef void (Col_ErrorProc) (Col_ErrorLevel level, const char *format, 
			     va_list args);

EXTERN void		Col_Error(Col_ErrorLevel level, const char *format, 
			    ...);
EXTERN void		Col_SetErrorProc(Col_ErrorProc *proc);


/*
================================================================================
Section: GC Control

Declarations:
	<Col_PauseGC>, <Col_TryPauseGC>, <Col_ResumeGC>
================================================================================
*/

EXTERN void		Col_PauseGC(void);
EXTERN int		Col_TryPauseGC(void);
EXTERN void		Col_ResumeGC(void);

/*
 * end block for C++
 */

#ifdef __cplusplus
}
#endif

#endif /* _COLIBRI */
