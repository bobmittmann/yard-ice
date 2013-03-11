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
/* 
   This bit is set and cleared by software, and cleared by hardware when PE=0.
   0: Releases SMBA pin high. Alert Response Address Header followed by NACK.
   1: Drives SMBA pin low. Alert Response Address Header followed by ACK. */

/* Bit 12 - Packet error checking */
#define I2C_PEC (1 << 12)
/* 
   This bit is set and cleared by software, and cleared by hardware when PEC is 
   transferred or by a START or Stop condition or when PE=0. 0: No PEC transfer
   1: PEC transfer (in Tx or Rx mode) Note: PEC calculation is corrupted by an
   arbitration loss. */

/* Bit 11 - Acknowledge/PEC Position (for data reception) */
#define I2C_POS (1 << 11)
/* 
   This bit is set and cleared by software and cleared by hardware when PE=0.
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
/* 
   This bit is set and cleared by software and cleared by hardware when PE=0.
   0: No acknowledge returned 1: Acknowledge returned after a byte is received
   (matched address or data) */

/* Bit 9 - Stop generation */
#define I2C_STOP (1 << 9)
/* 
   The bit is set and cleared by software, cleared by hardware when a Stop
   condition is detected, set by hardware when a timeout error is detected. In
   Master Mode: 0: No Stop generation. 1: Stop generation after the current
   byte transfer or after the current Start condition is sent. In Slave mode:
   0: No Stop generation. 1: Release the SCL and SDA lines after the current
   byte transfer. */

/* Bit 8 - Start generation */
#define I2C_START (1 << 8)
/* 
   This bit is set and cleared by software and cleared by hardware when start
   is sent or PE=0. In Master Mode: 0: No Start generation 1: Repeated start
   generation In Slave mode: 0: No Start generation 1: Start generation when
   the bus is free */

/* Bit 7 - Clock stretching disable (Slave mode) */
#define I2C_NOSTRETCH (1 << 7)
/* 
   This bit is used to disable clock stretching in slave mode when ADDR or BTF
   flag is set, until it is reset by software. 0: Clock stretching enabled 1:
   Clock stretching disabled */

/* Bit 6 - General call enable */
#define I2C_ENGC (1 << 6)
/* 
   0: General call disabled. Address 00h is NACKed. 1: General call enabled.
   Address 00h is ACKed. */

/* Bit 5 - PEC enable */
#define I2C_ENPEC (1 << 5)
/* 
   0: PEC calculation disabled 1: PEC calculation enabled */

/* Bit 4 - ARP enable */
#define I2C_ENARP (1 << 4)
/* 
   0: ARP disable 1: ARP enable SMBus Device default address recognized if
   SMBTYPE=0 SMBus Host address recognized if SMBTYPE=1 */

/* Bit 3 - SMBus type */
#define I2C_SMBTYPE (1 << 3)
/* 
   0: SMBus Device 1: SMBus Host */

/* Bit 2 Reserved, must be kept at reset value */

/* Bit 1 - SMBus mode */
#define I2C_SMBUS (1 << 1)
/* 
   0: I2C mode 1: SMBus mode */

/* Bit 0 - Peripheral enable */
#define I2C_PE (1 << 0)
/* 
   0: Peripheral disable 1: Peripheral enable Note: If this bit is reset while
   a communication is on going, the peripheral is disabled at the end of the
   current communication, when back to IDLE state. All bit resets due to PE=0
   occur at the end of the communication. In master mode, this bit must not be
   reset before the end of the communication. */

/*-------------------------------------------------------------------------
 * 25.6.2 I2C Control register 2 */
#define STM32F_I2C_CR2 0x04

/* [15..13] Reserved, must be kept at reset value */

/* Bit 12 - DMA last transfer */
#define I2C_LAST (1 << 12)
/* 
   0: Next DMA EOT is not the last transfer 1: Next DMA EOT is the last
   transfer Note: This bit is used in master receiver mode to permit the
   generation of a NACK on the last received data. */

