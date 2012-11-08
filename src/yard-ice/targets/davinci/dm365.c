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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include "target/davinci.h"
#include "davinci/dm36x.h"
#include "armice.h"

#include <stdlib.h>
#include <sys/delay.h>
#include <sys/dcclog.h>

void dm36x_pinmux(const struct ice_drv * ice, int offs, 
				  uint32_t mask, uint32_t value)
{
	uint32_t addr = DM36X_SYSTEM_BASE + SYS_PINMUX(offs);
	uint32_t data;

	ice_rd32(ice, addr, &data);
	ice_wr32(ice, addr, (data & ~mask) | (value & mask));
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
		/* Kick off Power Domain 0 Modules*/
		PSC_REG_WR(PSC_PTCMD, (1 << PdNum)); 

 /* Wait for GOSTAT = NO TRANSITION from PSC for Powerdomain 0 */
		while (!(((PSC_REG_RD(PSC_PTSTAT) >> PdNum) & 0x00000001) == 0)); 

 /* Wait for MODSTAT = ENABLE from LPSC's */
		for (i = start; i <= end; i++) {
			while(!((PSC_REG_RD(PSC_MDSTAT(i)) &  0x0000001f) == 0x3));   
		}	
	}	  
}

#define FOSC_HZ 24000000
#define FCORE_HZ 270000000
#define FDDR2E_HZ 270000000

int dm365_pll1_init(const ice_drv_t * ice)
{
	uint32_t pll_cfg;

	/* Power up the PLL*/
	PLL1_REG_CLR(PLL_PLLCTL, PLLCTL_PLLPWRDN);		

	/* Set PLLENSRC '0', PLL Enable(PLLEN) selection is 
	   controlled through MMR */
	PLL1_REG_CLR(PLL_PLLCTL, PLLCTL_PLLENSRC);	

	/* Set PLLEN = 0, PLL BYPASS MODE*/
	PLL1_REG_CLR(PLL_PLLCTL, PLLCTL_PLLEN);

	udelay(150); 				

	/*  PLLRST = 1 (reset assert) */
	PLL1_REG_SET(PLL_PLLCTL, PLLCTL_PLLRST); 
	udelay(300); 
	/* Bring PLL out of Reset*/ 
	PLL1_REG_CLR(PLL_PLLCTL, PLLCTL_PLLRST);

	/* VCO = FOSC_HZ * 2 * M / (N + 1) */
	/* VCO = 540MHz -> M = 180, N = 8 */

	/* Program the Multiper and Pre-Divider for PLL1 */
	PLL1_REG_WR(PLL_PLLM, PLLM_VAL(180)); /* M */
	PLL1_REG_WR(PLL_PREDIV, PREDIV_VAL(8)); /* N */

	/* Assert TENABLE = 1, TENABLEDIV = 1, TINITZ = 1  */
	PLL1_REG_WR(PLL_SECCTL, PLLSECCTL_STOPMODE | 
				PLLSECCTL_TENABLEDIV | PLLSECCTL_TENABLE | PLLSECCTL_TINITZ);

	/* Assert TENABLE = 1, TENABLEDIV = 1, TINITZ = 0  */
	PLL1_REG_WR(PLL_SECCTL, PLLSECCTL_STOPMODE | 
				PLLSECCTL_TENABLEDIV | PLLSECCTL_TENABLE);

	/* Assert TENABLE = 0, TENABLEDIV = 0, TINITZ = 0  */
	PLL1_REG_WR(PLL_SECCTL, PLLSECCTL_STOPMODE);

	/* Assert TENABLE = 0, TENABLEDIV = 0, TINITZ = 1 */
	PLL1_REG_WR(PLL_SECCTL, PLLSECCTL_STOPMODE | PLLSECCTL_TINITZ);

	PLL1_REG_WR(PLL_POSTDIV, PLLDIV_EN | PLLDIV_VAL(1));

	/* USB Reference Clock: 540MHz */
	PLL1_REG_WR(PLL_PLLDIV1, PLLDIV_EN | PLLDIV_VAL(1));
	/* ARM926EJ-S, HDVICP Block Clock: 270MHz */
	PLL1_REG_WR(PLL_PLLDIV2, PLLDIV_EN | PLLDIV_VAL(2)); 
	/* MJCP and HDVICP Coprocessor Bus Interface Clock: 270MHz */
	PLL1_REG_WR(PLL_PLLDIV3, PLLDIV_EN | PLLDIV_VAL(2)); 
	/* Config Bus, Peripheral System, EDMA:  135MHz */
	PLL1_REG_WR(PLL_PLLDIV4, PLLDIV_EN | PLLDIV_VAL(4)); 
	/* VPSS (max 270): 270MHz */
	PLL1_REG_WR(PLL_PLLDIV5,  PLLDIV_EN | PLLDIV_VAL(2)); 
	/* VENC Clock: 27MHz */
	PLL1_REG_WR(PLL_PLLDIV6, PLLDIV_EN | PLLDIV_VAL(20)); 
	/* DDR 2x Clock: 540MHz */
	PLL1_REG_WR(PLL_PLLDIV7, PLLDIV_EN | PLLDIV_VAL(1));
	/* MMC0/SD0L Clock: 135MHz*/
	PLL1_REG_WR(PLL_PLLDIV8,  PLLDIV_EN | PLLDIV_VAL(4));
	/* CLKOUT:  270MHz ??? */
	PLL1_REG_WR(PLL_PLLDIV9,  PLLDIV_EN | PLLDIV_VAL(2)); 

	udelay(300);

	/* Set the GOSET bit */   
	PLL1_REG_WR(PLL_PLLCMD, PLLCMD_GOSET);

	udelay(300);

	do {
		pll_cfg = SYS_REG_RD(SYS_PLLC1_CONFIG);
	} while((pll_cfg & PLLC1_CONFIG_LOCK) != PLLC1_CONFIG_LOCK);

	/* Enable the PLL Bit of PLLCTL */
	PLL1_REG_SET(PLL_PLLCTL, PLLCTL_PLLEN); 

	return 0;
}

