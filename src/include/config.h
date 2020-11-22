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

#define THINKOS_THREADS_MAX             32
#define THINKOS_ENABLE_THREAD_INFO      1
#define THINKOS_ENABLE_THREAD_STAT      1

#define THINKOS_ENABLE_JOIN             0
#define THINKOS_ENABLE_PAUSE            0
#define THINKOS_ENABLE_CANCEL           0
#define THINKOS_ENABLE_EXIT             0
#define THINKOS_ENABLE_TERMINATE        0
#define THINKOS_ENABLE_BREAK            0

#define THINKOS_ENABLE_TIMESHARE        0

#define THINKOS_MUTEX_MAX               24

#define THINKOS_COND_MAX                24

#define THINKOS_SEMAPHORE_MAX           16

#define THINKOS_EVENT_MAX               8

#define THINKOS_FLAG_MAX                16
#define THINKOS_ENABLE_FLAG_WATCH       0

#define THINKOS_GATE_MAX                8

#define THINKOS_ENABLE_TIMED_CALLS      1
#define THINKOS_ENABLE_CLOCK            1
#define THINKOS_ENABLE_ALARM            1
#define THINKOS_ENABLE_SLEEP            1

#define THINKOS_ENABLE_CTL              1

#define THINKOS_ENABLE_PREEMPTION       1
#define THINKOS_ENABLE_CRITICAL         0
#define THINKOS_ENABLE_ESCALATE         0

#define THINKOS_IRQ_MAX                 80
#define THINKOS_ENABLE_IRQ_CTL          1

#define THINKOS_ENABLE_CONSOLE          1
#define THINKOS_ENABLE_CONSOLE_MODE     0
#define THINKOS_ENABLE_CONSOLE_NONBLOCK 0
#define THINKOS_ENABLE_CONSOLE_BREAK    0
#define THINKOS_ENABLE_CONSOLE_DRAIN    0
#define THINKOS_ENABLE_CONSOLE_OPEN     0
#define THINKOS_ENABLE_CONSOLE_READ     1
#define THINKOS_ENABLE_CONSOLE_MISC     0

#define THINKOS_ENABLE_COMM             0

#define THINKOS_ENABLE_FPU                0
#define THINKOS_ENABLE_MPU                0
#define THINKOS_ENABLE_MEMFAULT           0

#if DEBUG
  #define THINKOS_ENABLE_PROFILING        1
  #define THINKOS_ENABLE_RUNMASK          1
  #define THINKOS_ENABLE_EXCEPTIONS       1
  #define THINKOS_ENABLE_STACK_INIT       1
  #define THINKOS_ENABLE_STACK_LIMIT      1
  #define THINKOS_ENABLE_ARG_CHECK        1
  #define THINKOS_ENABLE_DEADLOCK_CHECK   1
  #define THINKOS_ENABLE_SANITY_CHECK     1
  #define THINKOS_ENABLE_HARDFAULT        1
  #define THINKOS_ENABLE_BUSFAULT         1
  #define THINKOS_ENABLE_USAGEFAULT       1
  #define THINKOS_ENABLE_KRNSVC           0
  #define THINKOS_ENABLE_ERROR_TRAP       1
  #define THINKOS_ENABLE_THREAD_FAULT     0
  #define THINKOS_SYSRST_ONFAULT          0
#else
  #define THINKOS_ENABLE_PROFILING        1
  #define THINKOS_ENABLE_RUNMASK          0
  #define THINKOS_ENABLE_EXCEPTIONS       0
  #define THINKOS_ENABLE_STACK_INIT       0
  #define THINKOS_ENABLE_STACK_LIMIT      0
  #define THINKOS_ENABLE_ARG_CHECK        0
  #define THINKOS_ENABLE_DEADLOCK_CHECK   0
  #define THINKOS_ENABLE_SANITY_CHECK     0
  #define THINKOS_ENABLE_HARDFAULT        0
  #define THINKOS_ENABLE_BUSFAULT         0
  #define THINKOS_ENABLE_USAGEFAULT       0
  #define THINKOS_ENABLE_KRNSVC           0
  #define THINKOS_ENABLE_ERROR_TRAP       0
  #define THINKOS_ENABLE_THREAD_FAULT     0
  #define THINKOS_SYSRST_ONFAULT          0
