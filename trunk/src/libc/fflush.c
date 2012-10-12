/*
 * File:	fflush.c
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

int fflush(FILE * f)
{
	return f->op->flush(f->data);
}

