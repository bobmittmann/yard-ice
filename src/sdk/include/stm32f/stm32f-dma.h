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
 * @file stm32f-dma.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_DMA_H__
#define __STM32F_DMA_H__

/*-------------------------------------------------------------------------
  Direct Memory Access Controller (DMA)
  -------------------------------------------------------------------------*/

#if defined(STM32F2X) || defined(STM32F4X)

/* DMA low interrupt status register */
#define STM32F_DMA_LISR 0x000

/* Bits 31:28, 15:12 Reserved, always read as 0. */

/* Stream x transfer complete interrupt flag (x = 3..0) */
#define DMA_TCIF3 (1 << 27)
#define DMA_TCIF2 (1 << 21)
#define DMA_TCIF1 (1 << 11)
#define DMA_TCIF0 (1 << 5)
/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_LIFCR register.
	0: No transfer complete event on stream x
	1: A transfer complete event occurred on stream x */

/* Stream x half transfer interrupt flag (x=3..0) */
#define DMA_HTIF3 (1 << 26)
#define DMA_HTIF2 (1 << 20)
#define DMA_HTIF1 (1 << 10)
#define DMA_HTIF0 (1 << 4)
/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_LIFCR register.
	0: No half transfer event on stream x
	1: A half transfer event occurred on stream x */

/* Stream x transfer error interrupt flag (x=3..0) */
#define DMA_TEIF3 (1 << 25)
#define DMA_TEIF2 (1 << 19)
#define DMA_TEIF1 (1 << 9)
#define DMA_TEIF0 (1 << 3)
/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_LIFCR register.
	0: No transfer error on stream x
	1: A transfer error occurred on stream x */

/* Stream x direct mode error interrupt flag (x=3..0) */
#define DMA_DMEIF3 (1 << 24)
#define DMA_DMEIF2 (1 << 18)
#define DMA_DMEIF1 (1 << 8)
#define DMA_DMEIF0 (1 << 2)
/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_LIFCR register.
	0: No Direct Mode Error on stream x
	1: A Direct Mode Error occurred on stream x */

/* Bits 23, 17, 7, 1 Reserved, always read as 0 */

/* Stream x FIFO error interrupt flag (x=3..0) */
#define DMA_FEIF3 (1 << 22)
#define DMA_FEIF2 (1 << 16)
#define DMA_FEIF1 (1 << 6)
#define DMA_FEIF0 (1 << 0)
/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_LIFCR register.
	0: No FIFO Error event on stream x
	1: A FIFO Error event occurred on stream x */

/* DMA high interrupt status register */
#define STM32F_DMA_HISR 0x004

/* Bits 31:28, 15:12 Reserved, always read as 0. */

/* Stream x transfer complete interrupt flag (x=7..4) */
#define DMA_TCIF7 (1 << 27)
#define DMA_TCIF6 (1 << 21)
#define DMA_TCIF5 (1 << 11)
#define DMA_TCIF4 (1 << 5)
/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_HIFCR register.
	0: No transfer complete event on stream x
	1: A transfer complete event occurred on stream x */

/* Stream x half transfer interrupt flag (x=7..4) */
#define DMA_HTIF7 (1 << 26)
#define DMA_HTIF6 (1 << 20)
#define DMA_HTIF5 (1 << 10)
#define DMA_HTIF4 (1 << 4)
/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_HIFCR register.
	0: No half transfer event on stream x
	1: A half transfer event occurred on stream x */

/* Stream x transfer error interrupt flag (x=7..4) */
#define DMA_TEIF7 (1 << 25)
#define DMA_TEIF6 (1 << 19)
#define DMA_TEIF5 (1 << 9)
#define DMA_TEIF4 (1 << 3)
/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_HIFCR register.
	0: No transfer error on stream x
	1: A transfer error occurred on stream x */

/* Stream x direct mode error interrupt flag (x=7..4) */
#define DMA_DMEIF7 (1 << 24)
#define DMA_DMEIF6 (1 << 18)
#define DMA_DMEIF5 (1 << 8)
#define DMA_DMEIF4 (1 << 2)

/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_HIFCR register.
	0: No Direct mode error on stream x
	1: A Direct mode error occurred on stream x */

/* Bits 23, 17, 7, 1 Reserved, always read as 0 */

/* Stream x FIFO error interrupt flag (x=7..4) */
#define DMA_FEIF7 (1 << 22)
#define DMA_FEIF6 (1 << 16)
#define DMA_FEIF5 (1 << 6)
#define DMA_FEIF4 (1 << 0)

/* This bit is set by hardware. It is cleared by software writing 1 to the 
   corresponding bit in the DMA_HIFCR register.
	0: No FIFO error event on stream x
	1: A FIFO error event occurred on stream x */

/* DMA low interrupt flag clear register */
#define STM32F_DMA_LIFCR 0x008

/* Bits 31:28, 15:12 Reserved, always read as 0. */

/* Stream x clear transfer complete interrupt flag (x = 3..0) */
#define DMA_CTCIF3 (1 << 27)
#define DMA_CTCIF2 (1 << 21)
#define DMA_CTCIF1 (1 << 11)
#define DMA_CTCIF0 (1 << 5)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding TCIFx flag in the DMA_LISR register */

