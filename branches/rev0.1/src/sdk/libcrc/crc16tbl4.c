#include <stdlib.h>
#include <stdint.h>

#ifdef CONFIG_H
#include "config.h"
#endif

unsigned int crc16tbl4(const uint8_t *data, size_t data_len)
{
	unsigned int crc = 0xffff;
	
	static const uint16_t crc_table[32] = {
		0x0000, 0x1081, 0x2102, 0x3183, 0x4204, 0x5285, 0x6306, 0x7387,
		0x8408, 0x9489, 0xa50a, 0xb58b, 0xc60c, 0xd68d, 0xe70e, 0xf78f,
		0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
		0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7
	};

    while (data_len--) {
		crc ^= *data++;
		crc = (crc >> 8) ^ 
			crc_table[(crc >> 4) & 15] ^ 
			crc_table[(crc & 15) + 16];
    }
    return crc ^ 0xffff;
}

#if CRC16 == CRC16TBL4
unsigned int crc16(const void *, size_t) 
	__attribute__ ((weak, alias ("crc16tbl4")));
#endif 

