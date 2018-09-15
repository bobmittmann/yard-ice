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
 * @file dm36x-aemif.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __DM36X_AEMIF_H__
#define __DM36X_AEMIF_H__

/* AEMIF Registers */
#define AEMIF_ERCSR 0x00
#define AEMIF_AWCCR 0x04
#define AEMIF_A1CR 0x10
#define AEMIF_A2CR 0x14
#define AEMIF_EIRR 0x40
#define AEMIF_EIMR 0x44
#define AEMIF_EIMSR 0x48
#define AEMIF_EIMCR 0x4c
#define AEMIF_ONENANDCTL 0x5c  
#define AEMIF_NANDFCR 0x60
#define AEMIF_NANDFSR 0x64
#define AEMIF_NANDF1ECC 0x70
#define AEMIF_NANDF2ECC 0x74
#define AEMIF_NAND4BITECCLOAD 0xbc
#define AEMIF_NAND4BITECC1 0xc0
#define AEMIF_NAND4BITECC2 0xc4
#define AEMIF_NAND4BITECC3 0xc8
#define AEMIF_NAND4BITECC4 0xcc
#define AEMIF_NANDERRADD1 0xd0
#define AEMIF_NANDERRADD2 0xd4
#define AEMIF_NANDERRVAL1 0xd8
#define AEMIF_NANDERRVAL2 0xdc

/*****************************************************************************
 *
 ****************************************************************************/

/*****************************************************************************
 * Asynchronous Wait Cycle Configuration Register (AWCCR)
 ****************************************************************************/

/*
The asynchronous wait cycle configuration register (AWCCR) is used to configure the parameters for
extended wait cycles. Both the polarity of the EM_WAIT pin and the maximum allowable number of
extended wait cycles can be configured. */

/* WAIT polarity bit. This bit defines the polarity of the EM_WAIT pin. 
   0 Insert wait cycles if EM_WAIT is low.
   1 Insert wait cycles if EM_WAIT is high. */
#define AEMIF_WP0 (1 << 28)
#define AEMIF_WP_LOW (0 << 28)
#define AEMIF_WP_HIGH (1 << 28)

/* Maximum extended wait cycles. The EMIF will wait for a 
   maximum of (MEWC + 1) = 16 clock cycles before it stops inserting 
   asynchronous wait cycles and proceeds to the hold period of the access. */
#define AEMIF_MEWC(N) ((((N) - 1) & 0xff) << 0)

/*****************************************************************************
 * Asynchronous Configuration Registers (A1CR-A2CR)
 ****************************************************************************/


/* Select Strobe bit. This bit defines whether the asynchronous interface 
   operates in Normal mode or Select Strobe mode.
   0 Normal mode is enabled.
   1 Select Strobe mode is enabled. */
#define ACR_SS (1 << 31)

/* Extend Wait bit. This bit defines whether extended wait cycles will be 
   enabled. See Section 2.5.8 on extended wait cycles for details. 
   This bit field must be cleared to 0, if the EMIF on your device does 
   not have a EM_WAIT pin.
   0 Extended wait cycles are disabled.
   1 Extended wait cycles are enabled. */
#define ACR_EW (1 << 30)

/* Write setup width in EMIF clock cycles, minus 1 cycle. 
   See Section 2.5.3 for details. */
#define ACR_W_SETUP(N) ((((N) - 1) & 0xf) << 26)

/* Write strobe width in EMIF clock cycles, minus 1 cycle. 
   See Section 2.5.3 for details. */
#define ACR_W_STROBE(N) ((((N) - 1) & 0x3f) << 20)

/* Write hold width in EMIF clock cycles, minus 1 cycle. 
   See Section 2.5.3 for details. */
#define ACR_W_HOLD(N) ((((N) - 1) & 0x7) << 17)

/* Read setup width in EMIF clock cycles, minus 1 cycle. 
   See Section 2.5.3 for details. */
#define ACR_R_SETUP(N) ((((N) - 1) & 0xf) << 13)

/* Read strobe width in EMIF clock cycles, minus 1 cycle. 
   See Section 2.5.3 for details. */
#define ACR_R_STROBE(N) ((((N) - 1) & 0x3f) << 7)

/* Read hold width in EMIF clock cycles, minus 1 cycle. 
   See Section 2.5.3 for details. */
#define ACR_R_HOLD(N) ((((N) - 1) & 0x7) << 4)

