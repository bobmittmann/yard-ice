/* $Id: fputc.c,v 1.2 2008/04/17 18:24:21 bob Exp $ 
 *
 * File:	fputc.c
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

int fputc(int c, FILE * f)
{
	char buf[1];

	buf[0] = c;

	f->op->write(f->data, (void *)buf, sizeof(char));

	return c;
}

