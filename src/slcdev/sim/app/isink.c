/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

#include "board.h"
#include "isink.h"
#include <sys/delay.h>

#include <sys/dcclog.h>

#define FREQ_1MHZ 1000000
#define ISINK_DELAY_COMPENASTION 4

struct {
	uint8_t mode;
	uint32_t s1;
	uint32_t s2;
	uint32_t s3;
	uint32_t s4;
} isink_drv;


/*                            [ R40 R30 R35 R39 ] [ R40 R30 R35 R39 ] */
static void isink_io_cfg(unsigned int mode)
{
	int32_t s1;
	int32_t s2;
	int32_t s3;
	int32_t s4;

	switch (mode) {

	case 0:
	/* (101 37)[mA] (505 185)[mV] [ VCC GND VCC GND ] [ VCC GND GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = -1;
		break;

	case 1:
	/* (103 41)[mA] (515 205)[mV] [ VCC VCC NC  GND ] [ VCC GND NC  GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 2:
	/* (110 37)[mA] (550 185)[mV] [ VCC NC  VCC GND ] [ VCC GND GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = -1;
		break;

	case 3:
	/* (111 37)[mA] (555 185)[mV] [ VCC VCC GND NC  ] [ VCC GND GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = -1;
		break;

	case 4:
	/* (111 56)[mA] (555 280)[mV] [ VCC VCC GND NC  ] [ GND VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 5:
	/* (119 44)[mA] (595 220)[mV] [ VCC GND VCC NC  ] [ VCC GND GND NC  ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = 0;
		break;

	case 6:
	/* (125 37)[mA] (625 185)[mV] [ VCC VCC NC  NC  ] [ VCC GND GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = -1;
		break;

	case 7:
	/* (125 41)[mA] (625 205)[mV] [ VCC VCC NC  NC  ] [ VCC GND NC  GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 8:
	/* (125 50)[mA] (625 250)[mV] [ VCC VCC NC  NC  ] [ VCC GND NC  NC  ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = 0;
		s4 = 0;
		break;

	case 9:
	/* (125 62)[mA] (625 310)[mV] [ VCC VCC NC  NC  ] [ GND VCC NC  GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 10:
	/* (127 56)[mA] (635 280)[mV] [ NC  VCC VCC GND ] [ GND VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 11:
	/* (127 63)[mA] (635 315)[mV] [ NC  VCC VCC GND ] [ GND GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 12:
	/* (133 37)[mA] (665 185)[mV] [ VCC NC  VCC NC  ] [ VCC GND GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = -1;
		break;

	case 13:
	/* (133 44)[mA] (665 220)[mV] [ VCC NC  VCC NC  ] [ VCC GND GND NC  ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = 0;
		break;

	case 14:
	/* (133 63)[mA] (665 315)[mV] [ VCC NC  VCC NC  ] [ GND GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 15:
	/* (133 75)[mA] (665 375)[mV] [ VCC NC  VCC NC  ] [ GND GND VCC NC  ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = 0;
		s4 = 0;
		break;

	case 16:
	/* (142 56)[mA] (710 280)[mV] [ GND VCC VCC NC  ] [ GND VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 17:
	/* (142 67)[mA] (710 335)[mV] [ GND VCC VCC NC  ] [ GND VCC GND NC  ] */
		stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = 0;
		break;

	case 18:
	/* (152 56)[mA] (760 280)[mV] [ NC  VCC VCC NC  ] [ GND VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 19:
	/* (152 63)[mA] (760 315)[mV] [ NC  VCC VCC NC  ] [ GND GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 20:
	/* (152 67)[mA] (760 335)[mV] [ NC  VCC VCC NC  ] [ NC  GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 21:
	/* (152 75)[mA] (760 375)[mV] [ NC  VCC VCC NC  ] [ GND GND VCC NC  ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = 0;
		s4 = 0;
		break;

	case 22:
	/* (154 37)[mA] (770 185)[mV] [ VCC NC  GND VCC ] [ VCC GND GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = -1;
		break;

	case 23:
	/* (156 37)[mA] (780 185)[mV] [ VCC GND NC  VCC ] [ VCC GND GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = -1;
		break;

	case 24:
	/* (157 37)[mA] (785 185)[mV] [ VCC VCC VCC GND ] [ VCC GND GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = -1;
		break;

	default:
	case 25:
	/* (157 63)[mA] (785 315)[mV] [ VCC VCC VCC GND ] [ GND GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 26:
	/* (167 63)[mA] (835 315)[mV] [ GND GND VCC VCC ] [ GND GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 27:
	/* (169 60)[mA] (845 300)[mV] [ NC  VCC GND VCC ] [ NC  VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 28:
	/* (172 41)[mA] (860 205)[mV] [ VCC NC  NC  VCC ] [ VCC NC  GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 29:
	/* (172 46)[mA] (860 230)[mV] [ VCC NC  NC  VCC ] [ VCC NC  NC  GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 30:
	/* (177 62)[mA] (885 310)[mV] [ GND VCC NC  VCC ] [ GND VCC NC  GND ] */
		stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 31:
	/* (177 67)[mA] (885 335)[mV] [ NC  GND VCC VCC ] [ NC  GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 32:
	/* (182 69)[mA] (910 345)[mV] [ GND NC  VCC VCC ] [ GND NC  VCC GND ] */
		stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 33:
	/* (186 37)[mA] (930 185)[mV] [ VCC VCC VCC NC  ] [ VCC GND GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = -1;
		break;

	case 34:
	/* (186 56)[mA] (930 280)[mV] [ VCC VCC VCC NC  ] [ GND VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 35:
	/* (186 67)[mA] (930 335)[mV] [ VCC VCC VCC NC  ] [ GND VCC GND NC  ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = 0;
		break;

	case 36:
	/* (186 93)[mA] (930 465)[mV] [ VCC VCC VCC NC  ] [ VCC VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 37:
	/* (188 56)[mA] (940 280)[mV] [ NC  VCC NC  VCC ] [ GND VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 38:
	/* (188 62)[mA] (940 310)[mV] [ NC  VCC NC  VCC ] [ GND VCC NC  GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 39:
	/* (188 103)[mA] (940 515)[mV] [ NC  VCC NC  VCC ] [ GND GND GND VCC ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = -1;
		s4 = 0;
		break;

	case 40:
	/* (194 67)[mA] (970 335)[mV] [ NC  NC  VCC VCC ] [ NC  GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 41:
	/* (194 74)[mA] (970 370)[mV] [ NC  NC  VCC VCC ] [ NC  NC  VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 42:
	/* (194 110)[mA] (970 550)[mV] [ NC  NC  VCC VCC ] [ NC  GND GND VCC ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = 0;
		break;

	case 43:
	/* (197 56)[mA] (985 280)[mV] [ VCC VCC GND VCC ] [ GND VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 44:
	/* (197 103)[mA] (985 515)[mV] [ VCC VCC GND VCC ] [ GND GND GND VCC ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = -1;
		s4 = 0;
		break;

	case 45:
	/* (204 63)[mA] (1020 315)[mV] [ VCC GND VCC VCC ] [ GND GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 46:
	/* (204 103)[mA] (1020 515)[mV] [ VCC GND VCC VCC ] [ GND GND GND VCC ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = -1;
		s4 = 0;
		break;

	case 47:
	/* (218 41)[mA] (1090 205)[mV] [ VCC VCC NC  VCC ] [ VCC GND NC  GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 48:
	/* (218 62)[mA] (1090 310)[mV] [ VCC VCC NC  VCC ] [ GND VCC NC  GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 49:
	/* (218 103)[mA] (1090 515)[mV] [ VCC VCC NC  VCC ] [ VCC VCC NC  GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 50:
	/* (218 114)[mA] (1090 570)[mV] [ VCC VCC NC  VCC ] [ GND GND NC  VCC ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = 0;
		s4 = 0;
		break;

	case 51:
	/* (223 119)[mA] (1115 595)[mV] [ GND VCC VCC VCC ] [ GND VCC VCC GND ] */
		stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 52:
	/* (223 41)[mA] (1115 205)[mV] [ VCC NC  VCC VCC ] [ VCC NC  GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 53:
	/* (223 69)[mA] (1115 345)[mV] [ VCC NC  VCC VCC ] [ GND NC  VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 54:
	/* (223 101)[mA] (1115 505)[mV] [ VCC NC  VCC VCC ] [ VCC GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 55:
	/* (223 113)[mA] (1115 565)[mV] [ VCC NC  VCC VCC ] [ GND NC  GND VCC ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = 0;
		break;

	case 56:
	/* (236 60)[mA] (1180 300)[mV] [ NC  VCC VCC VCC ] [ NC  VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 57:
	/* (236 67)[mA] (1180 335)[mV] [ NC  VCC VCC VCC ] [ NC  GND VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = 0;
		s4 = -1;
		break;

	case 58:
	/* (236 110)[mA] (1180 550)[mV] [ NC  VCC VCC VCC ] [ NC  GND GND VCC ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = 0;
		break;

	case 59:
	/* (236 127)[mA] (1180 635)[mV] [ NC  VCC VCC VCC ] [ NC  VCC VCC GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = 0;
		s4 = -1;
		break;

	case 60:
	/* (260 56)[mA] (1300 280)[mV] [ VCC VCC VCC VCC ] [ GND VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 61:
	/* (260 93)[mA] (1300 465)[mV] [ VCC VCC VCC VCC ] [ VCC VCC GND GND ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		s4 = -1;
		break;

	case 62:
	/* (260 103)[mA] (1300 515)[mV] [ VCC VCC VCC VCC ] [ GND GND GND VCC ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = -1;
		s2 = -1;
		s3 = -1;
		s4 = 0;
		break;

	case 63:
	/* (260 141)[mA] (1300 705)[mV] [ VCC VCC VCC VCC ] [ VCC GND GND VCC ] */
		stm32_gpio_mode(IO_SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(IO_SINK4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		s4 = 0;
		break;

	}

	isink_drv.s1 = s1;
	isink_drv.s2 = s2;
	isink_drv.s3 = s3;
	isink_drv.s4 = s4;
}

/*
	| mode | i0(mA) | i1(mA) |
	+------+--------+--------+
	|    0 |    101 |     37 |
	|    1 |    103 |     41 |
	|    2 |    110 |     37 |
	|    3 |    111 |     37 |
	|    4 |    111 |     56 |
	|    5 |    119 |     44 |
	|    6 |    125 |     37 |
	|    7 |    125 |     41 |
	|    8 |    125 |     50 |
	|    9 |    125 |     62 |
	|   10 |    127 |     56 |
	|   11 |    127 |     63 |
	|   12 |    133 |     37 |
	|   13 |    133 |     44 |
	|   14 |    133 |     63 |
	|   15 |    133 |     75 |
	|   16 |    142 |     56 |
	|   17 |    142 |     67 |
	|   18 |    152 |     56 |
	|   19 |    152 |     63 |
	|   20 |    152 |     67 |
	|   21 |    152 |     75 |
	|   22 |    154 |     37 |
	|   23 |    156 |     37 |
	|   24 |    157 |     37 |
	|   25 |    157 |     63 |
	|   26 |    167 |     63 |
	|   27 |    169 |     60 |
	|   28 |    172 |     41 |
	|   29 |    172 |     46 |
	|   30 |    177 |     62 |
	|   31 |    177 |     67 |
	|   32 |    182 |     69 |
	|   33 |    186 |     37 |
	|   34 |    186 |     56 |
	|   35 |    186 |     67 |
	|   36 |    186 |     93 |
	|   37 |    188 |     56 |
	|   38 |    188 |     62 |
	|   39 |    188 |    103 |
	|   40 |    194 |     67 |
	|   41 |    194 |     74 |
	|   42 |    194 |    110 |
	|   43 |    197 |     56 |
	|   44 |    197 |    103 |
	|   45 |    204 |     63 |
	|   46 |    204 |    103 |
	|   47 |    218 |     41 |
	|   48 |    218 |     62 |
	|   49 |    218 |    103 |
	|   50 |    218 |    114 |
	|   51 |    223 |    119 |
	|   52 |    223 |     41 |
	|   53 |    223 |     69 |
	|   54 |    223 |    101 |
	|   55 |    223 |    113 |
	|   56 |    236 |     60 |
	|   57 |    236 |     67 |
	|   58 |    236 |    110 |
	|   59 |    236 |    127 |
	|   60 |    260 |     56 |
	|   61 |    260 |     93 |
	|   62 |    260 |    103 |
	|   63 |    260 |    141 |
	+------+--------+--------+
*/


void isink_pulse(unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;
	unsigned int t1 = pulse - pre;	

	tim->ccr1 = (isink_drv.s1 & t1) + 1;
	tim->ccr2 = (isink_drv.s2 & t1) + 1;
	tim->ccr3 = (isink_drv.s3 & t1) + 1;
	tim->ccr4 = (isink_drv.s4 & t1) + 1;
	tim->arr = 1;
 	tim->cnt = pulse + ISINK_DELAY_COMPENASTION;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
}

void isink_stop(void)
{
	struct stm32f_tim * tim = STM32_TIM4;

 	tim->cnt = 0;
}

#define SLEWRATE_DAC_VAL(X) ((4095 * ((X) - SLEWRATE_MIN)) / \
							 (SLEWRATE_MAX - SLEWRATE_MIN))

const uint16_t slewrate_dac_lut[4] = {
	[ISINK_RATE_VERY_SLOW] = SLEWRATE_DAC_VAL(100),
	[ISINK_RATE_SLOW]      = SLEWRATE_DAC_VAL(625),
	[ISINK_RATE_NORMAL]    = SLEWRATE_DAC_VAL(1250),
	[ISINK_RATE_FAST]      = SLEWRATE_DAC_VAL(2500)
};

void isink_mode_set(unsigned int mode)
{
	struct stm32f_dac * dac = STM32_DAC;
	unsigned int wvfm;


	if (isink_drv.mode == mode)
		return;

	wvfm = ISINK_WAVE(mode);
	DCC_LOG1(LOG_INFO, "wvfm=%d", wvfm);
	isink_io_cfg(wvfm);

	dac->dhr12r2 = slewrate_dac_lut[ISINK_RATE(mode)];
	isink_drv.mode = mode;

	DCC_LOG1(LOG_MSG, "irate DAC=%d", dac->dhr12r2);
}

void isink_sleep(void)
{
	struct stm32f_tim * tim = STM32_TIM4;

	/* stop the timer */
 	tim->cnt = 0;
	/* disable clock */
	stm32_clk_disable(STM32_RCC, STM32_CLK_TIM4);
}

/* This interrupt handler is implemented in "slcde-drv.c" as it is
   part of the SLC protocol driver. */
void stm32_tim4_isr(void);

#define PWM_FREQ 100000

void isink_init(void)
{
	struct stm32f_dac * dac = STM32_DAC;
	struct stm32f_tim * tim;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* ---------------------------------------------------------------------
	   Slew Rate Control
	 */
	stm32_gpio_mode(IO_IRATE, ANALOG, 0);
	stm32_gpio_clr(IO_IRATE);
	/* DAC clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DAC);
	/* DAC disable */
	dac->cr = 0;
	/* DAC configure */
	dac->cr = DAC_EN2;
	/* DAC channel 2 initial value */
	dac->dhr12r2 = 0;
	/* DAC channel 1 initial value */
	dac->dhr12r1 = 0;

	/* ---------------------------------------------------------------------
	   Negative voltage generator 
	 */
	stm32_gpio_mode(IO_VNEG_SW, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_VNEG_SW, GPIO_AF1);
	tim = STM32_TIM2;
	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
	/* get the total divisior */
	div = (stm32f_tim1_hz + (PWM_FREQ / 2)) / PWM_FREQ;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->ccmr1 = TIM_OC1M_PWM_MODE1 | TIM_OC2M_PWM_MODE1;
	tim->ccmr2 = TIM_OC3M_PWM_MODE1 | TIM_OC4M_PWM_MODE1;
	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;
	tim->ccr1 = (tim->arr * 4) / 8;
	tim->ccr2 = (tim->arr * 4) / 8;
	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */

	/* ---------------------------------------------------------------------
	   Waveform generator 
	 */
	stm32_gpio_clr(IO_SINK1);
	stm32_gpio_clr(IO_SINK2);
	stm32_gpio_clr(IO_SINK3);
	stm32_gpio_clr(IO_SINK4);
	stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_af(IO_SINK1, GPIO_AF2);
	stm32_gpio_af(IO_SINK2, GPIO_AF2);
	stm32_gpio_af(IO_SINK3, GPIO_AF2);
	stm32_gpio_af(IO_SINK4, GPIO_AF2);

	/* Timer clock enable */
	tim = STM32_TIM4;
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM4);
	
	/* get the total divisior */
	div = (stm32f_tim1_hz + (FREQ_1MHZ / 2)) / FREQ_1MHZ;
	/* Timer configuration */
	tim->cr1 = 0;
	tim->psc = div - 1;
	tim->arr = 1;
	tim->ccmr1 = TIM_OC1M_PWM_MODE2 | TIM_OC2M_PWM_MODE2;
	tim->ccmr2 = TIM_OC3M_PWM_MODE2 | TIM_OC4M_PWM_MODE2;
	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;
	//	TIM_CC1P | TIM_CC2P | TIM_CC3P | TIM_CC4P;
	tim->ccr1 = 1;
	tim->ccr2 = 1;
	tim->ccr3 = 1;
	tim->ccr4 = 1;


	tim->dier = TIM_UIE; /* Update interrupt enable */


#if defined(THINKAPP) || defined(CM3_RAM_VECTORS)
	/* configure and Enable interrupt */
	thinkos_irq_register(STM32_IRQ_TIM4, IRQ_PRIORITY_HIGH, stm32_tim4_isr);
#else
	/* configure interrupts */
	cm3_irq_pri_set(STM32_IRQ_TIM4, IRQ_PRIORITY_HIGH);
	/* Enable interrupt */
	cm3_irq_enable(STM32_IRQ_TIM4);
#endif

	isink_drv.mode = -1;
}

#if 0

void isink_slewrate_set(unsigned int rate)
{
	struct stm32f_dac * dac = STM32_DAC;
	unsigned int dac_val;

	if (rate < SLEWRATE_MIN)
		rate = SLEWRATE_MIN;
	else if (rate >= SLEWRATE_MAX)
		rate = SLEWRATE_MAX;

	dac_val = (4095 * (rate - SLEWRATE_MIN)) / (SLEWRATE_MAX - SLEWRATE_MIN); 
	DCC_LOG2(LOG_TRACE, "rate=%dmA/us DAC=%d", rate, dac_val);

	dac->dhr12r2 = dac_val;
}

#endif

void isink_test(void)
{
	unsigned int pre;
	unsigned int pulse;
	int mode;

	pre = 500;
	pulse = 1000;

	/* disable interrupts */	
	cm3_cpsid_i();
	for (mode = 0; mode < 64; ++mode) {
		isink_io_cfg(mode);
		isink_pulse(pre, pulse);
		udelay(pulse + 500);
	}
	cm3_cpsie_i();

	isink_drv.mode = -1;
}