/* Bit 11 - DMA requests enable */
#define I2C_DMAEN (1 << 11)
/* 
   0: DMA requests disabled 1: DMA request enabled when TxE=1 or RxNE =1 */

/* Bit 10 - Buffer interrupt enable */
#define I2C_ITBUFEN (1 << 10)
/* 
   0: TxE = 1 or RxNE = 1 does not generate any interrupt. 1:TxE = 1 or RxNE =
   1 generates Event Interrupt (whatever the state of DMAEN) */

/* Bit 9 - Event interrupt enable */
#define I2C_ITEVTEN (1 << 9)
/* 
   0: Event interrupt disabled 1: Event interrupt enabled This interrupt is
   generated when: –SB = 1 (Master) –ADDR = 1 (Master/Slave) –ADD10= 1
   (Master) –STOPF = 1 (Slave) –BTF = 1 with no TxE or RxNE event –TxE
   event to 1 if ITBUFEN = 1 –RxNE event to 1if ITBUFEN = 1 */

/* Bit 8 - Error interrupt enable */
#define I2C_ITERREN (1 << 8)
/* 
   0: Error interrupt disabled 1: Error interrupt enabled This interrupt is
   generated when: – BERR = 1 – ARLO = 1 – AF = 1 – OVR = 1 – PECERR
   = 1 – TIMEOUT = 1 – SMBALERT = 1 */

/* [7..6] Reserved, must be kept at reset value */

/* Bits [5..0] - Peripheral clock frequency */
#define I2C_FREQ_MSK (((1 << (5 + 1)) - 1) << 0)
#define I2C_FREQ_SET(VAL) (((VAL) << 0) & FREQ_MSK)
#define I2C_FREQ_GET(REG) (((REG) & FREQ_MSK) >> 0)
/* 
   The peripheral clock frequency must be configured using the input APB clock
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
#define I2C_ADD_MSK (((1 << (9 + 1)) - 1) << 8)
#define I2C_ADD_SET(VAL) (((VAL) << 8) & ADD_MSK)
#define I2C_ADD_GET(REG) (((REG) & ADD_MSK) >> 8)
/* 7-bit addressing mode: don’t care 10-bit addressing mode: bits9:8 of
   address */

/* Bits [7..1] - Interface address */
#define I2C_ADD_MSK (((1 << (7 + 1)) - 1) << 1)
#define I2C_ADD_SET(VAL) (((VAL) << 1) & ADD_MSK)
#define I2C_ADD_GET(REG) (((REG) & ADD_MSK) >> 1)
/* bits 7:1 of address */

/* Bit 0 - Interface address */
#define I2C_ADD0 (1 << 0)
/* 7-bit addressing mode: don’t care 10-bit addressing mode: bit 0 of address 
 */

/*-------------------------------------------------------------------------
 * 25.6.4 I2C Own address register 2 */
#define STM32F_I2C_OAR2 0x0C

/* [15..8] Reserved, must be kept at reset value */

/* Bits [7..1] - Interface address */
#define I2C_ADD2_MSK (((1 << (7 + 1)) - 1) << 1)
#define I2C_ADD2_SET(VAL) (((VAL) << 1) & ADD2_MSK)
#define I2C_ADD2_GET(REG) (((REG) & ADD2_MSK) >> 1)
/* bits 7:1 of address in dual addressing mode */

/* Bit 0 - Dual addressing mode enable */
#define I2C_ENDUAL (1 << 0)
/* 0: Only OAR1 is recognized in 7-bit addressing mode 1: Both OAR1 and OAR2
   are recognized in 7-bit addressing mode */

/* 25.6.5 I2C Data register */
#define STM32F_I2C_DR 0x10

/* [15..8] Reserved, must be kept at reset value */