/* Stream x clear half transfer interrupt flag (x = 3..0) */
#define DMA_CHTIF3 (1 << 26)
#define DMA_CHTIF2 (1 << 20)
#define DMA_CHTIF1 (1 << 10)
#define DMA_CHTIF0 (1 << 4)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding HTIFx flag in the DMA_LISR register */

/* Stream x clear transfer error interrupt flag (x = 3..0) */
#define DMA_CTEIF3 (1 << 25)
#define DMA_CTEIF2 (1 << 19)
#define DMA_CTEIF1 (1 << 9)
#define DMA_CTEIF0 (1 << 3)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding TEIFx flag in the DMA_LISR register */

/* Stream x clear direct mode error interrupt flag (x = 3..0) */
#define DMA_CDMEIF3 (1 << 24)
#define DMA_CDMEIF2 (1 << 18)
#define DMA_CDMEIF1 (1 << 8)
#define DMA_CDMEIF0 (1 << 2)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding DMEIFx flag in the DMA_LISR register */

/* Bits 23, 17, 7, 1 Reserved, always read as 0. */

/* Stream x clear FIFO error interrupt flag (x = 3..0) */
#define DMA_CFEIF3 (1 << 22)
#define DMA_CFEIF2 (1 << 16)
#define DMA_CFEIF1 (1 << 6)
#define DMA_CFEIF0 (1 << 0)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding CFEIFx flag in the DMA_LISR register */

/* DMA high interrupt flag clear register */
#define STM32F_DMA_HIFCR 0x00c

/* Bits 31:28, 15:12 Reserved, always read as 0. */

/* Stream x clear transfer complete interrupt flag (x = 7..4) */
#define DMA_CTCIF7 (1 << 27)
#define DMA_CTCIF6 (1 << 21)
#define DMA_CTCIF5 (1 << 11)
#define DMA_CTCIF4 (1 << 5)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding TCIFx flag in the DMA_HISR register */

/* Stream x clear half transfer interrupt flag (x = 7..4) */
#define DMA_CHTIF7 (1 << 26)
#define DMA_CHTIF6 (1 << 20)
#define DMA_CHTIF5 (1 << 10)
#define DMA_CHTIF4 (1 << 4)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding HTIFx flag in the DMA_HISR register */

/* Stream x clear transfer error interrupt flag (x = 7..4) */
#define DMA_CTEIF7 (1 << 25)
#define DMA_CTEIF6 (1 << 19)
#define DMA_CTEIF5 (1 << 9)
#define DMA_CTEIF4 (1 << 3)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding TEIFx flag in the DMA_HISR register */

/* Stream x clear direct mode error interrupt flag (x = 7..4) */
#define DMA_CDMEIF7 (1 << 24)
#define DMA_CDMEIF6 (1 << 18)
#define DMA_CDMEIF5 (1 << 8)
#define DMA_CDMEIF4 (1 << 2)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding DMEIFx flag in the DMA_HISR register */

/* Bits 23, 17, 7, 1 Reserved, always read as 0 */

/* Stream x clear FIFO error interrupt flag (x = 7..4) */
#define DMA_CFEIF7 (1 << 22)
#define DMA_CFEIF6 (1 << 16)
#define DMA_CFEIF5 (1 << 6)
#define DMA_CFEIF4 (1 << 0)
/* This bit is set and cleared by software.
	0: No effect
	1: Clears the corresponding CFEIFx flag in the DMA_HISR register */

/*	DMA stream x configuration register */
#define STM32F_DMA_S0CR (0x010 + 0x018 * 0)
#define STM32F_DMA_S1CR (0x010 + 0x018 * 1)
#define STM32F_DMA_S2CR (0x010 + 0x018 * 2)
#define STM32F_DMA_S3CR (0x010 + 0x018 * 3)
#define STM32F_DMA_S4CR (0x010 + 0x018 * 4)
#define STM32F_DMA_S5CR (0x010 + 0x018 * 5)
#define STM32F_DMA_S6CR (0x010 + 0x018 * 6)
#define STM32F_DMA_S7CR (0x010 + 0x018 * 7)

/* [31..28] Reserved, always read as 0. */

/* Bits [27..25] - Channel selection */
#define DMA_CHSEL_MSK (((1 << (2 + 1)) - 1) << 25)
#define DMA_CHSEL_SET(VAL) (((VAL) << 25) & DMA_CHSEL_MSK)
#define DMA_CHSEL_GET(REG) (((REG) & DMA_CHSEL_MSK) >> 25)
/* These bits are set and cleared by software.
	000: channel 0 selected
	001: channel 1 selected
	010: channel 2 selected
	011: channel 3 selected
	100: channel 4 selected
	101: channel 5 selected
	110: channel 6 selected
	111: channel 7 selected
	These bits are protected and can be written only if EN is ‘0’ */

