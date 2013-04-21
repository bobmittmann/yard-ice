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
 * @file dm365-misc.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/delay.h>

#include "davinci/dm365.h"
#include "target/davinci.h"


#define DM36X_PRTCIF_BASE DM365_PRTCIF_BASE
#define DM36X_SYSTEM_BASE DM365_SYSTEM_BASE
#define DM36X_GPIO_BASE DM365_GPIO_BASE
#define DM36X_AINTC_BASE DM365_AINTC_BASE
#define DM36X_PLL1_BASE DM365_PLL1_BASE
#define DM36X_PLL2_BASE DM365_PLL2_BASE
#define DM36X_PSC_BASE DM365_PSC_BASE
#define DM36X_DDR_BASE DM365_DDR_BASE
#define DM36X_AEMIF_BASE DM365_AEMIF_BASE
#define DM36X_UART0_BASE DM365_UART0_BASE
#define DM36X_UART1_BASE DM365_UART1_BASE
#define DM36X_TIMER0_BASE DM365_TIMER0_BASE
#define DM36X_CSL_RTSIF_0_REGS_BASE DM365_CSL_RTSIF_0_REGS_BASE
#define DM36X_I2C0_BASE DM365_I2C0_BASE



#include <sys/delay.h>

#define SYS_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_SYSTEM_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_SYSTEM_BASE + (OFFS), data | (VAL)); }

#define SYS_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_SYSTEM_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_SYSTEM_BASE + (OFFS), data & ~(VAL)); }

#define SYS_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_SYSTEM_BASE + (OFFS), &data); data; })

#define SYS_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_SYSTEM_BASE + (OFFS), (VAL));


#define PSC_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_PSC_BASE + (OFFS), &data); data; })

#define PSC_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_PSC_BASE + (OFFS), (VAL));



#define PLL1_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_PLL1_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_PLL1_BASE + (OFFS), data | (VAL)); }

#define PLL1_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_PLL1_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_PLL1_BASE + (OFFS), data & ~(VAL)); }

#define PLL1_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_PLL1_BASE + (OFFS), &data); data; })

#define PLL1_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_PLL1_BASE + (OFFS), (VAL));


#define PLL2_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_PLL2_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_PLL2_BASE + (OFFS), data | (VAL)); }

#define PLL2_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_PLL2_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_PLL2_BASE + (OFFS), data & ~(VAL)); }

#define PLL2_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_PLL2_BASE + (OFFS), &data); data; })

#define PLL2_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_PLL2_BASE + (OFFS), (VAL));


#define DDR_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_DDR_BASE + (OFFS), &data); data; })

#define DDR_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_DDR_BASE + (OFFS), (VAL));


#define AEMIF_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_AEMIF_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_AEMIF_BASE + (OFFS), data | (VAL)); }

#define AEMIF_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_AEMIF_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_AEMIF_BASE + (OFFS), data & ~(VAL)); }

#define AEMIF_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_AEMIF_BASE + (OFFS), &data); data; })

#define AEMIF_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_AEMIF_BASE + (OFFS), (VAL));


#define UART1_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_UART1_BASE + (OFFS), &data); data; })

#define UART1_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_UART1_BASE + (OFFS), (VAL));

#define UART0_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_UART0_BASE + (OFFS), &data); data; })

#define UART0_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_UART0_BASE + (OFFS), (VAL));

#define TIMER0_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_TIMER0_BASE + (OFFS), &data); data; })

#define TIMER0_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_TIMER0_BASE + (OFFS), (VAL));


#define I2C0_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_I2C0_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_I2C0_BASE + (OFFS), data | (VAL)); }

#define I2C0_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_I2C0_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_I2C0_BASE + (OFFS), data & ~(VAL)); }

#define I2C0_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_I2C0_BASE + (OFFS), &data); data; })

#define I2C0_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_I2C0_BASE + (OFFS), (VAL));


#define AINTC_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_AINTC_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_AINTC_BASE + (OFFS), data | (VAL)); }

#define AINTC_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_AINTC_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_AINTC_BASE + (OFFS), data & ~(VAL)); }

#define AINTC_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_AINTC_BASE + (OFFS), &data); data; })

#define AINTC_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_AINTC_BASE + (OFFS), (VAL));


#define GPIO_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_GPIO_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_GPIO_BASE + (OFFS), data | (VAL)); }