/* Bits 7:0 DR[7:0] 8-bit data register Byte received or to be transmitted to
   the bus. –Transmitter mode: Byte transmission starts automatically when a
   byte is written in the DR register. A continuous transmit stream can be
   maintained if the next data to be transmitted is put in DR once the
   transmission is started (TxE=1) –Receiver mode: Received byte is copied
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
   response address header to SMBALERT LOW received – Cleared by software
   writing 0, or by hardware when PE=0. */

/* Bit 14 - Timeout or Tlow error */
#define I2C_TIMEOUT (1 << 14)
/* 0: No timeout error 1: SCL remained LOW for 25 ms (Timeout) or Master
   cumulative clock low extend time more than 10 ms (Tlow:mext) or Slave
   cumulative clock low extend time more than 25 ms (Tlow:sext) – When set in 
   slave mode: slave resets the communication and lines are released by
   hardware – When set in master mode: Stop condition sent by hardware –
   Cleared by software writing 0, or by hardware when PE=0. Note: This
   functionality is available only in SMBus mode. */

/* Bit 13 Reserved, must be kept at reset value */

/* Bit 12 - PEC Error in reception */
#define I2C_PECERR (1 << 12)
/* 
   0: no PEC error: receiver returns ACK after PEC reception (if ACK=1) 1: PEC
   error: receiver returns NACK after PEC reception (whatever ACK) –Cleared
   by software writing 0, or by hardware when PE=0. –Note: When the received
   CRC is wrong, PECERR is not set in slave mode if the PEC control bit is not
   set before the end of the CRC reception. Nevertheless, reading the PEC value 
   determines whether the received CRC is right or wrong. */

/* Bit 11 - Overrun/Underrun */
#define I2C_OVR (1 << 11)
/* 
   0: No overrun/underrun 1: Overrun or underrun –Set by hardware in slave
   mode when NOSTRETCH=1 and: –In reception when a new byte is received
   (including ACK pulse) and the DR register has not been read yet. New
   received byte is lost. –In transmission when a new byte should be sent and 
   the DR register has not been written yet. The same byte is sent twice.
   –Cleared by software writing 0, or by hardware when PE=0. Note: If the DR
   write occurs very close to SCL rising edge, the sent data is unspecified and 
   a hold timing error occurs */

/* Bit 10 - Acknowledge failure */
#define I2C_AF (1 << 10)
/* 
   0: No acknowledge failure 1: Acknowledge failure –Set by hardware when no
   acknowledge is returned. –Cleared by software writing 0, or by hardware
   when PE=0. */

/* Bit 9 - Arbitration lost (master mode) */
#define I2C_ARLO (1 << 9)
/* 
   0: No Arbitration Lost detected 1: Arbitration Lost detected Set by hardware 
   when the interface loses the arbitration of the bus to another master
   –Cleared by software writing 0, or by hardware when PE=0. After an ARLO
   event the interface switches back automatically to Slave mode (M/SL=0).
   Note: In SMBUS, the arbitration on the data in slave mode occurs only during 
   the data phase, or the acknowledge transmission (not on the address
   acknowledge). */

/* Bit 8 - Bus error */
#define I2C_BERR (1 << 8)
/* 
   0: No misplaced Start or Stop condition 1: Misplaced Start or Stop condition
   –Set by hardware when the interface detects an SDA rising or falling edge
   while SCL is high, occurring in a non-valid position during a byte transfer.
   –Cleared by software writing 0, or by hardware when PE=0. */

/* Bit 7 - Data register empty (transmitters) */
#define I2C_TXE (1 << 7)
/* 
   0: Data register not empty 1: Data register empty –Set when DR is empty in 
   transmission. TxE is not set during address phase. –Cleared by software
   writing to the DR register or by hardware after a start or a stop condition
   or when PE=0. TxE is not set if either a NACK is received, or if next byte
   to be transmitted is PEC (PEC=1) Note: TxE is not cleared by writing the
   first data being transmitted, or by writing data when BTF is set, as in both 
   cases the data register is still empty. */