/* Bits [24..23] - Memory burst transfer configuration */
#define DMA_MBURST_MSK (((1 << (1 + 1)) - 1) << 23)
#define DMA_MBURST_SET(VAL) (((VAL) << 23) & DMA_MBURST_MSK)
#define DMA_MBURST_GET(REG) (((REG) & DMA_MBURST_MSK) >> 23)
#define DMA_MBURST_1 (0 << 23)
#define DMA_MBURST_4 (1 << 23)
#define DMA_MBURST_8 (2 << 23)
#define DMA_MBURST_16 (3 << 23)
/* These bits are set and cleared by software.
	00: single transfer
	01: INCR4 (incremental burst of 4 beats)
	10: INCR8 (incremental burst of 8 beats)
	11: INCR16 (incremental burst of 16 beats)
	These bits are protected and can be written only if EN is ‘0’
	In Direct mode, these bits are forced to 0x0 by hardware as soon 
	as bit EN= '1'. */

/* Bits [22..21] - Peripheral burst transfer configuration */
#define DMA_PBURST_MSK (((1 << (1 + 1)) - 1) << 21)
#define DMA_PBURST_SET(VAL) (((VAL) << 21) & DMA_PBURST_MSK)
#define DMA_PBURST_GET(REG) (((REG) & DMA_PBURST_MSK) >> 21)
#define DMA_PBURST_1 (0 << 23)
#define DMA_PBURST_4 (1 << 23)
#define DMA_PBURST_8 (2 << 23)
#define DMA_PBURST_16 (3 << 23)
/* These bits are set and cleared by software.
	00: single transfer
	01: INCR4 (incremental burst of 4 beats)
	10: INCR8 (incremental burst of 8 beats)
	11: INCR16 (incremental burst of 16 beats)
	These bits are protected and can be written only if EN is ‘0’
	In Direct mode, these bits are forced to 0x0 by hardware. */

/* Bit 20 Reserved. */

/* Bit 19 - Current target (only in double buffer mode) */
#define DMA_CT (1 << 19)
#define DMA_CT_M0AR (0 << 19)
#define DMA_CT_M1AR (1 << 19)
/* This bits is set and cleared by hardware. It can also be written by software.
	0: The current target memory is Memory 0 (addressed by the 
	DMA_SxM0AR pointer)
	1: The current target memory is Memory 1 (addressed by the 
	DMA_SxM1AR pointer)
	This bit can be written only if EN is ‘0’ to indicate the target 
	memory area of the first transfer.
	Once the stream is enabled, this bit operates as a status flag 
	indicating which memory area is the current target. */

/* Bit 18 - Double buffer mode */
#define DMA_DBM (1 << 18)
/* This bits is set and cleared by software.
	0: No buffer switching at the end of transfer
	1: Memory target switched at the end of the DMA transfer
	This bit is protected and can be written only if EN is ‘0’. */

/* Bits [17..16] - Priority level */
#define DMA_PL_MSK (((1 << (1 + 1)) - 1) << 16)
#define DMA_PL_SET(VAL) (((VAL) << 16) & DMA_PL_MSK)
#define DMA_PL_GET(REG) (((REG) & DMA_PL_MSK) >> 16)
/* These bits are set and cleared by software.
	00: Low
	01: Medium
	10: High
	11: Very high
	These bits are protected and can be written only if EN is ‘0’. */

/* Bit 15 - Peripheral increment offset size */
#define DMA_PINCOS (1 << 15)
/* This bit is set and cleared by software
	0: The offset size for the peripheral address calculation is 
	linked to the PSIZE
	1: The offset size for the peripheral address calculation is 
	fixed to 4 (32-bit alignment).
	This bit has no meaning if bit PINC = '0'.
	This bit is protected and can be written only if EN = '0'.
	This bit is forced low by hardware when the stream is enabled (bit EN = '1')
	if the direct mode is selected or if PBURST are different from “00”. */

/* Bits [14..13] - Memory data size */
#define DMA_MSIZE_MSK (((1 << (1 + 1)) - 1) << 13)
#define DMA_MSIZE_SET(VAL) (((VAL) << 13) & DMA_MSIZE_MSK)
#define DMA_MSIZE_GET(REG) (((REG) & DMA_MSIZE_MSK) >> 13)
#define DMA_MSIZE_8 (0 << 13)
#define DMA_MSIZE_16 (1 << 13)
#define DMA_MSIZE_32 (2 << 13)
/* These bits are set and cleared by software.
	00: byte (8-bit)
	01: half-word (16-bit)
	10: word (32-bit)
	11: reserved
	These bits are protected and can be written only if EN is ‘0’.
	In Direct mode, MSIZE is forced by hardware to the same value as 
	PSIZE as soon as bit EN = '1'. */

/* Bits [12..11] - Peripheral data size */
#define DMA_PSIZE_MSK (((1 << (1 + 1)) - 1) << 11)
#define DMA_PSIZE_SET(VAL) (((VAL) << 11) & DMA_PSIZE_MSK)
#define DMA_PSIZE_GET(REG) (((REG) & DMA_PSIZE_MSK) >> 11)
#define DMA_PSIZE_8 (0 << 11)
#define DMA_PSIZE_16 (1 << 11)
#define DMA_PSIZE_32 (2 << 11)
/* These bits are set and cleared by software.
	00: Byte (8-bit)
	01: Half-word (16-bit)
	10: Word (32-bit)
	11: reserved
	These bits are protected and can be written only if EN is ‘0’ */

