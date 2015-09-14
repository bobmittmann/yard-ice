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
 * @file stm32f-i2c.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */
#ifndef __STM32F_I2C_H__
#define __STM32F_I2C_H__

/*-------------------------------------------------------------------------
  I2C
  -------------------------------------------------------------------------*/

#if defined(STM32F3X)

/*-------------------------------------------------------------------------
 * I2C Control register 1 */
#define STM32F_I2C_CR1 0x00

/* Bits 31:24 Reserved, must be kept at reset value. */

/* Bit 23 PECEN: PEC enable */
#define I2C_PECEN (1 << 23)
/* 0: PEC calculation disabled
   1: PEC calculation enabled
   Note: If the SMBus feature is not supported, this bit is reserved 
   and forced by hardware to ‘0’. */

/* Bit 22 ALERTEN: SMBus alert enable */
#define I2C_ALERTEN (1 << 22)
/* Device mode (SMBHEN=0):
     0: Releases SMBA pin high and Alert Response Address Header 
        disabled: 0001100x followed by NACK.
     1: Drives SMBA pin low and Alert Response Address Header 
        enables: 0001100x followed by ACK.
   Host mode (SMBHEN=1):
     0: SMBus Alert pin (SMBA) not supported.
     1: SMBus Alert pin (SMBA) supported.
  Note: When ALERTEN=0, the SMBA pin can be used as a standard GPIO.
  If the SMBus feature is not supported, this bit is reserved and forced by 
  hardware to ‘0’. */

/* Bit 21 SMBDEN: SMBus Device Default address enable */
#define I2C_SMBDEN (1 << 21)
/* 0: Device default address disabled. Address 0b1100001x is NACKed.
   1: Device default address enabled. Address 0b1100001x is ACKed.
   Note: If the SMBus feature is not supported, this bit is reserved 
   and forced by hardware to ‘0’. */

/* Bit 20 SMBHEN: SMBus Host address enable */
#define I2C_SMBHEN (1 << 20)
/* 0: Host address disabled. Address 0b0001000x is NACKed.
   1: Host address enabled. Address 0b0001000x is ACKed.
   Note: If the SMBus feature is not supported, this bit is reserved 
   and forced by hardware to ‘0’. */

/* Bit 19 GCEN: General call enable */
#define I2C_GCEN (1 << 19)
/* 0: General call disabled. Address 0b00000000 is NACKed.
   1: General call enabled. Address 0b00000000 is ACKed. */

/* Bit 18 WUPEN: Wakeup from STOP enable */
#define I2C_WUPEN (1 << 18)
/* 0: Wakeup from STOP disable.
   1: Wakeup from STOP enable.
   Note: If the Wakeup from STOP feature is not supported, this bit is 
   reserved and forced by hardware to ‘0’.
   Note: WUPEN can be set only when DNF = ‘0000’ */

/* Bit 17 NOSTRETCH: Clock stretching disable */
#define I2C_NOSTRETCH (1 << 17)
/* This bit is used to disable clock stretching in slave mode.
   0: Clock stretching enabled
   1: Clock stretching disabled
   Note: This bit can only be programmed when the I2C is disabled (PE = 0). */

/* Bit 16 SBC: Slave byte control */
#define I2C_SBC (1 << 16)
/* This bit is used to enable hardware byte control in slave mode.
   0: Slave byte control disabled
   1: Slave byte control enabled */

/* Bit 15 RXDMAEN: DMA reception requests enable */
#define I2C_RXDMAEN (1 << 15)
/* 0: DMA mode disabled for reception
   1: DMA mode enabled for reception */

/* Bit 14 TXDMAEN: DMA transmission requests enable */
#define I2C_TXDMAEN (1 << 14)
/* 0: DMA mode disabled for transmission
   1: DMA mode enabled for transmission */

/* Bit 13 Reserved, must be kept at reset value. */

/* Bit 12 ANFOFF: Analog noise filter OFF */
#define I2C_ANFOFF (1 << 12)
/* 0: Analog noise filter enabled
   1: Analog noise filter disabled
   Note: This bit can only be programmed when the I2C is disabled (PE = 0). */

/* Bits 11:8 DNF[3:0]: Digital noise filter */
#define I2C_DNF_MSK (0xf << 8)
#define I2C_DNF_SET(VAL) (((VAL) << 8) & I2C_DNF_MSK) 
/* These bits are used to configure the digital noise filter on SDA and 
   SCL input. The digital filter will filter spikes with a length of up 
   to DNF[3:0] * tI2CCLK
   0000: Digital filter disabled
   0001: Digital filter enabled and filtering capability up to 1 tI2CCLK
   ...
   1111: digital filter enabled and filtering capability up to15 tI2CCLK
   Note: If the analog filter is also enabled, the digital filter is added 
   to the analog filter.
   This filter can only be programmed when the I2C is disabled (PE = 0).*/

/* Bit 7 ERRIE: Error interrupts enable */
#define I2C_ERRIE (1 << 7)
/* 0 : Error detection interrupts disabled
   1: Error detection interrupts enabled
   Note: Any of these errors generate an interrupt:
     Arbitration Loss (ARLO)
     Bus Error detection (BERR)
     Overrun/Underrun (OVR)
     Timeout detection (TIMEOUT)
     PEC error detection (PECERR)
     Alert pin event detection (ALERT) */

/* Bit 6 TCIE: Transfer Complete interrupt enable */
#define I2C_TCIE (1 << 6)
/* 0: Transfer Complete interrupt disabled
   1: Transfer Complete interrupt enabled
   Note: Any of these events will generate an interrupt:
	Transfer Complete (TC)
	Transfer Complete Reload (TCR) */

/* Bit 5 STOPIE: STOP detection Interrupt enable */
#define I2C_STOPIE (1 << 5)
/* 0: Stop detection (STOPF) interrupt disabled
   1: Stop detection (STOPF) interrupt enabled */

/* Bit 4 NACKIE: Not acknowledge received Interrupt enable */
#define I2C_NACKIE (1 << 4)
/* 0: Not acknowledge (NACKF) received interrupts disabled
   1: Not acknowledge (NACKF) received interrupts enabled */

/* Bit 3 ADDRIE: Address match Interrupt enable (slave only) */
#define I2C_ADDRIE (1 << 3)
/* 0: Address match (ADDR) interrupts disabled
   1: Address match (ADDR) interrupts enabled */

/* Bit 2 RXIE: RX Interrupt enable */
#define I2C_RXIE (1 << 2)
/* 0: Receive (RXNE) interrupt disabled
   1: Receive (RXNE) interrupt enabled */

/* Bit 1 TXIE: TX Interrupt enable */
#define I2C_TXIE (1 << 1)
/* 0: Transmit (TXIS) interrupt disabled
   1: Transmit (TXIS) interrupt enabled */

/* Bit 0 PE: Peripheral enable */
#define I2C_PE (1 << 0)
/* 0: Peripheral disable
   1: Peripheral enable
   Note: When PE=0, the I2C SCL and SDA lines are released. Internal 
   state machines and status bits are put back to their reset value. When 
   cleared, PE must be kept low for at least 3 APB clock cycles. */

