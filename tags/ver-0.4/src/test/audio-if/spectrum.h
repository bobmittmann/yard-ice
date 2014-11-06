/* 
 * File:	 spectrum.h
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

#ifndef __SPECTRUM_H__
#define __SPECTRUM_H__

#include <stdbool.h>
#include "sndbuf.h"
#include "fft.h"

#define SPECTRUM_LEN 256
#define SPECTRUM_BINS (SPECTRUM_LEN / 2)

struct spectrum {
	uint32_t mag[SPECTRUM_BINS];
	sndbuf_t * ring[SPECTRUM_LEN / SNDBUF_LEN];
	volatile uint32_t frm_cnt;
	uint32_t run_cnt;
	uint32_t rate;
	volatile bool locked;
};

#ifdef __cplusplus
extern "C" {
#endif

void spectrum_init(struct spectrum * sa, unsigned int rate);

void spectrum_run(struct spectrum * sa, bool window);

void spectrum_normalize(struct spectrum * sa);

void spectrum_reset(struct spectrum * sa);

void spectrum_rec(struct spectrum * sa, sndbuf_t * buf);

void spectrum_mag_show(struct spectrum * sa);

void spectrum_pwr_show(struct spectrum * sa);

void spectrum_print(struct spectrum * sa);

#ifdef __cplusplus
}
#endif	

#endif /* __SPECTRUM_H__ */

