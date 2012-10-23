/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <arpa/inet.h>
#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <netinet/in.h>

int cmd_reboot(FILE * f, int argc, char ** argv);

int cmd_ipcfg(FILE *f, int argc, char ** argv)
{
	struct ifnet * ifn;
//	struct route * rt;
	in_addr_t ip_addr;
	in_addr_t netmask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	int use_dhcp = 0;
	int change = 0;
	char s[64];
	char ip[16];
//	char * env;
	char * cp;
	int c;

	fprintf(f, "\n** IP configuration utility **\n\n");

	/* get current configuration */
	if ((ifn = get_ifn_byname("eth0")) == NULL)
		return -1;

#if 0
	FIXME: 
	ip_addr = ifn->if_ipv4_addr;
	netmask = ifn->if_ipv4_mask;
	if ((rt = route_get(INADDR_ANY, INADDR_ANY)) == NULL)
		gw_addr = INADDR_ANY;
	else
		gw_addr = rt->rt_gateway;
	use_dhcp = 0;

	/* get environment variable */
	if ((env=getenv("IPCFG")) != NULL) {
		strcpy(s, env);

		if (!inet_aton(strtok(s, " ,"), (struct in_addr *)&ip_addr)) {
			return -1;
		}

		if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&netmask)) {
			if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&gw_addr)) {
				use_dhcp = strtoul(strtok(NULL, " , "), NULL, 0);
			}
		}
	} else {
		ip_addr = ifn->if_ipv4_addr;
		netmask = ifn->if_ipv4_mask;
	}
#endif

	for(;;) {
		fprintf(f, " - IP addr(%s): ", 
		   inet_ntop(AF_INET, (void *)&ip_addr, ip, 16));
		fgets(s, 32, f);
		if (s[0] == '\n')
			break;
		if (inet_aton(s, (struct in_addr *)&ip_addr)) {
			change++;
			break;
		}
	}

	for(;;) {
		fprintf(f, " - Network mask(%s): ", 
		   inet_ntop(AF_INET, (void *)&netmask, ip, 16));
		fgets(s, 32, f);
		if (s[0] == '\n')
			break;
		if (inet_aton(s, (struct in_addr *)&netmask)) {
			change++;
			break;
		}
	}

	for (;;) {
		fprintf(f, " - Gateway addr(%s): ", 
		   inet_ntop(AF_INET, (void *)&gw_addr, ip, 16));
		fgets(s, 32, f);
		if (s[0] == '\n')
			break;
		if (inet_aton(s, (struct in_addr *)&gw_addr)) {
			change++;
			break;	
		}
	} 

	for (;;) {
		fprintf(f, " - Enable DHCP [y/n]? (%c): ", use_dhcp ? 'y' : 'n');
		fgets(s, 32, f);
		if (s[0] == '\n')
			break;
		c = tolower(s[0]);
		if ((c == 'y') || (c == 'n')) {
			use_dhcp = (c == 'y') ? 1 : 0;
			change++;
			break;
		}
	};

	if (!change) {
		fprintf(f, "\nKeeping current configuration.\n");
		return 0;
	}

	cp = s + sprintf(s, "%s", inet_ntop(AF_INET, (void *)&ip_addr, ip, 16));
	cp += sprintf(cp, " %s", inet_ntop(AF_INET, (void *)&netmask, ip, 16));
	cp += sprintf(cp, " %s", inet_ntop(AF_INET, (void *)&gw_addr, ip, 16));
	sprintf(cp, " %d", use_dhcp);

	if (setenv("IPCFG", s, 1) < 0) {
		fprintf(f, "setenv() error!\n");
		return -1; 
	}

	fprintf(f, "\nConfiguration saved.\n");

	fprintf(f, "\n - Restart the system [y/n]? ");
	fgets(s, 32, f);

	if ((tolower(s[0]) == 'y')) {
		cmd_reboot(f, 0, NULL);
	};

	return 0;
}

