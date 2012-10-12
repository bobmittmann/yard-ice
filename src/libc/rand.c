/* $Id: rand.c,v 2.7 2006/09/27 03:26:49 bob Exp $ 
 *
 * File:	rand.c
 * Module:	ulibc
 * Project:	
 * Author:    Robinson Mittmann (bobmittmann@gmail.com bob@boreste.com)
 * Target:    
 * Comment:
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <stdint.h>

uint64_t randseed = 1LL;

int rand(void)
{
	randseed = (randseed * 6364136223846793005LL) + 1;
	return (int)((randseed >> 32) & RAND_MAX);
}

void srand(unsigned int seed)
{
	randseed = seed & RAND_MAX;
}

