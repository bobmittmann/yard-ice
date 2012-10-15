/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file arpa/inet.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __ARPA_INET_H__
#define	__ARPA_INET_H__

/* To define `struct in_addr'.  */
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Convert Internet host address from numbers-and-dots notation in CP
   into binary data in network byte order.  */
in_addr_t inet_addr(const char * cp);

/*!	\brief Converts Internet host address to Internet dot address
 *	
 *	Converts the specified host address to a string in the Internet standard
 *	dot notation.
 *	\param addr \n
 *	The structure containing the Internet host address to be converted.
 *	\return
 *	A pointer to the network address in Internet standard dot notation.
 */
char * inet_ntoa(struct in_addr addr);

/*!	\brief Converts address from network format to presentation format.
 *
 *	This function converts an Internet address (either IPv4 or IPv6) from 
 *	network (binary) to presentation (textual) form.
 *	\ingroup inet
 *	\param af \n
 *	The address family. Should be either AF_INET or AF_INET6, as appropriate.
 *	\param cp \n
 *	A pointer to the address to be converted.
 *	\param buf \n
 *	A pointer to a buffer that stores the result. 
 *	\param len \n
 *	The length of <i>buf</i>.
 *	\return	
 *	The return value from the function is the \a buf address.
 */
const char * inet_ntop(int af, const void * cp, char * buf, int len);

/*!	\brief Converts from Internet host address to binary data.
 *	
 *	Converts the <i>name</i> string, in the Internet standard dot notation, to 
 *	a network address, storing it in the structure <i>addr</i>.
 *	\param name \n
 *	String containing the address in the standard numbers-and-dots notation.
 *	\param addr \n
 *	Buffer where the converted address is to be stored.
 *	\return
 *	Returns <b>1</b> if the address is successfully converted or <b>0</b> if 
 *	the conversion fails.
 */
int inet_aton(const char * name, struct in_addr * addr);

#ifdef __cplusplus
}
#endif

#endif /* _ARPA_INET_H_ */
/*! @} */
/*! @} */
