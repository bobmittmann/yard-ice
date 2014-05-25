#include <stdlib.h>
#include <stdint.h>

#ifdef CONFIG_H
#include "config.h"
#endif

unsigned int crc16bwe(const void * buf, size_t len)
{
	unsigned int crc = 0xffff;
	uint8_t * cp;

	cp = (uint8_t *)buf;

    while (len--) {
		crc = (crc >> 8) | ((crc & 0xff) << 8);
		crc ^= (unsigned int)*cp++ << 8;
		crc ^= (crc << 4) & 0xf000;
		crc ^= (crc >> 12) & 0x000f;
		crc ^= (crc >> 5) & 0x07f8;
	}

	return crc ^ 0xffff;
}

#if CRC16 == CRC16_BWE
unsigned int crc16(const void *, size_t) 
	__attribute__ ((weak, alias ("crc16bwe")));
#endif 

