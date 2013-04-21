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
 * @file arch/at91_.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __AT91_ISI_H__
#define __AT91_ISI_H__

/* Image Sensor Interface */

#define ISI_CR1         ( 0) /* Control Register 1 */
#define ISI_CR2         ( 4) /* Control Register 2 */
#define ISI_SR          ( 8) /* Status Register */
#define ISI_IER         (12) /* Interrupt Enable Register */
#define ISI_IDR         (16) /* Interrupt Disable Register */
#define ISI_IMR         (20) /* Interrupt Mask Register */
#define ISI_PSIZE       (32) /* Preview Size Register */
#define ISI_PDECF       (36) /* Preview Decimation Factor Register */
#define ISI_PFBD        (40) /* Preview Frame Buffer Address Register */
#define ISI_CDBA        (44) /* Codec Dma Address Register */
#define ISI_Y2RSET0     (48) /* Color Space Conversion Register */
#define ISI_Y2RSET1     (52) /* Color Space Conversion Register */
#define ISI_R2YSET0     (56) /* Color Space Conversion Register */
#define ISI_R2YSET1     (60) /* Color Space Conversion Register */
#define ISI_R2YSET2     (64) /* Color Space Conversion Register */

/* ISI_CR1 - ISI Control Register 1 */
#define ISI_RST             (0x1 <<  0) /* (ISI) Image sensor interface reset */
#define ISI_DISABLE         (0x1 <<  1) /* (ISI) image sensor disable. */
#define ISI_HSYNC_POL       (0x1 <<  2) /* (ISI) Horizontal synchronisation polarity */
#define ISI_PIXCLK_POL      (0x1 <<  4) /* (ISI) Pixel Clock Polarity */
#define ISI_EMB_SYNC        (0x1 <<  6) /* (ISI) Embedded synchronisation */
#define ISI_CRC_SYNC        (0x1 <<  7) /* (ISI) CRC correction */
#define ISI_FULL            (0x1 << 12) /* (ISI) Full mode is allowed */
#define ISI_THMASK          (0x3 << 13) /* (ISI) DMA Burst Mask */
#define 	ISI_THMASK_4_8_16_BURST         (0x0 << 13) /* (ISI) 4,8 and 16 AHB burst are allowed */
#define 	ISI_THMASK_8_16_BURST           (0x1 << 13) /* (ISI) 8 and 16 AHB burst are allowed */
#define 	ISI_THMASK_16_BURST             (0x2 << 13) /* (ISI) Only 16 AHB burst are allowed */
#define ISI_CODEC_ON        (0x1 << 15) /* (ISI) Enable the codec path */
#define ISI_SLD             (0xff << 16) /* (ISI) Start of Line Delay */
#define ISI_SFD             (0xff << 24) /* (ISI) Start of frame Delay */

/* ISI_CR2 - ISI Control Register 2 */
#define ISI_IM_VSIZE        (0x7ff <<  0) /* (ISI) Vertical size of the Image sensor [0..2047] */
#define ISI_GS_MODE         (0x1 << 11) /* (ISI) Grayscale Memory Mode */
#define ISI_RGB_MODE        (0x3 << 12) /* (ISI) RGB mode */
#define 	ISI_RGB_MODE_RGB_888              (0x0 << 12) /* (ISI) RGB 8:8:8 24 bits */
#define 	ISI_RGB_MODE_RGB_565              (0x1 << 12) /* (ISI) RGB 5:6:5 16 bits */
#define 	ISI_RGB_MODE_RGB_555              (0x2 << 12) /* (ISI) RGB 5:5:5 16 bits */
#define ISI_GRAYSCALE       (0x1 << 13) /* (ISI) Grayscale Mode */
#define ISI_RGB_SWAP        (0x1 << 14) /* (ISI) RGB Swap */
#define ISI_COL_SPACE       (0x1 << 15) /* (ISI) Color space for the image data */
#define ISI_IM_HSIZE        (0x7ff << 16) /* (ISI) Horizontal size of the Image sensor [0..2047] */
#define 	ISI_RGB_MODE_YCC_DEF              (0x0 << 28) /* (ISI) Cb(i) Y(i) Cr(i) Y(i+1) */
#define 	ISI_RGB_MODE_YCC_MOD1             (0x1 << 28) /* (ISI) Cr(i) Y(i) Cb(i) Y(i+1) */
#define 	ISI_RGB_MODE_YCC_MOD2             (0x2 << 28) /* (ISI) Y(i) Cb(i) Y(i+1) Cr(i) */
#define 	ISI_RGB_MODE_YCC_MOD3             (0x3 << 28) /* (ISI) Y(i) Cr(i) Y(i+1) Cb(i) */
#define ISI_RGB_CFG         (0x3 << 30) /* (ISI) RGB configuration */
#define 	ISI_RGB_CFG_RGB_DEF              (0x0 << 30) /* (ISI) R/G(MSB)  G(LSB)/B  R/G(MSB)  G(LSB)/B */
#define 	ISI_RGB_CFG_RGB_MOD1             (0x1 << 30) /* (ISI) B/G(MSB)  G(LSB)/R  B/G(MSB)  G(LSB)/R */
#define 	ISI_RGB_CFG_RGB_MOD2             (0x2 << 30) /* (ISI) G(LSB)/R  B/G(MSB)  G(LSB)/R  B/G(MSB) */
#define 	ISI_RGB_CFG_RGB_MOD3             (0x3 << 30) /* (ISI) G(LSB)/B  R/G(MSB)  G(LSB)/B  R/G(MSB) */

