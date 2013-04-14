/* 
 * File:	 fft.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __FFT_H__
#define __FFT_H__

#include <stdbool.h>
#include "sndbuf.h"
#include "fixpt.h"

#define FFT_LEN 256
#define SPECTRUM_BINS (FFT_LEN /2)

struct spectrum {
	uint32_t mag[SPECTRUM_BINS];
	sndbuf_t buf[FFT_LEN / SNDBUF_LEN];
	uint32_t frm_cnt;
	uint32_t run_cnt;
	uint32_t rate;
	volatile bool locked;
};

#ifdef __cplusplus
extern "C" {
#endif

void fftR4(cplx16_t y[], cplx16_t x[], int n);

void ifftR4(cplx16_t y[], cplx16_t x[], int n);

void spectrum_init(struct spectrum * sa, unsigned int rate);

void spectrum_run(struct spectrum * sa);

void spectrum_normalize(struct spectrum * sa, uint16_t * out);

void spectrum_rec(struct spectrum * sa, sndbuf_t buf);

void spectrum_show(uint16_t mag[]);

void spectrum_print(struct spectrum * sa, int16_t mag[]);

#ifdef __cplusplus
}
#endif	

#endif /* __FFT_H__ */

