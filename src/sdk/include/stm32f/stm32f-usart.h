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
 * @file stm32f-usart.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_USART_H__
#define __STM32F_USART_H__

/*-------------------------------------------------------------------------
  Universal synchronous asynchronous receiver transmitter (USART)
  -------------------------------------------------------------------------*/

/* USART Status register */
#define STM32F_USART_SR 0x00

/* [31..10] Reserved, forced by hardware to 0. */

#define USART_CTS (1 << 9) /* Bit 9 - CTS flag */
/* This bit is set by hardware when the nCTS input toggles, if the CTSE bit 
   is set. It is cleared by software (by writing it to 0). An interrupt is 
   generated if CTSIE=1 in the USART_CR3 register.
	0: No change occurred on the nCTS status line
	1: A change occurred on the nCTS status line
	Note: This bit is not available for UART4 & UART5. */

#define USART_LBD (1 << 8) /* Bit 8 - LIN break detection flag */
/* This bit is set by hardware when the LIN break is detected. It is cleared 
   by software (by writing it to 0). An interrupt is generated if LBDIE = 1 
   in the USART_CR2 register.
	0: LIN Break not detected
	1: LIN break detected
	Note: An interrupt is generated when LBD=1 if LBDIE=1 */

#define USART_TXE (1 << 7) /* Bit 7 - Transmit data register empty */
/* This bit is set by hardware when the content of the TDR register has been 
   transferred into the shift register. An interrupt is generated if the 
   TXEIE bit =1 in the USART_CR1 register. It is cleared by a write to the 
   USART_DR register.
	0: Data is not transferred to the shift register
	1: Data is transferred to the shift register)
	Note: This bit is used during single buffer transmission. */

#define USART_TC (1 << 6) /* Bit 6 - Transmission complete */
/* This bit is set by hardware if the transmission of a frame containing 
   data is complete and if TXE is set. An interrupt is generated if TCIE=1 
   in the USART_CR1 register. It is cleared by a software sequence (a read 
   from the USART_SR register followed by a write to the USART_DR register). 
   The TC bit can also be cleared by writing a '0' to it. This clearing 
   sequence is recommended only for multibuffer communication.
	0: Transmission is not complete
	1: Transmission is complete */

#define USART_RXNE (1 << 5) /* Bit 5 - Read data register not empty */
/* This bit is set by hardware when the content of the RDR shift register 
   has been transferred to the USART_DR register. An interrupt is generated 
   if RXNEIE=1 in the USART_CR1 register. It is cleared by a read to the 
   USART_DR register. The RXNE flag can also be cleared by writing a zero 
   to it. This clearing sequence is recommended only for multibuffer 
   communication.
	0: Data is not received
	1: Received data is ready to be read. */

#define USART_IDLE (1 << 4) /* Bit 4 - IDLE line detected */
/* This bit is set by hardware when an Idle Line is detected. An interrupt 
   is generated if the IDLEIE=1 in the USART_CR1 register. It is cleared by 
   a software sequence (an read to the USART_SR register followed by a read 
   to the USART_DR register).
	0: No Idle Line is detected
	1: Idle Line is detected
	Note: The IDLE bit will not be set again until the RXNE bit has been set 
	itself (i.e. a new idle line occurs). */

#define USART_ORE (1 << 3) /* Bit 3 - Overrun error */
/* This bit is set by hardware when the word currently being received in 
   the shift register is ready to be transferred into the RDR register while 
   RXNE=1. An interrupt is generated if RXNEIE=1 in the USART_CR1 register. 
   It is cleared by a software sequence (an read to the USART_SR register 
   followed by a read to the USART_DR register).
	0: No Overrun error
	1: Overrun error is detected
	Note: When this bit is set, the RDR register content will not be lost but 
	the shift register will be overwritten. An interrupt is generated on ORE 
	flag in case of Multi Buffer communication if the EIE bit is set. */

#define USART_NF (1 << 2) /* Bit 2 - Noise detected flag */
#define USART_NE (1 << 2) /* Bit 2 - Noise detected flag */
/* This bit is set by hardware when noise is detected on a received frame. It is cleared by a software sequence (an read to the USART_SR register followed by a read to the USART_DR register).
0: No noise is detected
1: Noise is detected
Note: This bit does not generate interrupt as it appears at the same time as the RXNE bit which itself generates an interrupting interrupt is generated on NF flag in case of Multi Buffer communication if the EIE bit is set.
Note: When the line is noise-free, the NF flag can be disabled by programming the ONEBIT bit to 1 to increase the USART tolerance to deviations (Refer to Section 24.3.5: USART receiver’s tolerance to clock deviation on page 621). */