#define GPIO_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_GPIO_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_GPIO_BASE + (OFFS), data & ~(VAL)); }

#define GPIO_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_GPIO_BASE + (OFFS), &data); data; })

#define GPIO_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_GPIO_BASE + (OFFS), (VAL));



#define GPINT_GPEN_ADDR 0x01c21c08 /* WDT special function */
#define GPTDAT_GPDIR_ADDR 0x01c21c0c /* WDT special function */

#define DDR_TEST_PATTERN 0xa55aa55a

static const uint32_t PLL1_Mult = PLL1_MULTIPLIER;
static const uint32_t PLL2_Mult = PLL2_MULTIPLIER;

void dm36x_pinmux(const struct ice_drv * ice, int offs, 
				  uint32_t mask, uint32_t value)
{
	uint32_t addr = DM36X_SYSTEM_BASE + SYS_PINMUX(offs);
	uint32_t data;

	ice_rd32(ice, addr, &data);
	ice_wr32(ice, addr, (data & ~mask) | (value & mask));
}

void vpss_sync_reset(const struct ice_drv * ice)
{
	uint32_t PdNum = 0;
	uint32_t val;

	/* VPSS_CLKMD 1:1 */
	SYS_REG_SET(SYS_VPSS_CLKCTL, 0x00000080); 

	/* LPSC SyncReset DDR Clock Enable */
	val = PSC_REG_RD(PSC_MDCTL(47));
	val = (val & 0xffffffe0) | 0x00000001;
	PSC_REG_WR(PSC_MDCTL(47), val);

	PSC_REG_WR(PSC_PTCMD, (1 << PdNum));

	while(! (((PSC_REG_RD(PSC_PTSTAT) >> PdNum) & 0x00000001) == 0));      

	while(!((PSC_REG_RD(PSC_MDSTAT(47)) &  0x0000001f) == 0x1));           
}

void por_reset(const struct ice_drv * ice)
{
	uint8_t rtc_ctrl[1];

	if ((PLL1_REG_RD(PLL_RSTYPE)) & 0x03) {
		vpss_sync_reset(ice); /* VPSS sync reset */

		/* XXX: ???? */
		ice_wr32(ice, GPINT_GPEN_ADDR, 0x00020000);
		ice_wr32(ice, GPTDAT_GPDIR_ADDR, 0x00020002);

		while(1);
	}

	*rtc_ctrl = 0;
	prtcss_wr(ice, PRTCSS_RTC_CTRL, rtc_ctrl, 1);
	prtcss_rd(ice, PRTCSS_RTC_CTRL, rtc_ctrl, 1);
}

void dm365_psc_init(const ice_drv_t * ice)
{

	unsigned char i = 0;
	int start;
	int end;
	int group;
	int min;
	int max;
	uint32_t  PdNum = 0; 

	min = 0;
	max = 2;

	for(group = min; group <= max; group++) {
		if (group == 0) {
			start = 0; /* Enabling LPSC 3 to 28 SCR first */
			end = 28;
		} else {
			if (group == 1) { /* Skip locked LPSCs [29-37] */
				start = 38;
				end = 47;
			} else {
				start = 50;
				end = 51;
			}
		}

 /* NEXT = 0x3, Enable LPSC's */
		for (i = start; i <= end; i++) {
 /* CSL_FINS(CSL_PSC_0_REGS->MDCTL[i], PSC_MDCTL_NEXT, 0x3);	   */
			PSC_REG_WR(PSC_MDCTL(i), PSC_REG_RD(PSC_MDCTL(i)) | 0x3); 
		}

 /* Program goctl to start transition sequence for LPSCs */
 /* CSL_PSC_0_REGS->PTCMD = (1<<PdNum);  */
		/* Kick off Power Domain 0 Modules*/
		PSC_REG_WR(PSC_PTCMD, (1 << PdNum)); 

 /* Wait for GOSTAT = NO TRANSITION from PSC for Powerdomain 0 */
 /* while(! (((CSL_PSC_0_REGS->PTSTAT >> PdNum) & 0x00000001) == 0)); */
		while (!(((PSC_REG_RD(PSC_PTSTAT) >> PdNum) & 0x00000001) == 0)); 

 /* Wait for MODSTAT = ENABLE from LPSC's */
		for (i = start; i <= end; i++) {
 /* while(!((CSL_PSC_0_REGS->MDSTAT[i] &  0x0000001f) == 0x3));    */
			while(!((PSC_REG_RD(PSC_MDSTAT(i)) &  0x0000001f) == 0x3));   

		}	
	}	  
}

