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
 * @file stm32f-spi.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_SPI_H__
#define __STM32F_SPI_H__

/*-------------------------------------------------------------------------
  Serial peripheral interface (SPI)
  -------------------------------------------------------------------------*/

/* SPI control register 1 (not used in I2S mode)*/
#define SPI_CR1 0x00

/* Bit 15 - Bidirectional data mode enable */
#define SPI_BIDIMODE (1 << 15)
/*	0: 2-line unidirectional data mode selected
	1: 1-line bidirectional data mode selected
	Note: Not used in I2S mode */

/* Bit 14 - Output enable in bidirectional mode */
#define SPI_BIDIOE (1 << 14)
/* This bit combined with the BIDImode bit selects the direction of transfer 
   in bidirectional mode
	0: Output disabled (receive-only mode)
	1: Output enabled (transmit-only mode)
	Note: In master mode, the MOSI pin is used and in slave mode, the MISO 
	pin is used. 
	Not used in I2S mode */

/* Bit 13 - Hardware CRC calculation enable */
#define SPI_CRCEN (1 << 13)
/*	0: CRC calculation disabled
	1: CRC calculation enabled
	Note: This bit should be written only when SPI is disabled (SPE = ‘0’) 
	for correct operation
	Not used in I2S mode */

/* Bit 12 - CRC transfer next */
#define SPI_CRCNEXT (1 << 12)
/*	0: Data phase (no CRC phase)
	1: Next transfer is CRC (CRC phase)
	Note: When the SPI is configured in full duplex or transmitter only modes, 
	CRCNEXT must be written as soon as the last data is written to the SPI_DR 
	register.
	When the SPI is configured in receiver only mode, CRCNEXT must be set 
	after the second last data reception.
	This bit should be kept cleared when the transfers are managed by DMA.
	Not used in I2S mode */

/* Bit 11 - Data frame format */
#define SPI_DFF (1 << 11)
/*	0: 8-bit data frame format is selected for transmission/reception
	1: 16-bit data frame format is selected for transmission/reception
	Note: This bit should be written only when SPI is disabled (SPE = ‘0’) 
	for correct operation
	Not used in I2S mode */

/* Bit 10 - Receive only */
#define SPI_RXONLY (1 << 10)
/* This bit combined with the BIDImode bit selects the direction of transfer 
   in 2-line unidirectional mode. This bit is also useful in a multislave 
   system in which this particular slave is not accessed, the output from the 
   accessed slave is not corrupted.
	0: Full duplex (Transmit and receive)
	1: Output disabled (Receive-only mode)
	Note: Not used in I2S mode */

/* Bit 9 - Software slave management */
#define SPI_SSM (1 << 9)
/* When the SSM bit is set, the NSS pin input is replaced with the value 
   from the SSI bit.
	0: Software slave management disabled
	1: Software slave management enabled
	Note: Not used in I2S mode and SPI TI mode */

/* Bit 8 - Internal slave select */
#define SPI_SSI (1 << 8)
/* This bit has an effect only when the SSM bit is set. The value of this 
   bit is forced onto the NSS pin and the IO value of the NSS pin is ignored.
	Note: Not used in I2S mode and SPI TI mode */

/* Bit 7 - Frame format */
#define SPI_LSBFIRST (1 << 7)
/*	0: MSB transmitted first
	1: LSB transmitted first
	Note: This bit should not be changed when communication is ongoing.
	Not used in I2S mode and SPI TI mode */

/* Bit 6 - SPI enable */
#define SPI_SPE (1 << 6)
/*	0: Peripheral disabled
	1: Peripheral enabled
	Note: 1- Not used in I2S mode.
	Note: 2- When disabling the SPI, follow the procedure described in 
	Section 25.3.8: Disabling the SPI. */

/* Bits [5..3] - Baud rate control */
#define SPI_BR_MSK (((1 << (2 + 1)) - 1) << 3)
#define SPI_BR_SET(VAL) (((VAL) << 3) & SPI_BR_MSK)
#define SPI_BR_GET(REG) (((REG) & SPI_BR_MSK) >> 3)
/*	000: fPCLK/2
	001: fPCLK/4
	010: fPCLK/8
	011: fPCLK/16
	100: fPCLK/32
	101: fPCLK/64
	110: fPCLK/128
	111: fPCLK/256
	Note: These bits should not be changed when communication is ongoing.
	Not used in I2S mode */

/* Bit 2 - Master selection */
#define SPI_MSTR (1 << 2)
/*	0: Slave configuration
	1: Master configuration
	Note: This bit should not be changed when communication is ongoing.
	Not used in I2S mode */