/* Bit 6 - Data register not empty (receivers) */
#define I2C_RXNE (1 << 6)
/* 
   0: Data register empty 1: Data register not empty –Set when data register
   is not empty in receiver mode. RxNE is not set during address phase.
   –Cleared by software reading or writing the DR register or by hardware
   when PE=0. RxNE is not set in case of ARLO event. Note: RxNE is not cleared
   by reading data when BTF is set, as the data register is still full. */

/* Bit 5 Reserved, must be kept at reset value */

/* Bit 4 - Stop detection (slave mode) */
#define I2C_STOPF (1 << 4)
/* 
   0: No Stop condition detected 1: Stop condition detected –Set by hardware
   when a Stop condition is detected on the bus by the slave after an
   acknowledge (if ACK=1). –Cleared by software reading the SR1 register
   followed by a write in the CR1 register, or by hardware when PE=0 Note: The
   STOPF bit is not set after a NACK reception. It is recommended to perform
   the complete clearing sequence (READ SR1 then WRITE CR1) after the STOPF is
   set. Refer to Figure 242: Transfer sequence diagram for slave receiver on
   page 714. */

/* Bit 3 - 10-bit header sent (Master mode) */
#define I2C_ADD10 (1 << 3)
/* 
   0: No ADD10 event occurred. 1: Master has sent first address byte (header).
   –Set by hardware when the master has sent the first byte in 10-bit address 
   mode. –Cleared by software reading the SR1 register followed by a write in 
   the DR register of the second address byte, or by hardware when PE=0. Note:
   ADD10 bit is not set after a NACK reception */

/* Bit 2 - Byte transfer finished */
#define I2C_BTF (1 << 2)
/* 
   0: Data byte transfer not done 1: Data byte transfer succeeded –Set by
   hardware when NOSTRETCH=0 and: –In reception when a new byte is received
   (including ACK pulse) and DR has not been read yet (RxNE=1). –In
   transmission when a new byte should be sent and DR has not been written yet
   (TxE=1). –Cleared by software by either a read or write in the DR register 
   or by hardware after a start or a stop condition in transmission or when
   PE=0. Note: The BTF bit is not set after a NACK reception The BTF bit is not 
   set if next byte to be transmitted is the PEC (TRA=1 in I2C_SR2 register and 
   PEC=1 in I2C_CR1 register) */

/* Bit 1 - Address sent (master mode)/matched (slave mode) */
#define I2C_ADDR (1 << 1)
/* 
   This bit is cleared by software reading SR1 register followed reading SR2,
   or by hardware when PE=0. Address matched (Slave) 0: Address mismatched or
   not received. 1: Received address matched. –Set by hardware as soon as the 
   received slave address matched with the OAR registers content or a general
   call or a SMBus Device Default Address or SMBus Host or SMBus Alert is
   recognized. (when enabled depending on configuration). Note: In slave mode,
   it is recommended to perform the complete clearing sequence (READ SR1 then
   READ SR2) after ADDR is set. Refer to Figure 242: Transfer sequence diagram
   for slave receiver on page 714. Address sent (Master) 0: No end of address
   transmission 1: End of address transmission –For 10-bit addressing, the
   bit is set after the ACK of the 2nd byte. –For 7-bit addressing, the bit
   is set after the ACK of the byte. Note: ADDR is not set after a NACK
   reception */

/* Bit 0 - Start bit (Master mode) */
#define I2C_SB (1 << 0)
/* 
   0: No Start condition 1: Start condition generated. –Set when a Start
   condition generated. –Cleared by software by reading the SR1 register
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
#define I2C_PEC (0xff << 8)

/* Bit 7 - Dual flag (Slave mode) */
#define I2C_DUALF (1 << 7)
/* 
   0: Received address matched with OAR1 1: Received address matched with OAR2
   –Cleared by hardware after a Stop condition or repeated Start condition,
   or when PE=0. */

