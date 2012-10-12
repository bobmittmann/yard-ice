/* 
 * File:	fgetc.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2006-2007 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdio.h>
#include <sys/file.h>

int fgetc(FILE * f)
{
	char c;

	if (f->op->read(f->data, (void *)&c, sizeof(char)) <= 0) {
		return EOF;
	}

	return c;
}

