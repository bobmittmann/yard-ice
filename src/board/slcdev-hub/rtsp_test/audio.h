/* 
 *
 * This file is part of the libtcpip.
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
 * @file rtp.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdint.h>

struct audio_player;

#ifdef __cplusplus
extern "C" {
#endif


struct audio_player * audio_init(void);

void audio_alaw_enqueue(uint32_t ts, uint8_t * data, unsigned int samples);

void audio_gain_set(int32_t gain);

int32_t audio_gain_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_H__ */