uint32_t dm365_pll1_init(const ice_drv_t * ice, uint32_t pllmult)
{

	uint32_t clksrc = 0x0;   /*clksrc = 1 = >External Oscilator */

	/*Power up the PLL*/
	PLL1_REG_CLR(PLL_PLLCTL, ~0xfffffffd);		

	PLL1_REG_CLR(PLL_PLLCTL, ~0xfffffeff);  			
	PLL1_REG_SET(PLL_PLLCTL, clksrc << 8);

	/*Set PLLENSRC '0', PLL Enable(PLLEN) selection is controlled through MMR*/
	PLL1_REG_CLR(PLL_PLLCTL, ~0xffffffdf);	

	/*Set PLLEN = 0 = > PLL BYPASS MODE*/
	PLL1_REG_CLR(PLL_PLLCTL, ~0xfffffffe);

	udelay(150); 				

	/*  PLLRST = 1(reset assert) */
	PLL1_REG_SET(PLL_PLLCTL, 0x00000008); 

	udelay(300); 

	/*Bring PLL out of Reset*/ 
	PLL1_REG_CLR(PLL_PLLCTL, ~0xfffffff7);

	/* Program the Multiper and Pre-Divider for PLL1 */
	PLL1_REG_WR(PLL_PLLM, 90); /* M = 90 ); VCO will 24*2M/N+1 = 540Mhz */
	PLL1_REG_WR(PLL_PREDIV, 0x8000 | 0x7); /* N = 7 */

	/* Assert TENABLE = 1, TENABLEDIV = 1, TINITZ = 1  */
	PLL1_REG_WR(PLL_SECCTL, 0x00470000); 

	/* 	for(j = 0; j<0x100; j++) {}  */
	/* Assert TENABLE = 1, TENABLEDIV = 1, TINITZ = 0  */
	PLL1_REG_WR(PLL_SECCTL, 0x00460000); 
	/* 	for(j = 0; j<0x100; j++) {} */
	/* Assert TENABLE = 0, TENABLEDIV = 0, TINITZ = 0  */
	PLL1_REG_WR(PLL_SECCTL, 0x00400000); 
	/* 	for(j = 0; j<0x100; j++) {} */
	/* Assert TENABLE = 0, TENABLEDIV = 0, TINITZ = 1 */
	PLL1_REG_WR(PLL_SECCTL, 0x00410000); 

	/* Program the PostDiv for PLL1 */
	PLL1_REG_WR(PLL_POSTDIV, 0x8000);

	/*  Post divider setting for PLL1   */
	PLL1_REG_WR(PLL_PLLDIV2, 0x8001); /* not use */
	/* POST DIV 540/2 = 270Mhz  -> MJCP/HDVICP */
	PLL1_REG_WR(PLL_PLLDIV3, 0x8001); 
	/* POST DIV 540/4 = 135Mhz  -> EDMA/Peripheral CFG0 (1/2 Kaleido clock) */
	PLL1_REG_WR(PLL_PLLDIV4, 0x8003); 
	/* POST DIV 540/2 = 270Mhz -> VPSS (max 270) */
	PLL1_REG_WR(PLL_PLLDIV5, 0x8001); 
	/* POST DIV 540/20 = 27Mhz -> VENC alternate for SD */
	PLL1_REG_WR(PLL_PLLDIV6, 0x8013); 
	PLL1_REG_WR(PLL_PLLDIV7, 0x8000); /* POST DIV 540/2 = 270Mhz -> DDR */
	PLL1_REG_WR(PLL_PLLDIV8, 0x8003); /* POST DIV 540/4 = 135Mhz-> MMC0/SD0 */
	PLL1_REG_WR(PLL_PLLDIV9, 0x8001); /* POST DIV 540/2 = 270Mhz-> CLKOUT */

	udelay(300);

	/*Set the GOSET bit */   
	PLL1_REG_WR(PLL_PLLCMD, 0x00000001); /* Go */

	udelay(300);


	/*Wait for PLL to LOCK */

	while(! (((SYS_REG_RD(SYS_PLL0_CONFIG)) & 0x07000000) == 0x07000000));


	/*Enable the PLL Bit of PLLCTL*/
	PLL1_REG_SET(PLL_PLLCTL, 0x00000001); /* PLLEN = 0 */

	return 0;
}