/*-------------------------------------------------------------------------
 * I2C Control register 2 */
#define STM32F_I2C_CR2 0x04

/* Bitts 31:27 Reserved, must be kept at reset value. */

/* Bit 26 PECBYTE: Packet error checking byte */
#define I2C_PECBYTE (1 << 26)
/* This bit is set by software, and cleared by hardware when the PEC 
   is transferred, or when a STOP condition or an Address Matched is 
   received, also when PE=0.
   0: No PEC transfer.
   1: PEC transmission/reception is requested
   Note: Writing ‘0’ to this bit has no effect.
   This bit has no effect when RELOAD is set.
   This bit has no effect is slave mode when SBC=0.
   If the SMBus feature is not supported, this bit is reserved and forced 
   by hardware to ‘0’. */

/* Bit 25 AUTOEND: Automatic end mode (master mode) */
#define I2C_AUTOEND (1 << 25)
/* This bit is set and cleared by software.
   0: software end mode: TC flag is set when NBYTES data are transferred, 
   stretching SCL low.
   1: Automatic end mode: a STOP condition is automatically sent when 
   NBYTES data are transferred.
   Note: This bit has no effect in slave mode or when the RELOAD bit is set. */

/* Bit 24 RELOAD: NBYTES reload mode */
#define I2C_RELOAD (1 << 24)
/* This bit is set and cleared by software.
   0: The transfer is completed after the NBYTES data transfer (STOP 
   or RESTART will follow).
   1: The transfer is not completed after the NBYTES data transfer 
   (NBYTES will be reloaded).
   TCR flag is set when NBYTES data are transferred, stretching SCL low. */

/* Bits 23:16 NBYTES[7:0]: Number of bytes */
#define I2C_NBYTES_MSK (0xff << 16)
#define I2C_NBYTES_SET(VAL) (((VAL) << 16) & I2C_NBYTES_MSK)
/* The number of bytes to be transmitted/received is programmed there. This 
   field is don’t care in slave mode with SBC=0.
   Note: Changing these bits when the START bit is set is not allowed. */

/* Bit 15 NACK: NACK generation (slave mode) */
#define I2C_NACK (1 << 15)
/* The bit is set by software, cleared by hardware when the NACK is sent, 
   or when a STOP condition or an Address Matched is received, or when PE=0.
   0: an ACK is sent after current received byte.
   1: a NACK is sent after current received byte.
   Note: Writing ‘0’ to this bit has no effect.
   This bit is used in slave mode only: in master receiver mode, NACK is 
   automatically generated after last byte preceding STOP or RESTART 
   condition, whatever the NACK bit value.
   When an overrun occurs in slave receiver NOSTRETCH mode, a NACK is 
   automatically generated whatever the NACK bit value.
   When hardware PEC checking is enabled (PECBYTE=1), the PEC acknowledge 
   value does not depend on the NACK value. */

/* Bit 14 STOP: Stop generation (master mode) */
#define I2C_STOP (1 << 14)
/* The bit is set by software, cleared by hardware when a Stop condition is 
   detected, or when PE = 0.
   In Master Mode:
   0: No Stop generation.
   1: Stop generation after current byte transfer.
   Note: Writing ‘0’ to this bit has no effect. */

/* Bit 13 START: Start generation */
#define I2C_START (1 << 13)
/* This bit is set by software, and cleared by hardware after the Start 
   followed by the address sequence is sent, by an arbitration loss, by a 
   timeout error detection, or when PE = 0. It can also be cleared #define by 
   software by writing ‘1’ to the ADDRCF bit in the I2Cx_ICR register.
   0: No Start generation.
   1: Restart/Start generation:
   – If the I2C is already in master mode with AUTOEND = 0, setting 
   this bit generates a
   Repeated Start condition when RELOAD=0, after the end of the NBYTES transfer.
   – Otherwise setting this bit will generate a START condition once 
   the bus is free.
   Note: Writing ‘0’ to this bit has no effect.
   The START bit can be set even if the bus is BUSY or I2C is in slave mode.
   This bit has no effect when RELOAD is set. */

/* Bit 12 HEAD10R: 10-bit address header only read direction 
   (master receiver mode) */
#define I2C_HEAD10R (1 << 12)
/* 0: The master sends the complete 10 bit slave address read sequence: 
   Start + 2 bytes 10bit address in write direction + Restart + 1st 7 bits 
   of the 10 bit address in read direction.
   1: The master only sends the 1st 7 bits of the 10 bit address, followed by Read direction.
   Note: Changing this bit when the START bit is set is not allowed. */

/* Bit 11 ADD10: 10-bit addressing mode (master mode) */
#define I2C_ADD10 (1 << 11)
/* 0: The master operates in 7-bit addressing mode,
   1: The master operates in 10-bit addressing mode
   Note: Changing this bit when the START bit is set is not allowed. */

/* Bit 10 RD_WRN: Transfer direction (master mode) */
#define I2C_RD_WRN (1 << 10)
/* 0: Master requests a write transfer.
   1: Master requests a read transfer.
   Note: Changing this bit when the START bit is set is not allowed. */

/* Bits 9:8 SADD[9:8]: Slave address bit 9:8 (master mode) */
#define I2C_SADD9 (1 << 9)
#define I2C_SADD8 (1 << 8)
/* In 7-bit addressing mode (ADD10 = 0):
   These bits are don’t care
   In 10-bit addressing mode (ADD10 = 1):
   These bits should be written with bits 9:8 of the slave address to be sent
   Note: Changing these bits when the START bit is set is not allowed. */

/* Bits 7:1 SADD[7:1]: Slave address bit 7:1 (master mode) */
#define I2C_SADD_MSK (0x7f << 1)
#define I2C_SADD_SET(VAL) (((VAL) << 1) & I2C_SADD_MSK) 
/* In 7-bit addressing mode (ADD10 = 0):
   These bits should be written with the 7-bit slave address to be sent
   In 10-bit addressing mode (ADD10 = 1):
   These bits should be written with bits 7:1 of the slave address to be sent.
   Note: Changing these bits when the START bit is set is not allowed. */

/* Bit 0 SADD0: Slave address bit 0 (master mode) */
#define I2C_SADD0 (1 << 0)
/* In 7-bit addressing mode (ADD10 = 0): This bit is don’t care
   In 10-bit addressing mode (ADD10 = 1): This bit should be written with 
   bit 0 of the slave address to be sent
   Note: Changing these bits when the START bit is set is not allowed */

/*-------------------------------------------------------------------------
 * I2C Own address register 1 */
#define STM32F_I2C_OAR1 0x08

/* Bit 15 ADDMODE Addressing mode (slave mode) 0: 7-bit slave address (10-bit
   address not acknowledged) 1: 10-bit slave address (7-bit address not
   acknowledged) Bit 14 Should always be kept at 1 by software. */

/* [13..10] Reserved, must be kept at reset value */

/* Bits [9..8] - Interface address */

/* 7-bit addressing mode: donâ€™t care 10-bit addressing mode: bits9:8 of
   address */

