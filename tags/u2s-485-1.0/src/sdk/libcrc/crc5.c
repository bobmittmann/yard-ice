/*
   crc5bwe.c
   CRC5
   Copyright(C) 2013 Robinson Mittmann.
 */

#include <stdlib.h>
#include <stdint.h>

unsigned int crc5(const void * data, size_t len)
{
	uint8_t * cp = (uint8_t *)data;
	uint8_t * end = cp + len;
	register unsigned int crc = 0x1f;

	while (cp != end) {
		register unsigned int a;

		crc ^= (unsigned int)*cp++;

		a = (crc << 2) ^ (crc << 3);
		crc ^= (crc >> 3) ^ a ^ (a << 4);
		crc = (crc & 0x18) | ((crc >> 5) & 0x07);
	}

	return crc ^ 0x1f;
}