uint32_t dm365_pll2_init(const ice_drv_t * ice)
{
	uint32_t clksrc = 0;

	/*Power up the PLL*/
	PLL2_REG_CLR(PLL_PLLCTL, ~0xfffffffd);	

	/*Select the Clock Mode as Onchip Oscilator or External Clock on MXI pin*/
	/*VDB has input on MXI pin */

	PLL2_REG_CLR(PLL_PLLCTL, ~0xfffffeff);  			
	PLL2_REG_SET(PLL_PLLCTL, clksrc << 8);

	/*Set PLLENSRC '0', PLL Enable(PLLEN) selection is controlled through MMR*/
	PLL2_REG_CLR(PLL_PLLCTL, ~0xffffffdf);	

	/*Set PLLEN = 0 = > PLL BYPASS MODE*/
	PLL2_REG_CLR(PLL_PLLCTL, ~0xfffffffe);

	udelay(50); 				

	/*  PLLRST = 1(reset assert) */
	PLL2_REG_SET(PLL_PLLCTL, 0x00000008);  

	udelay(300); 

	/*Bring PLL out of Reset*/
	PLL2_REG_CLR(PLL_PLLCTL, ~0xfffffff7);		

	/* Program the Multiper and Pre-Divider for PLL2 */
	PLL2_REG_WR(PLL_PLLM, 0x63); /* VCO will 24*2M/N+1 = 594Mhz */
	PLL2_REG_WR(PLL_PREDIV, 0x8000 | 0x7);

	PLL2_REG_WR(PLL_POSTDIV, 0x8000);

	/* Assert TENABLE = 1, TENABLEDIV = 1, TINITZ = 1  */
	PLL2_REG_WR(PLL_SECCTL, 0x00470000); 
	/* Assert TENABLE = 1, TENABLEDIV = 1, TINITZ = 0  */
	PLL2_REG_WR(PLL_SECCTL, 0x00460000); 
	/* Assert TENABLE = 0, TENABLEDIV = 0, TINITZ = 0  */
	PLL2_REG_WR(PLL_SECCTL, 0x00400000); 
	/* Assert TENABLE = 0, TENABLEDIV = 0, TINITZ = 1 */
	PLL2_REG_WR(PLL_SECCTL, 0x00410000); 

	/*  Post divider setting for PLL2  */
	PLL2_REG_WR(PLL_PLLDIV2, 0x8001); /* 594/2 = 297 Mhz -> ARM  */
	PLL2_REG_WR(PLL_PLLDIV4, 0x801c); /* POST DIV 594/29 = 20.48 Mhz -> VOICE */
	PLL2_REG_WR(PLL_PLLDIV5, 0x8007); /* POST DIV 594/8 = 74.25 Mhz->VIDEO HD */

	/* GoCmd for PostDivider to take effect */
	PLL2_REG_WR(PLL_PLLCMD, 0x00000001);  

	udelay(150);

	/*Wait for PLL to LOCK */
	while(! (((SYS_REG_RD(SYS_PLL1_CONFIG)) & 0x07000000) == 0x07000000)); 

	udelay(4100);

	/* Enable the PLL2 */

	PLL2_REG_SET(PLL_PLLCTL, 0x00000001); /* PLLEN = 0 */

	/* do this after PLL's have been set up */
	SYS_REG_WR(SYS_PERI_CLKCTRL, 0x243f04fc);

	return 0;

}

void lpsc_transition(const ice_drv_t * ice, int module, int domain, int state)
{
	/*  Wait for any outstanding transition to complete */
	while (PSC_REG_RD(PSC_PTSTAT) & (0x00000001 << domain));

	/*  If we are already in that state, just return */
	if (((PSC_REG_RD(PSC_MDSTAT(module))) & 0x1f) == state) return;

	/*  Perform transition */
	PSC_REG_WR(PSC_MDCTL(module), ((PSC_REG_RD(PSC_MDCTL(module))) & 
								   (0xffffffe0)) | (state));
	PSC_REG_WR(PSC_PTCMD, PSC_REG_RD(PSC_PTCMD) | (0x00000001 << domain));

	/*  Wait for transition to complete */
	while ((PSC_REG_RD(PSC_PTSTAT)) & (0x00000001 << domain) );

	/*  Wait and verify the state */
	while (((PSC_REG_RD(PSC_MDSTAT(module))) & 0x1f) != state);	
}