/* Bits [7..1] - Interface address */
#define I2C_OA1_MSK (((1 << (10)) - 1) << 1)
#define I2C_OA1_SET(VAL) (((VAL) << 1) & I2C_OA1_MSK)
#define I2C_OA1_GET(REG) (((REG) & I2C_OA1_MSK) >> 1)
/* bits 7:1 of address */

/* Bit 0 - Interface address */
#define I2C_ADD0 (1 << 0)
/* 7-bit addressing mode: donâ€™t care 10-bit addressing mode: bit 0 of 
   address */

/*-------------------------------------------------------------------------
 * I2C Own address register 2 */
#define STM32F_I2C_OAR2 0x0C

/*-------------------------------------------------------------------------
 * I2C Timing register */
#define STM32F_I2C_TIMINGR 0x10
/* Note: This register must be configured when the I2C is disabled (PE = 0). */

/* Bits 31:28 PRESC[3:0]: Timing prescaler */
#define I2C_PRESC_MSK (0xf << 28)
#define I2C_PRESC_SET(VAL) (((VAL) << 28) & I2C_PRESC_MSK)
/* This field is used to prescale I2CCLK in order to generate the clock 
   period tPRESC used for data setup and hold counters (refer to I2C 
   timings on page 659) and for SCL high and low level counters (refer to 
   I2C master initialization on page 672). 
   tPRESC = (PRESC+1) x tI2CCLK */

/* Bits 27:24 Reserved, must be kept at reset value. */

/* Bits 23:20 SCLDEL[3:0]: Data setup time */
#define I2C_SCLDEL_MSK (0xf << 20)
#define I2C_SCLDEL_SET(VAL) (((VAL) << 20) & I2C_SCLDEL_MSK)
/* This field is used to generate a delay tSCLDEL between SDA edge and SCL 
   rising edge in transmission mode. 
   tSCLDEL = (SCLDEL+1) x tPRESC
   Note: tSCLDEL is used to generate tSU:DAT timing. */

/* Bits 19:16 SDADEL[3:0]: Data hold time */
#define I2C_SDADEL_MSK (0xf << 16)
#define I2C_SDADEL_SET(VAL) (((VAL) << 16) & I2C_SDADEL_MSK)
/* This field is used to generate the delay tSDADEL between SCL falling 
   edge SDA edge in transmission mode. 
   tSDADEL= SDADEL x tPRESC
   Note: SDADEL is used to generate tHD:DAT timing. */

/* Bits 15:8 SCLH[7:0]: SCL high period (master mode) */
#define I2C_SCLHL_MSK (0xff << 8)
#define I2C_SCLHL_SET(VAL) (((VAL) << 8) & I2C_SCLHL_MSK)
/* This field is used to generate the SCL high period in master mode. 
   tSCLH = (SCLH+1) x tPRESC 
   Note: SCLH is also used to generate tSU:STO and tHD:STA timing. */

/* Bits 7:0 SCLL[7:0]: SCL low period (master mode) */
#define I2C_SCLL_MSK (0xff << 0)
#define I2C_SCLL_SET(VAL) (((VAL) << 0) & I2C_SCLL_MSK)
/* This field is used to generate the SCL low period in master mode.
   tSCLL = (SCLL+1) x tPRESC
   Note: SCLL is also used to generate tBUF and tSU:STA timings. */

/*-------------------------------------------------------------------------
 * I2C Timeout register */
#define STM32F_I2C_TIMEOUTR 0x14

/* Bit 31 TEXTEN: Extended clock timeout enable */
#define I2C_RD_TEXTEN (1 << 31)
/* 0: Extended clock timeout detection is disabled
   1: Extended clock timeout detection is enabled. When a cumulative 
   SCL stretch for more than tLOW:EXT is done by the I2C interface, a 
   timeout error is detected (TIMEOUT=1). */

/* Bits 30:29 Reserved, must be kept at reset value. */

/* Bits 27:16 TIMEOUTB[11:0]: Bus timeout B */
#define I2C_TIMEOUTB_MSK (0xfff << 16)
#define I2C_TIMEOUTB_SET(VAL) (((VAL) << 16) & I2C_TIMEOUTB_MSK)
/* This field is used to configure the cumulative clock extension timeout:
   In master mode, the master cumulative clock low extend time (tLOW:MEXT) 
   is detected 
   In slave mode, the slave cumulative clock low extend time (tLOW:SEXT) 
   is detected 
   tLOW:EXT= (TIMEOUTB+1) x 2048 x tI2CCLK
   Note: These bits can be written only when TEXTEN=0. */

/* Bit 15 TIMOUTEN: Clock timeout enable */
#define I2C_RD_TIMOUTEN (1 << 15)
/* 0: SCL timeout detection is disabled
   1: SCL timeout detection is enabled: when SCL is low for more than 
   tTIMEOUT (TIDLE=0) or high for more than tIDLE (TIDLE=1), a timeout 
   error is detected (TIMEOUT=1). */

/* Bits 14:13 Reserved, must be kept at reset value. */

/* Bit 12 TIDLE: Idle clock timeout detection */
#define I2C_RD_TIDLE (1 << 12)
/* 0: TIMEOUTA is used to detect SCL low timeout
   1: TIMEOUTA is used to detect both SCL and SDA high timeout (bus 
   idle condition)
   Note: This bit can be written only when TIMOUTEN=0. */

/* Bits 11:0 TIMEOUTA[11:0]: Bus Timeout A */
#define I2C_TIMEOUTA_MSK (0xfff << 0)
#define I2C_TIMEOUTA_SET(VAL) (((VAL) << 0) & I2C_TIMEOUTA_MSK)
/* This field is used to configure:
   – The SCL low timeout condition tTIMEOUT when TIDLE=0
   tTIMEOUT= (TIMEOUTA+1) x 2048 x tI2CCLK
   – The bus idle condition (both SCL and SDA high) when TIDLE=1
   tIDLE= (TIMEOUTA+1) x 4 x tI2CCLK
   Note: These bits can be written only when TIMOUTEN=0. */

/*-------------------------------------------------------------------------
 * I2C Interrupt and Status register */
#define STM32F_I2C_ISR 0x18

/* Bits 31:24 Reserved, must be kept at reset value.*/

/* Bits 23:17 ADDCODE[6:0]: Address match code (Slave mode) */
#define I2C_ADDCODE (0x7f << 17)
/* These bits are updated with the received address when an address 
   match event occurs (ADDR = 1).
   In the case of a 10-bit address, ADDCODE provides the 10-bit header 
   followed by the 2 MSBs of the address. */

/* Bit 16 DIR: Transfer direction (Slave mode) */
#define I2C_DIR (1 << 16)
/* This flag is updated when an address match event occurs (ADDR=1).
   0: Write transfer, slave enters receiver mode.
   1: Read transfer, slave enters transmitter mode. */

/* Bit 15 BUSY: Bus busy */
#define I2C_BUSY (1 << 15)
/* This flag indicates that a communication is in progress on the bus. It 
   is set by hardware when a START condition is detected. It is cleared by 
   hardware when a Stop condition is detected, or when PE=0. */

/* Bit 14 Reserved, must be kept at reset value. */

