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
 * @file config.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define THINKOS_IRQ_MAX 80

#define THINKOS_ENABLE_IDLE_WFI 1
#define THINKOS_ENABLE_IDLE_SNAPSHOT 0

#define THINKOS_ENABLE_CLOCK 1

#define THINKOS_ENABLE_TIMESHARE 0
#define THINKOS_SCHED_LIMIT_MAX 32
#define THINKOS_SCHED_LIMIT_MIN 1

#define THINKOS_THREADS_MAX 20
#define THINKOS_ENABLE_THREAD_ALLOC 1

#define THINKOS_ENABLE_JOIN 0
#define THINKOS_ENABLE_PAUSE 0
#define THINKOS_ENABLE_CANCEL 0
#define THINKOS_ENABLE_EXIT 0

#define THINKOS_MUTEX_MAX 16
#define THINKOS_ENABLE_MUTEX_ALLOC 1

#define THINKOS_COND_MAX 16
#define THINKOS_ENABLE_COND_ALLOC 1

#define THINKOS_SEMAPHORE_MAX 16
#define THINKOS_ENABLE_SEM_ALLOC 1

#define THINKOS_EVENT_MAX 2
#define THINKOS_ENABLE_EVENT_ALLOC 2

#define THINKOS_FLAG_MAX 16
#define THINKOS_ENABLE_FLAG_ALLOC 1

#define THINKOS_ENABLE_THREAD_STAT 1
#define THINKOS_ENABLE_TIMED_CALLS 1

#define THINKOS_ENABLE_ARG_CHECK      1

#define THINKOS_ENABLE_DEADLOCK_CHECK 1

#define THINKOS_ENABLE_EXCEPTIONS  1

#define THINKOS_EXCEPT_STACK_SIZE  384

#define THINKOS_ENABLE_SCHED_DEBUG 0

#define THINKOS_ENABLE_THREAD_INFO 1

#define THINKOS_ENABLE_RT_DEBUG    1

#define THINKOS_ENABLE_PROFILING   1

#define THINKOS_ENABLE_MONITOR     0

#define THINKOS_ENABLE_DEBUG_STEP  0

#define THINKOS_ENABLE_DEBUG_FAULT 0

#define THINKOS_ENABLE_MPU 0

#define THINKOS_ENABLE_FPU 0

/* -------------------------------------------------------------------------
 * USB device and CDC-ACM 
 * ------------------------------------------------------------------------- */

#define STM32_ENABLE_USB_DEVICE 1

#define CDC_EP_OUT_MAX_PKT_SIZE 64
#define CDC_EP_IN_MAX_PKT_SIZE  64
#define CDC_EP_INT_MAX_PKT_SIZE 8

/* -------------------------------------------------------------------------
 * TCP/IP
 * ------------------------------------------------------------------------- */

#define MBUF_POOL_SIZE               64

#define TCP_MAX_RCV_QUEUE            (1460 * 2)
#define TCP_MAX_SND_QUEUE            (1460 * 2)

#define TCP_DEFAULT_MSL_SEC          1
#define TCP_IDLE_DET_SEC             10
#define TCP_KEEP_ALIVE_PROBE_SEC     10
#define TCP_MAX_IDLE_SEC             15

#define TCP_ENABLE_TIMEWAIT          1
#define TCP_ENABLE_HEADER_PREDICTION 1
#define NET_TCP_PCB_ACTIVE_MAX       8
#define NET_UDP_PCB_ACTIVE_MAX       1

/* use hardware checksum */
#define  ENABLE_NET_TCP_CHECKSUM 0
#define  ENABLE_NET_UDP_CHECKSUM 0

/* -------------------------------------------------------------------------
 * JTAG
 * ------------------------------------------------------------------------- */

#define JTAG_VEC_BITS_MAX 256
#define JTAG_TAP_MAX 8
#define ENABLE_ICE_TEST 0

/* -------------------------------------------------------------------------
 * Shell
 * ------------------------------------------------------------------------- */

#define SHELL_LINE_MAX 72
#define SHELL_ARG_MAX 16
#define SHELL_HISTORY_MAX 32

/* -------------------------------------------------------------------------
 * Services
 * ------------------------------------------------------------------------- */

#define ENABLE_TCP_SEND 1
#define ENABLE_TCP_RECV 1
#define ENABLE_USB 1
#define ENABLE_NETWORK 1
#define ENABLE_TFTP 1
#define ENABLE_NAND 1
#define ENABLE_COMM 1
#define ENABLE_GDB 1
#define ENABLE_VCOM 1
#define ENABLE_I2C 1
#define ENABLE_TELNET 1

#endif /* __CONFIG_H__ */


