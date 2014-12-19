#include <stdlib.h>
#include <stdint.h>

#ifdef CONFIG_H
#include "config.h"
#endif

static inline unsigned int crc_bitwise_expression(unsigned int idx)
{
	unsigned int bits = idx;

    return (((bits >> 2) ^ (bits >> 1) ^ bits) & 0x0f) |
           (((bits >> 2) ^ (bits >> 1) ^ bits ^ (bits << 4)) & 0x30) |
           (((bits >> 1) ^ bits ^ (bits << 5)) & 0x40) |
           ((bits ^ (bits << 6) ^ (bits << 7)) & 0x80);
}

unsigned int crc8bwe(const uint8_t * data, size_t len)
{
	unsigned int crc = 0xff;
	unsigned int idx;

	while (len--) {
		idx = crc ^ *data;
		crc = crc_bitwise_expression(idx);
		data++;
	}

	return crc ^ 0xff;
}

#if CRC8 == CRC8BWE
unsigned int crc8(const void *, size_t) 
	__attribute__ ((weak, alias ("crc8bwe")));
#endif 

