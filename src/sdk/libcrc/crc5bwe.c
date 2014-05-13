/*
   crc5bwe.c
   CRC5
   Copyright(C) 2013 Robinson Mittmann.
 */

#include <stdlib.h>
#include <stdint.h>

#ifdef CONFIG_H
#include "config.h"
#endif

unsigned int crc5bwe(const void * data, size_t len)
{
	uint8_t * cp = (uint8_t *)data;
	uint8_t * end = cp + len;
	register unsigned int crc = 0x1f;

	while (cp != end) {
		register unsigned int a;

		crc ^= (unsigned int)*cp++;

		a = (crc << 2) ^ (crc << 3);
		crc ^= (crc >> 3) ^ a ^ (a << 4);
		crc = (crc & 0x18) | ((crc >> 5) & 0x07);
	}

	return crc ^ 0x1f;
}

#if 0

unsigned int __crc5bwe(const void * data, size_t len)
{
	uint8_t * cp = (uint8_t *)data;
	uint8_t * end = cp + len;
	register unsigned int crc = 0x1f;
	while (cp != end) {
		crc ^= *cp++;

/*
                        b7 b6 b5 b4 b3 b2 b1 b0
                  b7 b6 b5 b4 b3 b2 b1 b0
               b7 b6 b5 b4 b3 b2 b1 b0
                                 b7 b6 b5 b4 b3 b2 b1 b0
      b7 b6 b5 b4 b3 b2 b1 b0
   b7 b6 b5 b4 b3 b2 b1 b0
                        -- -- -- c4 c3 -- -- --


                                       b7 b6 b5 b4 b3 b2 b1 b0
                                 b7 b6 b5 b4 b3 b2 b1 b0
                              b7 b6 b5 b4 b3 b2 b1 b0
                                                b7 b6 b5 b4 b3 b2 b1 b0
                     b7 b6 b5 b4 b3 b2 b1 b0
                  b7 b6 b5 b4 b3 b2 b1 b0
                        -- -- -- -- -- c2 c1 c0 */

		crc ^= (crc >> 3) ^ (crc << 2) ^ (crc << 3) ^ (crc << 6) ^ (crc << 7);
		crc = (crc & 0x18) | ((crc >> 5) & 0x07);
	}

	return crc ^ 0x1f;
}


/*
                                 b7 b6 b5 b4 b3 b2 b1 b0
                        b7 b6 b5 b4 b3 b2 b1 b0
                  b7 b6 b5 b4 b3 b2 b1 b0
               b7 b6 b5 b4 b3 b2 b1 b0
      b7 b6 b5 b4 b3 b2 b1 b0
   b7 b6 b5 b4 b3 b2 b1 b0
                        -- -- -- c4 c3 -- -- --



                  b7 b6 b5 b4 b3 b2 b1 b0
                                 b7 b6 b5 b4 b3 b2 b1 b0
                                                b7 b6 b5 b4 b3 b2 b1 b0
                                       b7 b6 b5 b4 b3 b2 b1 b0
                              b7 b6 b5 b4 b3 b2 b1 b0
                     b7 b6 b5 b4 b3 b2 b1 b0
                        -- -- -- -- -- c2 c1 c0 */


unsigned int _crc5bwe(const void * data, size_t len)
{
	uint8_t * cp = (uint8_t *)data;
	uint8_t * end = cp + len;
	register unsigned int crc = 0x1f;
	while (cp != end) {
		unsigned int b;
		crc ^= *cp++;
		b = (crc << 2) ^ (crc << 3) ^ (crc >> 5) ^ (crc >> 3);
		crc = ((b ^ crc) & 0x18) | ((b ^ (crc << 1) ^ (crc >> 2)) & 0x07);
	}

	return crc ^ 0x1f;
}

uint8_t crc5bwe(const void * data, size_t data_len)
{
	uint8_t * cp = (uint8_t *)data;
	unsigned int crc = 0x1f;
	unsigned int a;
	unsigned int b;
	unsigned int c;
	unsigned int d;
	unsigned int e;
	unsigned int f;
	unsigned int g;

	(void)a;
	(void)b;
	(void)c;
	(void)d;
	(void)e;
	(void)f;

	while (data_len--) {
		crc ^= *cp++;

		g = crc;

/*
         g7 g6 g5 g4 g3 g2 g1 g0
                  g7 g6 g5 g4 g3 g2 g1 g0
      g7 g6 g5 g4 g3 g2 g1 g0
   g7 g6 g5 g4 g3 g2 g1 g0
                           g7 g6 g5 g4 g3 g2 g1 g0
                     g7 g6 g5 g4 g3 g2 g1 g0
            -- -- -- -- -- e2 e1 e0

      g7 g6 g5 g4 g3 g2 g1 g0
   g7 g6 g5 g4 g3 g2 g1 g0
                           g7 g6 g5 g4 g3 g2 g1 g0
                     g7 g6 g5 g4 g3 g2 g1 g0
            g7 g6 g5 g4 g3 g2 g1 g0
            f7 f6 f5 f4 f3 -- -- --
  */


		a = (g << 1) ^ (g >> 2);
		b = (g << 2) ^ (g << 3);
		c = (g >> 5) ^ (g >> 3);

		e = (c ^ b ^ a) & 0x07;
		f = (c ^ b ^ g) & 0x18;
		crc = e | f;
	}

	return crc ^ 0x1f;
}


