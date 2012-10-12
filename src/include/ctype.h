/* $Id: ctype.h,v 2.1 2006/07/21 16:42:31 bob Exp $ 
 *
 * File:	ctype.h
 * Module:	ulibc
 * Project:
 * Author:	Robinson Mittmann (bob.m@cnxtech.com)
 * Target:
 * Comment:	
 * Copyright(c) 2003-2004 CNX Technologies. All Rights Reserved.
 *
 */

/*! \defgroup ulibc ulibc
 * @{
 */
/*! \defgroup ctype ctype.h
 * @{
 */
/*! \file
 *	Revision tag: $Revision: 2.1 $ $Date: 2006/07/21 16:42:31 $
 *	@author Robinson Mittman(bob@cnxtech.com)
 */

#ifndef __CTYPE_H__
#define __CTYPE_H__

extern const char _ctype_[];

#ifdef __cplusplus
extern "C" {
#endif

int isalnum(int __c);
int isalpha(int __c);
int iscntrl(int __c);
int isdigit(int __c);
int isgraph(int __c);
int islower(int __c);
int isprint(int __c);
int ispunct(int __c);
int isspace(int __c);
int isupper(int __c);
int isxdigit(int __c);

/*! \brief Converts to lowercase ASCII value.
 *
 *	Returns the ASCII value for the lowercase equivalent of the ASCII character <i>c</i>, if it is a letter.
 *	If the character is already lowercase, it remains lowercase.
 *	\param c \n
 *	The character to be converted, if applicable.
 *	\return
 *	The equivalent ASCII for the lowercase character. If already lowercase, remains this way.
 */
int tolower(int __c);

/*! \brief Converts to uppercase ASCII value.
 *
 *	Returns the ASCII value for the uppercase equivalent of the ASCII character <i>c</i>, if it is a letter.
 *	If the character is already uppercase, it remains uppercase.
 *	\param c \n
 *	The character to be converted, if applicable.
 *	\return
 *	The equivalent ASCII for the uppercase character. If already uppercase, remains this way.
 */
int toupper(int __c);

#ifndef __STRICT_ANSI__
int isascii(int __c);
	
/*! \brief Converts a character to its ASCII value.
 *	
 *	Receives a character as an <b>int</b> and returns its corresponding ASCII value.
 *	\param c \n
 *	The character to convert
 *	\return
 *	The ASCII value for <i>c</i>.
 */
int toascii(int __c);

int _tolower(int __c);
int _toupper(int __c);
#endif

#define	_U	01
#define	_L	02
#define	_N	04
#define	_S	010
#define _P	020
#define _C	040
#define _X	0100
#define	_B	0200

#ifndef __cplusplus

#define	isalpha(c)	((_ctype_)[(unsigned)(c)]&(_U|_L))
#define	isupper(c)	((_ctype_)[(unsigned)(c)]&_U)
#define	islower(c)	((_ctype_)[(unsigned)(c)]&_L)
#define	isdigit(c)	((_ctype_)[(unsigned)(c)]&_N)
#define	isxdigit(c)	((_ctype_)[(unsigned)(c)]&(_X|_N))
#define	isspace(c)	((_ctype_)[(unsigned)(c)]&_S)
#define ispunct(c)	((_ctype_)[(unsigned)(c)]&_P)
#define isalnum(c)	((_ctype_)[(unsigned)(c)]&(_U|_L|_N))
#define isprint(c)	((_ctype_)[(unsigned)(c)]&(_P|_U|_L|_N|_B))
#define	isgraph(c)	((_ctype_)[(unsigned)(c)]&(_P|_U|_L|_N))
#define iscntrl(c)	((_ctype_)[(unsigned)(c)]&_C)

#ifdef __GNUC__
#define toupper(c) ({ int __x = (c); islower(__x) ? (__x - 'a' + 'A') : __x;})
#define tolower(c) ({ int __x = (c); isupper(__x) ? (__x - 'A' + 'a') : __x;})
#endif

#endif /* !__cplusplus */

#ifndef __STRICT_ANSI__
#define isascii(c)	((unsigned)(c)<=0177)
#define toascii(c)	((c)&0177)
#endif

#ifdef __cplusplus
}
#endif
#endif /* __CTYPE_H__ */

/*! @} */
/*! @} */
