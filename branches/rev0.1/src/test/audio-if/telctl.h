/* 
 * File:	 telctl.h
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

#ifndef __TELCTL_H__
#define __TELCTL_H__

#include <stdint.h>

#define TELCTL_I2C_ADDR 0x55

#define TELCTL_ID_REG 0
#define TELCTL_VER_REG 2
#define TELCTL_ADC_REG 4
#define TELCTL_LED_REG 14
#define TELCTL_RLY_REG 15
#define TELCTL_VR0_REG 16
#define TELCTL_VR1_REG 17
#define TELCTL_TONE0_REG 18
#define TELCTL_TONE1_REG 19
#define TELCTL_CRST_REG 20
#define TELCTL_SRST_REG 21

enum {
	LINE_UNDEFINED = 0,
	LINE_TROUBLE_OPEN,
	LINE_TROUBLE_SHORT,
	LINE_ON_HOOK,
	LINE_PHONE1,
	LINE_PHONE2,
	LINE_PHONE3
};

struct telctl_line {
	int sup_st;
	bool connected;
};

#define TELCTL_LINE_CNT 5

struct telctl_state {
	struct {
		int sup_st;
		bool connected;
	} line[TELCTL_LINE_CNT];  

	uint16_t adc[TELCTL_LINE_CNT];

	struct {
		int z;
		int g;
	} hybrid;

	struct {
		int cnt;
		int sum; 
	} load;
};

extern struct telctl_state telctl;

#ifdef __cplusplus
extern "C" {
#endif

int line_set_connect(int line_idx, bool connect);

int line_connect_off_hook(void);

int line_disconnect_on_hook(void);

int line_hangup_all(void);

bool line_toggle(int line_idx);

int hybrid_adjust(int z_vr, int g_vr);

void hybrid_impedance_set(int z_vr);

void hybrid_impedance_step(int d);

void hybrid_gain_set(int g_vr);

void hybrid_gain_step(int d);

int telctl_adc_scan(void);

int codec_hw_reset(void);

int telctl_tonegen_set(int tone0, int tone1);

int telctl_sync(void);

#ifdef __cplusplus
}
#endif	

#endif /* __TELCTL_H__ */