int dm365_ddr2_init(const ice_drv_t * ice)
{
	lpsc_transition(ice, LPSC_DDR, 0, PSC_ENABLE);

	SYS_REG_WR(SYS_VTPIOCR, (SYS_REG_RD(SYS_VTPIOCR)) & 0xffff9f3f);

	/*  Set bit CLRZ (bit 13) */
	SYS_REG_WR(SYS_VTPIOCR, (SYS_REG_RD(SYS_VTPIOCR)) | 0x00002000);

	/*  Check VTP READY Status */
	while( !(SYS_REG_RD(SYS_VTPIOCR) & 0x8000));     

	/*  Set bit VTP_IOPWRDWN bit 14 for DDR input buffers) */
	/* SYS_REG_WR(SYS_VTPIOCR, SYS_REG_RD(SYS_VTPIOCR) | 0x00004000); */

	/*  Set bit LOCK(bit7) and PWRSAVE (bit8) */
	SYS_REG_WR(SYS_VTPIOCR, SYS_REG_RD(SYS_VTPIOCR) | 0x00000080);     

	/*  Powerdown VTP as it is locked (bit 6) */
	/*  Set bit VTP_IOPWRDWN bit 14 for DDR input buffers) */
	SYS_REG_WR(SYS_VTPIOCR, SYS_REG_RD(SYS_VTPIOCR) | 0x00004040);

	/*  Wait for calibration to complete  */
	udelay( 150 );

	/*  Set the DDR to synreset, then enable it again */
	lpsc_transition(ice, LPSC_DDR, 0, PSC_SYNCRESET);
	lpsc_transition(ice, LPSC_DDR, 0, PSC_ENABLE);

	DDR_REG_WR(DDR_DDRPHYCR, 0x000080c6); 

	/* Program SDRAM Bank Config Register */
	DDR_REG_WR(DDR_SDBCR, 0x00d34a32); 
	DDR_REG_WR(DDR_SDBCR, 0x0053ca32);

	/* Program SDRAM Timing Control Register1 */
	DDR_REG_WR(DDR_SDTIMR, 0x45246412); 
	/* Program SDRAM Timing Control Register2 */
	DDR_REG_WR(DDR_SDTIMR2, 0x4225c742); 

	DDR_REG_WR(DDR_PBBPR, 0x000000fe);

	/* Program SDRAM Bank Config Register */
	DDR_REG_WR(DDR_SDBCR, 0x08534832); 

	/* Program SDRAM Refresh Control Register */
	DDR_REG_WR(DDR_SDRCR, 0x83a); 

	lpsc_transition(ice, LPSC_DDR, 0, PSC_SYNCRESET);

	lpsc_transition(ice, LPSC_DDR, 0, PSC_ENABLE);

	return 0;
}


int dm365_emif_init(const ice_drv_t * ice)
{

	AEMIF_REG_WR(AEMIF_AWCCR, 0xff);

	AEMIF_REG_WR(AEMIF_A1CR, 0x40800404); /* for 121.5 MHZ */

	AEMIF_REG_SET(AEMIF_NANDFCR, 1);

	AEMIF_REG_WR(AEMIF_A2CR, 0x00a00505);

	return 0;  

}

int dm365_uart1_init(const ice_drv_t * ice)
{
	unsigned int div; 

	/* divider = DEVICE_OSC_FREQ / (DEVICE_UART0_DESIRED_BAUD * 
	   DEVICE_UART0_OVERSAMPLE_CNT); */

	div = (540 / 4) * 1000000 / (16 * 115200);
	UART1_REG_WR(UART_PWREMU_MGNT, 0); /* Reset UART TX & RX components */

	udelay(100);

	UART1_REG_WR(UART_MDR, 0x0);
	UART1_REG_WR(UART_DLL, div); /* Set baud rate */
	UART1_REG_WR(UART_DLH, 0);

	UART1_REG_WR(UART_FCR, 0x0007); /* Clear UART TX & RX FIFOs */
	UART1_REG_WR(UART_FCR, 0x0000); /* Non-FIFO mode */
	UART1_REG_WR(UART_IER, 0x0007); /* Enable interrupts */

	UART1_REG_WR(UART_LCR, 0x0003); /* 8-bit words, */
	/*  1 STOP bit generated, */
	/*  No Parity, No Stick paritiy, */
	/*  No Break control */

	UART1_REG_WR(UART_MCR, 0x0000); /* RTS & CTS disabled, */
	/*  Loopback mode disabled, */
	/*  Autoflow disabled */

	UART1_REG_WR(UART_PWREMU_MGNT, 0xe001); /* Enable TX & RX componenets */

	return 0;
}

