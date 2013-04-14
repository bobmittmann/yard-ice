/* 
 * File:	 i2s.h
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

#ifndef __I2S_H__
#define __I2S_H__

#ifdef __cplusplus
extern "C" {
#endif

void i2s_slave_init(void);

void i2s_enable(void);

void i2s_disable(void);

void i2s_stat(void);

void i2s_rx_dump(void);

void i2s_tx_dump(void);

void i2s_tx_analyze(void);

void i2s_rx_analyze(void);

int i2s_tone_set(int tone, int32_t amp);

#ifdef __cplusplus
}
#endif	

#endif /* __I2S_H__ */