/* Minimum Turn-Around time. This field defines the minimum number of EMIF 
   clock cycles between the end of one asynchronous access and the start of 
   another, minus 1 cycle. This delay is not incurred by a read followed by
   a read or a write followed by a write to the same CS space. See Section 
   2.5.3 for details. */
#define ACR_TA(N) ((((N) - 1) & 0x3) << 2)

/* Asynchronous data bus width. This bit defines the width of 
   the asynchronous device's data bus. 
   0 - 8-bit data bus
   1 - 16-bit data bus
   2-3 - Reserved. */
#define ACR_ASIZE ((0x3) << 0)
#define ACR_ASIZE_8BITS ((0x0) << 0)
#define ACR_ASIZE_16BITS ((0x1) << 0)

/*****************************************************************************
 * EMIF Interrupt Raw Register (EIRR)
 ****************************************************************************/


/* Wait Rise. This bit is set to 1 by hardware to indicate that a rising edge 
   on the EM_WAIT pin has occurred.
   0 Indicates that a rising edge has not occurred on the EM_WAIT pin. 
   Writing a 0 has no effect.
   1 Indicates that a rising edge has occurred on the EM_WAIT pin. 
   Writing a 1 will clear this bit and the WRM bit in the EMIF interrupt 
   mask register (EIMR). */
#define EIRR_WR (1 << 2)


/* Asynchronous Timeout. This bit is set to 1 by hardware to indicate that 
   during an extended asynchronous memory access cycle the EM_WAIT pin did 
   not go inactive within the number of cycles defined by the MEWC field in 
   the asynchronous wait cycle configuration register (AWCCR).
   0 Indicates that an asynchronous timeout has not occurred. 
   Writing a 0 has no effect.
   1 Indicates that an asynchronous timeout has occurred. Writing a 1 will clear 
   this bit and the ATM bit in the EMIF interrupt mask register (EIMR). */
#define EIRR_AT (1 << 0)



/*****************************************************************************
 * EMIF Interrupt Mask Register (EIMR)
 ****************************************************************************/


/* Wait Rise Masked. This bit is set to 1 by hardware to indicate a rising edge 
   has occurred on the EM_WAIT pin, provided that the WRMSET bit is set to 1 
   in the EMIF interrupt mask set register (EIMSR).
   0 Indicates that a wait rise interrupt has not been generated. Writing a 
   0 has no effect.
   1 Indicates that a wait rise interrupt has been generated. Writing a 1 will 
   clear this bit and the WR bit in the EMIF interrupt mask register (EIMR). */
#define EIMR_WRM (1 << 2)


/* Asynchronous Timeout Masked. This bit is set to 1 by hardware to indicate 
   that during an extended asynchronous memory access cycle the EM_WAIT pin 
   did not go inactive within the number of cycles defined by the MEWC field 
   in the asynchronous wait cycle configuration register (AWCCR), provided
   that the ATMSET bit is set to 1 in the EMIF interrupt mask set register (EIMSR).
   0 Indicates that an asynchronous timeout interrupt has not been generated. 
   Writing a 0 has no effect.
   1 Indicates that an asynchronous timeout interrupt has been generated. 
   Writing a 1 will clear this bit and the AT bit in the EMIF interrupt 
   mask register (EIMR). */
#define EIMR_ATM (1 << 0)


/*****************************************************************************
 * EMIF Interrupt Mask Set Register (EIMSR)
 ****************************************************************************/


/* Wait Rise Mask Set. This bit determines whether or not the wait rise 
   Interrupt is enabled. Writing a 1 to this bit sets this bit, sets the WRMCLR 
   bit in the EMIF interrupt mask clear register (EIMCR), and enables the wait 
   rise interrupt. To clear this bit, a 1 must be written to the WRMCLR bit in EIMCR.
   0 Indicates that the wait rise interrupt is disabled. 
   Writing a 0 has no effect.
   1 Indicates that the wait rise interrupt is enabled. Writing a 1 sets this 
   bit and the WRMCLR bit in the EMIF interrupt mask clear register (EIMCR). */
#define EIMSR_WRMSET (1 << 2)


/* Asynchronous Timeout Mask Set. This bit determines whether or not the 
   asynchronous timeout interrupt is enabled. Writing a 1 to this bit sets this 
   bit, sets the ATMCLR bit in the EMIF interrupt mask clear register (EIMCR), 
   and enables the asynchronous timeout interrupt. To clear this bit, a 1 must be 
   written to the ATMCLR bit in EIMCR.
   0 Indicates that the asynchronous timeout interrupt is disabled. 
   Writing a 0 has no effect.
   1 Indicates that the asynchronous timeout interrupt is enabled. Writing a 1 
   sets this bit and the ATMCLR bit in the EMIF interrupt mask clear 
   register (EIMCR). */
