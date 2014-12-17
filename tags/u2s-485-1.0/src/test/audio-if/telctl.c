/* 
 * File:	 audio-if.c
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

#include <sys/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#include "telctl.h"
#include "i2c.h"
#include "trace.h"


struct telctl_state telctl;

static struct {
	uint8_t led;
	uint8_t relay;
	uint8_t z_vr;
	uint8_t g_vr;
	uint8_t tone[2];
} cache;

int line_set_connect(int line_idx, bool connect)
{
	uint8_t reg[2];
	unsigned int led;
	unsigned int relay;
	int ret;

	led = cache.led;
	relay = cache.relay;

	if (connect) {
		led |= (1 << line_idx);
		relay |= (1 << line_idx);
	} else {
		led &= ~(1 << line_idx);
		relay &= ~(1 << line_idx);
	}

	reg[0] = led;
	reg[1] = relay;

	if ((ret = i2c_write(TELCTL_I2C_ADDR, TELCTL_LED_REG, reg, 2)) > 0) {
		cache.led = led;
		cache.relay = relay;
		telctl.line[line_idx].connected = connect;
	} else {
		tracef("%s(): i2c_write() failed!", __func__);
	}

	return ret;
}

int line_connect_off_hook(void)
{
	uint8_t reg[2];
	int ret;
	int cnt;
	int i;

	DCC_LOG(LOG_TRACE, ".");

	reg[0] = 0;
	reg[1] = 0;

	for (i = 0; i < 5; ++i) {
		if (telctl.line[i].sup_st >= LINE_PHONE1) {
			reg[0] |= (1 << i);
			reg[1] |= (1 << i);
		}
	}

	if (cache.led == reg[0])
		return 0;

	if ((ret = i2c_write(TELCTL_I2C_ADDR, TELCTL_LED_REG, reg, 2)) > 0) {
		cache.led = reg[0];
		cache.relay = reg[1];
		cnt = 0;
		for (i = 0; i < 5; ++i) {
			if (telctl.line[i].sup_st >= LINE_PHONE1) {
				telctl.line[i].connected = true;
				cnt++;
			}
		}
		ret = cnt;
	} else {
		tracef("%s(): i2c_write() failed!", __func__);
	}

	return ret;
}

int line_disconnect_on_hook(void)
{
	uint8_t reg[2];
	int ret;
	int cnt;
	int i;

	DCC_LOG(LOG_TRACE, ".");

	reg[0] = cache.led;
	reg[1] = cache.relay;

	cnt = 0;
	for (i = 0; i < TELCTL_LINE_CNT; ++i) {
		if (telctl.line[i].connected) {
			cnt++;
		}
		if (telctl.line[i].sup_st < LINE_PHONE1) {
			reg[0] &= ~(1 << i);
			reg[1] &= ~(1 << i);
		}
	}

	if (cache.led == reg[0]) {
		/* return the number of connected lines */
		return cnt;
	}

	if ((ret = i2c_write(TELCTL_I2C_ADDR, TELCTL_LED_REG, reg, 2)) > 0) {
		cache.led = reg[0];
		cache.relay = reg[1];
		cnt = 0;
		for (i = 0; i < TELCTL_LED_REG; ++i) {
			if (telctl.line[i].sup_st < LINE_PHONE1) {
				telctl.line[i].connected = false;
			} else {
				telctl.line[i].connected = true;
				cnt++;
			}
		}
		ret = cnt;
	} else {
		tracef("%s(): i2c_write() failed!", __func__);
	}

	return ret;
}

int line_hangup_all(void)
{
	uint8_t reg[2];
	int ret;
	int i;


	reg[0] = 0;
	reg[1] = 0;

	if ((ret = i2c_write(TELCTL_I2C_ADDR, TELCTL_LED_REG, reg, 2)) > 0) {
		cache.led = reg[0];
		cache.relay = reg[1];
		for (i = 0; i < 5; ++i) {
			telctl.line[i].connected = false;
		}
	} else {
		tracef("%s(): i2c_write() failed!", __func__);
	}

	return ret;
}	

bool line_toggle(int line_idx)
{
	bool connect;

	connect = telctl.line[line_idx].connected ? false : true;
	line_set_connect(line_idx, connect);

	return telctl.line[line_idx].connected;
}


