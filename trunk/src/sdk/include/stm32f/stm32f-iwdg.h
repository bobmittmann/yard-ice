/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the libstm32f.
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
 * @file stm32f-iwd.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_IWDG_H__
#define __STM32F_IWDG_H__

/*-------------------------------------------------------------------------
  Independent Watchdog (IWDG)
  -------------------------------------------------------------------------*/

/* Key register */
#define STM32F_IWDG_KR 0x00

/* [31..16] Reserved, read as 0. */


/* Bits [15..0] - Key value (write only, read 0000h) */
#define IWDG_KEY_MSK (((1 << (15 + 1)) - 1) << 0)
#define IWDG_KEY_SET(VAL) (((VAL) << 0) & KEY_MSK)
#define IWDG_KEY_GET(REG) (((REG) & KEY_MSK) >> 0)
/* 
These bits must be written by software at regular intervals with the key value AAAAh,
otherwise the watchdog generates a reset when the counter reaches 0.
Writing the key value 5555h to enable access to the IWDG_PR and IWDG_RLR registers
(see Section 18.3.2)
Writing the key value CCCCh starts the watchdog (except if the hardware watchdog option is
selected) */

/* Prescaler register */
#define STM32F_IWDG_PR 0x04



/* [31..3] Reserved, read as 0. */


/* Bits [2..0] - Prescaler divider */
#define IWDG_PR_MSK (((1 << (2 + 1)) - 1) << 0)
#define IWDG_PR_SET(VAL) (((VAL) << 0) & PR_MSK)
#define IWDG_PR_GET(REG) (((REG) & PR_MSK) >> 0)

#define IWDG_PR_4   (0 << 0)
#define IWDG_PR_8   (1 << 0)
#define IWDG_PR_16  (2 << 0)
#define IWDG_PR_32  (3 << 0)
#define IWDG_PR_64  (4 << 0)
#define IWDG_PR_128 (5 << 0)
#define IWDG_PR_256 (6 << 0)

/* These bits are write access protected seeSection 18.3.2. They are written by software to
select the prescaler divider feeding the counter clock. PVU bit of IWDG_SR must be reset in
order to be able to change the prescaler divider.
000: divider /4
001: divider /8
010: divider /16
011: divider /32
100: divider /64
101: divider /128
110: divider /256
111: divider /256 */

/* Note: Reading this register returns the prescaler value from the VDD voltage domain. This
   value may not be up to date/valid if a write operation to this register is ongoing. For this
   reason the value read from this register is valid only when the PVU bit in the IWDG_SR
   register is reset. */

/* Reload register */
#define STM32F_IWDG_RLR 0x08



/* [31..12] Reserved, read as 0. */

/* Bits[11..0] Watchdog counter reload value */
#define IWDG_RL_MSK  ((0xfff) << 0)

/* These bits are write access protected see Section 18.3.2. They are written by software to
   define the value to be loaded in the watchdog counter each time the value AAAAh is written
   in the IWDG_KR register. The watchdog counter counts down from this value. The timeout
   period is a function of this value and the clock prescaler. Refer to Table 86.
   The RVU bit in the IWDG_SR register must be reset in order to be able to change the reload
   value. */

/* Note: Reading this register returns the reload value from the VDD voltage domain. This value
   may not be up to date/valid if a write operation to this register is ongoing on this register.
   For this reason the value read from this register is valid only when the RVU bit in the
   IWDG_SR register is reset. */

/* Status register */
#define STM32F_IWDG_SR 0x0C


/* [31..2] Reserved */


/* Bit 1 - Watchdog counter reload value update */
#define IWDG_RVU (1 << 1)
/* This bit is set by hardware to indicate that an update of the reload value is ongoing. It is reset
   by hardware when the reload value update operation is completed in the VDD voltage domain
   (takes up to 5 RC 40 kHz cycles).
   Reload value can be updated only when RVU bit is reset. */

/* Bit 0 - Watchdog prescaler value update */
#define IWDG_PVU (1 << 0)
/* This bit is set by hardware to indicate that an update of the prescaler value is ongoing. It is
   reset by hardware when the prescaler update operation is completed in the VDD voltage
   domain (takes up to 5 RC 40 kHz cycles).
   Prescaler value can be updated only when PVU bit is reset. */

/* Note: If several reload values or prescaler values are used by application, it is mandatory to wait
   until RVU bit is reset before changing the reload value and to wait until PVU bit is reset
   before changing the prescaler value. However, after updating the prescaler and/or the
   reload value it is not necessary to wait until RVU or PVU is reset before continuing code
   execution (even in case of low-power mode entry, the write operation is taken into account
   and will complete) */


#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_iwdg {
	volatile uint32_t kr;
	volatile uint32_t pr;
	volatile uint32_t rlr;
	volatile uint32_t sr;
};

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_IWDG_H__ */