#define USART_FE (1 << 1) /* Bit 1 - Framing error */
/* This bit is set by hardware when a de-synchronization, excessive noise or a break character is detected. It is cleared by a software sequence (an read to the USART_SR register followed by a read to the USART_DR register).
0: No Framing error is detected
1: Framing error or break character is detected
Note: This bit does not generate interrupt as it appears at the same time as the RXNE bit which itself generates an interrupt. If the word currently being transferred causes both frame error and overrun error, it will be transferred and only the ORE bit will be set.
An interrupt is generated on FE flag in case of Multi Buffer communication if the EIE bit is set. */

#define USART_PE (1 << 0) /* Bit 0 - Parity error */

/* This bit is set by hardware when a parity error occurs in receiver mode. It is cleared by a software sequence (a read from the status register followed by a read or write access to the USART_DR data register). The software must wait for the RXNE flag to be set before clearing the PE bit.
An interrupt is generated if PEIE = 1 in the USART_CR1 register.
0: No parity error
1: Parity error */


/* USART Data register */
#define STM32F_USART_DR 0x04 

#define USART_DR_MSK (((1 << (8 + 1)) - 1) << 0) /* Bits [8..0] - Data value */
#define USART_DR_SET(VAL) (((VAL) << 0) & DR_MSK)
#define USART_DR_GET(REG) (((REG) & DR_MSK) >> 0)
/* Contains the Received or Transmitted data character, depending on whether 
   it is read from or written to.
   The Data register performs a double function (read and write) since it is 
   composed of two registers, one for transmission (TDR) and one for reception 
   (RDR) The TDR register provides the parallel interface between the internal 
   bus and the output shift register (see Figure 1). The RDR register provides 
   the parallel interface between the input shift register and the internal 
   bus. When transmitting with the parity enabled (PCE bit set to 1 in the 
   USART_CR1 register), the value written in the MSB (bit 7 or bit 8 depending 
   on the data length) has no effect because it is replaced by the parity. */

/* Baud rate register */
#define STM32F_USART_BRR 0x08
/* Note: The baud counters stop counting if the TE or RE bits 
   are disabled respectively. */

#define USART_DIV_MANTISSA_MSK (((1 << (11 + 1)) - 1) << 4) /* Bits [15..4] - mantissa of USARTDIV */
#define USART_DIV_MANTISSA_SET(VAL) (((VAL) << 4) & DIV_MANTISSA_MSK)
#define USART_DIV_MANTISSA_GET(REG) (((REG) & DIV_MANTISSA_MSK) >> 4)

/*	These 12 bits define the mantissa of the USART Divider (USARTDIV) */

#define USART_DIV_FRACTION_MSK (((1 << (3 + 1)) - 1) << 0) /* Bits [3..0] - fraction of USARTDIV */
#define USART_DIV_FRACTION_SET(VAL) (((VAL) << 0) & DIV_FRACTION_MSK)
#define USART_DIV_FRACTION_GET(REG) (((REG) & DIV_FRACTION_MSK) >> 0)

/*	These 4 bits define the fraction of the USART Divider (USARTDIV) */

/* ------------------------------------------------------------------------- */
/* USART Control register 1 */
#if defined(STM32F3X)
#define STM32F_USART_CR1 0x00
#else
#define STM32F_USART_CR1 0x0C
#endif

/*	Reset value: 0x0000 */

/* Bit 15 - Oversampling mode */
#define USART_OVER8 (1 << 15) 
/* 0: oversampling by 16
   1: oversampling by 8
   Note: Oversampling by 8 is not available in the Smartcard, IrDA and 
   LIN modes: when SCEN=1,IREN=1 or LINEN=1 then OVER8 is forced to ‘0 by 
   hardware. */

#if defined(STM32F3X)

/* Bit 14 - Character match interrupt enable */
#define USART_CMIE (1 << 14) 
/* This bit is set and cleared by software.
   0: Interrupt is inhibited
   1: A USART interrupt is generated when the CMF bit is set in the 
   USART_ISR register. */

