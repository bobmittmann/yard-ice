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
 * @file stm32f-adc.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_LDTC_H__
#define __STM32F_LDTC_H__

/*-------------------------------------------------------------------------
 * LCD-TFT Controller (LTDC)
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * LTDC Synchronization Size Configuration Register (LTDC_SSCR) */
#define LTDC_SSCR 0x08
/* This register defines the number of Horizontal Synchronization 
   pixels minus 1 and the number of Vertical Synchronization lines minus 1. 
   Refer to Figure 82 and Section 16.4: 
   LTDC programmable parameters for an example of configuration. */


/* [31..28] Reserved, must be kept at reset value */


/* Bits [27..16] - Horizontal Synchronization Width 
   (in units of pixel clock period) */
#define HSW_MSK (((1 << (11 + 1)) - 1) << 16)
#define HSW_SET(VAL) (((VAL) << 16) & HSW_MSK)
#define HSW_GET(REG) (((REG) & HSW_MSK) >> 16)
/* These bits define the number of Horizontal Synchronization pixel minus 1. */

/* [15..11] Reserved, must be kept at reset value */


/* Bits [10..0] - Vertical Synchronization Height 
   (in units of horizontal scan line) */
#define VSH_MSK (((1 << (10 + 1)) - 1) << 0)
#define VSH_SET(VAL) (((VAL) << 0) & VSH_MSK)
#define VSH_GET(REG) (((REG) & VSH_MSK) >> 0)
/* These bits define the vertical Synchronization height minus 1. 
   It represents the number of horizontal synchronization lines. */

/* LTDC Back Porch Configuration Register (LTDC_BPCR)
   This register defines the accumulated number of Horizontal 
   Synchronization and back porch pixels minus 1 (HSYNC Width + HBP- 1) and 
   the accumulated number of Vertical Synchronization and back porch 
   lines minus 1 (VSYNC Height + VBP - 1). Refer to Figure 82 and 
   Section 16.4: LTDC programmable parameters for an example of configuration.
   Address offset: 0x0C */


/* [31..28] Reserved, must be kept at reset value */


/* Bits [27..16] - Accumulated Horizontal back porch (in units of pixel clock period) */
#define AHBP_MSK (((1 << (11 + 1)) - 1) << 16)
#define AHBP_SET(VAL) (((VAL) << 16) & AHBP_MSK)
#define AHBP_GET(REG) (((REG) & AHBP_MSK) >> 16)
/* 
These bits define the Accumulated Horizontal back porch width which includes the
Horizontal Synchronization and Horizontal back porch pixels minus 1.
The Horizontal back porch is the period between Horizontal Synchronization going
inactive and the start of the active display part of the next scan line. */

/* [15..11] Reserved, must be kept at reset value */


/* Bits [10..0] - Accumulated Vertical back porch (in units of horizontal scan line) */
#define AVBP_MSK (((1 << (10 + 1)) - 1) << 0)
#define AVBP_SET(VAL) (((VAL) << 0) & AVBP_MSK)
#define AVBP_GET(REG) (((REG) & AVBP_MSK) >> 0)
/* 
These bits define the accumulated Vertical back porch width which includes the Vertical
Synchronization and Vertical back porch lines minus 1.
The Vertical back porch is the number of horizontal scan lines at a start of frame to the
start of the first active scan line of the next frame. */

/* LTDC Active Width Configuration Register (LTDC_AWCR)
This register defines the accumulated number of Horizontal Synchronization, back porch
and Active pixels minus 1 (HSYNC width + HBP + Active Width - 1) and the accumulated
number of Vertical Synchronization, back porch lines and Active lines minus 1 (VSYNC
Height+ BVBP + Active Height - 1). Refer to Figure 82 and Section 16.4: LTDC
programmable parameters for an example of configuration.
Address offset: 0x10 */


/* [31..28] Reserved, must be kept at reset value */


/* Bits [27..16] - Accumulated Active Width (in units of pixel clock period) */
#define AAW_MSK (((1 << (11 + 1)) - 1) << 16)
#define AAW_SET(VAL) (((VAL) << 16) & AAW_MSK)
#define AAW_GET(REG) (((REG) & AAW_MSK) >> 16)
/* 
These bits define the Accumulated Active Width which includes the Horizontal
Synchronization, Horizontal back porch and Active pixels minus 1.
The Active Width is the number of pixels in active display area of the panel scan line. The maximum Active Width supported is 0x400. */

/* [15..11] Reserved, must be kept at reset value */


