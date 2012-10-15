/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file sys/serial.h
 * @brief YARD-ICE 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_SERIAL_H__
#define __SYS_SERIAL_H__

#define SERIAL_DATABITS_6 (0 << 0)
#define SERIAL_DATABITS_7 (1 << 0)
#define SERIAL_DATABITS_8 (2 << 0)
#define SERIAL_DATABITS_9 (3 << 0)
#define SERIAL_DATABITS_MASK (0x07 << 0)

#define SERIAL_PARITY_NONE (0 << 3)
#define SERIAL_PARITY_EVEN (1 << 3)
#define SERIAL_PARITY_ODD  (2 << 3)
#define SERIAL_PARITY_MARK (3 << 3)
#define SERIAL_PARITY_SPACE (4 << 3)
#define SERIAL_PARITY_MASK (0x07 << 3)

#define SERIAL_STOPBITS_1 (0 << 6)
#define SERIAL_STOPBITS_2 (1 << 6)
#define SERIAL_STOPBITS_1_5 (2 << 6)
#define SERIAL_STOPBITS_0_5 (3 << 6)
#define SERIAL_STOPBITS_MASK (0x03 << 6)

/* common character frame options */
#define SERIAL_8N1 (SERIAL_DATABITS_8 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_1) 
#define SERIAL_8N2 (SERIAL_DATABITS_8 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_2) 
#define SERIAL_7N1 (SERIAL_DATABITS_7 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_1) 
#define SERIAL_7N2 (SERIAL_DATABITS_7 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_2) 
#define SERIAL_8E1 (SERIAL_DATABITS_8 | SERIAL_PARITY_EVEN | SERIAL_STOPBITS_1) 
#define SERIAL_8E2 (SERIAL_DATABITS_8 | SERIAL_PARITY_EVEN | SERIAL_STOPBITS_2) 
#define SERIAL_7E1 (SERIAL_DATABITS_7 | SERIAL_PARITY_EVEN | SERIAL_STOPBITS_1) 
#define SERIAL_7E2 (SERIAL_DATABITS_7 | SERIAL_PARITY_EVEN | SERIAL_STOPBITS_2) 
#define SERIAL_8O1 (SERIAL_DATABITS_8 | SERIAL_PARITY_ODD | SERIAL_STOPBITS_1) 
#define SERIAL_8O2 (SERIAL_DATABITS_8 | SERIAL_PARITY_ODD | SERIAL_STOPBITS_2) 
#define SERIAL_7O1 (SERIAL_DATABITS_7 | SERIAL_PARITY_ODD | SERIAL_STOPBITS_1) 
#define SERIAL_7O2 (SERIAL_DATABITS_7 | SERIAL_PARITY_ODD | SERIAL_STOPBITS_2) 
#define SERIAL_9N1 (SERIAL_DATABITS_9 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_1) 
#define SERIAL_9N2 (SERIAL_DATABITS_9 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_2) 

/* character frame bits mask */
#define SERIAL_FRAME_MASK (0xff)

#define SERIAL_FLOWCTRL_NONE (0 << 8)
#define SERIAL_FLOWCTRL_RTSCTS (1 << 8)
#define SERIAL_FLOWCTRL_XONXOFF (2 << 8)
#define SERIAL_FLOWCTRL_MASK (0x03 << 8)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SYS_SERIAL_H__ */

