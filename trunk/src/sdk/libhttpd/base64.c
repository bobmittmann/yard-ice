/*
 * Copyright(c) 2004-2014 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libhttpd.
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
 * @file base64.c
 * @brief HTTPD library
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

const signed char _base64_[128] = {
/* x00 x01 x02 x03 x04 x05 x06 x07 */
	-1, -1, -1, -1, -1, -1, -1, -1,	
/* x08 x09 x0a x0b x0c x0d x0e x0f */
	-1, -1, -1, -1, -1, -1, -1, -1,	
/* x10 x11 x12 x13 x14 x15 x16 x17 */
	-1, -1, -1, -1, -1, -1, -1, -1,	
/* x18 x19 x1a x1b x1c x1d x1e x1f */
	-1, -1, -1, -1, -1, -1, -1, -1,	
/*       !   "   #   $   %   &   ' */
	-1, -1, -1, -1, -1, -1, -1, -1,	
/*   (   )   *   +   ,   -   .   / */
	-1, -1, -1, 62, -1, 62,  0, 63,
/*	 0   1   2   3   4   5   6   7 */
	52, 53, 54, 55, 56, 57, 58, 59,
/*	 8   9   :   ;   <   =   >   ? */
	60, 61, -1, -1, -1,  0, -1, -1,
/*	 @   A   B   C   D   E   F   G */
	-1,  0,  1,  2,  3,  4,  5,  6,
/*	 H   I   J   K   L   M   N   O */
	 7,  8,  9, 10, 11, 12, 13, 14,
/*	 P   Q   R   S   T   U   V   W */
	15, 16, 17, 18, 19, 20, 21, 22,
/*	 X   Y   Z   [   \   ]   ^   _ */
	23, 24, 25, -1, -1, -1, -1, 63,
/*	 `   a   b   c   d   e   f   g */
	-1, 26, 27, 28, 29, 30, 31, 32,
/*	 h   i   j   k   l   m   n   o */
	33, 34, 35, 36, 37, 38, 39, 40,
/*	 p   q   r   s   t   u   v   w */
	41, 42, 43, 44, 45, 46, 47, 48,
/*	 x   y   z  {   |   }   ~   ¦ */
	49, 50, 51, -1, -1, -1, -1, -1
};

