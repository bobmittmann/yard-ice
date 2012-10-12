/* $Id: fputs.c,v 1.3 2008/04/17 18:24:21 bob Exp $ 
 *
 * File:	fputs.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2006-2007 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/file.h>

int fputs(const char * s, FILE * f)
{
	return f->op->write(f->data, s, strlen(s));
}

