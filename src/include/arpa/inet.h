/* $Id: inet.h,v 2.0 2006/03/28 15:10:05 bob Exp $ 
 *
 * File:	arpa/inet.h
 * Module:	ulibc
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment:
 * Copyright(c) 2004 CNX Technologies. All Rights Reserved.
 *
 */

/*! \defgroup comm Communication
 *	
 *	To use this library, you must link with (at least) \b tcpip, \b mbuf and 
 *	\b mlink and the usual \b c, \b a7e10x2 and \b gcc (needed for all 
 *	projects), in that order.
 *	If you wish to use the DHCP client module, add \b dhcp on the beginning 
 *	of that list.
 *
 * @{
 */
/*! \defgroup arpa_inet arpa/inet.h
 * @{
 */	 
/*! \file
 *	Revision tag: $Revision: 2.0 $ $Date: 2006/03/28 15:10:05 $
 *	@author Robinson Mittman(bob@boreste.com)
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