/* Bit 13 - Mute mode enable */
#define USART_MME (1 << 13) 
/* This bit activates the mute mode function of the USART. When set, 
   the USART can switch between the active and mute modes, as defined by 
   the WAKE bit. It is set and cleared by software.
   0: Receiver in active mode permanently
   1: Receiver can switch between mute mode and active mode. */

/* Bit 1 - USART enable in Stop mode */
#define USART_UESM (1 << 1) 
/* When this bit is cleared, the USART is not able to wake up the MCU 
   from Stop mode. When this bit is set, the USART is able to wake up 
   the MCU from Stop mode, provided that the USART clock selection is 
   HSI or LSE in the RCC. 
   This bit is set and cleared by software.
   0: USART not able to wake up the MCU from Stop mode.
   1: USART able to wake up the MCU from Stop mode. When this function is 
   active, the clock source for the USART must be HSI or LSE (see RCC chapter)
   Note: 1: It is recommended to set the UESM bit just before entering 
   Stop mode and clear it on exit from Stop mode.
   2: If the USART does not support the wakeup from Stop feature, 
   this bit is reserved and forced by hardware to ‘0’. */

/* Bit 0 - USART enable */
#define USART_UE (1 << 0) 

#else

/* Bit 13 - USART enable */
#define USART_UE (1 << 13) 

#endif
/* When this bit is cleared the USART prescalers and outputs are stopped 
   and the end of the current byte transfer in order to reduce power 
   consumption. This bit is set and cleared by software.
	0: USART prescaler and outputs disabled
	1: USART enabled */

/* Bit 12 - Word length */
#define USART_M (1 << 12) 
#define USART_M8 (0 << 12)
#define USART_M9 (1 << 12)
/* This bit determines the word length. It is set or cleared by software.
	0: 1 Start bit, 8 Data bits, n Stop bit
	1: 1 Start bit, 9 Data bits, n Stop bit
	Note: The M bit must not be modified during a data transfer 
	(both transmission and reception) */

/* Bit 11 - Wakeup method */
#define USART_WAKE (1 << 11) 
/* This bit determines the USART wakeup method, it is set or cleared by 
   software.
	0: Idle Line
	1: Address Mark */

/* Bit 10 - Parity control enable */
#define USART_PCE (1 << 10) 
/* This bit selects the hardware parity control (generation and detection). When the parity control is enabled, the computed parity is inserted at the MSB position (9th bit if M=1; 8th bit if M=0) and parity is checked on the received data. This bit is set and cleared by software.
	Once it is set, PCE is active after the current byte (in reception and in transmission).
	0: Parity control disabled
	1: Parity control enabled */

/* Bit 9 - Parity selection */
#define USART_PS (1 << 9)
#define USART_PS_EVEN (0 << 9)
#define USART_PS_ODD (1 << 9)
/* This bit selects the odd or even parity when the parity 
   generation/detection is enabled (PCE bit set). It is set and cleared 
   by software. The parity will be selected after the current byte.
	0: Even parity
	1: Odd parity */

/* Bit 8 - PE interrupt enable */
#define USART_PEIE (1 << 8) 
/* This bit is set and cleared by software.
	0: Interrupt is inhibited
	1: An USART interrupt is generated whenever PE=1 in the USART_SR register */

/* Bit 7 - TXE interrupt enable */
#define USART_TXEIE (1 << 7)
/* This bit is set and cleared by software.
	0: Interrupt is inhibited
	1: An USART interrupt is generated whenever TXE=1 in the 
	USART_SR register */

/* Bit 6 - Transmission complete interrupt enable */
#define USART_TCIE (1 << 6) 
/* This bit is set and cleared by software.
	0: Interrupt is inhibited
	1: An USART interrupt is generated whenever TC=1 in the USART_SR register */

/* Bit 5 - RXNE interrupt enable */
#define USART_RXNEIE (1 << 5) 
/* This bit is set and cleared by software.
	0: Interrupt is inhibited
	1: An USART interrupt is generated whenever ORE=1 or RXNE=1 in 
	the USART_SR register */

/* Bit 4 - IDLE interrupt enable */
#define USART_IDLEIE (1 << 4) 
/* This bit is set and cleared by software.
	0: Interrupt is inhibited
	1: An USART interrupt is generated whenever IDLE=1 in the 
	USART_SR register */

/* Bit 3 - Transmitter enable */
#define USART_TE (1 << 3) 
/* This bit enables the transmitter. It is set and cleared by software.
	0: Transmitter is disabled
	1: Transmitter is enabled
	Note: 1: During transmission, a “0” pulse on the TE 
	bit (“0” followed by “1”) sends a preamble
	(idle line) after the current word, except in smartcard mode.
	2: When TE is set there is a 1 bit-time delay before the 
	transmission starts. */