#define EIMSR_ATMSET (1 << 0)


/*****************************************************************************
 * EMIF Interrupt Mask Clear Register (EIMCR)
 ****************************************************************************/


/* Wait Rise Mask Clear. This bit determines whether or not the wait rise 
   interrupt is enabled. Writing a 1 to this bit clears this bit, clears the 
   WRMSET bit in the EMIF interrupt mask set register (EIMSR), and disables 
   the wait rise interrupt. To set this bit, a 1 must be written to the 
   WRMSET bit in EIMSR.
   0 Indicates that the wait rise interrupt is disabled. 
   Writing a 0 has no effect.
   1 Indicates that the wait rise interrupt is enabled. Writing a 1 clears 
   this bit and the WRMSET bit in the EMIF interrupt mask set register (EIMSR). */
#define EIMCR_WRMCLR (1 << 2)

/* Asynchronous Timeout Mask Clear. This bit determines whether or not the 
   asynchronous timeout interrupt is enabled. Writing a 1 to this bit clears 
   this bit, clears the ATMSET bit in the EMIF interrupt mask set register 
   (EIMSR), and disables the asynchronous timeout interrupt. To set this bit, 
   a 1 must be written to the ATMSET bit in EIMSR.
   0 Indicates that the asynchronous timeout interrupt is disabled. 
   Writing a 0 has no effect.
   1 Indicates that the asynchronous timeout interrupt is enabled. Writing a 1
   clears this bit and the ATMSET bit in the EMIF interrupt 
   mask set register (EIMSR). */
#define EIMCR_ATMCLR (1 << 0)



/*****************************************************************************
 * OneNAND Flash Control Register (ONENANDCTL)
 ****************************************************************************/

/* Synchronous Mode Read Latency. Set to the required read latency value. 
   For example, if OneNAND needs to operate with read latency of 6, 
   program this field to 0x6. */
#define ONENANDCTL_RD_LATENCY ((0x7) << 8)

/* CS3 One NAND Read Mode. Set to 1 if using OneNAND Flash synchronous 
   read mode on CS3. 
   0 Async mode OneNAND operation
   1 Sync mode OneNAND operation */
#define ONENANDCTL_CS3ONENANDRDMOD  (1 << 5)


/* CS2 One NAND Read Mode. Set to 1 if using OneNAND Flash synchronous 
   read mode on CS2. 
   0 Async mode OneNAND operation
   1 Sync mode OneNAND operation */
#define ONENANDCTL_CS2ONENANDRDMOD  (1 << 4)


/* CS3 used for ONENAND mode operation. Set to 1 if using OneNAND Flash on CS3. 
   0 Do not enable OneNAND mode on chip select
   1 Enable OneNAND mode on chip select */
#define ONENANDCTL_CS3ONENANDSEL  (1 << 1)


/* CS2 used for ONENAND mode operation. Set to 1 if using OneNAND Flash on CS2.
   0 Do not enable OneNAND mode on chip select
   1 Enable OneNAND mode on chip select */
#define ONENANDCTL_CS2ONENANDSEL  (1 << 0)


/*****************************************************************************
 * NAND Flash Control Register (NANDFCR)
 ****************************************************************************/



/* NAND Flash 4-bit ECC address and error value calculation Start. Set to 1 to 
   start 4_bit ECC error address and error value calculation on read syndrome. 
   This bit is cleared when any of the NAND Flash Error Address registers or 
   NAND Flash Error Value registers are read.
   1 Begins 4-bit ECC Address and Value Calculation */
#define NANDFCR_4BITECC_ADD_CALC_START (1 << 13)

/* Nand Flash 4-bit ECC start for the selected chip select. Set to 1 to start 
   4_bit ECC calculation on data for NAND Flash on chip select selected by bit 
   4BITECCSEL. This bit is cleared when ay of the NAND Flash 4_bit 
   ECC registers are read.
   1 Begins 4-bit ECC Calculation */
#define NANDFCR_4BITECC_START (1 << 12)

/* NAND Flash 1_bit ECC start for chip select 3. Set to 1 to start 1_bit ECC 
   calculation on data for NAND Flash for this chip select. This bit is cleared 
   when CS3 1_bit EEC register is read.
   0 Do not start ECC calculation on chip select
   1 Start 1_bit ECC calculation on chip select */
#define NANDFCR_CS3ECC (1 << 9)

