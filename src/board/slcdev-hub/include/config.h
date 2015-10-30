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
#define THINKOS_IDLE_STACK_CONST        0
#define THINKOS_IDLE_STACK_BSS          0
#define THINKOS_IDLE_STACK_ALLOC        1

#define THINKOS_ENABLE_CLOCK            1

#define THINKOS_ENABLE_TIMESHARE        0
#define THINKOS_SCHED_LIMIT_MAX         32
#define THINKOS_SCHED_LIMIT_MIN         1

#define THINKOS_THREADS_MAX             32
#define THINKOS_ENABLE_THREAD_ALLOC     1

#define THINKOS_ENABLE_JOIN             1
#define THINKOS_ENABLE_PAUSE            1
#define THINKOS_ENABLE_CANCEL           1
#define THINKOS_ENABLE_EXIT             1

#define THINKOS_MUTEX_MAX               32
#define THINKOS_ENABLE_MUTEX_ALLOC      1

#define THINKOS_COND_MAX                64
#define THINKOS_ENABLE_COND_ALLOC       1

#define THINKOS_SEMAPHORE_MAX           24
#define THINKOS_ENABLE_SEM_ALLOC        1

#define THINKOS_EVENT_MAX               4
#define THINKOS_ENABLE_EVENT_ALLOC      1

#define THINKOS_FLAG_MAX                32
#define THINKOS_ENABLE_FLAG_ALLOC       1
#define THINKOS_ENABLE_FLAG_WATCH       1

#define THINKOS_GATE_MAX                16
#define THINKOS_ENABLE_GATE_ALLOC       1

#define THINKOS_ENABLE_THREAD_STAT      1
#define THINKOS_ENABLE_TIMED_CALLS      1

#define THINKOS_ENABLE_ARG_CHECK        1
#define THINKOS_ENABLE_DEADLOCK_CHECK   1
#define THINKOS_ENABLE_SANITY_CHECK     1

#define THINKOS_ENABLE_EXCEPTIONS       1
#define THINKOS_ENABLE_EXCEPT_CLEAR     1

#define THINKOS_EXCEPT_STACK_SIZE       320
#define THINKOS_STDERR_FAULT_DUMP       0

#define THINKOS_ENABLE_SCHED_DEBUG      0

#define THINKOS_ENABLE_RT_DEBUG         1

#define THINKOS_ENABLE_BREAK            1

#define THINKOS_ENABLE_PROFILING        1

#define THINKOS_ENABLE_MONITOR          1
#define THINKOS_ENABLE_CONSOLE          1

#define THINKOS_ENABLE_DEBUG_STEP       1
#define THINKOS_ENABLE_DEBUG_FAULT      1

#define THINKOS_ENABLE_BUSFAULT         1
#define THINKOS_ENABLE_USAGEFAULT       1
#define THINKOS_ENABLE_MEMFAULT         1
#define THINKOS_UNROLL_EXCEPTIONS       1

#define THINKOS_ENABLE_MPU              1
#define THINKOS_ENABLE_ESCALATE         1
#define THINKOS_ENABLE_FPU              0

#define THINKOS_ENABLE_THREAD_VOID      1
#define THINKOS_ENABLE_DMCLOCK          1

#define THINKOS_ENABLE_CTL              1

/* -------------------------------------------------------------------------
   USB 
   -------------------------------------------------------------------------- */

#define STM32_ENABLE_OTG_FS 1
#define STM32_OTG_FS_EP_MAX 4

/* -------------------------------------------------------------------------
   GDB
   -------------------------------------------------------------------------- */

#define GDB_ENABLE_MEM_MAP 0

/* -------------------------------------------------------------------------
   Bootloader 
   -------------------------------------------------------------------------- */

#define BOOT_ENABLE_GDB 1
#define MONITOR_CONFIGURE_ENABLE  1
#define MONITOR_DUMPMEM_ENABLE    1
#define MONITOR_UPGRADE_ENABLE    1
#define MONITOR_STACKUSAGE_ENABLE 1
#define MONITOR_THREADINFO_ENABLE 1
#define MONITOR_APPWIPE_ENABLE    1

/* -------------------------------------------------------------------------
 * TCP/IP
 * ------------------------------------------------------------------------- */

#define ENABLE_NETWORK 1
#define ENABLE_NET_TCP 1
#define ENABLE_NET_UDP 1
#define ENABLE_NET_RAW 1

#define ENABLE_TCPDUMP 0

#define IFNET_INTERFACES_MAX         2

#define MBUF_POOL_SIZE               128

#define TCP_FAST_TMR_MS              100
#define TCP_ENABLE_TIMEWAIT          1
#define TCP_ENABLE_HEADER_PREDICTION 1
#define NET_TCP_PCB_ACTIVE_MAX       64
#define NET_UDP_PCB_ACTIVE_MAX       1

#define TCP_DEFAULT_MSL_SEC          1
#define TCP_IDLE_DET_SEC             10
#define TCP_KEEP_ALIVE_PROBE_SEC     20
#define TCP_MAX_IDLE_SEC             60
#define TCP_MAX_SND_QUEUE            (1460 * 4)

/* use hardware checksum */
#define ENABLE_NET_TCP_CHECKSUM 0
#define ENABLE_NET_UDP_CHECKSUM 0

/* -------------------------------------------------------------------------
 * Ethernet Driver
 * ------------------------------------------------------------------------- */

#define ETH_PHY_RST_GPIO         STM32_GPIOE, 5

/* Number of reception DMA descriptors for the Ethernet driver */
#define STM32F_ETH_RX_NDESC      4
/* Number of transmision DMA descriptors for the Ethernet driver */
#define STM32F_ETH_TX_NDESC      4
/* Number of packet in the buffer pool */
#define PKTBUF_POOL_SIZE        10 

#endif /* __CONFIG_H__ */

