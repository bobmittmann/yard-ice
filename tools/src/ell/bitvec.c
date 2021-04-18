/*
 * LL(1) Predictive Parser Table Generator and RDP Generator
 * 
 * This file is part of Ell.
 *
 * Ell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/*! 
 * \file      bitvec.c
 * \brief     Bit vector handling routines
 * \author    Ivo Mateljan
 * \copyright 2003-2014, Ivo Mateljan
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ell.h"

static int bitvecsize; /* number of elements in a BITVEC */

/* Package of bit vector routines */
BITVEC BitVecNew() 
{
    BITVEC vp = (BITVEC ) calloc( (unsigned)bitvecsize, sizeof(int) );
	if (vp == NULL) {
		fprintf(stderr, "\nMemory error\n"); 
		exit(1);
	}
	/* bitvecsize = (lastsymnum + 31) >> 5; */
    return( vp );
}

void BitVecSetSize() 
{        
    bitvecsize = (lastsymnum + 31) >> 5;    
}

int BitVecUnion(BITVEC x,  BITVEC y)
{
    int i,old, changed=false;

    for( i=0; i < bitvecsize; i++ ) { 
		old = x[i];
		x[i]  |=  y[i];
		if(!changed)
			changed = (x[i] != old);
	}
	return changed;
}

int BitVecEmpty(BITVEC x)
{
    int i;
    for( i=0; i < bitvecsize; i++ ) { 		
		if (x[i])
			return false;
	}
	return true;
}

int BitVecHasIntersection(BITVEC x,  BITVEC y)
{
    int i;
    for( i=0; i < bitvecsize; i++ ) { 		
		if (x[i]  & y[i])
			return true;
	}
	return false;
}

void BitVecClear(BITVEC x)
{
    int i;
    for( i=0; i < bitvecsize; i++ )	
		x[i] =0;
	
}

int BitVecTest(BITVEC x, int pos)
{
	return ( x[pos >> 5] & (1 << (pos & 31)) );
}

int BitVecNumEl(BITVEC x)
{
	int i,num;
	num=0;
	for( i=0; i < lastsymnum; i++ )	{
		if( BitVecTest(x, i) ) 
		  num++;
	}
	return num;
}

/* x >> 5 is iqual to x/32 */
/* 31 is equal to 0x1F */
/* return true if change */

int BitVecAdd(BITVEC x, int pos)
{
    int old = BitVecTest(x, pos);
	x[pos >> 5] |= (1 << (pos & 31));
	return old != BitVecTest(x, pos);
}


