
#ifndef __FIXPT_H__
#define __FIXPT_H__

#include <stdint.h>

// Conversion form float to fixed point Q1.15
#define Q15(F) ((int32_t)((F) * 32768))
// Convert from fractional Q1.15 to float point
#define Q15F(Q) ((float)(((float)(Q)) / 32768.0))
// Q15 Multiply

#define Q15_MUL(X1, X2) (((int32_t)(X1) * (int32_t)(X2) + (1 << 14)) >> 15)
//#define Q15_MUL(X1, X2) (((int32_t)(X1) * (int32_t)(X2)) >> 15)

#define Q15_UMUL(X1, X2) ((((uint32_t)(X1) * (uint32_t)(X2)) + (1 << 14)) >> 15)

// Q15 Divide
#define Q15_DIV(X, Y) (((X) << 15) / (Y))
// Q15 Saturation
#define Q15_SAT(X) ((X) < -32768) ? -32768 : (((X) > 32767) ? 32767 : (X))

typedef struct {
	int16_t re;  
	int16_t im;  
} cplx16_t; 

#define Q15_ABS

int32_t isqrt(uint32_t x);

static inline uint16_t cplx16_abs(cplx16_t z) {
	uint32_t x;
	uint32_t y;

	x = z.re * z.re;
	y = z.im * z.im;

	return isqrt(x + y);
}

extern const uint16_t q15_db2amp_ltu[];
extern const int8_t q15_db2amp_min;

extern const uint16_t q15_db2pwr_ltu[];
extern const int8_t q15_db2pwr_min;

const uint16_t q15_db2amp(int amp);

const uint16_t q15_db2pwr(int pwr);

#endif // __FIXPT_H__

