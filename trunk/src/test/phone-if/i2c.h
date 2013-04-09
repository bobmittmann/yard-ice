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

struct i2c_io_blk;

enum {
	I2C_XFER_ERR = -1,
	I2C_XFER_IDX,
	I2C_XFER_IN,
	I2C_XFER_OUT
};

#ifdef __cplusplus
extern "C" {
#endif

struct i2c_io_blk * i2c_slave_init(unsigned int scl_freq, unsigned int addr,
								   void * rd_mem, void * wr_mem, 
								   unsigned int size);

void i2c_slave_enable(void);

int i2c_slave_io(void);

void i2c_reset(void);

#ifdef __cplusplus
}
#endif	

#endif /* __I2C_H__ */