/* NAND Flash ECC start for chip select 2. Set to 1 to start 1_bit ECC 
   calculation on data for NAND Flash for this chip select. This bit is cleared 
   when CS2 1_bit EEC register is read.
   0 Do not start ECC calculation on chip select
   1 Start ECC calculation on chip select */
#define NANDFCR_CS2ECC (1 << 8)

/* 4Bit ECC CS selection. This field selects the chip select on which the 
   4_bit ECC will be calculated.
   0 Select the CS2 for the 4bit ECC calculation
   1 Select the CS3 for the 4bit ECC calculation */
#define NANDFCR_4BITECCSEL (0x3 << 4)
#define NANDFCR_4BITECCSEL_CS2 (0 << 4)
#define NANDFCR_4BITECCSEL_CS3 (1 << 4)

/* NAND Flash mode for chip select 3. 
   0 Do not enable NAND Flash on chip select
   1 Enable NAND Flash on chip select */
#define NANDFCR_CS3NAND (1 << 1)

/* NAND Flash mode for chip select 2. 
   0 Do not enable NAND Flash on chip select
   1 Enable NAND Flash on chip select */
#define NANDFCR_CS2NAND (1 << 0)


/*****************************************************************************
 * NAND Flash Status Register (NANDFSR)
 ****************************************************************************/


/* Number of Errors found after the 4-Bit ECC Error Address and Error 
   Value Calculation is done
   0 - 1 error found
   1 - 2 errors found
   2 - 3 errors found
   3 - 4 errors found */
#define NANDFSR_ECC_ERRNUM_MASK (0x3 << 16)
#define NANDFSR_ECC_ERRNUM(FSR) (((FSR) >> 16 ) & 0x3)
#define NANDFSR_ECC_ERR_1  (0 << 16)
#define NANDFSR_ECC_ERR_2  (1 << 16)
#define NANDFSR_ECC_ERR_3  (2 << 16)
#define NANDFSR_ECC_ERR_4  (3 << 16)

/* ECC correction state while performing 4-bit ECC Address and Error 
   Value Calculation 
   0h No errors detected
   1h Errors cannot be corrected (5 or more)
   2h Error correction complete
   3h Error correction complete
   4h Reserved
   5h Calculating number of errors
   6h - 7h Preparing for error search
   8h Searching for errors
   9h - Bh Reserved
   Ch - Fh Calculating error value */
#define NANDFSR_ECC_STATE_MASK (0xf << 8)
#define NANDFSR_ECC_NO_ERR (0 << 8)
#define NANDFSR_ECC_TOO_MANY_ERRS (1 << 8)
#define NANDFSR_ECC_COMPLETE1 (2 << 8)
#define NANDFSR_ECC_COMPLETE2 (3 << 8)
#define NANDFSR_ECC_COMPLETE (3 << 8)


/* Raw status of the EM_WAIT input pin. Note that the WP0 bit in 
   the asynchronous wait cycle configuration register (AWCCR) has 
   no effect on WAITST. */
#define NANDFSR_WAITST (0xf << 0)

#define NANDFSR_READY (1 << 0)

/*****************************************************************************
 * NAND Flash n ECC Registers (NANDF1ECC-NANDF2ECC)
 ****************************************************************************/

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P2048O (1 << 27)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P1024O (1 << 26)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P512O (1 << 25)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P256O (1 << 24)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P128O (1 << 23)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P64O (1 << 22)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P32O (1 << 21)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P16O (1 << 20)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P8O (1 << 19)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P4O (1 << 18)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P2O (1 << 17)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P1O (1 << 16)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P2948E (1 << 11)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P102E (1 << 10)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P512E (1 << 9)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P256E (1 << 8)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P128E (1 << 7)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P64E (1 << 6)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P32E (1 << 5)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P15E (1 << 4)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P8E (1 << 3)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P4E (1 << 2)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P2E (1 << 1)

/* ECC code calculated while reading/writing NAND Flash. */
#define NANDECC_P1E (1 << 0)


/*****************************************************************************
 * NAND Flash 4-bit ECC LOAD Register (NAND4BITECCLOAD)
 ****************************************************************************/

/* 0-3FFh 4-bit ECC load. This register is used to load the ECC values when 
   performing the Syndrome calculation during reads */
#define NAND4BITECCLOAD_4BITECCLOAD ((0x3ff) << 0)


/*****************************************************************************
 * NAND Flash 4-bit ECC Register 1 (NAND4BITECC1)
 ****************************************************************************/