/* Bit 2 - Receiver enable */
#define USART_RE (1 << 2)
/* This bit enables the receiver. It is set and cleared by software.
	0: Receiver is disabled
	1: Receiver is enabled and begins searching for a start bit */

/* Bit 1 - Receiver wakeup */
#define USART_RWU (1 << 1)
/* This bit determines if the USART is in mute mode or not. It is set 
   and cleared by software and can be cleared by hardware when a wakeup 
   sequence is recognized.
	0: Receiver in active mode
	1: Receiver in mute mode
	Note: 1: Before selecting Mute mode (by setting the RWU bit) the 
	USART must first receive a data byte, otherwise it cannot function in 
	Mute mode with wakeup by Idle line detection.
	2: In Address Mark Detection wakeup configuration (WAKE bit=1) the RWU 
	bit cannot be modified by software while the RXNE bit is set. */

/* Bit 0 - Send break */
#define USART_SBK (1 << 0) 
/*	This bit set is used to send break characters. It can be set and 
	cleared by software. It should be set by software, and will be reset 
	by hardware during the stop bit of break.
	0: No break character is transmitted
	1: Break character will be transmitted */


/* ------------------------------------------------------------------------- */
/* USART Control register 2 */
#define STM32F_USART_CR2 0x10 

/* [31..15] Reserved, forced by hardware to 0. */

#define USART_LINEN (1 << 14) /* Bit 14 - LIN mode enable */
/* This bit is set and cleared by software.
	0: LIN mode disabled
	1: LIN mode enabled
	The LIN mode enables the capability to send LIN Synch Breaks (13 low bits) using the SBK bit in
	the USART_CR1 register, and to detect LIN Sync breaks. */

#define USART_STOP_MSK (0x3 << 12) /* Bits [13..12] - STOP bits */
#define USART_STOP_1   (0x0 << 12)
#define USART_STOP_0_5 (0x1 << 12)
#define USART_STOP_2   (0x2 << 12)
#define USART_STOP_1_5 (0x3 << 12)
/* These bits are used for programming the stop bits.
	00: 1 Stop bit
	01: 0.5 Stop bit
	10: 2 Stop bits
	11: 1.5 Stop bit
	Note: The 0.5 Stop bit and 1.5 Stop bit are not available for UART4 & UART5. */

#define USART_CLKEN (1 << 11) /* Bit 11 - Clock enable */
/* This bit allows the user to enable the SCLK pin.
	0: SCLK pin disabled
	1: SCLK pin enabled
	This bit is not available for UART4 & UART5. */

#define USART_CPOL (1 << 10) /* Bit 10 - Clock polarity */
/* This bit allows the user to select the polarity of the clock output on 
   the SCLK pin in synchronous mode. It works in conjunction with the 
   CPHA bit to produce the desired clock/data relationship
	0: Steady low value on SCLK pin outside transmission window.
	1: Steady high value on SCLK pin outside transmission window.
	This bit is not available for UART4 & UART5. */

#define USART_CPHA (1 << 9) /* Bit 9 - Clock phase */
/* This bit allows the user to select the phase of the clock output on the SCLK pin in synchronous mode. It works in conjunction with the CPOL bit to produce the desired clock/data relationship (see figures 234 to 235)
	0: The first clock transition is the first data capture edge
	1: The second clock transition is the first data capture edge
	Note: This bit is not available for UART4 & UART5. */

#define USART_LBCL (1 << 8) /* Bit 8 - Last bit clock pulse */
/* This bit allows the user to select whether the clock pulse associated with the last data bit
	transmitted (MSB) has to be output on the SCLK pin in synchronous mode.
	0: The clock pulse of the last data bit is not output to the SCLK pin
	1: The clock pulse of the last data bit is output to the SCLK pin
	Note: 1: The last bit is the 8th or 9th data bit transmitted depending on the 8 or 9 bit format selected
	by the M bit in the USART_CR1 register.
	2: This bit is not available for UART4 & UART5.
	Bit 7 Reserved, forced by hardware to 0.  */

#define USART_LBDIE (1 << 6) /* Bit 6 - LIN break detection interrupt enable */
/* Break interrupt mask (break detection using break delimiter).
	0: Interrupt is inhibited
	1: An interrupt is generated whenever LBD=1 in the USART_SR register */

