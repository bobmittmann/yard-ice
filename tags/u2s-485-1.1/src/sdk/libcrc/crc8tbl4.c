#include <stdlib.h>
#include <stdint.h>

#ifdef CONFIG_H
#include "config.h"
#endif

unsigned int crc8tbl4(const uint8_t *data, size_t len)
{
	unsigned crc = 0xff;
	unsigned idx;

	static const uint8_t crc_lut[]  = {
		0x00, 0x1c, 0x38, 0x24, 0x70, 0x6c, 0x48, 0x54,
		0xe0, 0xfc, 0xd8, 0xc4, 0x90, 0x8c, 0xa8, 0xb4,
		0x00, 0x91, 0xe3, 0x72, 0x07, 0x96, 0xe4, 0x75,
		0x0e, 0x9f, 0xed, 0x7c, 0x09, 0x98, 0xea, 0x7b

	};

	while (len--) {
		idx = crc ^ *data;
		crc = crc_lut[(idx & 0x0f) + 16] ^ crc_lut[idx >> 4];
		data++;
	}

	return crc ^ 0xff;
}

#if CRC8 == CRC8TBL4
unsigned int crc8(const void *, size_t) 
	__attribute__ ((weak, alias ("crc8tbl4")));
#endif 