int dm365_uart0_init(const ice_drv_t * ice)
{

	UART0_REG_WR(UART_PWREMU_MGNT, 0); /* Reset UART TX & RX components */

	udelay(100);

	UART0_REG_WR(UART_MDR, 0x0);
	UART0_REG_WR(UART_DLL, 0xd); /* Set baud rate	 */
	UART0_REG_WR(UART_DLH, 0);

	UART0_REG_WR(UART_FCR, 0x0007); /* Clear UART TX & RX FIFOs */
	UART0_REG_WR(UART_FCR, 0x0000); /* Non-FIFO mode */
	UART0_REG_WR(UART_IER, 0x0007); /* Enable interrupts */

	UART0_REG_WR(UART_LCR, 0x0003); /* 8-bit words, */
	/*  1 STOP bit generated, */
	/*  No Parity, No Stick paritiy, */
	/*  No Break control */

	UART0_REG_WR(UART_MCR, 0x0000); /* RTS & CTS disabled, */
	/*  Loopback mode disabled, */
	/*  Autoflow disabled */

	UART0_REG_WR(UART_PWREMU_MGNT, 0xe001); /* Enable TX & RX componenets */

	return 0;
}

int dm365_i2c0_init(const ice_drv_t * ice)
{

	I2C0_REG_WR(I2C_ICMDR, 0); /* Reset I2C */
	I2C0_REG_WR(I2C_ICPSC, 26); /* Config prescaler for 27MHz */
	I2C0_REG_WR(I2C_ICCLKL, 20); /* Config clk LOW for 20kHz */
	I2C0_REG_WR(I2C_ICCLKH, 20); /* Config clk HIGH for 20kHz */
	I2C0_REG_SET(I2C_ICMDR, I2C_ICMDR_IRS); /* Release I2C from reset */

	return 0;
}

int dm365_i2c0_reset(const ice_drv_t * ice)
{
	I2C0_REG_CLR(I2C_ICMDR, I2C_ICMDR_IRS);
	udelay(200);

	/*  Read and clear interrupt status register */
	I2C0_REG_SET(I2C_ICSTR, 0x00007fff);

	/*  Read ICIVR until clear */
	while ((I2C0_REG_RD(I2C_ICIVR) & 0x7) != 0x0);

	/*  Take I2C Out of Reset */
	I2C0_REG_SET(I2C_ICMDR, I2C_ICMDR_IRS);
	udelay(200);
	return 0;
}

int dm365_timer0_init(const ice_drv_t * ice)
{
	/*  Put timer into reset */
	TIMER0_REG_WR(TMR_EMUMGT_CLKSPD, 0x00000003);
	TIMER0_REG_WR(TMR_TCR, 0x00000000);

	/*  Enable TINT0, TINT1 interrupt */
	TIMER0_REG_WR(TMR_INTCTL_STAT, 0x00000001);

	/* Set to 64-bit GP Timer mode, enable TIMER12 & TIMER34 */
	TIMER0_REG_WR(TMR_TGCR, 0x00000003);

	/*  Reset timers to zero  */
	TIMER0_REG_WR(TMR_TIM12, 0x00000000);
	TIMER0_REG_WR(TMR_TIM34, 0x00000000);

	/*  Set timer period (5 second timeout = (24000000 * 5) 
		cycles = 0x07270e00)  */
	TIMER0_REG_WR(TMR_PRD34, 0x00000000);
	TIMER0_REG_WR(TMR_PRD12, 0x07270e00);

	return 0;
}

 /* interrupt for Timer0 in DM35x and DM36x is the same */