/* Bit 13 ALERT: SMBus alert */
#define I2C_ALERT (1 << 13)
/* This flag is set by hardware when SMBHEN=1 (SMBus host configuration), 
   ALERTEN=1 and a SMBALERT event (falling edge) is detected on SMBA pin. 
   It is cleared by software by setting the ALERTCF bit.
   Note: This bit is cleared by hardware when PE=0.
   If the SMBus feature is not supported, this bit is reserved and forced 
   by hardware to ‘0’. */

/* Bit 12 TIMEOUT: Timeout or tLOW detection flag */
#define I2C_TIMEOUT (1 << 12)
/* This flag is set by hardware when a timeout or extended clock timeout 
   occurred. It is cleared by software by setting the TIMEOUTCF bit.
   Note: This bit is cleared by hardware when PE=0.
   If the SMBus feature is not supported, this bit is reserved and forced 
   by hardware to ‘0’. */

/* Bit 11 PECERR: PEC Error in reception */
#define I2C_PECERR (1 << 11)
/* This flag is set by hardware when the received PEC does not match with 
   the PEC register content. A NACK is automatically sent after the wrong 
   PEC reception. It is cleared by software by setting the PECCF bit.
   Note: This bit is cleared by hardware when PE=0.
   If the SMBus feature is not supported, this bit is reserved and forced 
   by hardware to ‘0’. */

/* Bit 10 OVR: Overrun/Underrun (slave mode) */
#define I2C_OVR (1 << 10)
/* This flag is set by hardware in slave mode with NOSTRETCH=1, when an 
   overrun/underrun error occurs. It is cleared by software by setting the 
   OVRCF bit.
   Note: This bit is cleared by hardware when PE=0. */

/* Bit 9 ARLO: Arbitration lost */
#define I2C_ARLO (1 << 9)
/* This flag is set by hardware in case of arbitration loss. It is cleared 
   by software by setting the ARLOCF bit.
   Note: This bit is cleared by hardware when PE=0. */

/* Bit 8 BERR: Bus error */
#define I2C_BERR (1 << 8)
/* This flag is set by hardware when a misplaced Start or Stop condition is 
   detected whereas the peripheral is involved in the transfer. The flag 
   is not set during the address phase in slave mode. It is cleared by 
   software by setting BERRCF bit.
   Note: This bit is cleared by hardware when PE=0. */

/* Bits 7 TCR: Transfer Complete Reload */
#define I2C_TCR (1 << 7)
/* This flag is set by hardware when RELOAD=1 and NBYTES data have been 
   transferred. It is cleared by software when NBYTES is written to a 
   non-zero value.
   Note: This bit is cleared by hardware when PE=0.
   This flag is only for master mode, or for slave mode when the SBC 
   bit is set. */

/* Bit 6 TC: Transfer Complete (master mode) */
#define I2C_TC (1 << 6)
/* This flag is set by hardware when RELOAD=0, AUTOEND=0 and NBYTES data 
   have been transferred. It is cleared by software when START bit or STOP 
   bit is set.
   Note: This bit is cleared by hardware when PE=0. */

/* Bit 5 STOPF: Stop detection flag */
#define I2C_STOPF (1 << 5)
/* This flag is set by hardware when a Stop condition is detected on the 
   bus and the peripheral is involved in this transfer:
   – either as a master, provided that the STOP condition is generated by 
   the peripheral.
   – or as a slave, provided that the peripheral has been addressed 
   previously during this transfer.
   It is cleared by software by setting the STOPCF bit.
   Note: This bit is cleared by hardware when PE=0. */

/* Bit 4 NACKF: Not Acknowledge received flag */
#define I2C_NACKF (1 << 4)
/* This flag is set by hardware when a NACK is received after a byte 
   transmission. It is cleared by software by setting the NACKCF bit.
   Note: This bit is cleared by hardware when PE=0. */

/* Bit 3 ADDR: Address matched (slave mode) */
#define I2C_ADDR (1 << 3)
/* This bit is set by hardware as soon as the received slave address matched 
   with one of the enabled slave addresses. It is cleared by software by 
   setting ADDRCF bit.
   Note: This bit is cleared by hardware when PE=0. */

/* Bit 2 RXNE: Receive data register not empty (receivers) */
#define I2C_RXNE (1 << 2)
/* This bit is set by hardware when the received data is copied into the 
   I2Cx_RXDR register, and is ready to be read. It is cleared when 
   I2Cx_RXDR is read.
   Note: This bit is cleared by hardware when PE=0. */

/* Bit 1 TXIS: Transmit interrupt status (transmitters) */
#define I2C_TXIS (1 << 1)
/* This bit is set by hardware when the I2Cx_TXDR register is empty and 
   the data to be transmitted must be written in the I2Cx_TXDR register. It 
   is cleared when the next data to be sent is written in the I2Cx_TXDR 
   register.
   This bit can be written to ‘1’ by software when NOSTRETCH=1 only, in 
   order to generate a TXIS event (interrupt if TXIE=1 or DMA request if 
   TXDMAEN=1).
   Note: This bit is cleared by hardware when PE=0. */

/* Bit 0 TXE: Transmit data register empty (transmitters) */
#define I2C_TXE (1 << 0)
/* This bit is set by hardware when the I2Cx_TXDR register is empty. It is 
   cleared when the next data to be sent is written in the I2Cx_TXDR register.
   This bit can be written to ‘1’ by software in order to flush the 
   transmit data register I2Cx_TXDR.
   Note: This bit is set by hardware when PE=0. */

/*-------------------------------------------------------------------------
 * I2C Interrupt Clear register */
#define STM32F_I2C_ICR 0x18
/* Bit 13 ALERTCF: Alert flag clear */
#define I2C_ALERTCF (1 << 13)
/* Writing 1 to this bit clears the ALERT flag in the I2Cx_ISR register.
   Note: If the SMBus feature is not supported, this bit is reserved and 
   forced by hardware to ‘0’. */

/* Bit 12 TIMOUTCF: Timeout detection flag clear */
#define I2C_TIMOUTCF (1 << 12)
/* Writing 1 to this bit clears the TIMEOUT flag in the I2Cx_ISR register.
   Note: If the SMBus feature is not supported, this bit is reserved and 
   forced by hardware to ‘0’. */

/* Bit 11 PECCF: PEC Error flag clear */
#define I2C_PECCF (1 << 11)
/* Writing 1 to this bit clears the PECERR flag in the I2Cx_ISR register.
   Note: If the SMBus feature is not supported, this bit is reserved and 
   forced by hardware to ‘0’. */

/* Bit 10 OVRCF: Overrun/Underrun flag clear */
#define I2C_OVRCF (1 << 10)
/* Writing 1 to this bit clears the OVR flag in the I2Cx_ISR register.*/

/* Bit 9 ARLOCF: Arbitration Lost flag clear */
#define I2C_ARLOCF (1 << 9)
/* Writing 1 to this bit clears the ARLO flag in the I2Cx_ISR register. */
	
/* Bit 8 BERRCF: Bus error flag clear */
#define I2C_BERRCF (1 << 8)
/* Writing 1 to this bit clears the BERRF flag in the I2Cx_ISR register. */

/* Bits 7:6 Reserved, must be kept at reset value. */

