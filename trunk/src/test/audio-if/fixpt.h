
#ifndef __FIXPT_H__
#define __FIXPT_H__

#include <stdint.h>

// Conversion form float to fixed point Q1.15
#define Q15(F) ((LWORD_S)((F) * 32768))
// Convert from fractional Q1.15 to float point
#define Q15F(Q) ((float)(((float)(Q)) / 32768.0))
// Q15 Multiply
#define Q15_MUL(X, Y) (((X) * (Y) + (1 << 14)) >> 15)
// Q15 Divide
#define Q15_DIV(X, Y) (((X) << 15) / (Y))
// Q15 Saturation
#define Q15_SAT(X) ((X) < -32768) ? -32768 : (((X) > 32767) ? 32767 : (X))

typedef struct {
	int16_t re;  
	int16_t im;  
} cplx16_t; 

#define Q15_ABS

static inline int32_t int32_sqrt(int32_t x)
{
	int32_t res = 0;
	int32_t one = 1uL << ((sizeof(int32_t ) * 8) - 2);

	// "one" starts at the highest power of four <= than the argument.
	while (one > x)
		one >>= 2;

	while (one != 0) {
		if (x >= res + one) {
			x = x - (res + one);
			res = res +  2 * one;
		}
		res >>= 1;
		one >>= 2;
	}

	// arithmetic rounding to nearest integer
	return (x > res) ? res + 1 : res;
}

static inline int16_t cplx16_abs(cplx16_t z) {
	int32_t x;
	int32_t y;

	x = z.re * z.re;
	y = z.im * z.im;

	return int32_sqrt(x + y);
}


#endif // __FIXPT_H__

