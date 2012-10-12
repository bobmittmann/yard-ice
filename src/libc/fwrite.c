/* $Id: fwrite.c,v 1.2 2008/04/17 18:24:21 bob Exp $ 
 *
 * File:	fwrite.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment:
 * Copyright(c) 2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdio.h>
#include <sys/file.h>

size_t fwrite(const void * ptr, size_t size, size_t nmemb, FILE * f)
{
	return f->op->write(f->data, ptr, size * nmemb);
}