/* Bit 10 - Memory increment mode */
#define DMA_MINC (1 << 10)
/* This bit is set and cleared by software.
	0: Memory address pointer is fixed
	1: Memory address pointer is incremented after each data 
	transfer (increment is done according to MSIZE)
	This bit is protected and can be written only if EN is ‘0’. */

/* Bit 9 - Peripheral increment mode */
#define DMA_PINC (1 << 9)
/* This bit is set and cleared by software.
	0: Peripheral address pointer is fixed
	1: Peripheral address pointer is incremented after each data 
	transfer (increment is done according to PSIZE)
	This bit is protected and can be written only if EN is ‘0’. */

/* Bit 8 - Circular mode */
#define DMA_CIRC (1 << 8)
/* This bit is set and cleared by software and can be cleared by hardware.
	0: Circular mode disabled
	1: Circular mode enabled
	When the peripheral is the flow controller (bit PFCTRL=1) and the 
	stream is enabled (bit EN=1), then this bit is automatically forced 
	by hardware to 0.
	It is automatically forced by hardware to 1 if the DBM bit is set, 
	as soon as the stream is enabled (bit EN ='1'). */

/* Bits [7..6] - Data transfer direction */
#define DMA_DIR_MSK (((1 << (1 + 1)) - 1) << 6)
#define DMA_DIR_SET(VAL) (((VAL) << 6) & DMA_DIR_MSK)
#define DMA_DIR_GET(REG) (((REG) & DMA_DIR_MSK) >> 6)
#define DMA_DIR_PTM (0 << 6)
#define DMA_DIR_MTP (1 << 6)
#define DMA_DIR_MTM (2 << 6)
/* These bits are set and cleared by software.
	00: Peripheral-to-memory
	01: Memory-to-peripheral
	10: Memory-to-memory
	11: reserved
	These bits are protected and can be written only if EN is ‘0’. */

/* Bit 5 - Peripheral flow controller */
#define DMA_PFCTRL (1 << 5)
/* This bit is set and cleared by software.
	0: The DMA is the flow controller
	1: The peripheral is the flow controller
	This bit is protected and can be written only if EN is ‘0’.
	When the memory-to-memory mode is selected (bits DIR[1:0]=10), 
	then this bit is automatically forced to 0 by hardware. */

/* Bit 4 - Transfer complete interrupt enable */
#define DMA_TCIE (1 << 4)
/* This bit is set and cleared by software.
	0: TC interrupt disabled
	1: TC interrupt enabled */

/* Bit 3 - Half transfer interrupt enable */
#define DMA_HTIE (1 << 3)
/* This bit is set and cleared by software.
	0: HT interrupt disabled
	1: HT interrupt enabled */

/* Bit 2 - Transfer error interrupt enable */
#define DMA_TEIE (1 << 2)
/* This bit is set and cleared by software.
	0: TE interrupt disabled
	1: TE interrupt enabled */

/* Bit 1 - Direct mode error interrupt enable */
#define DMA_DMEIE (1 << 1)
/* This bit is set and cleared by software.
	0: DME interrupt disabled
	1: DME interrupt enabled */

/* Bit 0 - Stream enable / flag stream ready when read low */
#define DMA_EN (1 << 0)
/* This bit is set and cleared by software.
	0: Stream disabled
	1: Stream enabled
	This bit may be cleared by hardware:
	– on a DMA end of transfer (stream ready to be configured)
	– if a transfer error occurs on the AHB master buses
	– when the FIFO threshold on memory AHB port is not compatible with the 
	size of the burst When this bit is read as 0, the software is allowed to 
	program the Configuration and FIFO bits registers. It is forbidden to 
	write these registers when the EN bit is read as 1. */


/* DMA stream x number of data register */
#define STM32F_DMA_S0NDTR (0x014 + 0x018 * 0)
#define STM32F_DMA_S1NDTR (0x014 + 0x018 * 1)
#define STM32F_DMA_S2NDTR (0x014 + 0x018 * 2)
#define STM32F_DMA_S3NDTR (0x014 + 0x018 * 3)
#define STM32F_DMA_S4NDTR (0x014 + 0x018 * 4)
#define STM32F_DMA_S5NDTR (0x014 + 0x018 * 5)
#define STM32F_DMA_S6NDTR (0x014 + 0x018 * 6)
#define STM32F_DMA_S7NDTR (0x014 + 0x018 * 7)

/* [31..16] Reserved, always read as 0. */