/* Bit 5 STOPCF: Stop detection flag clear */
#define I2C_STOPCF (1 << 5)
/* Writing 1 to this bit clears the STOPF flag in the I2Cx_ISR register. */

/* Bit 4 NACKCF: Not Acknowledge flag clear */
#define I2C_NACKCF (1 << 4)
/* Writing 1 to this bit clears the ACKF flag in I2Cx_ISR register. */

/* Bit 3 ADDRCF: Address Matched flag clear */
#define I2C_ADDRCF (1 << 3)
/* Writing 1 to this bit clears the ADDR flag in the I2Cx_ISR register. 
   Writing 1 to this bit also clears the START bit in the I2Cx_CR2 register. */

/* Bits 2:0 Reserved, must be kept at reset value. */

/*-------------------------------------------------------------------------
 * I2C PEC register */
#define STM32F_I2C_PECR 0x20

/* I2C Receive Data register */
#define STM32F_I2C_RXDR 0x24

/* I2C Transmit Data register */
#define STM32F_I2C_TXDR 0x28

/*-------------------------------------------------------------------------
 * 25.6.4 I2C Own address register 2 */
#define STM32F_I2C_OAR2 0x0C

#else

/*-------------------------------------------------------------------------
 * 25.6.1 I2C Control register 1 */
#define STM32F_I2C_CR1 0x00

/* Bit 15 - Software reset */
#define I2C_SWRST (1 << 15)
/* 
   When set, the I2C is under reset state. Before resetting this bit, make sure 
   the I2C lines are released and the bus is free. 0: I2C Peripheral not under
   reset 1: I2C Peripheral under reset state Note: This bit can be used to
   reinitialize the peripheral after an error or a locked state. As an example, 
   if the BUSY bit is set and remains locked due to a glitch on the bus, the
   SWRST bit can be used to exit from this state. */

/* Bit 14 Reserved, must be kept at reset value */

/* Bit 13 - SMBus alert */
#define I2C_ALERT (1 << 13)
/* This bit is set and cleared by software, and cleared by hardware when PE=0.
   0: Releases SMBA pin high. Alert Response Address Header followed by NACK.
   1: Drives SMBA pin low. Alert Response Address Header followed by ACK. */

/* Bit 12 - Packet error checking */
#define I2C_PEC (1 << 12)
/* This bit is set and cleared by software, and cleared by hardware when PEC is 
   transferred or by a START or Stop condition or when PE=0. 0: No PEC transfer
   1: PEC transfer (in Tx or Rx mode) Note: PEC calculation is corrupted by an
   arbitration loss. */

/* Bit 11 - Acknowledge/PEC Position (for data reception) */
#define I2C_POS (1 << 11)
/* This bit is set and cleared by software and cleared by hardware when PE=0.
   0: ACK bit controls the (N)ACK of the current byte being received in the
   shift register. The PEC bit indicates that current byte in shift register is 
   a PEC. 1: ACK bit controls the (N)ACK of the next byte which will be
   received in the shift register. The PEC bit indicates that the next byte in
   the shift register is a PEC Note: The POS bit must be used only in 2-byte
   reception configuration in master mode. It must be configured before data
   reception starts, as described in the 2-byte reception procedure recommended 
   in Section : Master receiver on page 717. */

/* Bit 10 - Acknowledge enable */
#define I2C_ACK (1 << 10)
/* This bit is set and cleared by software and cleared by hardware when PE=0.
   0: No acknowledge returned 1: Acknowledge returned after a byte is received
   (matched address or data) */

/* Bit 9 - Stop generation */
#define I2C_STOP (1 << 9)
/* The bit is set and cleared by software, cleared by hardware when a Stop
   condition is detected, set by hardware when a timeout error is detected. In
   Master Mode: 0: No Stop generation. 1: Stop generation after the current
   byte transfer or after the current Start condition is sent. In Slave mode:
   0: No Stop generation. 1: Release the SCL and SDA lines after the current
   byte transfer. */

/* Bit 8 - Start generation */
#define I2C_START (1 << 8)
/* This bit is set and cleared by software and cleared by hardware when start
   is sent or PE=0. In Master Mode: 0: No Start generation 1: Repeated start
   generation In Slave mode: 0: No Start generation 1: Start generation when
   the bus is free */

/* Bit 7 - Clock stretching disable (Slave mode) */
#define I2C_NOSTRETCH (1 << 7)
/* This bit is used to disable clock stretching in slave mode when ADDR or BTF
   flag is set, until it is reset by software. 0: Clock stretching enabled 1:
   Clock stretching disabled */

/* Bit 6 - General call enable */
#define I2C_ENGC (1 << 6)
/* 0: General call disabled. Address 00h is NACKed. 
   1: General call enabled.
   Address 00h is ACKed. */

/* Bit 5 - PEC enable */
#define I2C_ENPEC (1 << 5)
/* 0: PEC calculation disabled 
   1: PEC calculation enabled */

/* Bit 4 - ARP enable */
#define I2C_ENARP (1 << 4)
/* 0: ARP disable 
   1: ARP enable SMBus Device default address recognized if
   SMBTYPE=0 SMBus Host address recognized if SMBTYPE=1 */

/* Bit 3 - SMBus type */
#define I2C_SMBTYPE (1 << 3)
/* 0: SMBus Device 
   1: SMBus Host */

/* Bit 2 Reserved, must be kept at reset value */

/* Bit 1 - SMBus mode */
#define I2C_SMBUS (1 << 1)
/* 0: I2C mode 
  1: SMBus mode */

/* Bit 0 - Peripheral enable */
#define I2C_PE (1 << 0)
/* 0: Peripheral disable  
   1: Peripheral enable 
   Note: If this bit is reset while a communication is on going, the 
   peripheral is disabled at the end of the current communication, when 
   back to IDLE state. All bit resets due to PE=0 occur at the end of the 
   communication. In master mode, this bit must not be reset before the 
   end of the communication. */

/*-------------------------------------------------------------------------
 * 25.6.2 I2C Control register 2 */
#define STM32F_I2C_CR2 0x04

/* [15..13] Reserved, must be kept at reset value */

/* Bit 12 - DMA last transfer */
#define I2C_LAST (1 << 12)
/* 0: Next DMA EOT is not the last transfer 
   1: Next DMA EOT is the last transfer Note: This bit is used in master 
   receiver mode to permit the generation of a NACK on the last received 
   data. */

/* Bit 11 - DMA requests enable */
#define I2C_DMAEN (1 << 11)
/* 0: DMA requests disabled 
   1: DMA request enabled when TxE=1 or RxNE =1 */

/* Bit 10 - Buffer interrupt enable */
#define I2C_ITBUFEN (1 << 10)
/* 0: TxE = 1 or RxNE = 1 does not generate any interrupt.
   1:TxE = 1 or RxNE = 1 generates Event Interrupt (whatever the state 
   of DMAEN) */