/* Bit 6 - SMBus host header (Slave mode) */
#define I2C_SMBHOST (1 << 6)
/* 
   0: No SMBus Host address 1: SMBus Host address received when SMBTYPE=1 and
   ENARP=1. –Cleared by hardware after a Stop condition or repeated Start
   condition, or when PE=0. */

/* Bit 5 - SMBus device default address (Slave mode) */
#define I2C_SMBDEFAULT (1 << 5)
/* 
   0: No SMBus Device Default address 1: SMBus Device Default address received
   when ENARP=1 –Cleared by hardware after a Stop condition or repeated Start 
   condition, or when PE=0. */

/* Bit 4 - General call address (Slave mode) */
#define I2C_GENCALL (1 << 4)
/* 
   0: No General Call 1: General Call Address received when ENGC=1 –Cleared
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
   0: No communication on the bus 1: Communication ongoing on the bus –Set by 
   hardware on detection of SDA or SCL low –cleared by hardware on detection
   of a Stop condition. It indicates a communication in progress on the bus.
   This information is still updated when the interface is disabled (PE=0). */

/* Bit 0 - Master/slave */
#define I2C_MSL (1 << 0)
/* 
   0: Slave Mode 1: Master Mode –Set by hardware as soon as the interface is
   in Master mode (SB=1). –Cleared by hardware after detecting a Stop
   condition on the bus or a loss of arbitration (ARLO=1), or by hardware when
   PE=0. */

/*-------------------------------------------------------------------------
 * I2C Clock control register */
#define STM32F_I2C_CCR 0x1C

/* Note: fPCLK1 must be at least 2 MHz to achieve standard mode I²C
   frequencies. It must be at least 4 MHz to achieve fast mode I²C
   frequencies. It must be a multiple of 10MHz to reach the 400 kHz maximum
   I²C fast mode clock. The CCR register must be configured only when the I2C 
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
#define I2C_CCR_SET(VAL) (((VAL) << 0) & CCR_MSK)
#define I2C_CCR_GET(REG) (((REG) & CCR_MSK) >> 0)
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
#define I2C_TRISE_SET(VAL) (((VAL) << 0) & TRISE_MSK)
#define I2C_TRISE_GET(REG) (((REG) & TRISE_MSK) >> 0)
/* 
   These bits must be programmed with the maximum SCL rise time given in the
   I2C bus specification, incremented by 1. For instance: in standard mode, the 
   maximum allowed SCL rise time is 1000 ns. If, in the I2C_CR2 register, the
   value of FREQ[5:0] bits is equal to 0x08 and TPCLK1 = 125 ns therefore the
   TRISE[5:0] bits must be programmed with 09h. (1000 ns / 125 ns = 8 + 1) The
   filter value can also be added to TRISE[5:0]. If the result is not an
   integer, TRISE[5:0] must be programmed with the integer part, in order to
   respect the tHIGH parameter. Note: TRISE[5:0] must be configured only when
   the I2C is disabled (PE = 0). */

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
#define I2C_DNF_SET(VAL) (((VAL) << 0) & DNF_MSK)
#define I2C_DNF_GET(REG) (((REG) & DNF_MSK) >> 0)
/* 
   These bits are used to configure the digital noise filter on SDA and SCL
   inputs. The digital filter will suppress the spikes with a length of up to
   DNF[3:0] * TPCLK1. 0000: Digital noise filter disable 0001: Digital noise
   filter enabled and filtering capability up to 1* TPCLK1. ... 1111: Digital
   noise filter enabled and filtering capability up to 15* TPCLK1. Note:
   DNF[3:0] must be configured only when the I2C is disabled (PE = 0). If the
   analog filter is also enabled, the digital filter is added to the analog
   filter. */

#ifndef __ASSEMBLER__

#include <stdint.h>

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
};

#endif				/* __ASSEMBLER__ */

#endif				/* __STM32F_I2C_H__ */
