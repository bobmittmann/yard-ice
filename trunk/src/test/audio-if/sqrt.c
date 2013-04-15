#include <stdint.h>
#include <arch/cortex-m3.h>

const int16_t armRecipTableQ15[64] = {
  0x7F03, 0x7D13, 0x7B31, 0x795E, 0x7798, 0x75E0,
  0x7434, 0x7294, 0x70FF, 0x6F76, 0x6DF6, 0x6C82,
  0x6B16, 0x69B5, 0x685C, 0x670C, 0x65C4, 0x6484,
  0x634C, 0x621C, 0x60F3, 0x5FD0, 0x5EB5, 0x5DA0,
  0x5C91, 0x5B88, 0x5A85, 0x5988, 0x5890, 0x579E,
  0x56B0, 0x55C8, 0x54E4, 0x5405, 0x532B, 0x5255,
  0x5183, 0x50B6, 0x4FEC, 0x4F26, 0x4E64, 0x4DA6,
  0x4CEC, 0x4C34, 0x4B81, 0x4AD0, 0x4A23, 0x4978,
  0x48D1, 0x482D, 0x478C, 0x46ED, 0x4651, 0x45B8,
  0x4521, 0x448D, 0x43FC, 0x436C, 0x42DF, 0x4255,
  0x41CC, 0x4146, 0x40C2, 0x4040
};

#define INDEX_MASK 			0x0000003F

static inline uint32_t arm_recip_q15(int16_t in, int16_t * dst)
{

	uint32_t out = 0, tempVal = 0;
	uint32_t index = 0, i = 0;
	uint32_t signBits = 0;

	if (in > 0) {
		signBits = __clz(in) - 17;
	} else {
		signBits = __clz(-in) - 17;
	}

	/* Convert input sample to 1.15 format */
	in = in << signBits;

	/* calculation of index for initial approximated Val */
	index = in >> 8;
	index = (index & INDEX_MASK);

	/*      1.15 with exp 1  */
	out = armRecipTableQ15[index];

	/* calculation of reciprocal value */
	/* running approximation for two iterations */
	for (i = 0; i < 2; i++)
	{
		tempVal = (int16_t) (((int32_t) in * out) >> 15);
		tempVal = 0x7FFF - tempVal;
		/*      1.15 with exp 1 */
		out = (int16_t) (((int32_t) out * tempVal) >> 14);
	}

	/* write output */
	*dst = out;

	/* return num of signbits of out = 1/in value */
	return (signBits + 1);
}

int16_t q15_sqrt(int16_t x)
{
	int32_t prevOut;
	int16_t oneByOut;
	uint32_t sign_bits;
	int32_t y;

	/* run for ten iterations */

	/* Take xitial guess as half of the xput and first iteration */
	y = ((int32_t) x >> 1u) + 0x3FFF;

	/* Calculation of reciprocal of y */
	/* oneByOut contaxs reciprocal of y which is x 2.14 format   
	   and oneByOut should be upscaled by signBits */
	sign_bits = arm_recip_q15((int16_t) y, &oneByOut);

	/* 0.5 * (y) */
	y = y >> 1u;
	/* prevOut = 0.5 * y + (x * (oneByOut << signBits))) */
	prevOut = y + (((int16_t) (((int32_t) x * oneByOut) >> 16)) << sign_bits);

	/* Third iteration */
	sign_bits = arm_recip_q15((int16_t) prevOut, &oneByOut);
	prevOut = prevOut >> 1u;
	y = prevOut + (((int16_t) (((int32_t) x * oneByOut) >> 16)) << sign_bits);

	sign_bits = arm_recip_q15((int16_t) y, &oneByOut);
	y = y >> 1u;
	prevOut = y + (((int16_t) (((int32_t) x * oneByOut) >> 16)) << sign_bits);

	/* Fifth iteration */
	sign_bits = arm_recip_q15((int16_t) prevOut, &oneByOut);
	prevOut = prevOut >> 1u;
	y = prevOut + (((int16_t) (((int32_t) x * oneByOut) >> 16)) << sign_bits);

	sign_bits = arm_recip_q15((int16_t) y, &oneByOut);
	y = y >> 1u;
	prevOut = y + (((int16_t) (((int32_t) x * oneByOut) >> 16)) << sign_bits);

	/* Seventh iteration */
	sign_bits = arm_recip_q15((int16_t) prevOut, &oneByOut);
	prevOut = prevOut >> 1u;
	y = prevOut + (((int16_t) (((int32_t) x * oneByOut) >> 16)) << sign_bits);

	sign_bits = arm_recip_q15((int16_t) y, &oneByOut);
	y = y >> 1u;
	prevOut = y + (((int16_t) (((int32_t) x * oneByOut) >> 16)) << sign_bits);

	sign_bits = arm_recip_q15((int16_t) prevOut, &oneByOut);
	prevOut = prevOut >> 1u;
	y = prevOut + (((int16_t) (((int32_t) x * oneByOut) >> 16)) << sign_bits);

	/* tenth iteration */
	sign_bits = arm_recip_q15((int16_t) y, &oneByOut);
	y = y >> 1u;
	return y + (((int16_t) (((int32_t) x * oneByOut) >> 16)) << sign_bits);
}

