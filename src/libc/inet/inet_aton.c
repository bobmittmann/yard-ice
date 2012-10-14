/* $Id: inet_aton.c,v 2.1 2006/05/21 18:32:50 bob Exp $ 
 *
 * File:	inet_aton.c
 * Module:
 * Project:	AT91X40DK	
 * Author:	Robinson Mittmann (bob@cnxtech.com)
 * Target:	at91x40
 * Comment:
 * Copyright(c) 2003 CNX Technologies. All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <ctype.h>
#include <netinet/in.h>

int inet_aton(const char * name, struct in_addr * addr)
{
	unsigned char * ip;
	int a1;
	int a2;
	int a3;
	int a4;
	char * cp;

	for (cp = (char *)name; isspace(*cp); cp++);

	ip = (unsigned char *)addr;

	if (*cp == '*') {
		ip[0] = 0;
		ip[1] = 0;
		ip[2] = 0;
		ip[3] = 0;
		return 1;
	}

 	a1 = strtoul(cp, &cp, 0);
 	if ((*cp++ != '.' ) || (a1 > 255))
		return 0;
		
	a2 = strtoul(cp, &cp, 0);
	if ((*cp++ != '.' ) || (a2 > 255))
		return 0;
		
 	a3 = strtoul(cp, &cp, 0);
	if ((*cp++ != '.' ) || (a3 > 255))
		return 0;

	a4 = strtoul(cp, &cp, 0);

	for (;isspace(*cp); cp++);

	if ((*cp != '\0' ) || (a4 > 255))
		return 0;

#if __BYTE_ORDER == __LITTLE_ENDIAN
	ip[0] = a1;
	ip[1] = a2;
	ip[2] = a3;
	ip[3] = a4;
#elif __BYTE_ORDER == __BIG_ENDIAN
	ip[3] = a1;
	ip[2] = a2;
	ip[1] = a3;
	ip[0] = a4;
#else
# error "Please fix <bits/endian.h>"
#endif

	return 1;
}

