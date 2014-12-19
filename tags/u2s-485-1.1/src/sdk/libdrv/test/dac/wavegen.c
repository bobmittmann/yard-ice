#include "wavegen.h"

/* Convert form float to fixed point Q1.15 */
#define Q15(F) ((int32_t)((F) * (1 << 15)))
/* Q1.15 Multiply */
#define Q15_MUL(X, Y) (((X) * (Y) + (1 << 14)) >> 15)
/* Q1.15 Saturation */
#define Q15_SAT(X) ((X) < -32768) ? -32768 : (((X) > 32767) ? 32767 : (X))

void wavegen_init(struct wavegen * gen, int16_t c, int16_t ampl,
                  unsigned int frame_len)
{
	gen->x = Q15(-1.0);
	gen->y = (c + 1) / 2;
	gen->c = c;
	gen->a = ampl;
	gen->frame_len = frame_len;
}

static inline int32_t __attribute__((always_inline)) __ssat16(int32_t val) {
	register int32_t ret;
	asm volatile ("ssat %0, #16, %1" : "=r" (ret) : "r" (val));
	return ret;
}

void wavegen_apply(struct wavegen * gen, int16_t out[])
{
	int32_t x0;
	int32_t y0;
	int32_t x;
	int32_t y;
	int16_t c;
	int16_t a;
	int i;

	x = gen->x;
	y = gen->y;
	a = gen->a;
	c = gen->c;

	for (i = 0; i < gen->frame_len; i++) 
	{
		x0 = x;
		y0 = y;

		x = x0 - Q15_MUL(c, y0);
		y = Q15_MUL(c, x)  + y0;

		// saturate 
//		x = Q15_SAT(x);
		x = __ssat16(x);

		out[i] = Q15_MUL(a, x);
	}

	gen->x = x;
	gen->y = y;
}