/* Bits [15..0] - Number of data items to transfer */
#define DMA_NDT_MSK (((1 << (15 + 1)) - 1) << 0)
#define DMA_NDT_SET(VAL) (((VAL) << 0) & DMA_NDT_MSK)
#define DMA_NDT_GET(REG) (((REG) & DMA_NDT_MSK) >> 0)
/* Number of data items to be transferred (0 up to 65535). This register can 
   be written only when the stream is disabled. When the stream is enabled, 
   this register is read-only, indicating the remaining data items to be 
   transmitted. This register decrements after each DMA transfer.
   Once the transfer has completed, this register can either stay at zero or 
   be reloaded automatically with the previously programmed value if the 
   stream is configured in Circular mode.
   If the value of this register is zero, no transaction can be served even 
   if the stream is enabled. */

/* DMA stream x peripheral address register */
#define STM32F_DMA_S0PAR (0x018 + 0x018 * 0)
#define STM32F_DMA_S1PAR (0x018 + 0x018 * 1)
#define STM32F_DMA_S2PAR (0x018 + 0x018 * 2)
#define STM32F_DMA_S3PAR (0x018 + 0x018 * 3)
#define STM32F_DMA_S4PAR (0x018 + 0x018 * 4)
#define STM32F_DMA_S5PAR (0x018 + 0x018 * 5)
#define STM32F_DMA_S6PAR (0x018 + 0x018 * 6)
#define STM32F_DMA_S7PAR (0x018 + 0x018 * 7)

/* Bits [31..0] - Peripheral address */
#define DMA_PAR_MSK (((1 << (31 + 1)) - 1) << 0)
#define DMA_PAR_SET(VAL) (((VAL) << 0) & DMA_PAR_MSK)
#define DMA_PAR_GET(REG) (((REG) & DMA_PAR_MSK) >> 0)
/* Base address of the peripheral data register from/to which the data will 
   be read/written. 
   These bits are write-protected and can be written only when 
   bit EN = '0' in the DMA_SxCR register. */

/*  DMA stream x memory 0 address register */
#define STM32F_DMA_S0M0AR (0x01c + 0x018 * 0)
#define STM32F_DMA_S1M0AR (0x01c + 0x018 * 1)
#define STM32F_DMA_S2M0AR (0x01c + 0x018 * 2)
#define STM32F_DMA_S3M0AR (0x01c + 0x018 * 3)
#define STM32F_DMA_S4M0AR (0x01c + 0x018 * 4)
#define STM32F_DMA_S5M0AR (0x01c + 0x018 * 5)
#define STM32F_DMA_S6M0AR (0x01c + 0x018 * 6)
#define STM32F_DMA_S7M0AR (0x01c + 0x018 * 7)

/* Bits [31..0] - Memory 0 address */
#define DMA_M0A_MSK (((1 << (31 + 1)) - 1) << 0)
#define DMA_M0A_SET(VAL) (((VAL) << 0) & DMA_M0A_MSK)
#define DMA_M0A_GET(REG) (((REG) & DMA_M0A_MSK) >> 0)
/* Base address of Memory area 0 from/to which the data will be read/written.
   These bits are write-protected. They can be written only if:
   – the stream is disabled (bit EN= '0' in the DMA_SxCR register) or
   – the stream is enabled (EN=’1’ in DMA_SxCR register) and bit CT = '1' 
   in the DMA_SxCR register (in Double buffer mode). */

/* DMA stream x memory 1 address register */
#define STM32F_DMA_S0M1AR (0x020 + 0x018 * 0)
#define STM32F_DMA_S1M1AR (0x020 + 0x018 * 1)
#define STM32F_DMA_S2M1AR (0x020 + 0x018 * 2)
#define STM32F_DMA_S3M1AR (0x020 + 0x018 * 3)
#define STM32F_DMA_S4M1AR (0x020 + 0x018 * 4)
#define STM32F_DMA_S5M1AR (0x020 + 0x018 * 5)
#define STM32F_DMA_S6M1AR (0x020 + 0x018 * 6)
#define STM32F_DMA_S7M1AR (0x020 + 0x018 * 7)

/* Bits [31..0] - Memory 1 address (used in case of Double buffer mode) */
#define DMA_M1A_MSK (((1 << (31 + 1)) - 1) << 0)
#define DMA_M1A_SET(VAL) (((VAL) << 0) & DMA_M1A_MSK)
#define DMA_M1A_GET(REG) (((REG) & DMA_M1A_MSK) >> 0)
/* Base address of Memory area 1 from/to which the data will be read/written.
   This register is used only for the Double buffer mode.
   These bits are write-protected. They can be written only if:
   – the stream is disabled (bit EN= '0' in the DMA_SxCR register) or
   – the stream is enabled (EN=’1’ in DMA_SxCR register) and bit CT = '0' 
   in the DMA_SxCR register. */

/* DMA stream x FIFO control register */
#define STM32F_DMA_S0FCR (0x24 + 0x018 * 0)
#define STM32F_DMA_S1FCR (0x24 + 0x018 * 1)
#define STM32F_DMA_S2FCR (0x24 + 0x018 * 2)
#define STM32F_DMA_S3FCR (0x24 + 0x018 * 3)
#define STM32F_DMA_S4FCR (0x24 + 0x018 * 4)
#define STM32F_DMA_S5FCR (0x24 + 0x018 * 5)
#define STM32F_DMA_S6FCR (0x24 + 0x018 * 6)
#define STM32F_DMA_S7FCR (0x24 + 0x018 * 7)