/* Bits [10..0] - Accumulated Active Height (in units of horizontal scan line) */
#define AAH_MSK (((1 << (10 + 1)) - 1) << 0)
#define AAH_SET(VAL) (((VAL) << 0) & AAH_MSK)
#define AAH_GET(REG) (((REG) & AAH_MSK) >> 0)
/* These bits define the Accumulated Height which includes the Vertical 
   Synchronization, Vertical back porch and the Active Height lines minus 1. 
   The Active Height is the number of active lines in the panel. 
   The maximum Active Height supported is 0x300. */

/* LTDC Total Width Configuration Register (LTDC_TWCR)
   This register defines the accumulated number of Horizontal Synchronization, 
   back porch, Active and front porch pixels minus 1 (HSYNC Width + HBP + 
   Active Width + HFP - 1) and the accumulated number of Vertical 
   Synchronization, back porch lines, Active and Front lines minus 1 
   (VSYNC Height+ BVBP + Active Height + VFP - 1). 
   Refer to Figure 82 and LTDC programmable parameters for an example 
   of configuration.

Address offset: 0x14 */


/* [31..28] Reserved, must be kept at reset value */


/* Bits [27..16] - Total Width (in units of pixel clock period) */
#define TOTALW_MSK (((1 << (11 + 1)) - 1) << 16)
#define TOTALW_SET(VAL) (((VAL) << 16) & TOTALW_MSK)
#define TOTALW_GET(REG) (((REG) & TOTALW_MSK) >> 16)
/* 
These bits defines the accumulated Total Width which includes the Horizontal
Synchronization, Horizontal back porch, Active Width and Horizontal front porch pixels
minus 1. */

/* [15..11] Reserved, must be kept at reset value */


/* Bits [10..0] - Total Height (in units of horizontal scan line) */
#define TOTALH_MSK (((1 << (10 + 1)) - 1) << 0)
#define TOTALH_SET(VAL) (((VAL) << 0) & TOTALH_MSK)
#define TOTALH_GET(REG) (((REG) & TOTALH_MSK) >> 0)
/* 
These bits defines the accumulated Height which includes the Vertical Synchronization,
Vertical back porch, the Active Height and Vertical front porch Height lines minus 1. */

/* LTDC Global Control Register (LTDC_GCR)
This register defines the global configuration of the LCD-TFT controller.
Address offset: 0x18 */


/* Bit 31 - Horizontal Synchronization Polarity */
#define HSPOL (1 << 31)
/* 
This bit is set and cleared by software.
0: Horizontal Synchronization polarity is active low
1: Horizontal Synchronization polarity is active high */

/* Bit 30 - Vertical Synchronization Polarity */
#define VSPOL (1 << 30)
/* 
This bit is set and cleared by software.
0: Vertical Synchronization is active low
1: Vertical Synchronization is active high */

/* Bit 29 - Data Enable Polarity */
#define DEPOL (1 << 29)
/* 
This bit is set and cleared by software.
0: Data Enable polarity is active low
1: Data Enable polarity is active high */

/* Bit 28 - Pixel Clock Polarity */
#define PCPOL (1 << 28)
/* 
This bit is set and cleared by software.
0: input pixel clock
1: inverted input pixel clock */

/* [27..17] Reserved, must be kept at reset value */


/* Bit 16 - Dither Enable */
#define DEN (1 << 16)
/* 
This bit is set and cleared by software.
0: Dither disable
1: Dither enable */

/* Bit 15 Reserved, must be kept at reset value */


/* Bits [14..12] - Dither Red Width */
#define DRW_MSK (((1 << (2 + 1)) - 1) << 12)
#define DRW_SET(VAL) (((VAL) << 12) & DRW_MSK)
#define DRW_GET(REG) (((REG) & DRW_MSK) >> 12)
/* 
These bits return the Dither Red Bits */

/* Bit 11 Reserved, must be kept at reset value */


/* Bits [10..8] - Dither Green Width */
#define DGW_MSK (((1 << (2 + 1)) - 1) << 8)
#define DGW_SET(VAL) (((VAL) << 8) & DGW_MSK)
#define DGW_GET(REG) (((REG) & DGW_MSK) >> 8)
/* 
These bits return the Dither Green Bits */

/* Bit 7 Reserved, must be kept at reset value */


/* Bits [6..4] - Dither Blue Width */
#define DBW_MSK (((1 << (2 + 1)) - 1) << 4)
#define DBW_SET(VAL) (((VAL) << 4) & DBW_MSK)
#define DBW_GET(REG) (((REG) & DBW_MSK) >> 4)
/* 
These bits return the Dither Blue Bits */

