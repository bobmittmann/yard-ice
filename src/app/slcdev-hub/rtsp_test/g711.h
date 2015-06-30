#ifndef __G711_H__
#define __G711_H__

#include <stdint.h>
#include <arch/cortex-m3.h>

#define ALAW_AMI_MASK       0x55


static inline int __top_bit(uint16_t val)
{
	return 31 - __clz((uint16_t)val);
}

#if 0
static inline int __top_bit(uint16_t bits)
{
	int16_t i;

	if (bits == 0)
		return -1;

	i = 0;

	if (bits & 0xff00) {
		bits &= 0xff00;
		i += 8;
	}
	if (bits & 0xf0f0) {
		bits &= 0xf0f0;
		i += 4;
	}
	if (bits & 0xcccc) {
		bits &= 0xcccc;
		i += 2;
	}
	if (bits & 0xaaaa) {
		bits &= 0xaaaa;
		i += 1;
	}
	return i;
}
#endif

static inline uint8_t linear2alaw(int16_t linear)
{
	unsigned int mask;
	int16_t seg;

	if (linear >= 0) {
		/* Sign (bit 7) bit = 1 */
		mask = ALAW_AMI_MASK | 0x80;
	} else {
		/* Sign (bit 7) bit = 0 */
		mask = ALAW_AMI_MASK;
		linear = -linear - 8;
	}

	/* Convert the scaled magnitude to segment number. */
	seg = __top_bit(linear | 0xff) - 7;
	if (seg >= 8) {
		if (linear >= 0) {
			/* Out of range. Return maximum value. */
			return (uint8_t)(0x7f ^ mask);
		}
		/* We must be just a tiny step below zero */
		return (uint8_t)(0x00 ^ mask);
	}
	/* Combine the sign, segment, and quantization bits. */
	return (uint8_t)(((seg << 4) | 
					   ((linear >> ((seg) ? (seg + 3) : 4)) & 0x0f)) ^ mask);
}

static inline int16_t alaw2linear(uint8_t alaw)
{
	int16_t i;
	int16_t seg;

	alaw ^= ALAW_AMI_MASK;

	i = ((alaw & 0x0F) << 4);

	seg = (((int16_t) alaw & 0x70) >> 4);

	if (seg)
		i = (i + 0x108) << (seg - 1);
	else
		i += 8;

	return (int16_t) ((alaw & 0x80) ? i : -i);
}

#endif /* __G711_H__ */