/* [31..8] Reserved, always read as 0. */

/* Bit 7 - FIFO error interrupt enable */
#define DMA_FEIE (1 << 7)
/* This bit is set and cleared by software.
	0: FE interrupt disabled
	1: FE interrupt enabled */

/* Bit 6 Reserved, always read as 0 */

/* Bits [5..3] - FIFO status */
#define DMA_FS_MSK (((1 << (2 + 1)) - 1) << 3)
#define DMA_FS_SET(VAL) (((VAL) << 3) & DMA_FS_MSK)
#define DMA_FS_GET(REG) (((REG) & DMA_FS_MSK) >> 3)
/* These bits are read-only.
	000: 0 < fifo_level < 1/4
	001: 1/4 ≤ fifo_level < 1/2
	010: 1/2 ≤ fifo_level < 3/4
	011: 3/4 ≤ fifo_level < full
	100: FIFO is empty
	101: FIFO is full
	others: no meaning 
	These bits are not relevant in the DIrect mode (DMDIS bit is zero). */

/* Bit 2 - Direct mode disable */
#define DMA_DMDIS (1 << 2)
/* This bit is set and cleared by software. It can be set by hardware.
	0: Direct mode enabled
	1: Direct mode disabled
	This bit is protected and can be written only if EN is ‘0’.
	This bit is set by hardware if the memory-to-memory mode is 
	selected (DIR bit in DMA_SxCR are “10”) and the EN bit in the DMA_SxCR 
	register is ‘1’ because the Direct mode is not
	allowed in the memory-to-memory configuration. */

/* Bits [1..0] - FIFO threshold selection */
#define DMA_FTH_MSK (((1 << (1 + 1)) - 1) << 0)
#define DMA_FTH_SET(VAL) (((VAL) << 0) & DMA_FTH_MSK)
#define DMA_FTH_GET(REG) (((REG) & DMA_FTH_MSK) >> 0)
#define DMA_FTH_1_4 (0 << 0)
#define DMA_FTH_1_2 (1 << 0)
#define DMA_FTH_1_3 (2 << 0)
#define DMA_FTH_FULL (3 << 0)
/* These bits are set and cleared by software.
	00: 1/4 full FIFO
	01: 1/2 full FIFO
	10: 3/4 full FIFO
	11: full FIFO
	These bits are not used in the Direct Mode when the DMIS value is zero.
	These bits are protected and can be written only if EN is ‘1’. */

#endif



#if defined(STM32F1X) || defined(STM32F3X) || defined(STM32L1X)

/* DMA interrupt status register */
#define STM32F_DMA_ISR 0x000

/* Bits 31:28 Reserved, must be kept at reset value.
Bits 27, 23, 19, 15,
11, 7, 3
TEIFx: Channel x transfer error flag (x = 1 ..7)
This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the
DMA_IFCR register.
0: No transfer error (TE) on channel x
1: A transfer error (TE) occurred on channel x
Bits 26, 22, 18, 14,
10, 6, 2
HTIFx: Channel x half transfer flag (x = 1 ..7)
This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the
DMA_IFCR register.
0: No half transfer (HT) event on channel x
1: A half transfer (HT) event occurred on channel x
Bits 25, 21, 17, 13,
9, 5, 1
TCIFx: Channel x transfer complete flag (x = 1 ..7)
This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the
DMA_IFCR register.
0: No transfer complete (TC) event on channel x
1: A transfer complete (TC) event occurred on channel x
Bits 24, 20, 16, 12,
8, 4, 0
GIFx: Channel x global interrupt flag (x = 1 ..7)
This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the
DMA_IFCR register.
0: No TE, HT or TC event on channel x
1: A TE, HT or TC event occurred on channel x */

#define DMA_TEIF7 (1 << 27)
#define DMA_HTIF7 (1 << 26)
#define DMA_TCIF7 (1 << 25)
#define DMA_GIF7  (1 << 24)

#define DMA_TEIF6 (1 << 23)
#define DMA_HTIF6 (1 << 22)
#define DMA_TCIF6 (1 << 21)
#define DMA_GIF6  (1 << 20)

#define DMA_TEIF5 (1 << 19)
#define DMA_HTIF5 (1 << 18)
#define DMA_TCIF5 (1 << 17)
#define DMA_GIF5  (1 << 16)

#define DMA_TEIF4 (1 << 15)
#define DMA_HTIF4 (1 << 14)
#define DMA_TCIF4 (1 << 13)
#define DMA_GIF4  (1 << 12)

#define DMA_TEIF3 (1 << 11)
#define DMA_HTIF3 (1 << 10)
#define DMA_TCIF3 (1 << 9)
#define DMA_GIF3  (1 << 8)

#define DMA_TEIF2 (1 << 7)
#define DMA_HTIF2 (1 << 6)
#define DMA_TCIF2 (1 << 5)
#define DMA_GIF2  (1 << 4)

