/*
   crc5tbl4.c
   CRC5
   Copyright(C) 2013 Robinson Mittmann.
 */

#include <stdlib.h>
#include <stdint.h>

#ifdef CONFIG_H
#include "config.h"
#endif

uint8_t crc5tbl4(const uint8_t *data, size_t len)
{
	unsigned crc = 0x1f;
	unsigned idx;

	static const uint8_t crc5lut[]  = {
		0x00, 0x16, 0x05, 0x13, 0x0a, 0x1c, 0x0f, 0x19,
		0x14, 0x02, 0x11, 0x07, 0x1e, 0x08, 0x1b, 0x0d,
		0x00, 0x0e, 0x1c, 0x12, 0x11, 0x1f, 0x0d, 0x03,
		0x0b, 0x05, 0x17, 0x19, 0x1a, 0x14, 0x06, 0x08
	};

	while (len--) {
		idx = crc ^ *data;
		crc = crc5lut[(idx & 0x0f) + 16] ^ crc5lut[idx >> 4];
		data++;
	}

	return crc ^ 0x1f;
}

#if CRC5 == CRC5TBL4
unsigned int crc5(const void *, size_t) 
	__attribute__ ((weak, alias ("crc5tbl4")));
#endif 

