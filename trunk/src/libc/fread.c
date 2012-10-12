/* $Id: fread.c,v 2.2 2008/05/01 01:14:41 bob Exp $
 *
 * File:	fread.c	
 * Module:	ulibc
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdio.h>
#include <sys/file.h>

size_t fread(void * ptr, size_t size, size_t  nmemb, FILE * f)
{
	return f->op->read(f->data, ptr, size * nmemb);
}