int dm365_pll2_init(const ice_drv_t * ice)
{
	uint32_t clksrc = 0;
	uint32_t pll_cfg;

	/*Power up the PLL*/
	PLL2_REG_CLR(PLL_PLLCTL, PLLCTL_PLLPWRDN);		

	/* XXX: Select the Clock Mode as Onchip Oscilator or 
	   External Clock on MXI pin*/
	/* VDB has input on MXI pin */
	PLL2_REG_CLR(PLL_PLLCTL, ~0xfffffeff);  			
	PLL2_REG_SET(PLL_PLLCTL, clksrc << 8);

	/* Set PLLENSRC '0', PLL Enable(PLLEN) selection is 
	   controlled through MMR */
	PLL2_REG_CLR(PLL_PLLCTL, PLLCTL_PLLENSRC);	

	/* Set PLLEN = 0, PLL BYPASS MODE */
	PLL2_REG_CLR(PLL_PLLCTL, PLLCTL_PLLEN);

	udelay(50); 				

	/*  PLLRST = 1 (reset assert) */
	PLL2_REG_SET(PLL_PLLCTL, PLLCTL_PLLRST); 
	udelay(300); 
	/* Bring PLL out of Reset*/ 
	PLL2_REG_CLR(PLL_PLLCTL, PLLCTL_PLLRST);

	/* VCO = FOSC_HZ * 2 * M / (N + 1) */
	/* VCO = 594MHz -> FOSC=24MHz, M=198, N=8 */

	/* Program the Multiper and Pre-Divider for PLL2 */
	PLL2_REG_WR(PLL_PLLM, PLLM_VAL(198)); /* M */
	PLL2_REG_WR(PLL_PREDIV, PREDIV_VAL(8)); /* N */

	PLL2_REG_WR(PLL_POSTDIV, PLLDIV_EN | PLLDIV_VAL(1));

	/* Assert TENABLE = 1, TENABLEDIV = 1, TINITZ = 1  */
	PLL2_REG_WR(PLL_SECCTL, PLLSECCTL_STOPMODE | 
				PLLSECCTL_TENABLEDIV | PLLSECCTL_TENABLE | PLLSECCTL_TINITZ);
	/* Assert TENABLE = 1, TENABLEDIV = 1, TINITZ = 0  */
	PLL2_REG_WR(PLL_SECCTL, PLLSECCTL_STOPMODE | 
				PLLSECCTL_TENABLEDIV | PLLSECCTL_TENABLE);
	/* Assert TENABLE = 0, TENABLEDIV = 0, TINITZ = 0  */
	PLL2_REG_WR(PLL_SECCTL, PLLSECCTL_STOPMODE);
	/* Assert TENABLE = 0, TENABLEDIV = 0, TINITZ = 1 */
	PLL2_REG_WR(PLL_SECCTL, PLLSECCTL_STOPMODE | PLLSECCTL_TINITZ);

	/* USB Reference Clock: 594MHz */
	PLL2_REG_WR(PLL_PLLDIV1, PLLDIV_EN | PLLDIV_VAL(1));
	/* ARM926EJ-S, HDVICP Block Clock: 297MHz */
	PLL2_REG_WR(PLL_PLLDIV2, PLLDIV_EN | PLLDIV_VAL(2)); 
	/* DDR 2x Clock: 594MHz */
	PLL2_REG_WR(PLL_PLLDIV3, PLLDIV_EN | PLLDIV_VAL(1));
	/* Voice Codeck Clock: 20.48Hz */
	PLL2_REG_WR(PLL_PLLDIV4, PLLDIV_EN | PLLDIV_VAL(29)); 
	/* VENC Clock: 74.25 MHz */
	PLL2_REG_WR(PLL_PLLDIV5, PLLDIV_EN | PLLDIV_VAL(8)); 

	/* GoCmd for PostDivider to take effect */
	/* Set the GOSET bit */   
	PLL2_REG_WR(PLL_PLLCMD, PLLCMD_GOSET);

	udelay(150);

	/* Wait for PLL to LOCK */
	do {
		pll_cfg = SYS_REG_RD(SYS_PLLC2_CONFIG);
	} while((pll_cfg & PLLC2_CONFIG_LOCK) != PLLC2_CONFIG_LOCK);

	udelay(4100);

	/* Enable the PLL2 */
	PLL2_REG_SET(PLL_PLLCTL, PLLCTL_PLLEN); 

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

	/* Disable power down control, Clear VTP flops, 
	   Unlock impedance, Disable power down */ 
	SYS_REG_CLR(SYS_VTPIOCR, VTPIOCR_IOPWRDN | VTPIOCR_CLRZ | 
				 VTPIOCR_LOCK | VTPIOCR_PWRDN);
//	SYS_REG_WR(SYS_VTPIOCR, vtpiocr & 0xffff9f3f);

	/* VTP clear = 1. Enable VTP flops. */
	SYS_REG_SET(SYS_VTPIOCR, VTPIOCR_CLRZ);
//	SYS_REG_WR(SYS_VTPIOCR, (SYS_REG_RD(SYS_VTPIOCR)) | 0x00002000);

	/*  Check VTP READY Status */
	while (!(SYS_REG_RD(SYS_VTPIOCR) & VTPIOCR_READY));

	/*  Set bit VTP_IOPWRDWN bit 14 for DDR input buffers) */
	/* SYS_REG_WR(SYS_VTPIOCR, SYS_REG_RD(SYS_VTPIOCR) | 0x00004000); */

	/*  Set bit LOCK(bit7) and PWRSAVE (bit8) */
//	SYS_REG_WR(SYS_VTPIOCR, SYS_REG_RD(SYS_VTPIOCR) | 0x00000080);     
   	/* Lock VTP impedance */
	SYS_REG_SET(SYS_VTPIOCR, VTPIOCR_LOCK);

	/*  Powerdown VTP as it is locked (bit 6) */
	/*  Set bit VTP_IOPWRDWN bit 14 for DDR input buffers) */
	SYS_REG_SET(SYS_VTPIOCR, VTPIOCR_PWRDN | VTPIOCR_IOPWRDN);

	/*  Wait for calibration to complete  */
	udelay(150);

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
	uint32_t awccr;
	uint32_t acr;

	awccr = AEMIF_WP_LOW | AEMIF_MEWC(256);

	/* WAIT polarity Low, Maximum extended wait cycles = 256 */
	AEMIF_REG_WR(AEMIF_AWCCR, awccr);

	DCC_LOG1(LOG_TRACE, "AEMIF_AWCCR=%08x", awccr);

	acr = ACR_EW | ACR_W_SETUP(1) | ACR_W_STROBE(5) | ACR_W_HOLD(1) |
	               ACR_R_SETUP(1) | ACR_R_STROBE(5) | ACR_R_HOLD(1) |
	               ACR_TA(2) | ACR_ASIZE_8BITS;

	/* for 121.5 MHZ */
	AEMIF_REG_WR(AEMIF_A1CR, acr);

	DCC_LOG1(LOG_TRACE, "AEMIF_A1CRL=%08x", acr);

	/* NAND Flash mode enable */
	AEMIF_REG_SET(AEMIF_NANDFCR, DM365_CE0_BASE);

	acr = ACR_W_SETUP(1) | ACR_W_STROBE(11) | ACR_W_HOLD(1) |
	      ACR_R_SETUP(1) | ACR_R_STROBE(11) | ACR_R_HOLD(1) |
	      ACR_TA(2) | ACR_ASIZE_16BITS;

	AEMIF_REG_WR(AEMIF_A2CR, acr);

	DCC_LOG1(LOG_TRACE, "AEMIF_A2CRL=%08x", acr);
	return 0;  
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

#define GPINT_GPEN_ADDR 0x01c21c08 /* WDT special function */
#define GPTDAT_GPDIR_ADDR 0x01c21c0c /* WDT special function */

int dm365_on_init(FILE * f, const ice_drv_t * ice, ice_mem_entry_t * mem)
{
	int ret = 0;
	uint8_t rtc_ctrl[1];
	uint32_t data;
	uint32_t clkctl;

	fprintf(f, "- Mask all interrupts\n");
	/*  Mask all interrupts */
	AINTC_REG_WR(AINTC_INTCTL, 0x4);
	AINTC_REG_WR(AINTC_EABASE, 0x0);
	AINTC_REG_WR(AINTC_EINT0, 0x0);
	AINTC_REG_WR(AINTC_EINT1, 0x0);		

	/* Clear all interrupts */
	fprintf(f, "- Clear all interrupts\n");
	AINTC_REG_WR(AINTC_FIQ0, 0xffffffff);
	AINTC_REG_WR(AINTC_FIQ1, 0xffffffff);
	AINTC_REG_WR(AINTC_IRQ0, 0xffffffff);
	AINTC_REG_WR(AINTC_IRQ1, 0xffffffff);

	/* Disable 24MHz output */
	fprintf(f, "- Disable 24MHz output\n");
	SYS_REG_SET(SYS_PERI_CLKCTL, 1); 

	fprintf(f, "- Power On Reset???\n");

	if ((data= PLL1_REG_RD(PLL_RSTYPE)) & 0x03) {
		vpss_sync_reset(ice); /* VPSS sync reset */
		/* XXX: ???? */
		fprintf(f, "- Reset...\n");
		ice_wr32(ice, GPINT_GPEN_ADDR, 0x00020000);
		ice_wr32(ice, GPTDAT_GPDIR_ADDR, 0x00020002);
	}

	davinci_prtcss_wr(ice, PRTCSS_RTC_CTRL, rtc_ctrl, 1);
	fprintf(f, "- PRTCSS_RTC_CTRL=%02x\n", *rtc_ctrl);

	if (*rtc_ctrl & CTRL_WEN) {
		fprintf(f, "- PRTCSS WDT enabled, disabling it...\n");
		while (*rtc_ctrl & CTRL_WDTBUSY) {
			davinci_prtcss_rd(ice, PRTCSS_RTC_CTRL, rtc_ctrl, 1);
		}
		*rtc_ctrl &= ~CTRL_WEN; 
		davinci_prtcss_wr(ice, PRTCSS_RTC_CTRL, rtc_ctrl, 1);
	}

	fprintf(f, "- System PSC setup...\n");
	/*  System PSC setup - enable all */
	dm365_psc_init(ice);

	fprintf(f, "- Setup IO pins...\n");
	dm36x_pinmux(ice, 0, 0xffffffff, 0x00fd0000); /* All Video Inputs */
	dm36x_pinmux(ice, 1, 0xffffffff, 0x00145555); /* All Video Outputs */
	dm36x_pinmux(ice, 2, 0xffffffff, 0x000000da); /* EMIFA */
	/* SPI0, SPI1, UART1, I2C, SD0, SD1, McBSP0, CLKOUTs */
	dm36x_pinmux(ice, 3, 0xffffffff, 0x375affff); 
	/* MMC/SD0 instead of MS, SPI0 */
	dm36x_pinmux(ice, 4, 0xffffffff, 0x55555555); 

	GPIO_REG_CLR(GPIO_DIR23, ~0xfeffffff);
	GPIO_REG_WR(GPIO_CLRDATA23, 0x01000000);

	/*  System PLL setup */
	fprintf(f, "- PLL1 init...\n");
	ret = dm365_pll1_init(ice);

	/*  DDR PLL setup */
	if (ret == 0) {
		fprintf(f, "- PLL2 init...\n");
		ret = dm365_pll2_init(ice);
	}

	/* Configure Clock Sources */
	clkctl = PRTCCLKS_RTCXI | ARMCLKS_PLLC1SYSCLK2 | KEYSCLKS_RTCXI | 
		DDRCLKS_PLLC1SYSCLK7 | HDVICPCLKS_PLLC2SYSCLK2 | 
		KEYSCAN_DIV(64)  | VCODEC_DIV(10) | CLKOUT2_DIV(16) |
		CLOCKOUT2_DISABLE | CLOCKOUT1_ENABLE | CLOCKOUT0_ENABLE;

	SYS_REG_WR(SYS_PERI_CLKCTL, clkctl);

	fprintf(f, "- SYS_PERI_CLKCTL = %08x\n", clkctl);

	/*  DDR module setup */
	if (ret == 0) {
		fprintf(f, "- DDR init...\n");
		ret = dm365_ddr2_init(ice);
	}

	/*  AEMIF Setup */
	if (ret == 0) {
		fprintf(f, "- EMIF init...\n");
		ret = dm365_emif_init(ice);
	}

	if (ret == 0) {
		nand_dev_t * nand;
		nand_chip_t * chip;
		uint32_t size_mb;

		fprintf(f, "- NAND init...\n");

		if ((nand = nand_dev_register(ice, 0, DM365_CE0_BASE, NAND_BUS_8_BITS,
								 &dm365_nand_io8, &dm365_nand_ecc)) == NULL) {
			fprintf(f, " # nand_dev_register() fail!!!\n");
			ret = -1;
		} else {
			if ((chip = nand_chip_probe(nand, 0)) == NULL) {
				fprintf(f, " # nand_chip_probe() fail!!!\n");
				ret = -1;
			} else {
				fprintf(f, " - Manufacturer: 0x%02x, Device: 0x%02x\n",
						chip->mfg_id, chip->dev_id);

				fprintf(f, " - %d blocks, %d pages/block, %d+%d bytes/page\n", 
						chip->num_blocks, chip->pgs_per_blk, 
						chip->data_size, chip->spare_size);

				size_mb = ((chip->pgs_per_blk * chip->num_blocks) >> 9) * 
					(chip->data_size >> 8);

				fprintf(f, " - %d Mbits\n", size_mb);
			}
		}
	}

	if (ret == 0) {
		fprintf(f, "- NAND memory mapping init...\n");
/*
		if ((nand = nand_mem_map(ice, DM365_CE0_BASE,
								 &dm365_nand_io8, 
								 &dm365_nand_ecc, 
								 &dm365_nand_bb)) == NULL)  {
			fprintf(f, "- nand_mem_map_init() fail!!!\n");
			ret = -1; */
	}

	return ret;
}

int dm365_on_jtag_setup(FILE * f, const ice_drv_t * drv,
						const target_info_t * taerget)
{
	jtag_tap_t * tap;
	int has_arm = 0;
	int has_etb = 0;
	int has_icepick = 0;
#if 0
	int has_dsp = 0;
#endif
	uint32_t vec[1];
	int ret;
	int i;

	for (i = 0; i < 3; i++) {
		if (jtag_tap_get(&tap, i) != JTAG_OK) {
			break;
		}

		if (tap->idcode == 0x0792602f) {
			fprintf(f, " - %d (0x%08x) -> ARM926ejs\n", tap->pos, tap->idcode); 
			has_arm = i + 1;
		}
		if (tap->idcode == 0x2b900f0f) {
			fprintf(f, " - %d (0x%08x) -> ARM ETB11\n", tap->pos, tap->idcode); 
			has_etb = i + 1;
			has_etb = has_etb;
		}
		if (tap->idcode == 0x8b83e02f) {
			fprintf(f, " - %d (0x%08x) -> ICEPick C\n", tap->pos, 
					tap->idcode); 
			has_icepick = i + 1;
		}
#if 0
		if (tap->idcode == 0x1707c05e) {
			fprintf(f, " - %d (0x%08x) -> DSP\n", tap->pos, 
					tap->idcode); 
			has_dsp = i + 1;
		}
#endif
	}

	if (has_arm)
		return 0;

	if (!has_icepick)
		return -1;

	*vec = 0x7;
	if ((ret = jtag_ir_scan(vec, NULL, 6, JTAG_TAP_IRUPDATE)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_ir_scan() fail!");
		return ret;
	}

	*vec = 0x89;
	if ((ret = jtag_dr_scan(vec, NULL, 8, JTAG_TAP_DRUPDATE)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_dr_scan() fail!");
		return ret;
	}

	*vec = 0x2;
	if ((ret = jtag_ir_scan(vec, NULL, 6, JTAG_TAP_IRUPDATE)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_ir_scan() fail!");
		return ret;
	}

	/* ARM9 */
	*vec = 0xa0002108;
	if ((ret = jtag_dr_scan(vec, NULL, 32, JTAG_TAP_DRUPDATE)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_dr_scan() fail!");
		return ret;
	}

	/* ARM ETB11 */
	*vec = 0xa1002108;
	if ((ret = jtag_dr_scan(vec, NULL, 32, JTAG_TAP_DRUPDATE)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_dr_scan() fail!");
		return ret;
	}

	*vec = 0x3f;
	if ((ret = jtag_ir_scan(vec, NULL, 6, JTAG_TAP_IRPAUSE)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_ir_scan() fail!");
		return ret;
	}

	jtag_run_test(10, JTAG_TAP_IDLE);


	if ((ret = jtag_tap_insert(&tap, 0, 4, 0x2b900f0f)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_tap_insert() fail!");
		return ret;
	}
	if ((ret = jtag_tap_insert(&tap, 1, 4, 0x0792602f)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_tap_insert() fail!");
		return ret;
	}


#if 0
	int n;
	char s[64];

	for (len = 0, i = 0; i < cnt; i++) {
		n = irlen[i];
		vec_cat(vec, len, ir_idcode, n);
		len += n;
	}

	printf("[%s]", vec_fmt(s, vec, len));

	/* shift the vector trough the instruction register... */
	if ((ret = jtag_ir_scan(vec, vec, len, JTAG_TAP_IRUPDATE)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_ir_scan() fail!");
		return ret;
	}
#endif


	return 0;
}

const struct target_arch ti_dm365_arch = {
	.name = "DM365",
	.model = "generic",
	.vendor = "i-Lax",
	.cpu = &davinci_cpu,
	.sym = void_sym
};

const struct ice_mem_entry ti_dm365_mem[] = {
	{ .name = "sram0", .flags = MEM_32_BITS,
		.addr = { .base = 0x00010000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(16)},
		.op = &arm_ram_oper
	},
	{ .name = "sram1", .flags = MEM_32_BITS,
		.addr = { .base = 0x00014000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(16)},
		.op = &arm_ram_oper
	},
	{ .name = "sysbase", .flags = MEM_32_BITS,
		.addr = { .base = 0x01c40000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(4)},
		.op = &arm_ram_oper
	},
	{ .name = "ce0", .flags = MEM_16_BITS,
		.addr = { .base = 0x02000000, .offs = 0 },
		.blk = {.count = 1, .size = MEM_MiB(4)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "ce1", .flags = MEM_16_BITS,
		.addr = { .base = 0x04000000, .offs = 0 },
		.blk = {.count = 1, .size = MEM_MiB(4)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "ddr", .flags = MEM_32_BITS,
		.addr = { .base = 0x80000000, .offs = 0 },
		.blk = {.count = 256, .size = MEM_MiB(1)},
		.op = &arm_ram_oper
	},
	{ .name = "nand", .flags = MEM_8_BITS,
		.addr = { .base = 0xc0000000, .offs = 0 },
		.blk = {.count = 32 * 4096, .size = 512},
		.op = &nand_mem_oper
	},
	{ .name = "", .flags = 0, .addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct target_info ti_dm365 = {
	.name = "dm365",
	.arch = &ti_dm365_arch,
//	.cpu = &davinci_cpu,
	.mem = (struct ice_mem_entry *)ti_dm365_mem,

	.ice_drv = &armice_drv,
	.ice_cfg = (void *)&davinci_cfg,

	.jtag_clk_slow = 1000000,
	.jtag_clk_def = 15000000,
	.jtag_clk_max = 15000000,

	/* The target has a TRST connection */
	.has_trst = YES,
	/* The target has a nRST connection */
	.has_nrst = YES,
	/* Target supports adaptive clock */
	.has_rtck = YES,
	/* The preferred clock method is adaptive (RTCK) */
	.prefer_rtck = YES,
	/* Start with slow clock */
	.clk_slow_on_connect = YES,
	/* Set default clock after init  */
	.clk_def_on_init = YES,
	/* auto probe scan path */
	.jtag_probe = YES,
	/* hardware reset before ICE configure */
	.reset_on_config = YES,

	/* The preferred reset method is nRST */
	.reset_mode = RST_HARD,

	.start_addr = 0x00000000,

	.jtag_setup = (target_setup_script_t)dm365_on_jtag_setup,
	.on_init = (target_script_t)dm365_on_init,
	.on_halt = (target_script_t)davinci_on_halt,
	.on_run = NULL,
	.reset_script = (target_script_t)davinci_reset,
	.probe = (target_script_t)davinci_probe,
	.test = (target_test_t)davinci_test
};

const struct target_info ti_dm365hs = {
	.name = "dm365hs",
	.arch = &ti_dm365_arch,
//	.cpu = &davinci_cpu,
	.mem = (struct ice_mem_entry *)ti_dm365_mem,

	.ice_drv = &armice_drv,
	.ice_cfg = (void *)&davinci_cfg,

	.jtag_clk_slow = 1000000,
	.jtag_clk_def = 15000000,
	.jtag_clk_max = 15000000,

	/* The target has a TRST connection */
	.has_trst = YES,
	/* The target has a nRST connection */
	.has_nrst = YES,
	/* Target supports adaptive clock */
	.has_rtck = NO,
	/* The preferred clock method is adaptive (RTCK) */
	.prefer_rtck = YES,
	/* Start with slow clock */
	.clk_slow_on_connect = YES,
	/* Set default clock after init  */
	.clk_def_on_init = YES,
	/* auto probe scan path */
	.jtag_probe = YES,
	/* hardware reset before ICE configure */
	.reset_on_config = YES,

	/* The preferred reset method is nRST */
	.reset_mode = RST_HARD,

	.start_addr = 0x00000000,

	.jtag_setup = (target_setup_script_t)dm365_on_jtag_setup,
	.on_init = (target_script_t)dm365_on_init,
	.on_halt = (target_script_t)davinci_on_halt,
	.on_run = NULL,
	.reset_script = (target_script_t)davinci_reset,
	.probe = (target_script_t)davinci_probe,
	.test = (target_test_t)davinci_test
};