static inline uint8_t crc_bitwise_expression(unsigned int b)
{
	unsigned int crc;
	unsigned int a;

/*
                           b7 b6 b5 b4 b3 b2 b1 b0
                     b7 b6 b5 b4 b3 b2 b1 b0
                  b7 b6 b5 b4 b3 b2 b1 b0
            -- -- -- -- -- -- -- c0

                           b7 b6 b5 b4 b3 b2 b1 b0
                     b7 b6 b5 b4 b3 b2 b1 b0
                  b7 b6 b5 b4 b3 b2 b1 b0
         b7 b6 b5 b4 b3 b2 b1 b0
            -- -- -- -- -- -- c1 --

                           b7 b6 b5 b4 b3 b2 b1 b0
                     b7 b6 b5 b4 b3 b2 b1 b0
                  b7 b6 b5 b4 b3 b2 b1 b0
         b7 b6 b5 b4 b3 b2 b1 b0
       7 b6 b5 b4 b3 b2 b1 b0
            -- -- -- -- -- c2 -- --


      b7 b6 b5 b4 b3 b2 b1 b0
   b7 b6 b5 b4 b3 b2 b1 b0
                     b7 b6 b5 b4 b3 b2 b1 b0
            b7 b6 b5 b4 b3 b2 b1 b0
            -- -- -- c4 c3 -- -- --
  */

	a = (b >> 5) ^ (b >> 3) ^ (b >> 2);
	crc = a & 0x01;
	crc |= (a ^ (b << 1)) & 0x02;
	crc |= (a ^ (b << 1) ^ (b << 2)) & 0x04;
	crc |= ((b >> 3) ^ (b << 0) ^ (b << 2) ^ (b << 3)) & 0x18;

	return crc;
}

static inline uint8_t crc_bitwise_expression(unsigned int b)
{
	unsigned int crc;
	unsigned int a;

/*
                     b7 b6 b5 b4 b3 b2 b1 b0
                           b7 b6 b5 b4 b3 b2 b1 b0
                  b7 b6 b5 b4 b3 b2 b1 b0
            -- -- -- -- -- -- -- c0

                     b7 b6 b5 b4 b3 b2 b1 b0
                           b7 b6 b5 b4 b3 b2 b1 b0
                  b7 b6 b5 b4 b3 b2 b1 b0
         b7 b6 b5 b4 b3 b2 b1 b0
            -- -- -- -- -- -- c1 --

                     b7 b6 b5 b4 b3 b2 b1 b0
                           b7 b6 b5 b4 b3 b2 b1 b0
                  b7 b6 b5 b4 b3 b2 b1 b0
         b7 b6 b5 b4 b3 b2 b1 b0
       7 b6 b5 b4 b3 b2 b1 b0
            -- -- -- -- -- c2 -- --


                     b7 b6 b5 b4 b3 b2 b1 b0
      b7 b6 b5 b4 b3 b2 b1 b0
   b7 b6 b5 b4 b3 b2 b1 b0
            b7 b6 b5 b4 b3 b2 b1 b0
            -- -- -- c4 c3 -- -- --
  */

	a ^= (b >> 3) | (b >> 5)
	crc = (b >> 2) & 0x01;
	crc |= ((b >> 2) ^ (b << 1)) & 0x02;
	crc |= (a ^ (b << 1) ^ (b << 2)) & 0x04;
	crc |= ((b >> 3) ^ (b << 0) ^ (b << 2) ^ (b << 3)) & 0x18;

	return crc;
}

uint8_t crc5bwe(const uint8_t *data, size_t data_len)
{
	unsigned int idx;
	uint8_t crc = 0x1f;

	while (data_len--) {
		tbl_idx = crc ^ *data;
		crc = crc_bitwise_expression(idx);
		data++;
	}

	return crc ^ 0x1f;
}

#endif

#if CRC5_ALGORITHM_CRC5BWE
unsigned int crc5(const void *, size_t) 
	__attribute__ ((weak, alias ("crc5bwe")));
#endif 

