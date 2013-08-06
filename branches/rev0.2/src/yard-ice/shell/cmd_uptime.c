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
 * @file cmd_uptime.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <tcpip/tnshell.h>

unsigned int uptime(void);

int shell_cmd_uptime(int argc, char ** argv, struct tty * tty, void * data)
{
	char s[64];
	int n;
	time_t now;
	struct tm tm;
	int hour;
	int min;
	int sec;
	unsigned int tenth;

	now = time(NULL);
	gmtime_r(&now, &tm);

	tenth = uptime();

	sec = tenth / 100;
	tenth -= sec * 100;

	min = sec / 60;
	sec -= min * 60;

	hour = min / 60;
	min -= hour * 60;

	n = sprintf(s, "\n%02d:%02d up %d:%02d:%02d.%02d\n\n",
				tm.tm_hour, tm.tm_min, hour, min, sec, tenth);

	shell_write(s, n, tty);

	return 0;
}