#define DMA_TEIF1 (1 << 3)
#define DMA_HTIF1 (1 << 2)
#define DMA_TCIF1 (1 << 1)
#define DMA_GIF1  (1 << 0)

/* DMA interrupt flag clear register */
#define STM32F_DMA_IFCR 0x004

/*
Bits 31:28 Reserved, must be kept at reset value.
Bits 27, 23, 19, 15,
11, 7, 3
CTEIFx: Channel x transfer error clear (x = 1 ..7)
This bit is set and cleared by software.
0: No effect
1: Clears the corresponding TEIF flag in the DMA_ISR register
Bits 26, 22, 18, 14,
10, 6, 2
CHTIFx: Channel x half transfer clear (x = 1 ..7)
This bit is set and cleared by software.
0: No effect
1: Clears the corresponding HTIF flag in the DMA_ISR register
Bits 25, 21, 17, 13,
9, 5, 1
CTCIFx: Channel x transfer complete clear (x = 1 ..7)
This bit is set and cleared by software.
0: No effect
1: Clears the corresponding TCIF flag in the DMA_ISR register
Bits 24, 20, 16, 12,
8, 4, 0
CGIFx: Channel x global interrupt clear (x = 1 ..7)
This bit is set and cleared by software.
0: No effect
1: Clears the GIF, TEIF, HTIF and TCIF flags in the DMA_ISR register
*/

#define DMA_CTEIF7 (1 << 27)
#define DMA_CHTIF7 (1 << 26)
#define DMA_CTCIF7 (1 << 25)
#define DMA_CGIF7  (1 << 24)

#define DMA_CTEIF6 (1 << 23)
#define DMA_CHTIF6 (1 << 22)
#define DMA_CTCIF6 (1 << 21)
#define DMA_CGIF6  (1 << 20)

#define DMA_CTEIF5 (1 << 19)
#define DMA_CHTIF5 (1 << 18)
#define DMA_CTCIF5 (1 << 17)
#define DMA_CGIF5  (1 << 16)

#define DMA_CTEIF4 (1 << 15)
#define DMA_CHTIF4 (1 << 14)
#define DMA_CTCIF4 (1 << 13)
#define DMA_CGIF4  (1 << 12)

#define DMA_CTEIF3 (1 << 11)
#define DMA_CHTIF3 (1 << 10)
#define DMA_CTCIF3 (1 << 9)
#define DMA_CGIF3  (1 << 8)

#define DMA_CTEIF2 (1 << 7)
#define DMA_CHTIF2 (1 << 6)
#define DMA_CTCIF2 (1 << 5)
#define DMA_CGIF2  (1 << 4)

#define DMA_CTEIF1 (1 << 3)
#define DMA_CHTIF1 (1 << 2)
#define DMA_CTCIF1 (1 << 1)
#define DMA_CGIF1  (1 << 0)

/*	DMA channel x configuration register */
#define STM32F_DMA_CCR1 (0x08 + 20 * 0)
#define STM32F_DMA_CCR2 (0x08 + 20 * 1)
#define STM32F_DMA_CCR3 (0x08 + 20 * 2)
#define STM32F_DMA_CCR4 (0x08 + 20 * 3)
#define STM32F_DMA_CCR5 (0x08 + 20 * 4)
#define STM32F_DMA_CCR6 (0x08 + 20 * 5)
#define STM32F_DMA_CCR7 (0x08 + 20 * 6)

/* Bits 31:15 Reserved, must be kept at reset value. */

/*Bit 14 MEM2MEM: Memory to memory mode */
#define DMA_MEM2MEM (1 << 14)
/* This bit is set and cleared by software.
0: Memory to memory mode disabled
1: Memory to memory mode enabled */

/* Bits 13:12 PL[1:0]: Channel priority level */
#define DMA_PL_LOW       (0 << 12)
#define DMA_PL_MEDIUM    (1 << 12)
#define DMA_PL_HIGH      (2 << 12)
#define DMA_PL_VERY_HIGH (2 << 12)
/*These bits are set and cleared by software.
00: Low
01: Medium
10: High
11: Very high */

/* Bits 11:10 MSIZE[1:0]: Memory size */
#define DMA_MSIZE_8  (0 << 10)
#define DMA_MSIZE_16 (1 << 10)
#define DMA_MSIZE_32 (2 << 10)
/* These bits are set and cleared by software.
00: 8-bits
01: 16-bits
10: 32-bits
11: Reserved */

/* Bits 9:8 PSIZE[1:0]: Peripheral size */
#define DMA_PSIZE_8  (0 << 8)
#define DMA_PSIZE_16 (1 << 8)
#define DMA_PSIZE_32 (2 << 8)
/* These bits are set and cleared by software.
00: 8-bits
01: 16-bits
10: 32-bits
11: Reserved */

/* Bit 7 MINC: Memory increment mode */
#define DMA_MINC (1 << 7)
/* This bit is set and cleared by software.
0: Memory increment mode disabled
1: Memory increment mode enabled */

/* Bit 6 PINC: Peripheral increment mode */
#define DMA_PINC (1 << 6)
/* This bit is set and cleared by software.
0: Peripheral increment mode disabled
1: Peripheral increment mode enabled */