/* Calculated 4-bit ECC or Syndrom Value2 */
#define NAND4BITECC1_4BITECCVAL2 ((0x3ff) << 16)

/* Calculated 4-bit ECC or Syndrom Value1 */
#define NAND4BITECC1_4BITECCVAL1 ((0x3ff) << 0)


/*****************************************************************************
 * NAND Flash 4-bit ECC Register 2 (NAND4BITECC2)
 ****************************************************************************/

/* Calculated 4-bit ECC or Syndrom Value4 */
#define NAND4BITECC2_4BITECCVAL4 ((0x3ff) << 16)

/* Calculated 4-bit ECC or Syndrom Value3 */
#define NAND4BITECC2_4BITECCVAL3 ((0x3ff) << 0)


/*****************************************************************************
 * NAND Flash 4-bit ECC Register 3 (NAND4BITECC3)
 ****************************************************************************/

/* Calculated 4-bit ECC or Syndrom Value6 */
#define NAND4BITECC3_4BITECCVAL6 ((0x3ff) << 16)

/* Calculated 4-bit ECC or Syndrom Value5 */
#define NAND4BITECC3_4BITECCVAL5 ((0x3ff) << 0)


/*****************************************************************************
 * NAND Flash 4-bit ECC Register 4 (NAND4BITECC4)
 ****************************************************************************/

/* Calculated 4-bit ECC or Syndrom Value8 */
#define NAND4BITECC4_4BITECCVAL8 ((0x3ff) << 16)

/* Calculated 4-bit ECC or Syndrom Value7 */
#define NAND4BITECC4_4BITECCVAL7 ((0x3ff) << 0)


/*****************************************************************************
 * NAND Flash 4-bit ECC Error Address Register 1 (NANDERRADD1)
 ****************************************************************************/



/* Calculated 4-bit ECC Error Address 2 */
#define NANDERRADD1_4BITECCERRADD2 ((0x3ff) << 16)

/* Calculated 4-bit ECC Error Address 1 */
#define NANDERRADD1_4BITECCERRADD1 ((0x3ff) << 0)


/*****************************************************************************
 * NAND Flash 4-bit ECC Error Address Register 2 (NANDERRADD2)
 ****************************************************************************/

/* Calculated 4-bit ECC Error Address 4 */
#define NANDERRADD2_4BITECCERRADD4 ((0x3ff) << 16)

/* Calculated 4-bit ECC Error Address 3 */
#define NANDERRADD2_4BITECCERRADD3 ((0x3ff) << 0)


/*****************************************************************************
 * NAND Flash 4-bit ECC Error Value Register 1 (NANDERRVAL1)
 ****************************************************************************/

/* Calculated 4-bit ECC Error Value 2 */
#define NANDERRVAL1_4BITECCERRVAL2 ((0x3ff) << 16)

/* Calculated 4-bit ECC Error Value 1 */
#define NANDERRVAL1_4BITECCERRVAL1 ((0x3ff) << 0)


/*****************************************************************************
 * NAND Flash 4-bit ECC Error Value Register 2 (NANDERRVAL2)
 ****************************************************************************/

/* Calculated 4-bit ECC Error Value 4 */
#define NANDERRVAL2_4BITECCERRVAL4 ((0x3ff) << 16)

/* Calculated 4-bit ECC Error Value 3 */
#define NANDERRVAL2_4BITECCERRVAL3 ((0x3ff) << 0)



#define EMIF_NUMBER_CE_REGION 2
#define EMIF_FIRST_CE_START_ADDR 0x02000000
#define EMIF_INTER_CE_REGION_SIZE 0x04000000

#define EMIF_NANDFCR_4BITECC_SEL_MASK 0x00000030
#define EMIF_NANDFCR_4BITECC_SEL_SHIFT 4

#define EMIF_AWCC_WAITSTATE_MASK 0x000000ff

#define EMIF_NANFCR_4BITECC_START_MASK 0x00001000
#define EMIF_NANFCR_4BITECC_START_SHIFT 12
#define EMIF_NANFCR_4BITECC_ADD_CALC_START_MASK 0x00002000
#define EMIF_NANFCR_4BITECC_ADD_CALC_START_SHIFT 13

#define EMIF_NANDFSR_ECC_STATE_MASK 0x00000f00
#define EMIF_NANDFSR_ECC_STATE_SHIFT 8
#define EMIF_NANDFSR_ECC_ERRNUM_MASK 0x00030000
#define EMIF_NANDFSR_ECC_ERRNUM_SHIFT 16



#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_aemif {
	uint32_t pid;
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_AEMIF_H__ */

