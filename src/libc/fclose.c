/* $Id: fclose.c,v 1.2 2008/04/17 18:24:21 bob Exp $ 
 *
 * File:	fclose.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdio.h>
#include <sys/file.h>

int fclose(FILE * f)
{
	return f->op->close(f->data);
}

