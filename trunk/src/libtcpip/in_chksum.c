/* $Id: in_chksum.c,v 2.1 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	in_chksum.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment:	
 * Copyright(c) 2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdint.h>

uint32_t in_chksum(uint32_t __sum, const void * __data, int __len) 
{
	uint8_t * cp;
	uint16_t * hp;

	if (((uint32_t)__data & 0x1) == 0) {
		hp = (uint16_t *)__data;

		while (__len >= (8 * sizeof(uint16_t))) {
			__sum += hp[0];
			__sum += hp[1];
			__sum += hp[2];
			__sum += hp[3];
			__sum += hp[4];
			__sum += hp[5];
			__sum += hp[6];
			__sum += hp[7];
			__len -= (8 * sizeof(uint16_t));
			hp += 8;
		}

		if (__len >= (4 * sizeof(uint16_t))) {
			__sum += hp[0];
			__sum += hp[1];
			__sum += hp[2];
			__sum += hp[3];
			__len -= (4 * sizeof(uint16_t));
			hp += 4;
		}

		if (__len >= (2 * sizeof(uint16_t))) {
			__sum += hp[0];
			__sum += hp[1];
			__len -= (2 * sizeof(uint16_t));
			hp += 2;
		}

		if (__len >= sizeof(uint16_t)) {
			__sum += hp[0];
			__len -= sizeof(uint16_t);
			hp ++;
		}

		cp = (uint8_t *)hp;
	} else {
		cp = (uint8_t *)__data;

		while (__len >= (8 * sizeof(uint16_t))) {
			__sum += cp[0] + (cp[1] << 8);
			__sum += cp[2] + (cp[3] << 8);
			__sum += cp[4] + (cp[5] << 8);
			__sum += cp[6] + (cp[7] << 8);
			__sum += cp[8] + (cp[9] << 8);
			__sum += cp[10] + (cp[11] << 8);
			__sum += cp[12] + (cp[13] << 8);
			__sum += cp[14] + (cp[15] << 8);
			__len -= (8 * sizeof(uint16_t));
			cp += (8 * sizeof(uint16_t));
		}

		if (__len >= (4 * sizeof(uint16_t))) {
			__sum += cp[0] + (cp[1] << 8);
			__sum += cp[2] + (cp[3] << 8);
			__sum += cp[4] + (cp[5] << 8);
			__sum += cp[6] + (cp[7] << 8);
			__len -= (4 * sizeof(uint16_t));
			cp += (4 * sizeof(uint16_t));
		}

		if (__len >= (2 * sizeof(uint16_t))) {
			__sum += cp[0] + (cp[1] << 8);
			__sum += cp[2] + (cp[3] << 8);
			__len -= (2 * sizeof(uint16_t));
			cp += (2 * sizeof(uint16_t));
		}
		
		if (__len >= sizeof(uint16_t)) {
			__sum += cp[0] + (cp[1] << 8);
			__len -= sizeof(uint16_t);
			cp += sizeof(uint16_t);
		}
	}

	if (__len)
		__sum += cp[0];

	while (__sum >> 16) {
		__sum = (__sum & 0xffff) + (__sum >> 16);
	}

	return __sum;
}