/* [3..1] Reserved, must be kept at reset value */


/* Bit 0 - LCD-TFT controller enable bit */
#define LTDCEN (1 << 0)
/* 
This bit is set and cleared by software.
0: LTDC disable
1: LTDC enable */

/* LTDC Shadow Reload Configuration Register (LTDC_SRCR)
This register allows to reload either immediately or during the vertical blanking period, the
shadow registers values to the active registers. The shadow registers are all Layer1 and
Layer2 registers except the LTDC_L1CLUTWR and the LTDC_L2CLUTWR.
Address offset: 0x24 */


/* [31..2] Reserved, must be kept at reset value */


/* Bit 1 - Vertical Blanking Reload */
#define VBR (1 << 1)
/* 
This bit is set by software and cleared only by hardware after reload. (it cannot
be cleared through register write once it is set)
0: No effect
1: The shadow registers are reloaded during the vertical blanking period (at the
beginning of the first line after the Active Display Area) */

/* Bit 0 - Immediate Reload */
#define IMR (1 << 0)
/* 
This bit is set by software and cleared only by hardware after reload.
0: No effect
1: The shadow registers are reloaded immediately
Note: The shadow registers read back the active values. Until the reload has been done, the 'old'
value will be read.
16.7.7 LTDC Background Color Configuration Register (LTDC_BCCR) */

/* This register defines the background color */
#define RGB888 0x2C



/* [31..24] Reserved, must be kept at reset value */


/* Bits [23..16] - Background Color Red value */
#define BCRED_MSK (((1 << (7 + 1)) - 1) << 16)
#define BCRED_SET(VAL) (((VAL) << 16) & BCRED_MSK)
#define BCRED_GET(REG) (((REG) & BCRED_MSK) >> 16)
/* 
These bits configure the background red value */

/* Bits [15..8] - Background Color Green value */
#define BCGREEN_MSK (((1 << (7 + 1)) - 1) << 8)
#define BCGREEN_SET(VAL) (((VAL) << 8) & BCGREEN_MSK)
#define BCGREEN_GET(REG) (((REG) & BCGREEN_MSK) >> 8)
/* 
These bits configure the background green value */

/* Bits [7..0] - Background Color Blue value */
#define BCBLUE_MSK (((1 << (7 + 1)) - 1) << 0)
#define BCBLUE_SET(VAL) (((VAL) << 0) & BCBLUE_MSK)
#define BCBLUE_GET(REG) (((REG) & BCBLUE_MSK) >> 0)
/* 
These bits configure the background blue value */

/* LTDC Interrupt Enable Register (LTDC_IER)
This register determines which status flags generate an interrupt request by setting the
corresponding bit to 1.
Address offset: 0x34 */


/* [31..4] Reserved, must be kept at reset value */


/* Bit 3 - Register Reload interrupt enable */
#define RRIE (1 << 3)
/* 
This bit is set and cleared by software
0: Register Reload interrupt disable
1: Register Reload interrupt enable */

/* Bit 2 - Transfer Error Interrupt Enable */
#define TERRIE (1 << 2)
/* 
This bit is set and cleared by software
0: Transfer Error interrupt disable
1: Transfer Error interrupt enable */

/* Bit 1 - FIFO Underrun Interrupt Enable */
#define FUIE (1 << 1)
/* 
This bit is set and cleared by software
0: FIFO Underrun interrupt disable
1: FIFO Underrun Interrupt enable */

/* Bit 0 - Line Interrupt Enable */
#define LIE (1 << 0)
/* 
This bit is set and cleared by software
0: Line interrupt disable
1: Line Interrupt enable */

/* LTDC Interrupt Status Register (LTDC_ISR)
This register returns the interrupt status flag
Address offset: 0x38 */


/* [31..24] Reserved, must be kept at reset value */


/* Bit 3 - Register Reload Interrupt Flag */
#define RRIF (1 << 3)
/* 
0: No Register Reload interrupt generated
1: Register Reload interrupt generated when a vertical blanking reload occurs (and the
first line after the active area is reached) */

/* Bit 2 - Transfer Error interrupt flag */
#define TERRIF (1 << 2)
/* 
0: No Transfer Error interrupt generated
1: Transfer Error interrupt generated when a Bus error occurs */

/* Bit 1 - FIFO Underrun Interrupt flag */
#define FUIF (1 << 1)
/* 
0: NO FIFO Underrun interrupt generated.
1: A FIFO underrun interrupt is generated, if one of the layer FIFOs is empty and pixel
data is read from the FIFO */