int hybrid_adjust(int z_vr, int g_vr)
{
	uint8_t reg[2];
	int ret;

	if (z_vr < 0 )
		z_vr = 0;
	else if (z_vr > 63)
		z_vr = 63;

	if (g_vr < 0)
		g_vr = 0;
	else if (g_vr > 63)
		g_vr = 63;

	if ((cache.z_vr == z_vr) && (cache.g_vr == g_vr)) {
		return 0;
	}

	reg[0] = 63 - z_vr;
	reg[1] = 63 - g_vr;

	if ((ret = i2c_write(TELCTL_I2C_ADDR, TELCTL_VR0_REG, reg, 2)) > 0) {
		if (cache.z_vr != z_vr) {
			telctl.hybrid.z = ((z_vr * 5000) / 63) + 500;
			cache.z_vr = z_vr;
		}

		if (cache.g_vr != g_vr) {
			telctl.hybrid.g = 100 + (5000 * g_vr) / (25 * 63);
			cache.g_vr = g_vr;
		}

	} else {
		tracef("%s(): i2c_write() failed!", __func__);
	}

	return ret;
}

void hybrid_impedance_set(int z_vr)
{
	hybrid_adjust(z_vr, cache.g_vr);
}

void hybrid_impedance_step(int d)
{
	hybrid_adjust(cache.z_vr + d, cache.g_vr);
}

void hybrid_gain_step(int d)
{
	hybrid_adjust(cache.z_vr, cache.g_vr + d);
}

void hybrid_gain_set(int g_vr)
{
	hybrid_adjust(cache.z_vr, g_vr);
}

int telctl_adc_scan(void)
{
	int state;
	int sum;
	int cnt;
	int ret;
	int val;
	int i;

	if ((ret = i2c_read(TELCTL_I2C_ADDR, TELCTL_ADC_REG, 
						telctl.adc, 2 * TELCTL_LINE_CNT)) > 0) {

		DCC_LOG5(LOG_TRACE, "ADC %5d %5d %5d %5d %5d",
				 telctl.adc[0], telctl.adc[1], adc[2], adc[3], adc[4]);

		//			tracef("ADC %5d %5d %5d %5d %5d\n", 
		//				  adc[0], adc[1], adc[2], adc[3], adc[4]);
		cnt = 0;
		sum = 0;

		for (i = 0; i < TELCTL_LINE_CNT; ++i) {

			val = telctl.adc[i];

			if (val < 3) {
				state = LINE_TROUBLE_OPEN;
			} else if (val < 40) {
				state = LINE_ON_HOOK;
			} else if (val < 340) {
				state = LINE_PHONE1;
				if (telctl.line[i].connected) {
					sum += val;
					cnt += 1;
				}
			} else if (val < 520) {
				state = LINE_PHONE2;
				if (telctl.line[i].connected) {
					sum += val;
					cnt += 2;
				}
			} else {
				state = LINE_TROUBLE_SHORT;
			}

			telctl.line[i].sup_st = state;
		}

		telctl.load.cnt = cnt;
		telctl.load.sum = sum;
	} else {
		for (i = 0; i < TELCTL_LINE_CNT; ++i) {
			telctl.line[i].sup_st = LINE_UNDEFINED;
		}
	}

	return ret;
}

int codec_hw_reset(void)
{
	uint8_t reg[1];
	int ret;

	tracef("%s(): ...", __func__);

	reg[0] = 'R';

	if ((ret = i2c_write(TELCTL_I2C_ADDR, TELCTL_CRST_REG, reg, 1)) < 0) {
		tracef("%s(): i2c_write() failed!", __func__);
	} else {
		/* wait for the coddec to start up */
		thinkos_sleep(2);
	}

	return ret;
}

int telctl_tonegen_set(int tone0, int tone1)
{
	uint8_t reg[2];
	int ret;

	if (tone0 < 0 )
		tone0 = 0;
	else if (tone0 > 7)
		tone0 = 7;

	if (tone1 < 0 )
		tone1 = 0;
	else if (tone1 > 7)
		tone1 = 7;

	tracef("%s(): tone0=%d tone1=%d!", __func__, tone0, tone1);

//	if ((cache.tone[0] == tone0) && (cache.tone[1] == tone1)) {
//		return 0;
//	}

	reg[0] = tone0;
	reg[1] = tone1;

	if ((ret = i2c_write(TELCTL_I2C_ADDR, TELCTL_TONE0_REG, reg, 2)) > 0) {
		cache.tone[0] = tone0;
		cache.tone[1] = tone1;
	} else {
		tracef("%s(): i2c_write() failed!", __func__);
	}

	return ret;
}

int telctl_sync(void)
{
	int ret;

	if ((ret = i2c_read(TELCTL_I2C_ADDR, TELCTL_LED_REG, &cache, 6)) < 0) {
		tracef("%s(): i2c_read() failed!", __func__);
	}

	return ret;
}