#endif

#define THINKOS_ENABLE_STACK_ALIGN        1
#define THINKOS_EXCEPT_STACK_SIZE         512

#define THINKOS_ENABLE_MONITOR            1
#define THINKOS_ENABLE_MONITOR_THREADS    0
#define THINKOS_ENABLE_MONITOR_CLOCK      0
#define THINKOS_ENABLE_DEBUG_STEP         0
#define THINKOS_ENABLE_DEBUG_BKPT         0
#define THINKOS_ENABLE_DEBUG_WPT          0
#define THINKOS_ENABLE_MEMORY_CLEAR       0
#define THINKOS_ENABLE_OFAST              0
#define THINKOS_ENABLE_FLASH_MEM          0
#define THINKOS_ENABLE_MEM_MAP            0

#if DEBUG
  /* Enable scheduler trace calls */
  #define THINKOS_ENABLE_SCHED_DEBUG      0
  /* Enable scheduler stack error detection */
  #define THINKOS_ENABLE_SCHED_ERROR      1
#else
  #define THINKOS_ENABLE_SCHED_DEBUG      0
  #define THINKOS_ENABLE_SCHED_ERROR      0
#endif

#define THINKOS_MONITOR_STACK_SIZE        512

#define DMPRINTF_ENABLE_FAST              1
#define DMPRINTF_ENABLE_UNSIGNED          0
#define THINKOS_CONSOLE_RX_FIFO_LEN       64
#define THINKOS_CONSOLE_TX_FIFO_LEN       512

#define THINKOS_ENABLE_IRQ_CYCCNT         0
#define THINKOS_ENABLE_IRQ_TIMEDWAIT      0
#define THINKOS_ENABLE_IRQ_PRIORITY_0     0
#define THINKOS_ENABLE_WQ_IRQ             0
#define THINKOS_ENABLE_DMA                0

#define THINKOS_ENABLE_IDLE_HOOKS         0
#define THINKOS_ENABLE_IDLE_WFI           1
#define THINKOS_ENABLE_I_CALLS            1
#define THINKOS_ENABLE_PRIVILEGED_THREAD  1

#define THINKOS_ENABLE_OBJ_ALLOC          1
#define THINKOS_ENABLE_OBJ_FREE           1
#define THINKOS_ENABLE_DIV0_TRAP          0
#define THINKOS_ENABLE_UNALIGN_TRAP       0

#define THINKOS_ENABLE_APP                0
#define THINKOS_MONITOR_ENABLE_COMM_BRK   0

/* -------------------------------------------------------------------------
 * delay
 * ------------------------------------------------------------------------- */

#define UDELAY_FACTOR     1706

/* -------------------------------------------------------------------------
   Clock 
   -------------------------------------------------------------------------- */

#define STM32_HSE_HZ           24000000
#define STM32_HCLK_HZ         120000000
#define STM32_ENABLE_HSE              1
#define STM32_ENABLE_PLL              1

/* -------------------------------------------------------------------------
   USB 
   -------------------------------------------------------------------------- */

#define STM32_ENABLE_OTG_FS          1

#define STM32_OTG_FS_INEP_MAX        3
#define STM32_OTG_FS_OUTEP_MAX       2
#define STM32_OTG_FS_IO_INIT         0
#define STM32_OTG_FS_VBUS_SENS       0
#define STM32_OTG_FS_VBUS_CONNECT    0
#define STM32_OTG_FS_RX_FIFO_SIZE    192
#define STM32_OTG_FS_BULK_PKTS       6

#define CDC_EP_IN_MAX_PKT_SIZE       64

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
#define TCP_TMR_PRIORITY             32

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
#define ENABLE_GDB      0
#define ENABLE_VCOM     0
#define ENABLE_I2C      1
#define ENABLE_TELNET   1

#define ENABLE_YMODEM   1

/* -------------------------------------------------------------------------
 * Trace 
 * ------------------------------------------------------------------------- */

#define TRACE_LEVEL           TRACE_LVL_DBG
#define TRACE_ENABLE_VT100    1

/* -------------------------------------------------------------------------
 * LibC
 * ------------------------------------------------------------------------- */

#define PRINTF_ENABLE_FLOAT 0

#endif /* __CONFIG_H__ */