/* Bit 0 - Line Interrupt flag */
#define LIF (1 << 0)
/* 
0: No Line interrupt generated
1: A Line interrupt is generated, when a programmed line is reached */

/* LTDC Interrupt Clear Register (LTDC_ICR)
Address offset: 0x3C */

/* [31..24] Reserved, must be kept at reset value */


/* Bit 3 - Clears Register Reload Interrupt Flag */
#define CRRIF (1 << 3)
/* 
0: No effect
1: Clears the RRIF flag in the LTDC_ISR register */

/* Bit 2 - Clears the Transfer Error Interrupt Flag */
#define CTERRIF (1 << 2)
/* 
0: No effect
1: Clears the TERRIF flag in the LTDC_ISR register. */

/* Bit 1 - Clears the FIFO Underrun Interrupt flag */
#define CFUIF (1 << 1)
/* 
0: No effect
1: Clears the FUDERRIF flag in the LTDC_ISR register. */

/* Bit 0 - Clears the Line Interrupt Flag */
#define CLIF (1 << 0)
/* 
0: No effect
1: Clears the LIF flag in the LTDC_ISR register. */

/* LTDC Line Interrupt Position Configuration Register (LTDC_LIPCR)
   This register defines the position of the line interrupt. The 
   line value to be programmed depends on the timings parameters. 
   Refer to Figure 82.
Address offset: 0x40 */


/* [31..11] Reserved, must be kept at reset value */


/* Bits [10..0] - Line Interrupt Position */
#define LIPOS_MSK (((1 << (10 + 1)) - 1) << 0)
#define LIPOS_SET(VAL) (((VAL) << 0) & LIPOS_MSK)
#define LIPOS_GET(REG) (((REG) & LIPOS_MSK) >> 0)
/* These bits configure the line interrupt position */

/* LTDC Current Position Status Register (LTDC_CPSR)
Address offset: 0x44 */

/* Bits 31:16: CXPOS[15:0]: Current X Position
These bits return the current X position */

/* Bits [15..0] - Current Y Position */
#define CYPOS_MSK (((1 << (15 + 1)) - 1) << 0)
#define CYPOS_SET(VAL) (((VAL) << 0) & CYPOS_MSK)
#define CYPOS_GET(REG) (((REG) & CYPOS_MSK) >> 0)
/* These bits return the current Y position */

/* LTDC Current Display Status Register (LTDC_CDSR)
This register returns the status of the current display phase which is controlled by the
HSYNC, VSYNC, and Horizontal/Vertical DE signals.
Example: if the current display phase is the vertical synchronization, the VSYNCS bit is set
(active high). If the current display phase is the horizontal synchronization, the HSYNCS bit
is active high.
Address offset: 0x48 */


/* [31..24] Reserved, must be kept at reset value */


/* Bit 3 - Horizontal Synchronization display Status */
#define HSYNCS (1 << 3)
/* 
0: Active low
1: Active high */

/* Bit 2 - Vertical Synchronization display Status */
#define VSYNCS (1 << 2)
/* 
0: Active low
1: Active high */

/* Bit 1 - Horizontal Data Enable display Status */
#define HDES (1 << 1)
/* 
0: Active low
1: Active high */

/* Bit 0 - Vertical Data Enable display Status */
#define VDES (1 << 0)
/* 
0: Active low
1: Active high
Note:
The returned status does not depend on the configured polarity in the LTDC_GCR register,
instead it returns the current active display phase. */

/* LTDC Layerx Control Register (LTDC_LxCR) (where x=1..2)
Address offset: 0x84 + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* [31..5] Reserved, must be kept at reset value */


/* Bit 4 - Color Look-Up Table Enable */
#define CLUTEN (1 << 4)
/* 
This bit is set and cleared by software.
0: Color Look-Up Table disable
1: Color Look-Up Table enable
The CLUT is only meaningful for L8, AL44 and AL88 pixel format. Refer to Color Look-Up
Table (CLUT) on page 485

/* Bit 3 Reserved, must be kept at reset value */


/* Bit 2 Reserved, must be kept at reset value */


/* Bit 1 - Color Keying Enable */
#define COLKEN (1 << 1)
/* 
This bit is set and cleared by software.
0: Color Keying disable
1: Color Keying enable */

