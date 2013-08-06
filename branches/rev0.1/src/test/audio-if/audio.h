/* 
 * File:	 audio.h
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

#ifndef __AUDIO_H__
#define __AUDIO_H__

enum {
	TONE_OFF,
	TONE_DAC,
	TONE_ADC
};

#ifdef __cplusplus
extern "C" {
#endif

void audio_init(void);

void audio_enable(void);

void audio_disable(void);

void audio_reset(void);

void audio_stat(void);

void audio_rx_dump(void);

void audio_tx_dump(void);

void audio_tx_analyze(void);

void audio_rx_analyze(void);

int audio_tone_set(int tone, int32_t amp);

int audio_tone_mode_set(int mode);

void audio_stream_enable(void);

void audio_stream_disable(void);

void audio_dac_gain_set(int gain);

#ifdef __cplusplus
}
#endif	

#endif /* __AUDIO_H__ */
