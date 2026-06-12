/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libcrc.
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
 * @file audio.h
 * @brief YARD-ICE libdrv
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdint.h>

#define TONE_DC 0
#define TONE_A4 1
#define TONE_A4S 2
#define TONE_B4 3
#define TONE_C4 4
#define TONE_C4S 5
#define TONE_D4 6
#define TONE_D4S 7
#define TONE_E4 8
#define TONE_F4 9
#define TONE_F4S 10
#define TONE_G4 11
#define TONE_G4S 12
#define TONE_1KHZ 13

#ifdef __cplusplus
extern "C" {
#endif

void wave_set(uint8_t * wave, unsigned int len);

void wave_play(void);

void wave_pause(void);

#ifdef __cplusplus
}
#endif	

#endif /* __AUDIO_H__ */