/* Bit 0 - Layer Enable */
#define LEN (1 << 0)
/* 
This bit is set and cleared by software.
0: Layer disable
1: Layer enable */
16.7.15
LTDC Layerx Window Horizontal Position Configuration Register
(LTDC_LxWHPCR) (where x=1..2)
This register defines the Horizontal Position (first and last pixel) of the layer 1 or 2 window.
The first visible pixel of a line is the programmed value of AHBP[10:0] bits + 1 in the
LTDC_BPCR register.
The last visible pixel of a line is the programmed value of AAW[10:0] bits in the
LTDC_AWCR register. All values within this range are allowed.
Address offset: 0x88 + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* [31..28] Reserved, must be kept at reset value */


/* Bits [27..16] - Window Horizontal Stop Position */
#define WHSPPOS_MSK (((1 << (11 + 1)) - 1) << 16)
#define WHSPPOS_SET(VAL) (((VAL) << 16) & WHSPPOS_MSK)
#define WHSPPOS_GET(REG) (((REG) & WHSPPOS_MSK) >> 16)
/* 
These bits configure the last visible pixel of a line of the layer window. */

/* [15..12] Reserved, must be kept at reset value */


/* Bits [11..0] - Window Horizontal Start Position */
#define WHSTPOS_MSK (((1 << (11 + 1)) - 1) << 0)
#define WHSTPOS_SET(VAL) (((VAL) << 0) & WHSTPOS_MSK)
#define WHSTPOS_GET(REG) (((REG) & WHSTPOS_MSK) >> 0)
/* 
These bits configure the first visible pixel of a line of the layer window.
Example:
The LTDC_BPCR register is configured to 0x000E0005(AHBP[11:0] is 0xE) and the
LTDC_AWCR register is configured to 0x028E01E5(AAW[11:0] is 0x28E). To configure the
horizontal position of a window size of 630x460, with horizontal start offset of 5 pixels in the
Active data area.
16.7.16
1. Layer window first pixel: WHSTPOS[11:0] should be programmed to 0x14 (0xE+1+0x5)
2. Layer window last pixel: WHSPPOS[11:0] should be programmed to 0x28A
LTDC Layerx Window Vertical Position Configuration Register
(LTDC_LxWVPCR) (where x=1..2)
This register defines the vertical position (first and last line) of the layer1 or 2 window.
The first visible line of a frame is the programmed value of AVBP[10:0] bits + 1 in the
register LTDC_BPCR register.
The last visible line of a frame is the programmed value of AAH[10:0] bits in the
LTDC_AWCR register. All values within this range are allowed.
Address offset: 0x8C + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* [31..27] Reserved, must be kept at reset value */


/* Bits [26..16] - Window Vertical Stop Position */
#define WVSPPOS_MSK (((1 << (10 + 1)) - 1) << 16)
#define WVSPPOS_SET(VAL) (((VAL) << 16) & WVSPPOS_MSK)
#define WVSPPOS_GET(REG) (((REG) & WVSPPOS_MSK) >> 16)
/* 
These bits configures the last visible line of the layer window. */

/* [15..11] Reserved, must be kept at reset value */


/* Bits [10..0] - Window Vertical Start Position */
#define WVSTPOS_MSK (((1 << (10 + 1)) - 1) << 0)
#define WVSTPOS_SET(VAL) (((VAL) << 0) & WVSTPOS_MSK)
#define WVSTPOS_GET(REG) (((REG) & WVSTPOS_MSK) >> 0)
/* 
These bits configure the first visible line of the layer window.
Example:
The LTDC_BPCR register is configured to 0x000E0005 (AVBP[10:0] is 0x5) and the
LTDC_AWCR register is configured to 0x028E01E5 (AAH[10:0] is 0x1E5). To configure the
vertical position of a window size of 630x460, with vertical start offset of 8 lines in the Active
data area:
16.7.17
1. Layer window first line: WVSTPOS[10:0] should be programmed to 0xE (0x5 + 1 + 0x8)
2. Layer window last line: WVSPPOS[10:0] should be programmed to 0x1DA
LTDC Layerx Color Keying Configuration Register (LTDC_LxCKCR)
(where x=1..2) */

/* This register defines the color key value */
#define RGB 0x90



/* [31..24] Reserved, must be kept at reset value */


/* Bits [23..16] - Color Key Red value */
#define CKRED_MSK (((1 << (7 + 1)) - 1) << 16)
#define CKRED_SET(VAL) (((VAL) << 16) & CKRED_MSK)
#define CKRED_GET(REG) (((REG) & CKRED_MSK) >> 16)