/*	Bit1 CPOL: Clock polarity */
#define SPI_CPOL (1 << 1)
/*	0: CK to 0 when idle
	1: CK to 1 when idle
	Note: This bit should not be changed when communication is ongoing.
	Not used in I2S mode and SPI TI mode */

/* Bit 0 - Clock phase */
#define SPI_CPHA (1 << 0)
/*	0: The first clock transition is the first data capture edge
	1: The second clock transition is the first data capture edge
	Note: This bit should not be changed when communication is ongoing.
	Note: Not used in I2S mode and SPI TI mode */

/* SPI control register 2 */
#define SPI_CR2 0x04

/* [15..8] Reserved. Forced to 0 by hardware. */

/* Bit 7 - Tx buffer empty interrupt enable */
#define SPI_TXEIE (1 << 7)
/*	0: TXE interrupt masked
	1: TXE interrupt not masked. Used to generate an interrupt request 
	when the TXE flag is set. */

/* Bit 6 - RX buffer not empty interrupt enable */
#define SPI_RXNEIE (1 << 6)
/*	0: RXNE interrupt masked
	1: RXNE interrupt not masked. Used to generate an interrupt request 
	when the RXNE flag is set. */

/* Bit 5 - Error interrupt enable */
#define SPI_ERRIE (1 << 5)
/* This bit controls the generation of an interrupt when an error 
   condition occurs (CRCERR, OVR, MODF in SPI mode and UDR, OVR in I2S mode).
	0: Error interrupt is masked
	1: Error interrupt is enabled */

/* Bit 4 - Frame format */
#define SPI_FRF (1 << 4)
/* 0: SPI Motorola mode
1 SPI TI mode
Note: Not used in I2S mode */

/* Bit 3 Reserved. Forced to 0 by hardware. */

/* Bit 2 - SS output enable */
#define SPI_SSOE (1 << 2)
/* 0: SS output is disabled in master mode and the cell can work in multimaster configuration
1: SS output is enabled in master mode and when the cell is enabled. The cell cannot work
in a multimaster environment.
Note: Not used in I2S mode and SPI TI mode */

/* Bit 1 - Tx buffer DMA enable */
#define SPI_TXDMAEN (1 << 1)
/* When this bit is set, the DMA request is made whenever the TXE flag is set.
0: Tx buffer DMA disabled
1: Tx buffer DMA enabled */

/* Bit 0 - Rx buffer DMA enable */
#define SPI_RXDMAEN (1 << 0)
/* When this bit is set, the DMA request is made whenever the RXNE flag is set.
0: Rx buffer DMA disabled
1: Rx buffer DMA enabled */

/* SPI status register */
#define SPI_SR 0x08

/* [15..9] Reserved. Forced to 0 by hardware. */

/* Bit 8 - frame format error */
#define SPI_FRE (1 << 8)
/* 0: No frame format error
   1: A frame format error occurred 
   This flag is set by hardware and cleared by software when the SPIx_SR 
   register is read. Note: This flag is used when the SPI operates in TI 
   slave mode or I2S slave mode (refer to Section 27.3.10). */

/* Bit 7 - Busy flag */
#define SPI_BSY (1 << 7)
/* 0: SPI (or I2S)not busy
   1: SPI (or I2S)is busy in communication or Tx buffer is not empty
   This flag is set and cleared by hardware.
   Note: BSY flag must be used with caution: refer to Section 25.3.7: 
   Status flags and Section 25.3.8: Disabling the SPI. */

/* Bit 6 - Overrun flag */
#define SPI_OVR (1 << 6)
/* 0: No overrun occurred
   1: Overrun occurred
   This flag is set by hardware and reset by a software sequence. Refer 
   to Section 25.4.7 on page 691 for the software sequence. */

/* Bit 5 - Mode fault */
#define SPI_MODF (1 << 5)
/* 0: No mode fault occurred
   1: Mode fault occurred This flag is set by hardware and reset by a 
   software sequence. Refer to Section 25.3.10 on page 674 for the 
   software sequence.
Note: Not used in I2S mode */

/* Bit 4 - CRC error flag */
#define SPI_CRCERR (1 << 4)
/* 0: CRC value received matches the SPI_RXCRCR value
1: CRC value received does not match the SPI_RXCRCR value
This flag is set by hardware and cleared by software writing 0.
Note: Not used in I2S mode */

/* Bit 3 - Underrun flag */
#define SPI_UDR (1 << 3)
/* 0: No underrun occurred
1: Underrun occurred
This flag is set by hardware and reset by a software sequence. Refer to Section 25.4.7 on page 691 for the software sequence.
Note: Not used in SPI mode */