/* Bit 9 - Event interrupt enable */
#define I2C_ITEVTEN (1 << 9)
/* 0: Event interrupt disabled
   1: Event interrupt enabled This interrupt is generated when: â€“SB = 1 
   (Master) â€“ADDR = 1 (Master/Slave) â€“ADD10= 1 (Master) â€“STOPF = 1 
   (Slave) â€“BTF = 1 with no TxE or RxNE event â€“TxE
   event to 1 if ITBUFEN = 1 â€“RxNE event to 1if ITBUFEN = 1 */

/* Bit 8 - Error interrupt enable */
#define I2C_ITERREN (1 << 8)
/* 0: Error interrupt disabled
   1: Error interrupt enabled This interrupt is generated when: â€“ BERR = 1
   â€“ ARLO = 1 â€“ AF = 1 â€“ OVR = 1 â€“ PECERR = 1 
   â€“ TIMEOUT = 1 â€“ SMBALERT = 1 */

/* [7..6] Reserved, must be kept at reset value */

/* Bits [5..0] - Peripheral clock frequency */
#define I2C_FREQ_MSK (((1 << (5 + 1)) - 1) << 0)
#define I2C_FREQ_SET(VAL) (((VAL) << 0) & I2C_FREQ_MSK)
#define I2C_FREQ_GET(REG) (((REG) & I2C_FREQ_MSK) >> 0)
/* The peripheral clock frequency must be configured using the input APB clock
   frequency (I2C peripheral connected to APB). The minimum allowed frequency
   is 2 MHz, the maximum frequency is limited by the maximum APB frequency (42
   MHz)and an intrinsic limitation of 46 MHz. 0b000000: Not allowed 0b000001:
   Not allowed 0b000010: 2 MHz ... 0b101010: 42MHz Higher than 0b101010: Not
   allowed */

/*-------------------------------------------------------------------------
 * 25.6.3 I2C Own address register 1 */
#define STM32F_I2C_OAR1 0x08

/* Bit 15 ADDMODE Addressing mode (slave mode) 0: 7-bit slave address (10-bit
   address not acknowledged) 1: 10-bit slave address (7-bit address not
   acknowledged) Bit 14 Should always be kept at 1 by software. */

/* [13..10] Reserved, must be kept at reset value */

/* Bits [9..8] - Interface address */

/* 7-bit addressing mode: donâ€™t care 10-bit addressing mode: bits9:8 of
   address */

/* Bits [7..1] - Interface address */
#define I2C_ADD_MSK (((1 << (10)) - 1) << 1)
#define I2C_ADD_SET(VAL) (((VAL) << 1) & I2C_ADD_MSK)
#define I2C_ADD_GET(REG) (((REG) & I2C_ADD_MSK) >> 1)
/* bits 7:1 of address */

/* Bit 0 - Interface address */
#define I2C_ADD0 (1 << 0)
/* 7-bit addressing mode: donâ€™t care 10-bit addressing mode: bit 0 of address 
 */

/*-------------------------------------------------------------------------
 * 25.6.4 I2C Own address register 2 */
#define STM32F_I2C_OAR2 0x0C

/* [15..8] Reserved, must be kept at reset value */

/* Bits [7..1] - Interface address */
#define I2C_ADD2_MSK (((1 << (7 + 1)) - 1) << 1)
#define I2C_ADD2_SET(VAL) (((VAL) << 1) & I2C_ADD2_MSK)
#define I2C_ADD2_GET(REG) (((REG) & I2C_ADD2_MSK) >> 1)
/* bits 7:1 of address in dual addressing mode */

/* Bit 0 - Dual addressing mode enable */
#define I2C_ENDUAL (1 << 0)
/* 0: Only OAR1 is recognized in 7-bit addressing mode 1: Both OAR1 and OAR2
   are recognized in 7-bit addressing mode */

/* 25.6.5 I2C Data register */
#define STM32F_I2C_DR 0x10

/* [15..8] Reserved, must be kept at reset value */

/* Bits 7:0 DR[7:0] 8-bit data register Byte received or to be transmitted to
   the bus. â€“Transmitter mode: Byte transmission starts automatically when a
   byte is written in the DR register. A continuous transmit stream can be
   maintained if the next data to be transmitted is put in DR once the
   transmission is started (TxE=1) â€“Receiver mode: Received byte is copied
   into DR (RxNE=1). A continuous transmit stream can be maintained if DR is
   read before the next data byte is received (RxNE=1). Note: In slave mode,
   the address is not copied into DR. Note: Write collision is not managed (DR
   can be written if TxE=0). Note: If an ARLO event occurs on ACK pulse, the
   received byte is not copied into DR and so cannot be read. */

/*-------------------------------------------------------------------------
 * I2C Status register 1 */
#define STM32F_I2C_SR1 0x14

/* Bit 15 - SMBus alert */
#define I2C_SMBALERT (1 << 15)
/* In SMBus host mode: 0: no SMBALERT 1: SMBALERT event occurred on pin In
   SMBus slave mode: 0: no SMBALERT response address header 1: SMBALERT
   response address header to SMBALERT LOW received â€“ Cleared by software
   writing 0, or by hardware when PE=0. */

/* Bit 14 - Timeout or Tlow error */
#define I2C_TIMEOUT (1 << 14)
/* 0: No timeout error 1: SCL remained LOW for 25 ms (Timeout) or Master
   cumulative clock low extend time more than 10 ms (Tlow:mext) or Slave
   cumulative clock low extend time more than 25 ms (Tlow:sext) â€“ When set in 
   slave mode: slave resets the communication and lines are released by
   hardware â€“ When set in master mode: Stop condition sent by hardware â€“
   Cleared by software writing 0, or by hardware when PE=0. Note: This
   functionality is available only in SMBus mode. */

/* Bit 13 Reserved, must be kept at reset value */

/* Bit 12 - PEC Error in reception */
#define I2C_PECERR (1 << 12)
/* 
   0: no PEC error: receiver returns ACK after PEC reception (if ACK=1) 1: PEC
   error: receiver returns NACK after PEC reception (whatever ACK) â€“Cleared
   by software writing 0, or by hardware when PE=0. â€“Note: When the received
   CRC is wrong, PECERR is not set in slave mode if the PEC control bit is not
   set before the end of the CRC reception. Nevertheless, reading the PEC value 
   determines whether the received CRC is right or wrong. */

/* Bit 11 - Overrun/Underrun */
#define I2C_OVR (1 << 11)
/* 
   0: No overrun/underrun 1: Overrun or underrun â€“Set by hardware in slave
   mode when NOSTRETCH=1 and: â€“In reception when a new byte is received
   (including ACK pulse) and the DR register has not been read yet. New
   received byte is lost. â€“In transmission when a new byte should be sent and 
   the DR register has not been written yet. The same byte is sent twice.
   â€“Cleared by software writing 0, or by hardware when PE=0. Note: If the DR
   write occurs very close to SCL rising edge, the sent data is unspecified and 
   a hold timing error occurs */

/* Bit 10 - Acknowledge failure */
#define I2C_AF (1 << 10)
/* 
   0: No acknowledge failure 1: Acknowledge failure â€“Set by hardware when no
   acknowledge is returned. â€“Cleared by software writing 0, or by hardware
   when PE=0. */

