/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
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
 * @file config.h
 * @brief USB to Serial Converter configuration
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __CONFIG_H__
#define __CONFIG_H__

#define THINKOS_EXCEPT_STACK_SIZE   320

#define THINKOS_IRQ_MAX               0

#define THINKOS_THREADS_MAX           4
#define THINKOS_ENABLE_THREAD_ALLOC   0
#define THINKOS_ENABLE_THREAD_INFO    0
#define THINKOS_ENABLE_THREAD_STAT    0

#define THINKOS_ENABLE_IDLE_WFI       1
#define THINKOS_IDLE_STACK_CONST      0
#define THINKOS_IDLE_STACK_BSS        1
#define THINKOS_IDLE_STACK_ALLOC      0

#define THINKOS_ENABLE_JOIN           0
#define THINKOS_ENABLE_PAUSE          0
#define THINKOS_ENABLE_CANCEL         0
#define THINKOS_ENABLE_EXIT           0

#define THINKOS_ENABLE_TIMESHARE      0
#define THINKOS_SCHED_LIMIT_MAX      32
#define THINKOS_SCHED_LIMIT_MIN       1

#define THINKOS_MUTEX_MAX             0
#define THINKOS_ENABLE_MUTEX_ALLOC    0

#define THINKOS_COND_MAX              0
#define THINKOS_ENABLE_COND_ALLOC     0

#define THINKOS_SEMAPHORE_MAX         0
#define THINKOS_ENABLE_SEM_ALLOC      0

#define THINKOS_EVENT_MAX             0
#define THINKOS_ENABLE_EVENT_ALLOC    0

#define THINKOS_FLAG_MAX              8
#define THINKOS_ENABLE_FLAG_ALLOC     0
#define THINKOS_ENABLE_FLAG_WATCH     0

#define THINKOS_GATE_MAX              4
#define THINKOS_ENABLE_GATE_ALLOC     0

#define THINKOS_ENABLE_TIMED_CALLS    1
#define THINKOS_ENABLE_CLOCK          1
#define THINKOS_ENABLE_ALARM          1
#define THINKOS_ENABLE_SLEEP          1

#define THINKOS_ENABLE_BREAK 0

#ifdef DEBUG
  #define THINKOS_ENABLE_EXCEPTIONS   1
#else
  #define THINKOS_ENABLE_EXCEPTIONS   0
#endif

#define THINKOS_ENABLE_ARG_CHECK      0
#define THINKOS_ENABLE_DEADLOCK_CHECK 0
#define THINKOS_ENABLE_SANITY_CHECK   0
#define THINKOS_UNROLL_EXCEPTIONS     0
#define THINKOS_SYSRST_ONFAULT        1
#define THINKOS_ENABLE_RT_DEBUG       0
#define THINKOS_ENABLE_PROFILING      0

/* -------------------------------------------------------------------------
 * USB device and CDC-ACM 
 * ------------------------------------------------------------------------- */

#define STM32_ENABLE_USB_DEV    1
#define STM32F_USB_DEV_EP_MAX   4

#define CDC_EP_OUT_MAX_PKT_SIZE 64
#define CDC_EP_IN_MAX_PKT_SIZE  64
#define CDC_EP_INT_MAX_PKT_SIZE 8

/* -------------------------------------------------------------------------
 * ThinkOS static resources
 * ------------------------------------------------------------------------- */

/* Flags */
#define SERDRV_RX_FLAG_NO   0
#define SERDRV_CTL_FLAG_NO  1
#define CDC_TX_DONE_NO      2
#define CDC_TX_LOCK_NO      3
#define CDC_CTL_FLAG_NO     4
#define CDC_RX_FLAG_NO      5
#define VCOM_MODE_FLAG_NO   6

/* Gates */
#define SERDRV_TX_GATE_NO   0

/* Semaphores */


#endif /* __CONFIG_H__ */

