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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dcclog.h>

#include "debugger.h"

int cmd_dynfs_ls(FILE * f, int argc, char ** argv)
{
	struct dynfs * fs = dynfs_get_instance(0);
	struct dynfs_dirent dent;
	struct dynfs_dir * dir;

	dir = dynfs_opendir(fs, NULL);

	for (;;) {
		if (dynfs_readdir(dir, &dent) <= 0)
			break;
		
		fprintf(f, "%s %d\n", dent.d_name, dent.d_size);
	}

	dynfs_closedir(dir);
	return 0;
}

int cmd_dynfs_cat(FILE * f, int argc, char ** argv)
{
	struct dynfs * fs = dynfs_get_instance(0);
	struct dynfs_dirent dent;
	struct dynfs_dir * dir;
	struct dynfs_file * file;
	int i;

	if ((dir = dynfs_opendir(fs, NULL)) == NULL) {
		return 0;
	}

	for (i=0; i < 2; ++i) {
		char buf[257];
		int offs;
		int n;

		if (dynfs_readdir(dir, &dent) == 0)
			break;
		
		if ((file = dynfs_fopen(fs, dent.d_name)) == NULL)
			break;
		offs = 0;
		while ((n = dynfs_fread(file, buf, offs, sizeof(buf) - 1)) > 0) {
			buf[n] = '\0';
			fputs(buf, f);
			offs += n;
		}
		dynfs_fclose(file);
	}

	dynfs_closedir(dir);
	
	return 0;
}
