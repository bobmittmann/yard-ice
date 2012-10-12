#include <stdint.h>

uint32_t __bswap_32(uint32_t __val)
{
	return ((__val & 0xff) << 24) |
		((__val & 0xff00) << 8) |
		((__val & 0xff0000) >> 8) |
		((__val & 0xff000000) >> 24);
}