/* Bit 2 - Channel side */
#define SPI_CHSIDE (1 << 2)
/* 0: Channel Left has to be transmitted or has been received
1: Channel Right has to be transmitted or has been received
Note: Not used for the SPI mode. No meaning in PCM mode */

/* Bit 1 - Transmit buffer empty */
#define SPI_TXE (1 << 1)
/* 0: Tx buffer not empty
1: Tx buffer empty */

/* Bit 0 - Receive buffer not empty */
#define SPI_RXNE (1 << 0)
/* 0: Rx buffer empty
1: Rx buffer not empty */

/* SPI data register */
#define SPI_DR 0x0C


/* Bits [15..0] - Data register */
#define SPI_DR_MSK (((1 << (15 + 1)) - 1) << 0)
#define SPI_DR_SET(VAL) (((VAL) << 0) & DR_MSK)
#define SPI_DR_GET(REG) (((REG) & DR_MSK) >> 0)
/* Data received or to be transmitted.
   The data register is split into 2 buffers - one for writing (Transmit 
   Buffer) and another one for reading (Receive buffer). A write to the data 
   register will write into the Tx buffer and a read from the data register 
   will return the value held in the Rx buffer.
   Notes for the SPI mode:
   Depending on the data frame format selection bit (DFF in SPI_CR1 register), 
   the data sent or received is either 8-bit or 16-bit. This selection has to 
   be made before enabling the SPI to ensure correct operation.
   For an 8-bit data frame, the buffers are 8-bit and only the LSB of the 
   register (SPI_DR[7:0]) is used for transmission/reception. When in 
   reception mode, the MSB of the register (SPI_DR[15:8]) is forced to 0.
   For a 16-bit data frame, the buffers are 16-bit and the entire register, 
   SPI_DR[15:0] is used for transmission/reception.
   SPI CRC polynomial register (SPI_CRCPR) (not used in I2S mode)
   Address offset: 0x10 */


/* Bits [15..0] - CRC polynomial register */
#define SPI_CRCPOLY_MSK (((1 << (15 + 1)) - 1) << 0)
#define SPI_CRCPOLY_SET(VAL) (((VAL) << 0) & CRCPOLY_MSK)
#define SPI_CRCPOLY_GET(REG) (((REG) & CRCPOLY_MSK) >> 0)
/* This register contains the polynomial for the CRC calculation.
   The CRC polynomial (0007h) is the reset value of this register. Another 
   polynomial can be configured as required.
Note: Not used for the I2S mode. */

/* SPI RX CRC register */
#define SPI_RXCRCR 0x14


/* Bits [15..0] - Rx CRC register */
#define SPI_RXCRC_MSK (((1 << (15 + 1)) - 1) << 0)
#define SPI_RXCRC_SET(VAL) (((VAL) << 0) & RXCRC_MSK)
#define SPI_RXCRC_GET(REG) (((REG) & RXCRC_MSK) >> 0)
/* When CRC calculation is enabled, the RxCRC[15:0] bits contain the 
   computed CRC value of the subsequently received bytes. This register is 
   reset when the CRCEN bit in SPI_CR1 register is written to 1. The CRC is 
   calculated serially using the polynomial programmed in the SPI_CRCPR 
   register.
   Only the 8 LSB bits are considered when the data frame format is set to 
   be 8-bit data (DFF bit of SPI_CR1 is cleared). CRC calculation is done 
   based on any CRC8 standard. The entire 16-bits of this register are 
   considered when a 16-bit data frame format is selected (DFF bit of the 
   SPI_CR1 register is set). CRC calculation is done based on any CRC16 
   standard.
	Note: A read to this register when the BSY Flag is set could return 
	an incorrect value.
	Not used for the I2S mode. */

/* SPI TX CRC register */
#define SPI_TXCRCR 0x18



/* SPI_I2S configuration register */
#define SPI_I2SCFGR 0x1C



/* [15..12] Reserved: Forced to 0 by hardware */


/* Bit 11 - I2S mode selection */
#define SPI_I2SMOD (1 << 11)
/*	0: SPI mode is selected
	1: I2S mode is selected
	Note: This bit should be configured when the SPI or I2S is disabled */

/* Bit 10 - I2S Enable */
#define SPI_I2SE (1 << 10)
/*	0: I2S peripheral is disabled
	1: I2S peripheral is enabled
	Note: Not used in SPI mode */