#define USART_LBDL (1 << 5) /* Bit 5 - lin break detection length */
/* This bit is for selection between 11 bit or 10 bit break detection.
	0: 10-bit break detection
	1: 11-bit break detection
	Bit 4 Reserved, forced by hardware to 0. */

#define USART_ADD_MSK (((1 << (3 + 1)) - 1) << 0) /* Bits [3..0] - Address of the USART node */
#define USART_ADD_SET(VAL) (((VAL) << 0) & ADD_MSK)
#define USART_ADD_GET(REG) (((REG) & ADD_MSK) >> 0)
/* This bit-field gives the address of the USART node.
	This is used in multiprocessor communication during mute mode, for wake up with address mark detection.
	Note: These 3 bits (CPOL, CPHA, LBCL) should not be written while the transmitter is enabled. */


/* ------------------------------------------------------------------------- */
/* USART Control register 3 */
#define STM32F_USART_CR3 0x14 

#define USART_ONEBIT (1 << 11) /* Bit 11 - One sample bit method enable */
/* This bit allows the user to select the sample method. When the one sample bit method is selected the noise detection flag (NF) is disabled.
	0: Three sample bit method
	1: One sample bit method */

#define USART_CTSIE (1 << 10) /* Bit 10 - CTS interrupt enable */
/* 0: Interrupt is inhibited
	1: An interrupt is generated whenever CTS=1 in the USART_SR register
	Note: This bit is not available for UART4 & UART5. */

#define USART_CTSE (1 << 9) /* Bit 9 - CTS enable */
/* 0: CTS hardware flow control disabled
	1: CTS mode enabled, data is only transmitted when the nCTS input is asserted (tied to 0).
	If the nCTS input is deasserted while a data is being transmitted, then the transmission is completed before stopping. If a data is written into the data register while nCTS is asserted, the transmission is postponed until nCTS is asserted.
	Note: This bit is not available for UART4 & UART5. */

#define USART_RTSE (1 << 8) /* Bit 8 - RTS enable */
/* 0: RTS hardware flow control disabled
	1: RTS interrupt enabled, data is only requested when there is space in the receive buffer. The transmission of data is expected to cease after the current character has been transmitted. The nRTS output is asserted (tied to 0) when a data can be received.
	Note: This bit is not available for UART4 & UART5. */

#define USART_DMAT (1 << 7) /* Bit 7 - DMA enable transmitter */
/* This bit is set/reset by software
	1: DMA mode is enabled for transmission.
	0: DMA mode is disabled for transmission.
	Note: This bit is not available for UART5. */

#define USART_DMAR (1 << 6) /* Bit 6 - DMA enable receiver */
/* This bit is set/reset by software
	1: DMA mode is enabled for reception
	0: DMA mode is disabled for reception
	Note: This bit is not available for UART5. */

#define USART_SCEN (1 << 5) /* Bit 5 - Smartcard mode enable */
/* This bit is used for enabling Smartcard mode.
	0: Smartcard Mode disabled
	1: Smartcard Mode enabled
	Note: This bit is not available for UART4 & UART5. */

#define USART_NACK (1 << 4) /* Bit 4 - Smartcard NACK enable */
/* 0: NACK transmission in case of parity error is disabled
	1: NACK transmission during parity error is enabled
	Note: This bit is not available for UART4 & UART5. */

#define USART_HDSEL (1 << 3) /* Bit 3 - Half-duplex selection */
/* Selection of Single-wire Half-duplex mode
	0: Half duplex mode is not selected
	1: Half duplex mode is selected  */

#define USART_IRLP (1 << 2) /* Bit 2 - IrDA low-power */
/* This bit is used for selecting between normal and low-power IrDA modes
	0: Normal mode
	1: Low-power mode */

#define USART_IREN (1 << 1) /* Bit 1 - IrDA mode enable */
/* This bit is set and cleared by software.
	0: IrDA disabled
	1: IrDA enabled */

#define USART_EIE (1 << 0) /* Bit 0 - Error interrupt enable */
/* Error Interrupt Enable Bit is required to enable interrupt generation in case of a framing error, overrun error or noise flag (FE=1 or ORE=1 or NF=1 in the USART_SR register) in case of Multi Buffer Communication (DMAR=1 in the USART_CR3 register).
	0: Interrupt is inhibited
	1: An interrupt is generated whenever DMAR=1 in the USART_CR3 register and FE=1 or ORE=1 or NF=1 in the USART_SR register. */