void dm365_timer0_start(const ice_drv_t * ice)
{
	/*  Clear interrupt */
	AINTC_REG_SET(AINTC_IRQ1,  0x00000001);

	/*  Put timer in reset */
	TIMER0_REG_WR(TMR_TGCR, 0x00000000);

	/*  Reset timer count to zero  */
	TIMER0_REG_WR(TMR_TIM12, 0x00000000);

	/*  Setup for one-shot mode */
	TIMER0_REG_WR(TMR_TCR, 0x00000040);

	/*  Start TIMER12 */
	TIMER0_REG_WR(TMR_TGCR, 0x00000005);
}

void dm365_timer0_stop(const ice_drv_t * ice)
{
	/*  Clear interrupt */
	AINTC_REG_SET(AINTC_IRQ1,  0x00000001);

	/*  Put timer in reset */
	TIMER0_REG_WR(TMR_TCR, 0x00000000);
	TIMER0_REG_WR(TMR_TGCR, 0x00000000);

	/*  Reset timer count to zero  */
	TIMER0_REG_WR(TMR_TIM12, 0x00000000);
}

uint32_t dm365_timer0_status(const ice_drv_t * ice)
{
	return ((AINTC_REG_RD(AINTC_IRQ1)) & 0x1);
}

void ipnc_dm365_init(FILE * f, const ice_drv_t * ice)
{
#ifdef IPNC_DM365  
	dm36x_pinmux(ice, 0, 0xffffffff, 0x00000000); /* gio */
	dm36x_pinmux(ice, 1, 0xffffffff, 0x00430000); /* gio */
	dm36x_pinmux(ice, 2, 0xffffffff, 0x00001980); /* gio */
	dm36x_pinmux(ice, 3, 0xffffffff, 0x615affff); /* uart1 phy i2c */
	dm36x_pinmux(ice, 4, 0xffffffff, 0x0030c000); /* clkout0 uar1 */

	GPIO_REG_WR(GPIO_DIR01, 0xffffffff); /* set all as input */
	/* set nand_wp phy_rst led1 led2  as output  */
	GPIO_REG_WR(GPIO_DIR23, 0x3ef3ffff); 
	GPIO_REG_WR(GPIO_SETDATA23, 0x000c0000); /* nand_wp phy_rst set hi */
	GPIO_REG_WR(GPIO_CLRDATA23, 0x01000000); /* nand_ce low */
	GPIO_REG_WR(GPIO_DIR45, 0xf67dfffe); /* cmos_rst as output */
	GPIO_REG_WR(GPIO_SETDATA45, 0x01800001); /* led3_r on */
	GPIO_REG_WR(GPIO_CLRDATA45, 0x04000000); /* cmos_rst = low */
	GPIO_REG_WR(GPIO_DIR6, 0xffffffff); /* set all as input */

	{
		uint32_t cnt, RegData, clk;
		cnt = 0;
		pin = 0;
		clk = 1;
		/* check for 74.25 oscillator (gio80) ? */
		while (pin == 0x10000 && cnt < 0x1000) {
			cnt ++;
			pin = GPIO_REG_RD(GPIO_INDTATA45) & 1 << 16;
		}
		if(cnt >= 0x1000)
			clk = 0;
		while (pin == 0 && cnt < 0x1000){
			cnt ++;
			pin = GPIO_REG_RD(GPIO_INDTATA45) & 1 << 16; 
		}
		if(cnt >= 0x1000)
			clk = 0;
		while(pin == 0x10000 && cnt < 0x1000){
			cnt ++;
			pin = GPIO_REG_RD(GPIO_INDTATA45) & 1 << 16;
		}
		if(cnt >= 0x1000)
			clk = 0;
		if(clk)
			dm36x_pinmux(ice, 1, 3 << 20, 1 << 20);
	}

#else

	dm36x_pinmux(ice, 0, 0xffffffff, 0x00fd0000); /* All Video Inputs */
	dm36x_pinmux(ice, 1, 0xffffffff, 0x00145555); /* All Video Outputs */
	dm36x_pinmux(ice, 2, 0xffffffff, 0x000000da); /* EMIFA */
	/* SPI0, SPI1, UART1, I2C, SD0, SD1, McBSP0, CLKOUTs */
	dm36x_pinmux(ice, 3, 0xffffffff, 0x375affff); 
	/* MMC/SD0 instead of MS, SPI0 */
	dm36x_pinmux(ice, 4, 0xffffffff, 0x55555555); 

	GPIO_REG_CLR(GPIO_DIR23, ~0xfeffffff);
	GPIO_REG_WR(GPIO_CLRDATA23, 0x01000000);
#endif
}