/* Bits [9..8] - I2S configuration mode */
#define SPI_I2SCFG ((9 - 8) << 8)
#define SPI_I2SCFG_SLV_XMT (0 << 8)
#define SPI_I2SCFG_SLV_RCV (1 << 8)
#define SPI_I2SCFG_MST_XMT (2 << 8)
#define SPI_I2SCFG_MST_RCV (3 << 8)
/*	00: Slave - transmit
	01: Slave - receive
	10: Master - transmit
	11: Master - receive
	Note: This bit should be configured when the I2S is disabled.
	Not used for the SPI mode */

/* Bit 7 - PCM frame synchronization */
#define SPI_PCMSYNC (1 << 7)
#define SPI_PCMSYNC_SHORT (0 << 7)
#define SPI_PCMSYNC_LONG (1 << 7)
/*	0: Short frame synchronization
	1: Long frame synchronization
	Note: This bit has a meaning only if I2SSTD = 11 (PCM standard is used)
	Not used for the SPI mode */

/* Bit 6 - forced at 0 by hardware */

/* Bits [5..4] - I2S standard selection */
#define SPI_I2SSTD ((5 - 4) << 4)
#define SPI_I2SSTD_PHILIPS (0 << 4)
#define SPI_I2SSTD_MSB_JST (1 << 4)
#define SPI_I2SSTD_LSB_JST (2 << 4)
#define SPI_I2SSTD_PCM     (3 << 4)
/*	00: I2S Phillips standard.
	01: MSB justified standard (left justified)
	10: LSB justified standard (right justified)
	11: PCM standard
	For more details on I2S standards, refer to Section 25.4.2 on page 677. 
	Not used in SPI mode.
	Note: For correct operation, these bits should be configured when the 
	I2S is disabled. */

/* Bit 3 - Steady state clock polarity */
#define SPI_CKPOL (1 << 3)
#define SPI_CKPOL_LO (0 << 3)
#define SPI_CKPOL_HI (1 << 3)
/*	0: I2S clock steady state is low level
	1: I2S clock steady state is high level
	Note: For correct operation, this bit should be configured when the I2S 
	is disabled.
	Not used in SPI mode */

/* Bits [2..1] - Data length to be transferred */
#define SPI_DATLEN ((2 - 1) << 1)
#define SPI_DATLEN_16 (0 << 1)
#define SPI_DATLEN_24 (1 << 1)
#define SPI_DATLEN_32 (2 << 1)
/*	00: 16-bit data length
	01: 24-bit data length
	10: 32-bit data length
	11: Not allowed
	Note: For correct operation, these bits should be configured when the I2S 
	is disabled.
	Not used in SPI mode. */

/* Bit 0 - Channel length (number of bits per audio channel) */
#define SPI_CHLEN (1 << 0)
#define SPI_CHLEN_16 (0 << 0)
#define SPI_CHLEN_32 (1 << 0)
/*	0: 16-bit wide
	1: 32-bit wide
	The bit write operation has a meaning only if DATLEN = 00 otherwise 
	the channel length is fixed to
	32-bit by hardware whatever the value filled in. Not used in SPI mode.
	Note: For correct operation, this bit should be configured when the 
	I2S is disabled. */

/* SPI_I2S prescaler register */
#define SPI_I2SPR 0x20

/* [15..10] Reserved: Forced to 0 by hardware */


/* Bit 9 - Master clock output enable */
#define SPI_MCKOE (1 << 9)
/*	0: Master clock output is disabled
	1: Master clock output is enabled
	Note: This bit should be configured when the I2S is disabled. It is 
	used only when the I2S is in master mode.
	Not used in SPI mode. */

/* Bit 8 - Odd factor for the prescaler */
#define SPI_ODD (1 << 8)
/*	0: real divider value is = I2SDIV *2
	1: real divider value is = (I2SDIV * 2)+1
	Refer to Section 25.4.3 on page 684. Not used in SPI mode.
	Note: This bit should be configured when the I2S is disabled. It is 
	used only when the I2S is in master mode. */

/* Bits [7..0] - I2S Linear prescaler */
#define SPI_I2SDIV ((7 - 0) << 0)
/* I2SDIV [7:0] = 0 or I2SDIV [7:0] = 1 are forbidden values.
   Refer to Section 25.4.3 on page 684. Not used in SPI mode.
	Note: These bits should be configured when the I2S is disabled. It is 
	used only when the I2S is in master mode. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_spi {
	volatile uint32_t cr1;
	volatile uint32_t cr2;
	volatile uint32_t sr;
	volatile uint32_t dr;

	volatile uint32_t crcpr;
	volatile uint32_t rxcrcr;
	volatile uint32_t txcrcr;
	volatile uint32_t i2scfgr;

	volatile uint32_t i2spr;
};

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_SPI_H__ */