/* Bits [15..8] - Color Key Green value */
#define CKGREEN_MSK (((1 << (7 + 1)) - 1) << 8)
#define CKGREEN_SET(VAL) (((VAL) << 8) & CKGREEN_MSK)
#define CKGREEN_GET(REG) (((REG) & CKGREEN_MSK) >> 8)

/* Bits [7..0] - Color Key Blue value */
#define CKBLUE_MSK (((1 << (7 + 1)) - 1) << 0)
#define CKBLUE_SET(VAL) (((VAL) << 0) & CKBLUE_MSK)
#define CKBLUE_GET(REG) (((REG) & CKBLUE_MSK) >> 0)

/* 
LTDC Layerx Pixel Format Configuration Register (LTDC_LxPFCR)
(where x=1..2)
This register defines the pixel format which is used for the stored data in the frame buffer of
a layer. The pixel data is read from the frame buffer and then transformed to the internal
format 8888 (ARGB).
Address offset: 0x94 + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* [31..3] Reserved, must be kept at reset value */


/* Bits [2..0] - Pixel Format */
#define PF_MSK (((1 << (2 + 1)) - 1) << 0)
#define PF_SET(VAL) (((VAL) << 0) & PF_MSK)
#define PF_GET(REG) (((REG) & PF_MSK) >> 0)
/* 
These bits configures the Pixel format
000: ARGB8888
001: RGB888
010: RGB565
011: ARGB1555
100: ARGB4444
101: L8 (8-Bit Luminance)
110: AL44 (4-Bit Alpha, 4-Bit Luminance)
111: AL88 (8-Bit Alpha, 8-Bit Luminance) */

/* LTDC Layerx Constant Alpha Configuration Register (LTDC_LxCACR)
(where x=1..2)
This register defines the constant alpha value (divided by 255 by Hardware), which is used
in the alpha blending. Refer to LTDC_LxBFCR register.
Address offset: 0x98 + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* [31..8] Reserved, must be kept at reset value */


/* Bits [7..0] - Constant Alpha */
#define CONSTA_MSK (((1 << (7 + 1)) - 1) << 0)
#define CONSTA_SET(VAL) (((VAL) << 0) & CONSTA_MSK)
#define CONSTA_GET(REG) (((REG) & CONSTA_MSK) >> 0)
/* These bits configure the Constant Alpha used for blending. The 
   Constant Alpha is divided by 255 by hardware.
   Example: if the programmed Constant Alpha is 0xFF, the Constant 
   Alpha value is 255/255=1 */

/* LTDC Layerx Default Color Configuration Register (LTDC_LxDCCR)
(where x=1..2)
This register defines the default color of a layer in the format ARGB. The default color is
used outside the defined layer window or when a layer is disabled. The reset value of
0x00000000 defines a transparent black color.
Address offset: 0x9C + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* Bits [31..24] - Default Color Alpha */
#define DCALPHA_MSK (((1 << (7 + 1)) - 1) << 24)
#define DCALPHA_SET(VAL) (((VAL) << 24) & DCALPHA_MSK)
#define DCALPHA_GET(REG) (((REG) & DCALPHA_MSK) >> 24)
/* These bits configure the default alpha value */

/* Bits [23..16] - Default Color Red */
#define DCRED_MSK (((1 << (7 + 1)) - 1) << 16)
#define DCRED_SET(VAL) (((VAL) << 16) & DCRED_MSK)
#define DCRED_GET(REG) (((REG) & DCRED_MSK) >> 16)
/* These bits configure the default red value */

/* Bits [15..8] - Default Color Green */
#define DCGREEN_MSK (((1 << (7 + 1)) - 1) << 8)
#define DCGREEN_SET(VAL) (((VAL) << 8) & DCGREEN_MSK)
#define DCGREEN_GET(REG) (((REG) & DCGREEN_MSK) >> 8)
/* These bits configure the default green value */

/* Bits [7..0] - Default Color Blue */
#define DCBLUE_MSK (((1 << (7 + 1)) - 1) << 0)
#define DCBLUE_SET(VAL) (((VAL) << 0) & DCBLUE_MSK)
#define DCBLUE_GET(REG) (((REG) & DCBLUE_MSK) >> 0)
/* These bits configure the default blue value */

/* LTDC Layerx Blending Factors Configuration Register (LTDC_LxBFCR)
(where x=1..2)
This register defines the blending factors F1 and F2.
The general blending formula is: BC = BF1 x C + BF2 x Cs
• BC = Blended color
• BF1 = Blend Factor 1
• C = Current layer color
• BF2 = Blend Factor 2
• Cs = subjacent layers blended color
Address offset: 0xA0 + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* [31..11] Reserved, must be kept at reset value */


