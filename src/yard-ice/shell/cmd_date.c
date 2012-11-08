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
 * @file cmd_date.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/shell.h>
#include <sys/clock.h>

static const char * const month[12] = {
	"Jan", "Fev", "Mar", "Apr", "May", "Jun", 
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

static const char * const wday[7] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

void date_help(FILE * f, char * s)
{
	fprintf(f, "usage: %s [MMDDhhmm[[CC]YY][.ss]]\n", s);
}

int cmd_date(FILE *f, int argc, char ** argv)
{
	struct timespec t;
	struct tm tm;
	time_t now;
	char c;
	char c1;
	char * cp;
	int i;

	if (argc > 2) {
		date_help(f, argv[0]);
		return -1;
	}

	if (argc > 1) {
		i = 0;
		for (cp = argv[1]; (c = *cp); cp++) {
			if ((c < '0') || (c > '9'))
				break;
			i++;
		}
		if (c == '?') {
			date_help(f, argv[0]);
			return 0;
		}
		
		if ((i < 8) || (i > 12))
			return -1;

		cp = argv[1];
		c = *cp++ - '0';
		tm.tm_mon = (c * 10) + *cp++ - '0'; 
		c = *cp++ - '0';
		tm.tm_mday = (c * 10) + *cp++ - '0'; 
		c = *cp++ - '0';
		tm.tm_hour = (c * 10) + *cp++ - '0'; 
		c = *cp++  - '0';
		tm.tm_min = (c * 10) + *cp++ - '0'; 
		switch (i) {
		case 8:
			break;
		case 10:
			c = *cp++ - '0';
			tm.tm_year = (c * 10) + *cp++ - '0';
			tm.tm_year += (tm.tm_year > 69) ? 1900 : 2000;
			break;
		case 12:
			c = *cp++ - '0';
			c1 = *cp++ - '0';
			tm.tm_year = (c * 1000) + c1 * 100;
			c = *cp++ - '0';
			tm.tm_year += (c * 10) + *cp++  - '0';		
			break;
		default:
			return -1;	
		}
		
		if (*cp == '.') {
			cp++;
			c = *cp++;
			c1 = *cp++;
			if (*cp != '\0') {
				printf("CP = %x\n", *cp);
				return -1;
			}
			
			if ((c < '0') || (c > '9'))
				return -1;
			if ((c1 < '0') || (c1 > '9'))
				return -1;
				
			tm.tm_sec = ((c - '0') * 10) + c1 - '0';
		}
		
		t.tv_sec = mktime(&tm);
		t.tv_nsec = 0;
		clock_settime(CLOCK_REALTIME, &t);
	}

	now = time(NULL);
	gmtime_r(&now, &tm);

	fprintf(f, "%s %s %2d %2d:%02d:%02d %d\n", wday[tm.tm_wday], 
		month[tm.tm_mon], tm.tm_mday, 
		tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_year);

	return 0;
}

