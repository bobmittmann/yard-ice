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

/* -------------------------------------------------------------------------
   ThinkOS configuration
   -------------------------------------------------------------------------- */

#define THINKOS_IRQ_MAX                 80
#define THINKOS_ENABLE_IRQ_CTL          1

#define THINKOS_ENABLE_IDLE_WFI         1
#define THINKOS_IDLE_STACK_BSS          0
#define THINKOS_IDLE_STACK_ALLOC        1

#define THINKOS_ENABLE_CLOCK            1

#define THINKOS_ENABLE_TIMESHARE        0
#define THINKOS_SCHED_LIMIT_MAX         32
#define THINKOS_SCHED_LIMIT_MIN         1

#define THINKOS_THREADS_MAX             20
#define THINKOS_ENABLE_THREAD_ALLOC     1

#define THINKOS_ENABLE_JOIN             0
#define THINKOS_ENABLE_PAUSE            0
#define THINKOS_ENABLE_CANCEL           0
#define THINKOS_ENABLE_EXIT             0

#define THINKOS_MUTEX_MAX               16
#define THINKOS_ENABLE_MUTEX_ALLOC      1

#define THINKOS_COND_MAX                16
#define THINKOS_ENABLE_COND_ALLOC       1

#define THINKOS_SEMAPHORE_MAX           16
#define THINKOS_ENABLE_SEM_ALLOC        1

#define THINKOS_EVENT_MAX               2
#define THINKOS_ENABLE_EVENT_ALLOC      1

#define THINKOS_FLAG_MAX                16
#define THINKOS_ENABLE_FLAG_ALLOC       1
#define THINKOS_ENABLE_FLAG_WATCH       0

#define THINKOS_GATE_MAX                16
#define THINKOS_ENABLE_GATE_ALLOC       1

#define THINKOS_ENABLE_THREAD_STAT      1
#define THINKOS_ENABLE_TIMED_CALLS      1

#define THINKOS_ENABLE_ARG_CHECK        0
#define THINKOS_ENABLE_DEADLOCK_CHECK   0
#define THINKOS_ENABLE_SANITY_CHECK     0

#define THINKOS_ENABLE_CTL              0

#define THINKOS_ENABLE_SCHED_DEBUG      0

#define THINKOS_ENABLE_RT_DEBUG         0

#define THINKOS_ENABLE_BREAK            0

#define THINKOS_ENABLE_PROFILING        1

#define THINKOS_ENABLE_MONITOR          1
#define THINKOS_ENABLE_DMCLOCK          1

#define THINKOS_ENABLE_CONSOLE          1

#define THINKOS_EXCEPT_STACK_SIZE       320
#define THINKOS_ENABLE_EXCEPTIONS       0
#define THINKOS_UNROLL_EXCEPTIONS       0
#define THINKOS_ENABLE_EXCEPT_CLEAR     0
#define THINKOS_ENABLE_BUSFAULT         0
#define THINKOS_ENABLE_USAGEFAULT       0
#define THINKOS_ENABLE_MEMFAULT         0
#define THINKOS_SYSRST_ONFAULT          0
#define THINKOS_STDERR_FAULT_DUMP       0
#define THINKOS_ENABLE_DEBUG_BKPT       0
#define THINKOS_ENABLE_DEBUG_WPT        0
#define THINKOS_ENABLE_DEBUG_STEP       0
#define THINKOS_ENABLE_DEBUG_FAULT      0
#define THINKOS_ENABLE_ERROR_TRAP       0

#define THINKOS_ENABLE_MPU              0
#define THINKOS_ENABLE_ESCALATE         1
#define THINKOS_ENABLE_FPU              0

#define THINKOS_ENABLE_THREAD_VOID      0

#define THINKOS_DBGMON_STACK_SIZE       480

#define THINKOS_ENABLE_STACK_INIT       0

#define THINKOS_ASM_SCHEDULER           1

#define THINKOS_ENABLE_OFAST            0

#define THINKOS_ENABLE_RESET_RAM_VECTORS 0
#define THINKOS_DBGMON_ENABLE_FLOWCTL    0 
#define THINKOS_DBGMON_ENABLE_IRQ_MGMT   0 
#define DMPRINTF_ENABLE_FAST             1
#define DMPRINTF_ENABLE_UNSIGNED         0
#define THINKOS_CONSOLE_RX_FIFO_LEN     64
#define THINKOS_CONSOLE_TX_FIFO_LEN    128

/* -------------------------------------------------------------------------
 * delay
 * ------------------------------------------------------------------------- */

#define UDELAY_FACTOR 1706

/* -------------------------------------------------------------------------
   USB 
   -------------------------------------------------------------------------- */

#define STM32_ENABLE_OTG_FS          1
#define STM32_OTG_FS_EP_MAX          4

#define CDC_EP_OUT_MAX_PKT_SIZE      64
#define CDC_EP_IN_MAX_PKT_SIZE       64
#define CDC_EP_INT_MAX_PKT_SIZE      8

/* -------------------------------------------------------------------------
 * TCP/IP
 * ------------------------------------------------------------------------- */

#define MBUF_POOL_SIZE               64
#define PKTBUF_LEN                   (1500 + 16)

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
#define  ENABLE_NET_TCP_CHECKSUM     0
#define  ENABLE_NET_UDP_CHECKSUM     0

/* -------------------------------------------------------------------------
 * JTAG
 * ------------------------------------------------------------------------- */

#define JTAG_VEC_BITS_MAX 256
#define JTAG_TAP_MAX      8
#define ENABLE_ICE_TEST   0

/* -------------------------------------------------------------------------
 * Shell
 * ------------------------------------------------------------------------- */

#define SHELL_LINE_MAX    72
#define SHELL_ARG_MAX     16
#define SHELL_HISTORY_MAX 32

/* -------------------------------------------------------------------------
 * Services
 * ------------------------------------------------------------------------- */

#define ENABLE_TCP_SEND 1
#define ENABLE_TCP_RECV 1
#define ENABLE_USB      0
#define ENABLE_MONITOR  1
#define ENABLE_NETWORK  1
#define ENABLE_TFTP     1
#define ENABLE_NAND     1
#define ENABLE_COMM     1
#define ENABLE_GDB      1
#define ENABLE_VCOM     1
#define ENABLE_I2C      1
#define ENABLE_TELNET   1

/* -------------------------------------------------------------------------
 * Trace 
 * ------------------------------------------------------------------------- */

#define TRACE_LEVEL 5

#endif /* __CONFIG_H__ */