/* Bits [10..8] - Blending Factor 1 */
#define BF1_MSK (((1 << (2 + 1)) - 1) << 8)
#define BF1_SET(VAL) (((VAL) << 8) & BF1_MSK)
#define BF1_GET(REG) (((REG) & BF1_MSK) >> 8)
/* 
These bits select the blending factor F1
000: Reserved
001: Reserved
010: Reserved
011: Reserved
100: Constant Alpha
101: Reserved
110: Pixel Alpha x Constant Alpha
111:Reserved */

/* [7..3] Reserved, must be kept at reset value */


/* Bits [2..0] - Blending Factor 2 */
#define BF2_MSK (((1 << (2 + 1)) - 1) << 0)
#define BF2_SET(VAL) (((VAL) << 0) & BF2_MSK)
#define BF2_GET(REG) (((REG) & BF2_MSK) >> 0)
/* 
These bits select the blending factor F2
000: Reserved
001: Reserved
010: Reserved
011: Reserved
100: Reserved
101: 1 - Constant Alpha
110: Reserved
111: 1 - (Pixel Alpha x Constant Alpha)
The Constant Alpha value, is the programmed value in the LxCACR register divided by 255
by hardware.
Example: Only layer1 is enabled, BF1 configured to Constant Alpha
BF2 configured to 1 - Constant Alpha
Constant Alpha: The Constant Alpha programmed in the LxCACR register is 240 (0xF0).
Thus, the Constant Alpha value is 240/255 = 0.94
C: Current Layer Color is 128
Cs: Background color is 48
Layer1 is blended with the background color.
BC = Constant Alpha x C + (1 - Constant Alpha ) x Cs = 0.94 x 128 + (1- 0.94) x 48 = 123. */

/* LTDC Layerx Color Frame Buffer Address Register (LTDC_LxCFBAR)
(where x=1..2)
This register defines the color frame buffer start address which has to point to the address
where the pixel data of the top left pixel of a layer is stored in the frame buffer.
Address offset: 0xAC + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* Bits [31..0] - Color Frame Buffer Start Address */
#define CFBADD_MSK (((1 << (31 + 1)) - 1) << 0)
#define CFBADD_SET(VAL) (((VAL) << 0) & CFBADD_MSK)
#define CFBADD_GET(REG) (((REG) & CFBADD_MSK) >> 0)
/* 
These bits defines the color frame buffer start address. */

/* LTDC Layerx Color Frame Buffer Length Register (LTDC_LxCFBLR)
(where x=1..2)
This register defines the color frame buffer line length and pitch.
Address offset: 0xB0 + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* [31..29] Reserved, must be kept at reset valuer */


/* Bits [28..16] - Color Frame Buffer Pitch in bytes */
#define CFBP_MSK (((1 << (17 + 1)) - 1) << 16)
#define CFBP_SET(VAL) (((VAL) << 16) & CFBP_MSK)
#define CFBP_GET(REG) (((REG) & CFBP_MSK) >> 16)
/* 
These bits define the pitch which is the increment from the start of one line of pixels to the
start of the next line in bytes. */

/* [15..13] Reserved, must be kept at reset value */


/* Bits [12..0] - Color Frame Buffer Line Length */
#define CFBLL_MSK (((1 << (12 + 1)) - 1) << 0)
#define CFBLL_SET(VAL) (((VAL) << 0) & CFBLL_MSK)
#define CFBLL_GET(REG) (((REG) & CFBLL_MSK) >> 0)
/* 
These bits define the length of one line of pixels in bytes + 3.
The line length is computed as follows: Active high width x number of bytes per pixel + 3.
Example:
16.7.24
• A frame buffer having the format RGB565 (2 bytes per pixel) and a width of 256 pixels
(total number of bytes per line is 256x2=512 bytes), where pitch = line length requires a
value of 0x02000203 to be written into this register.
• A frame buffer having the format RGB888 (3 bytes per pixel) and a width of 320 pixels
(total number of bytes per line is 320x3=960), where pitch = line length requires a value
of 0x03C003C3 to be written into this register. */

/* LTDC Layerx ColorFrame Buffer Line Number Register
(LTDC_LxCFBLNR) (where x=1..2)
This register defines the number of lines in the color frame buffer.
Address offset: 0xB4 + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* [31..11] Reserved, must be kept at reset value */