/* ISI_SR - ISI Status Register */
#define ISI_SOF             (0x1 <<  0) /* (ISI) Start of Frame */
#define ISI_DIS             (0x1 <<  1) /* (ISI) Image Sensor Interface disable */
#define ISI_SOFTRST         (0x1 <<  2) /* (ISI) Software Reset */
#define ISI_CRC_ERR         (0x1 <<  4) /* (ISI) CRC synchronisation error */
#define ISI_FO_C_OVF        (0x1 <<  5) /* (ISI) Fifo Codec Overflow */
#define ISI_FO_P_OVF        (0x1 <<  6) /* (ISI) Fifo Preview Overflow */
#define ISI_FO_P_EMP        (0x1 <<  7) /* (ISI) Fifo Preview Empty */
#define ISI_FO_C_EMP        (0x1 <<  8) /* (ISI) Fifo Codec Empty */
#define ISI_FR_OVR          (0x1 <<  9) /* (ISI) Frame rate overun */

/* ISI_IER - ISI Interrupt Enable Register */

/* ISI_IDR - ISI Interrupt Disable Register */

/* ISI_IMR - ISI Interrupt Mask Register */

/* ISI_PSIZE - ISI Preview Register */
#define ISI_PREV_VSIZE      (0x3ff <<  0) /* (ISI) Vertical size for the preview path */
#define ISI_PREV_HSIZE      (0x3ff << 16) /* (ISI) Horizontal size for the preview path */

/* -------- ISI_Y2R_SET0 : (ISI Offset: 0x30) Color Space Conversion YCrCb to RGB Register -------- */
#define ISI_Y2R_C0          (0xff <<  0) /* (ISI) Color Space Conversion Matrix Coefficient C0 */
#define ISI_Y2R_C1          (0xff <<  8) /* (ISI) Color Space Conversion Matrix Coefficient C1 */
#define ISI_Y2R_C2          (0xff << 16) /* (ISI) Color Space Conversion Matrix Coefficient C2 */
#define ISI_Y2R_C3          (0xff << 24) /* (ISI) Color Space Conversion Matrix Coefficient C3 */

/* ISI_Y2R_SET1 - ISI Color Space Conversion YCrCb to RGB set 1 Register */
#define ISI_Y2R_C4          (0x1ff <<  0) /* (ISI) Color Space Conversion Matrix Coefficient C4 */
#define ISI_Y2R_YOFF        (0xff << 12) /* (ISI) Color Space Conversion Luninance default offset */
#define ISI_Y2R_CROFF       (0xff << 13) /* (ISI) Color Space Conversion Red Chrominance default offset */
#define ISI_Y2R_CBFF        (0xff << 14) /* (ISI) Color Space Conversion Luninance default offset */

/* -------- ISI_R2Y_SET0 : (ISI Offset: 0x38) Color Space Conversion RGB to YCrCb set 0 register -------- */
#define ISI_R2Y_C0          (0x7f <<  0) /* (ISI) Color Space Conversion RGB to YCrCb Matrix coefficient C0 */
#define ISI_R2Y_C1          (0x7f <<  1) /* (ISI) Color Space Conversion RGB to YCrCb Matrix coefficient C1 */
#define ISI_R2Y_C2          (0x7f <<  3) /* (ISI) Color Space Conversion RGB to YCrCb Matrix coefficient C2 */
#define ISI_R2Y_ROFF        (0x1 <<  4) /* (ISI) Color Space Conversion Red component offset */

/* ISI_R2Y_SET1 - Color Space Conversion RGB to YCrCb set 1 register */
#define ISI_R2Y_C3          (0x7f <<  0) /* (ISI) Color Space Conversion RGB to YCrCb Matrix coefficient C3 */
#define ISI_R2Y_C4          (0x7f <<  1) /* (ISI) Color Space Conversion RGB to YCrCb Matrix coefficient C4 */
#define ISI_R2Y_C5          (0x7f <<  3) /* (ISI) Color Space Conversion RGB to YCrCb Matrix coefficient C5 */
#define ISI_R2Y_GOFF        (0x1 <<  4) /* (ISI) Color Space Conversion Green component offset */

/* ISI_R2Y_SET2 - Color Space Conversion RGB to YCrCb set 2 register */
#define ISI_R2Y_C6          (0x7f <<  0) /* (ISI) Color Space Conversion RGB to YCrCb Matrix coefficient C6 */
#define ISI_R2Y_C7          (0x7f <<  1) /* (ISI) Color Space Conversion RGB to YCrCb Matrix coefficient C7 */
#define ISI_R2Y_C8          (0x7f <<  3) /* (ISI) Color Space Conversion RGB to YCrCb Matrix coefficient C8 */
#define ISI_R2Y_BOFF        (0x1 <<  4) /* (ISI) Color Space Conversion Blue component offset */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_isi {
	volatile uint32_t cr1; /* Control Register 1 */
	volatile uint32_t cr2; /* Control Register 2 */
	volatile uint32_t sr; /* Status Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	volatile uint32_t psize; /* Preview Size Register */
	volatile uint32_t pdecf; /* Preview Decimation Factor Register */
	volatile uint32_t pfbd; /* Preview Frame Buffer Address Register */
	volatile uint32_t cdba; /* Codec Dma Address Register */
	volatile uint32_t y2rset0; /* Color Space Conversion Register */
	volatile uint32_t y2rset1; /* Color Space Conversion Register */
	volatile uint32_t r2yset0; /* Color Space Conversion Register */
	volatile uint32_t r2yset1; /* Color Space Conversion Register */
	volatile uint32_t r2yset2; /* Color Space Conversion Register */
};

#endif /* __ASSEMBLER__ */


#endif /* __AT91_ISI_H__ */