/* Bit 9 - Arbitration lost (master mode) */
#define I2C_ARLO (1 << 9)
/* 
   0: No Arbitration Lost detected 1: Arbitration Lost detected Set by hardware 
   when the interface loses the arbitration of the bus to another master
   â€“Cleared by software writing 0, or by hardware when PE=0. After an ARLO
   event the interface switches back automatically to Slave mode (M/SL=0).
   Note: In SMBUS, the arbitration on the data in slave mode occurs only during 
   the data phase, or the acknowledge transmission (not on the address
   acknowledge). */

/* Bit 8 - Bus error */
#define I2C_BERR (1 << 8)
/* 
   0: No misplaced Start or Stop condition 1: Misplaced Start or Stop condition
   â€“Set by hardware when the interface detects an SDA rising or falling edge
   while SCL is high, occurring in a non-valid position during a byte transfer.
   â€“Cleared by software writing 0, or by hardware when PE=0. */

/* Bit 7 - Data register empty (transmitters) */
#define I2C_TXE (1 << 7)
/* 
   0: Data register not empty 1: Data register empty â€“Set when DR is empty in 
   transmission. TxE is not set during address phase. â€“Cleared by software
   writing to the DR register or by hardware after a start or a stop condition
   or when PE=0. TxE is not set if either a NACK is received, or if next byte
   to be transmitted is PEC (PEC=1) Note: TxE is not cleared by writing the
   first data being transmitted, or by writing data when BTF is set, as in both 
   cases the data register is still empty. */

/* Bit 6 - Data register not empty (receivers) */
#define I2C_RXNE (1 << 6)
/* 
   0: Data register empty 1: Data register not empty â€“Set when data register
   is not empty in receiver mode. RxNE is not set during address phase.
   â€“Cleared by software reading or writing the DR register or by hardware
   when PE=0. RxNE is not set in case of ARLO event. Note: RxNE is not cleared
   by reading data when BTF is set, as the data register is still full. */

/* Bit 5 Reserved, must be kept at reset value */

/* Bit 4 - Stop detection (slave mode) */
#define I2C_STOPF (1 << 4)
/* 
   0: No Stop condition detected 1: Stop condition detected â€“Set by hardware
   when a Stop condition is detected on the bus by the slave after an
   acknowledge (if ACK=1). â€“Cleared by software reading the SR1 register
   followed by a write in the CR1 register, or by hardware when PE=0 Note: The
   STOPF bit is not set after a NACK reception. It is recommended to perform
   the complete clearing sequence (READ SR1 then WRITE CR1) after the STOPF is
   set. Refer to Figure 242: Transfer sequence diagram for slave receiver on
   page 714. */

/* Bit 3 - 10-bit header sent (Master mode) */
#define I2C_ADD10 (1 << 3)
/* 
   0: No ADD10 event occurred. 1: Master has sent first address byte (header).
   â€“Set by hardware when the master has sent the first byte in 10-bit address 
   mode. â€“Cleared by software reading the SR1 register followed by a write in 
   the DR register of the second address byte, or by hardware when PE=0. Note:
   ADD10 bit is not set after a NACK reception */

/* Bit 2 - Byte transfer finished */
#define I2C_BTF (1 << 2)
/* 
   0: Data byte transfer not done 1: Data byte transfer succeeded â€“Set by
   hardware when NOSTRETCH=0 and: â€“In reception when a new byte is received
   (including ACK pulse) and DR has not been read yet (RxNE=1). â€“In
   transmission when a new byte should be sent and DR has not been written yet
   (TxE=1). â€“Cleared by software by either a read or write in the DR register 
   or by hardware after a start or a stop condition in transmission or when
   PE=0. Note: The BTF bit is not set after a NACK reception The BTF bit is not 
   set if next byte to be transmitted is the PEC (TRA=1 in I2C_SR2 register and 
   PEC=1 in I2C_CR1 register) */

/* Bit 1 - Address sent (master mode)/matched (slave mode) */
#define I2C_ADDR (1 << 1)
/* 
   This bit is cleared by software reading SR1 register followed reading SR2,
   or by hardware when PE=0. Address matched (Slave) 0: Address mismatched or
   not received. 1: Received address matched. â€“Set by hardware as soon as the 
   received slave address matched with the OAR registers content or a general
   call or a SMBus Device Default Address or SMBus Host or SMBus Alert is
   recognized. (when enabled depending on configuration). Note: In slave mode,
   it is recommended to perform the complete clearing sequence (READ SR1 then
   READ SR2) after ADDR is set. Refer to Figure 242: Transfer sequence diagram
   for slave receiver on page 714. Address sent (Master) 0: No end of address
   transmission 1: End of address transmission â€“For 10-bit addressing, the
   bit is set after the ACK of the 2nd byte. â€“For 7-bit addressing, the bit
   is set after the ACK of the byte. Note: ADDR is not set after a NACK
   reception */

/* Bit 0 - Start bit (Master mode) */
#define I2C_SB (1 << 0)
/* 
   0: No Start condition 1: Start condition generated. â€“Set when a Start
   condition generated. â€“Cleared by software by reading the SR1 register
   followed by writing the DR register, or by hardware when PE=0 */

/*-------------------------------------------------------------------------
 * I2C Status register 2 */
#define STM32F_I2C_SR2 0x18

/* 
   Note: Reading I2C_SR2 after reading I2C_SR1 clears the ADDR flag, even if
   the ADDR flag was set after reading I2C_SR1. Consequently, I2C_SR2 must be
   read only when ADDR is found set in I2C_SR1 or when the STOPF bit is cleared 
 */

/* Bits 15:8 PEC[7:0] Packet error checking register This register contains the 
   internal PEC when ENPEC=1. */
#define I2C_PEC_MSK (((1 << 8) - 1) << 8)
#define I2C_PEC_SET(VAL) (((VAL) << 8) & I2C_PEC_MSK)
#define I2C_PEC_GET(REG) (((REG) & I2C_PEC_MSK) >> 8)

/* Bit 7 - Dual flag (Slave mode) */
#define I2C_DUALF (1 << 7)
/* 
   0: Received address matched with OAR1 1: Received address matched with OAR2
   â€“Cleared by hardware after a Stop condition or repeated Start condition,
   or when PE=0. */

/* Bit 6 - SMBus host header (Slave mode) */
#define I2C_SMBHOST (1 << 6)
/* 
   0: No SMBus Host address 1: SMBus Host address received when SMBTYPE=1 and
   ENARP=1. â€“Cleared by hardware after a Stop condition or repeated Start
   condition, or when PE=0. */

/* Bit 5 - SMBus device default address (Slave mode) */
#define I2C_SMBDEFAULT (1 << 5)
/* 
   0: No SMBus Device Default address 1: SMBus Device Default address received
   when ENARP=1 â€“Cleared by hardware after a Stop condition or repeated Start 
   condition, or when PE=0. */

/* Bit 4 - General call address (Slave mode) */
#define I2C_GENCALL (1 << 4)
/* 
   0: No General Call 1: General Call Address received when ENGC=1 â€“Cleared
   by hardware after a Stop condition or repeated Start condition, or when
   PE=0. */

/* Bit 3 Reserved, must be kept at reset value */