/* Bits [10..0] - Frame Buffer Line Number */
#define CFBLNBR_MSK (((1 << (10 + 1)) - 1) << 0)
#define CFBLNBR_SET(VAL) (((VAL) << 0) & CFBLNBR_MSK)
#define CFBLNBR_GET(REG) (((REG) & CFBLNBR_MSK) >> 0)
/* These bits define the number of lines in the frame buffer which 
   corresponds to the Active high width.
Note:
The number of lines and line length settings define how much data is fetched per frame for
every layer. If it is configured to less bytes than required, a FIFO underrun interrupt will be
generated if enabled.
The start address and pitch settings on the other hand define the correct start of every line in
memory. */

/* LTDC Layerx CLUT Write Register (LTDC_LxCLUTWR)
(where x=1..2)
This register defines the CLUT address and the RGB value.
Address offset: 0xC4 + 0x80 x (Layerx -1), Layerx = 1 or 2 */


/* Bits [31..24] - CLUT Address */
#define CLUTADD_MSK (((1 << (7 + 1)) - 1) << 24)
#define CLUTADD_SET(VAL) (((VAL) << 24) & CLUTADD_MSK)
#define CLUTADD_GET(REG) (((REG) & CLUTADD_MSK) >> 24)
/* These bits configure the CLUT address (color position within the CLUT) 
   of each RGB value */

/* Bits [23..16] - Red value */
#define RED_MSK (((1 << (7 + 1)) - 1) << 16)
#define RED_SET(VAL) (((VAL) << 16) & RED_MSK)
#define RED_GET(REG) (((REG) & RED_MSK) >> 16)
/* These bits configure the red value */

/* Bits [15..8] - Green value */
#define GREEN_MSK (((1 << (7 + 1)) - 1) << 8)
#define GREEN_SET(VAL) (((VAL) << 8) & GREEN_MSK)
#define GREEN_GET(REG) (((REG) & GREEN_MSK) >> 8)
/* These bits configure the green value */

/* Bits [7..0] - Blue value */
#define BLUE_MSK (((1 << (7 + 1)) - 1) << 0)
#define BLUE_SET(VAL) (((VAL) << 0) & BLUE_MSK)
#define BLUE_GET(REG) (((REG) & BLUE_MSK) >> 0)
/* These bits configure the blue value 
Note:
The CLUT write register should only be configured during blanking period or if the layer is
disabled. The CLUT can be enabled or disabled in the LTDC_LxCR register.
The CLUT is only meaningful for L8, AL44 and AL88 pixel format. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_ldtc {
	uint32_t res0;
	uint32_t res1;
	volatile uint32_t sscr;
	volatile uint32_t bpcr;

	volatile uint32_t awcr;
	volatile uint32_t twcr;
	volatile uint32_t gcr;
	uint32_t res2;

	uint32_t res3;
	volatile uint32_t srcr;
	uint32_t res4;
	volatile uint32_t bccr;

	uint32_t res5;
	volatile uint32_t ier;
	volatile uint32_t isr;
	volatile uint32_t icr;

	volatile uint32_t lipcr;
	volatile uint32_t cpsr;
	volatile uint32_t cdsr;
	uint32_t res6[(0x80 - 0x4c) / 4];

	uint32_t res7;
	volatile uint32_t l1cr;
	volatile uint32_t l1whpcr;
	volatile uint32_t l1wvpcr;

	volatile uint32_t l1ckcr;
	volatile uint32_t l1pcfr;
	volatile uint32_t l1cacr;
	volatile uint32_t l1dccr;

	volatile uint32_t l1bfcr;
	uint32_t res8[2];
	volatile uint32_t l1cfbar;

	volatile uint32_t l1cfblr;
	volatile uint32_t l1cfblnr;
	uint32_t res8[3];
	volatile uint32_t l1clutwr;
	uint32_t res9[(0x100 - 0xc8) / 4];

	uint32_t res10;
	volatile uint32_t l2cr;
	volatile uint32_t l2whpcr;
	volatile uint32_t l2wvpcr;

	volatile uint32_t l2ckcr;
	volatile uint32_t l2pcfr;
	volatile uint32_t l2cacr;
	volatile uint32_t l2dccr;

	volatile uint32_t l2bfcr;
	uint32_t res11[2];
	volatile uint32_t l2cfbar;

	volatile uint32_t l2cfblr;
	volatile uint32_t l2cfblnr;
	uint32_t res12[3];
	volatile uint32_t l2clutwr;

};


#endif /* __ASSEMBLER__ */


#endif /* __STM32F_LDTC_H__ */
