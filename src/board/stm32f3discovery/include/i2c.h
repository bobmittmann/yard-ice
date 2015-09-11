/* 
 * File:	 i2c.h
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

#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

void i2c_master_init(unsigned int scl_freq);

void i2c_master_enable(void);

int i2c_master_wr(unsigned int addr, const void * buf, int len);

int i2c_master_rd(unsigned int addr, void * buf, int len);


int i2c_read(unsigned int addr, void * buf, int n);

int i2c_write(unsigned int addr, const void * buf, int n);

int i2c_reg_read(unsigned int addr, unsigned int reg, void * buf, int n);

int i2c_reg_write(unsigned int addr, unsigned int reg, const void * buf, int n);

void i2c_reset(void);

void i2c_init(void);

int i2c_bus_scan(unsigned int from, unsigned int to, uint8_t lst[], int len);

#ifdef __cplusplus
}
#endif	

#endif /* __I2C_H__ */