/* Bit 5 CIRC: Circular mode */
#define DMA_CIRC (1 << 5)
/* This bit is set and cleared by software.
0: Circular mode disabled
1: Circular mode enabled */

/* Bit 4 DIR: Data transfer direction */
#define DMA_DIR (1 << 4)
#define DMA_DIR_PTM (0 << 4)
#define DMA_DIR_MTP (1 << 4)
/* This bit is set and cleared by software.
0: Read from peripheral
1: Read from memory */

/* Bit 3 TEIE: Transfer error interrupt enable */
#define DMA_TEIE (1 << 3)
/* This bit is set and cleared by software.
0: TE interrupt disabled
1: TE interrupt enabled */

/* Bit 2 HTIE: Half transfer interrupt enable */
#define DMA_HTIE (1 << 2)
/* This bit is set and cleared by software.
0: HT interrupt disabled
1: HT interrupt enabled */

/* Bit 1 TCIE: Transfer complete interrupt enable */
#define DMA_TCIE (1 << 1)
/* This bit is set and cleared by software.
0: TC interrupt disabled
1: TC interrupt enabled */

/* Bit 0 EN: Channel enable */
#define DMA_EN (1 << 0)
/* This bit is set and cleared by software.
0: Channel disabled
1: Channel enabled*/

/*	DMA channel x number of data register */
#define STM32F_DMA_CNDTR1 (0x0C + 20 * 0)
#define STM32F_DMA_CNDTR2 (0x0C + 20 * 1)
#define STM32F_DMA_CNDTR3 (0x0C + 20 * 2)
#define STM32F_DMA_CNDTR4 (0x0C + 20 * 3)
#define STM32F_DMA_CNDTR5 (0x0C + 20 * 4)
#define STM32F_DMA_CNDTR6 (0x0C + 20 * 5)
#define STM32F_DMA_CNDTR7 (0x0C + 20 * 6)

/*	DMA channel x peripheral address register */
#define STM32F_DMA_CPAR1 (0x10 + 20 * 0)
#define STM32F_DMA_CPAR2 (0x10 + 20 * 1)
#define STM32F_DMA_CPAR3 (0x10 + 20 * 2)
#define STM32F_DMA_CPAR4 (0x10 + 20 * 3)
#define STM32F_DMA_CPAR5 (0x10 + 20 * 4)
#define STM32F_DMA_CPAR6 (0x10 + 20 * 5)
#define STM32F_DMA_CPAR7 (0x10 + 20 * 6)

/*	DMA channel x memory address register */
#define STM32F_DMA_CMAR1 (0x14 + 20 * 0)
#define STM32F_DMA_CMAR2 (0x14 + 20 * 1)
#define STM32F_DMA_CMAR3 (0x14 + 20 * 2)
#define STM32F_DMA_CMAR4 (0x14 + 20 * 3)
#define STM32F_DMA_CMAR5 (0x14 + 20 * 4)
#define STM32F_DMA_CMAR6 (0x14 + 20 * 5)
#define STM32F_DMA_CMAR7 (0x14 + 20 * 6)

#define STM32_DMA_CHANNEL1 0
#define STM32_DMA_CHANNEL2 1
#define STM32_DMA_CHANNEL3 2
#define STM32_DMA_CHANNEL4 3
#define STM32_DMA_CHANNEL5 4
#define STM32_DMA_CHANNEL6 5
#define STM32_DMA_CHANNEL7 6

#endif


#ifndef __ASSEMBLER__

#include <stdint.h>

#if defined(STM32F2X) || defined(STM32F4X)
struct stm32f_dma_stream {
	volatile uint32_t cr;
	volatile uint32_t ndtr;
	volatile void * par;
	volatile void * m0ar;
	volatile void * m1ar;
	volatile uint32_t fcr;
};

struct stm32f_dma {
	volatile uint32_t lisr;
	volatile uint32_t hisr;
	volatile uint32_t lifcr;
	volatile uint32_t hifcr;
	struct stm32f_dma_stream s[8];
};
#endif

#if defined(STM32F1X) || defined(STM32F3X) || defined(STM32L1X)

/* Fake Channel selection */
#define DMA_CHSEL_SET(VAL) 0

struct stm32f_dma_channel {
	volatile uint32_t ccr;
	volatile uint32_t cndtr;
	volatile void * cpar;
	volatile void * cmar;
	uint32_t res1;
};

/*
XXX: t seems that the 1xx and 3xx devices implements a reduced version
of the the DMA stream found on 2xx and 4xx parts... */
struct stm32f_dma_stream {
	volatile uint32_t cr;
	volatile uint32_t ndtr;
	volatile void * par;
	volatile void * m0ar;
};

struct stm32f_dma {
	volatile uint32_t isr;
	volatile uint32_t ifcr;
	union {
		struct stm32f_dma_channel ch[8];
		struct stm32f_dma_stream s[8];
	};
};
#endif


#endif /* __ASSEMBLER__ */

#endif /* __STM32F_DMA_H__ */