/* Bit 2 - Transmitter/receiver */
#define I2C_TRA (1 << 2)
/* 
   0: Data bytes received 1: Data bytes transmitted This bit is set depending
   on the R/W bit of the address byte, at the end of total address phase. It is 
   also cleared by hardware after detection of Stop condition (STOPF=1),
   repeated Start condition, loss of bus arbitration (ARLO=1), or when PE=0. */

/* Bit 1 - Bus busy */
#define I2C_BUSY (1 << 1)
/* 
   0: No communication on the bus 1: Communication ongoing on the bus â€“Set by 
   hardware on detection of SDA or SCL low â€“cleared by hardware on detection
   of a Stop condition. It indicates a communication in progress on the bus.
   This information is still updated when the interface is disabled (PE=0). */

/* Bit 0 - Master/slave */
#define I2C_MSL (1 << 0)
/* 
   0: Slave Mode 1: Master Mode â€“Set by hardware as soon as the interface is
   in Master mode (SB=1). â€“Cleared by hardware after detecting a Stop
   condition on the bus or a loss of arbitration (ARLO=1), or by hardware when
   PE=0. */

/*-------------------------------------------------------------------------
 * I2C Clock control register */
#define STM32F_I2C_CCR 0x1C

/* Note: fPCLK1 must be at least 2 MHz to achieve standard mode IÂ²C
   frequencies. It must be at least 4 MHz to achieve fast mode IÂ²C
   frequencies. It must be a multiple of 10MHz to reach the 400 kHz maximum
   IÂ²C fast mode clock. The CCR register must be configured only when the I2C 
   is disabled (PE = 0). */

/* Bit 15 F/S: I2C master mode selection 0: Standard Mode I2C 1: Fast Mode I2C */
#define I2C_FAST (1 << 15)

/* Bit 14 - Fast mode duty cycle */
#define I2C_DUTY (1 << 14)
/* 
   0: Fast Mode tlow/thigh = 2 1: Fast Mode tlow/thigh = 16/9 (see CCR) */

/* [13..12] Reserved, must be kept at reset value */

/* Bits [11..0] - Clock control register in Fast/Standard mode (Master mode) */
#define I2C_CCR_MSK (((1 << (11 + 1)) - 1) << 0)
#define I2C_CCR_SET(VAL) (((VAL) << 0) & I2C_CCR_MSK)
#define I2C_CCR_GET(REG) (((REG) & I2C_CCR_MSK) >> 0)
/* Controls the SCL clock in master mode. Standard mode or SMBus: Thigh = CCR * 
   TPCLK1 Tlow = CCR * TPCLK1 Fast mode: If DUTY = 0: Thigh = CCR * TPCLK1 Tlow 
   = 2 * CCR * TPCLK1 If DUTY = 1: (to reach 400 kHz) Thigh = 9 * CCR * TPCLK1
   Tlow = 16 * CCR * TPCLK1 For instance: in standard mode, to generate a 100
   kHz SCL frequency: If FREQR = 08, TPCLK1 = 125 ns so CCR must be programmed
   with 0x28 (0x28 <=> 40d x 125 ns = 5000 ns.) Note: 1. The minimum allowed
   value is 0x04, except in FAST DUTY mode where the minimum allowed value is
   0x01. These timings are without filters. The CCR register must be
   configured only when the I2C is disabled (PE = 0). */

/*-------------------------------------------------------------------------
 * I2C TRISE register */
#define STM32F_I2C_TRISE 0x20

/* [15..6] Reserved, must be kept at reset value */

/* Bits [5..0] - Maximum rise time in Fast/Standard mode (Master mode) */
#define I2C_TRISE_MSK (((1 << (5 + 1)) - 1) << 0)
#define I2C_TRISE_SET(VAL) (((VAL) << 0) & I2C_TRISE_MSK)
#define I2C_TRISE_GET(REG) (((REG) & I2C_TRISE_MSK) >> 0)
/* These bits must be programmed with the maximum SCL rise time given in the
   I2C bus specification, incremented by 1. For instance: in standard mode, the 
   maximum allowed SCL rise time is 1000 ns. If, in the I2C_CR2 register, the
   value of FREQ[5:0] bits is equal to 0x08 and TPCLK1 = 125 ns therefore the
   TRISE[5:0] bits must be programmed with 09h. (1000 ns / 125 ns = 8 + 1) The
   filter value can also be added to TRISE[5:0]. If the result is not an
   integer, TRISE[5:0] must be programmed with the integer part, in order to
   respect the tHIGH parameter. Note: TRISE[5:0] must be configured only when
   the I2C is disabled (PE = 0). */

#if defined(STM32F2X)

/*-------------------------------------------------------------------------
 * I2C FLTR register */
#define STM32F_I2C_FLTR 0x24

/* The I2C_FLTR is available on STM32F42xxx and STM32F43xxx only. ts 15:5
   Reserved, must be kept at reset value */

/* Bit 4 - Analog noise filter OFF */
#define I2C_ANOFF (1 << 4)
/* 
   0: Analog noise filter enable 1: Analog noise filter disable Note: ANOFF
   must be configured only when the I2C is disabled (PE = 0). */

/* Bits [3..0] - Digital noise filter */
#define I2C_DNF_MSK (((1 << (3 + 1)) - 1) << 0)
#define I2C_DNF_SET(VAL) (((VAL) << 0) & I2C_DNF_MSK)
#define I2C_DNF_GET(REG) (((REG) & I2C_DNF_MSK) >> 0)
/* 
   These bits are used to configure the digital noise filter on SDA and SCL
   inputs. The digital filter will suppress the spikes with a length of up to
   DNF[3:0] * TPCLK1. 0000: Digital noise filter disable 0001: Digital noise
   filter enabled and filtering capability up to 1* TPCLK1. ... 1111: Digital
   noise filter enabled and filtering capability up to 15* TPCLK1. Note:
   DNF[3:0] must be configured only when the I2C is disabled (PE = 0). If the
   analog filter is also enabled, the digital filter is added to the analog
   filter. */

#endif /* STM32F2X */

#endif /* STM32F3X */

#ifndef __ASSEMBLER__

#include <stdint.h>

#if defined(STM32F3X)

struct stm32f_i2c {
	volatile uint32_t cr1;
	volatile uint32_t cr2;
	volatile uint32_t oar1;
	volatile uint32_t oar2;

	volatile uint32_t timingr;
	volatile uint32_t timeoutr;
	volatile uint32_t isr;
	volatile uint32_t icr;

	volatile uint32_t pecr;
	volatile uint32_t rxdr;
	volatile uint32_t txdr;
};

#else

struct stm32f_i2c {
	volatile uint32_t cr1;
	volatile uint32_t cr2;
	volatile uint32_t oar1;
	volatile uint32_t oar2;

	volatile uint32_t dr;
	volatile uint32_t sr1;
	volatile uint32_t sr2;
	volatile uint32_t ccr;

	volatile uint32_t trise;
#if defined(STM32F2X)
	volatile uint32_t fltr;
#endif
};

#endif /* STM32F3X */

#endif				/* __ASSEMBLER__ */

#endif				/* __STM32F_I2C_H__ */