int dm365_on_init(FILE * f, const ice_drv_t * ice, ice_mem_entry_t * mem)
{
	int ret = 0;


	/*  Mask all interrupts */
	AINTC_REG_WR(AINTC_INTCTL, 0x4);
	AINTC_REG_WR(AINTC_EABASE, 0x0);
	AINTC_REG_WR(AINTC_EINT0, 0x0);
	AINTC_REG_WR(AINTC_EINT1, 0x0);		

	/*  Clear all interrupts */
	AINTC_REG_WR(AINTC_FIQ0, 0xffffffff);
	AINTC_REG_WR(AINTC_FIQ1, 0xffffffff);
	AINTC_REG_WR(AINTC_IRQ0, 0xffffffff);
	AINTC_REG_WR(AINTC_IRQ1, 0xffffffff);

#ifndef SKIP_LOW_LEVEL_INIT
	SYS_REG_SET(SYS_PERI_CLKCTRL, 1); /* disable output 24Mhze */
	por_reset(ice);

	/*  System PSC setup - enable all */
	dm365_psc_init(ice);

#if 1
	dm36x_pinmux(ice, 0, 0xffffffff, 0x00fd0000); /* All Video Inputs */
	dm36x_pinmux(ice, 1, 0xffffffff, 0x00145555); /* All Video Outputs */
	dm36x_pinmux(ice, 2, 0xffffffff, 0x000000da); /* EMIFA */
	dm36x_pinmux(ice, 3, 0xffffffff, 0x375affff); /* SPI0, SPI1, UART1, I2C, SD0, SD1, McBSP0, CLKOUTs */
	dm36x_pinmux(ice, 4, 0xffffffff, 0x55555555); /* MMC/SD0 instead of MS, SPI0 */
#else
	ipnc_dm365_init(f, ice);
#endif

	GPIO_REG_CLR(GPIO_DIR23, ~0xfeffffff);
	GPIO_REG_WR(GPIO_CLRDATA23, 0x01000000);

	/*  System PLL setup */
	if (ret == 0) 
		ret = dm365_pll1_init(ice, PLL1_Mult);

	/*  DDR PLL setup */
	if (ret == 0) 
		ret = dm365_pll2_init(ice);

	/*  DDR module setup */
	if (ret == 0) 
		ret = dm365_ddr2_init(ice);
#endif


	/*  AEMIF Setup */
	if (ret == 0) 
		ret = dm365_emif_init(ice);

#ifdef IPNC_DM365
	SYS_CLR(SYS_PERI_CLKCTRL, 1); /* output 24Mhz */
	PLL1_REG_SET(PLL_CKEN, 0x2);
	GPIO_REG_SET(GPIO_SETDATA45, 0x04000000); /* cmos_rst hi */

	/*  UART1 Setup */
	if (ret == 0) 
		ret = dm365_uart1_init(ice);
#endif

	/*  UART0 Setup */
	if (ret == 0) 
		ret = dm365_uart0_init(ice);

	/*  TIMER0 Setup */
	if (ret == 0) 
		ret = dm365_timer0_init(ice);

	/*  I2C0 Setup */
	if (ret == 0) 
		ret = dm365_i2c0_init(ice);

	return ret;
}



#if 0
DEVICE_BootMode DEVICE_bootMode( void )
{
	return (DEVICE_BootMode) ((SYSTEM->BOOTCFG & DEVICE_BOOTCFG_BOOTMODE_MASK) >> DEVICE_BOOTCFG_BOOTMODE_SHIFT);
}

DEVICE_BusWidth DEVICE_emifBusWidth( void )
{
	if ( ( (SYSTEM->BOOTCFG & DEVICE_BOOTCFG_EMIFWIDTH_MASK) >> DEVICE_BOOTCFG_EMIFWIDTH_SHIFT) & 0x1 )
	{
		return DEVICE_BUSWIDTH_16BIT;
	}
	else
	{
		return DEVICE_BUSWIDTH_8BIT;
	}
}
#endif