/* Guard time and prescaler register */
#define STM32F_USART_GTPR 0x18 


/* Bits [15..8] - Guard time value */
#define USART_GT_MSK (((1 << (7 + 1)) - 1) << 8) 
#define USART_GT_SET(VAL) (((VAL) << 8) & GT_MSK)
#define USART_GT_GET(REG) (((REG) & GT_MSK) >> 8)
/* This bit-field gives the Guard time value in terms of number of baud clocks.
   This is used in Smartcard mode. The Transmission Complete flag is set after 
   this guard time value.
   Note: This bit is not available for UART4 & UART5. */

/* Bits [7..0] - Prescaler value */
#define USART_PSC_MSK (((1 << (7 + 1)) - 1) << 0) 
#define USART_PSC_SET(VAL) (((VAL) << 0) & PSC_MSK)
#define USART_PSC_GET(REG) (((REG) & PSC_MSK) >> 0)
/* – In IrDA Low-power mode:
 	PSC[7:0] = IrDA Low-Power Baud Rate

	Used for programming the prescaler for dividing the system clock to achieve 
	the low-power frequency:

	The source clock is divided by the value given in the register (8 significant bits):
	00000000: Reserved - do not program this value
	00000001: divides the source clock by 1
	00000010: divides the source clock by 2
	...

  – In normal IrDA mode: PSC must be set to 00000001.
  – In smartcard mode:
	PSC[4:0]: Prescaler value
	Used for programming the prescaler for dividing the system clock 
	to provide the smartcard clock.

	The value given in the register (5 significant bits) is multiplied by 2 
	to give the division factor of the source clock frequency:
	00000: Reserved - do not program this value
	00001: divides the source clock by 2
	00010: divides the source clock by 4
	00011: divides the source clock by 6
	...
	Note: 
	1: Bits [7:5] have no effect if Smartcard mode is used.
	2: This bit is not available for UART4 & UART5. */


#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32_usart {
#if defined(STM32F3X)
	volatile uint32_t cr1;
	volatile uint32_t cr2;
	volatile uint32_t cr3;
	volatile uint32_t brr;

	volatile uint32_t gtpr;
	volatile uint32_t rtor;
	volatile uint32_t rqr;
	union {
		volatile uint32_t isr;
		volatile uint32_t sr; /* compatibility name */
	};

	volatile uint32_t icr;
	volatile uint32_t rdr;
	volatile uint32_t tdr;
#else
	union {
		volatile uint32_t isr; /* compatibility name */
		volatile uint32_t sr; 
	};
	union {
		volatile uint32_t dr;
		volatile uint32_t rdr;
		volatile uint32_t tdr;
	};
	volatile uint32_t brr;
	volatile uint32_t cr1;
	volatile uint32_t cr2;
	volatile uint32_t cr3;
	volatile uint32_t gtpr;
#endif
};

extern const uint8_t stm32_usart_irq_lut[];

extern const struct stm32_usart * const stm32_usart_lut[];

#ifdef __cplusplus
extern "C" {
#endif

int stm32_usart_lookup(struct stm32_usart * usart);

int stm32_usart_putc(struct stm32_usart * usart, int c);

int stm32_usart_getc(struct stm32_usart * usart, unsigned int msec);

int stm32_usart_read(struct stm32_usart * usart, char * buf,
					  unsigned int len, unsigned int msec);

int stm32_usart_write(struct stm32_usart * usart, const void * buf,
					   unsigned int len);

int stm32_usart_canon_write(struct stm32_usart * usart, const void * buf,
							 unsigned int len);

int stm32_usart_power_off(struct stm32_usart * us);

int stm32_usart_flush(struct stm32_usart * usart);

int stm32_usart_init(struct stm32_usart * us);

int stm32_usart_baudrate_set(struct stm32_usart * us, unsigned int baudrate);

unsigned int stm32_usart_baudrate_get(struct stm32_usart * us);

int stm32_usart_mode_set(struct stm32_usart * us, unsigned int flags);

void stm32_usart_enable(struct stm32_usart * us);

void stm32_usart_disable(struct stm32_usart * us);

static inline int stm32_usart_irq_lookup(struct stm32_usart * usart) {
	return stm32_usart_irq_lut[stm32_usart_lookup(usart)];
}

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_USART_H__ */
