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

#include "target/at91sam9.h"
#include <sys/os.h>

/*
 * System Controller Peripherals Base Addresses 
 */
#define AT91_BASE_ECC       0xffffe800
#define AT91_BASE_SDRAMC    0xffffea00
#define AT91_BASE_SMC       0xffffec00
#define AT91_BASE_MATRIX    0xffffee00
#define AT91_BASE_CCFG      0xffffef10
#define AT91_BASE_AIC       0xfffff000
#define AT91_BASE_DBGU      0xfffff200
#define AT91_BASE_PIOA      0xfffff400
#define AT91_BASE_PIOA      0xfffff400
#define AT91_BASE_PIOB      0xfffff600
#define AT91_BASE_PIOC      0xfffff800
#define AT91_BASE_PMC       0xfffffc00
#define AT91_BASE_CKGR      0xfffffc20
#define AT91_BASE_RSTC      0xfffffd00
#define AT91_BASE_SHDWC     0xfffffd10
#define AT91_BASE_RTTC      0xfffffd20
#define AT91_BASE_PITC      0xfffffd30
#define AT91_BASE_WDTC      0xfffffd40
#define AT91_BASE_GPBR      0xfffffd50

#define AT91SAM9260_CHIP_ID 0x019803A2
#define AT91SAM9260_CHIP_ID_MASK 0xfffffff0

#define AT91SAM9260_JTAG_ID 0x05B1303F
#define AT91SAM9260_TAP_ID 0x0792603F

#define MCLK_NS 10
#define BOOT_FLASH_TIME_NS 90
#define FLASH_CYCLE ((BOOT_FLASH_TIME_NS + MCLK_NS - 1) / MCLK_NS)

const struct target_sym at91sam9_sym[] = {
	{0, "", 0} 
};

static void at91_cs0_init(FILE * f, const struct ice_drv * ice)
{
	ice_wr32(ice, AT91_BASE_SMC + SMC_MODE0, 
			 SMC_DBW_WIDTH_16_BITS | SMC_READMODE | SMC_WRITEMODE | 
			 SMC_TDF_MODE | SMC_TDF_CYCLES(1));

	ice_wr32(ice, AT91_BASE_SMC + SMC_MODE0, 
			 SMC_DBW_WIDTH_16_BITS | SMC_READMODE | SMC_WRITEMODE | 
			 SMC_TDF_MODE | SMC_TDF_CYCLES(1));

	ice_wr32(ice, AT91_BASE_SMC + SMC_SETUP0, 
			 SMC_NWESETUP(0) | SMC_NCSSETUPWR(0) |
			 SMC_NRDSETUP(FLASH_CYCLE - 4) | SMC_NCSSETUPRD(0));

	ice_wr32(ice, AT91_BASE_SMC + SMC_PULSE0, 
			 SMC_NWEPULSE(4) | SMC_NCSPULSEWR(4) | 
			 SMC_NRDPULSE(4) | SMC_NCSPULSERD(FLASH_CYCLE));

	ice_wr32(ice, AT91_BASE_SMC + SMC_CYCLE0, 
			 SMC_NWECYCLE(FLASH_CYCLE) | SMC_NRDCYCLE(FLASH_CYCLE));
}


/* TODO: allow multiple SDRAM controller configuration */
#ifdef AT91SAM9263
#define AT91_SDRAMC AT91_SDRAMC0
#define AT91_EBI_SDRAM AT91_EBI0_SDRAM 
#define EBI_PIO AT91_PIOD
#else
#define EBI_PIO AT91_PIOC
#endif

/*
 * Micron MT48LCxxxx SDRAM mode register definitions
 */
#define SINGLE (1 << 9)
#define BURST (0 << 9)
#define SEQUENTIAL (0 << 0)
#define INTERLEAVED (1 << 3)
#define CAS_LATENCY(X) ((X) << 4)
#define BURST_LEN_1 0
#define BURST_LEN_2 1
#define BURST_LEN_4 2
#define BURST_LEN_8 3
#define BURST_LEN_PAGE 7

#define AT91_EBI_SDRAM 0x20000000

void at91_sdram_config(const ice_drv_t * ice, int chips, int cols, 
					   int rows, int banks)
{
	uint32_t mem = AT91_EBI_SDRAM;
	uint32_t mr;
	int i;

	if (chips == 2) {
		/* Configure  SDRAM - 32bits mode */
		ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_CR, 
				 SDRAMC_NC(cols) | SDRAMC_NR(rows) | SDRAMC_NB(banks) | 
				 SDRAMC_DBW_32_BITS | SDRAMC_CAS_2 | 
				 SDRAMC_TXSR_8 | SDRAMC_TRAS_5 | SDRAMC_TRCD_2 | 
				 SDRAMC_TRP_2 | SDRAMC_TRC_7 | SDRAMC_TWR_2);
	} else {
		/* 16 bits mode */
		ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_CR, 
				 SDRAMC_NC(cols) | SDRAMC_NR(rows) | SDRAMC_NB(banks) | 
				 SDRAMC_DBW_16_BITS | SDRAMC_CAS_2 | 
				 SDRAMC_TXSR_8 | SDRAMC_TRAS_5 | SDRAMC_TRCD_2 | 
				 SDRAMC_TRP_2 | SDRAMC_TRC_7 | SDRAMC_TWR_2);
	}

	/* 1. A minimum pause of 200us is provided to precede any signal toggle */
	__os_sleep(2);

	ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_MR, SDRAMC_MODE_NOP_CMD);
	ice_wr32(ice, mem, 0);

	/* 2. A Precharge All command is issued to the SDRAM */
	ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_MR, SDRAMC_MODE_PRCGALL_CMD);
	ice_wr32(ice, mem, 0);

	/* 3. Eight Auto-refresh are provided */
	ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_MR,  SDRAMC_MODE_RFSH_CMD);
	for (i = 0; i < 8; i++) {
		ice_wr32(ice, mem, 0);
	}

	/* 4. A mode register cycle is issued to program the SDRAM parameters */
//	mr = BURST | CAS_LATENCY(2) | SEQUENTIAL | BURST_LEN_PAGE;
	mr = BURST | CAS_LATENCY(2) | SEQUENTIAL | BURST_LEN_1;
	ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_MR, SDRAMC_MODE_LMR_CMD);
	ice_wr32(ice, mem, mr | (mr << 16));

	/* 5. A Normal Mode Command is provided, 3 clocks after tMRD is set */
	ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_MR, SDRAMC_MODE_NORMAL_CMD);
	ice_wr32(ice, mem +  20, 0);

	/* 6. Write refresh rate into SDRAMC refresh timer COUNT register */
	ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_TR, SDRAMC_COUNT(1562));
}

#define PATTERN_A 0x11112222
#define PATTERN_B 0x44448888

/*
 * Configure the SDRAM controller
 * Automatically probe
 */
static int at91_sdram_init(FILE * f, const ice_drv_t * ice)
{
	uint32_t mem = AT91_EBI_SDRAM;
	uint32_t probe;
	uint32_t data;
	int chips;
	int cols;
	int rows;
	int banks;
	int i;

	/* Init MATRIX to support EBI0 CS1 for SDRAM */
#ifdef AT91SAM9263
	ice_wr32(ice,  AT91_BASE_CCFG + CCFG_EBI0CSA,  
			 EBI_CS1A | EBI_VDDIOMSEL | EBI_DBPUC);
#else
	ice_wr32(ice,  AT91_BASE_CCFG + CCFG_EBICSA, 
			 EBI_CS1A | EBI_VDDIOMSEL | EBI_DBPUC);
#endif

	ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_LPR, SDRAMC_LPCB_DISABLE);
	ice_wr32(ice, AT91_BASE_SDRAMC + SDRAMC_MDR, SDRAMC_MD_SDRAM);

	/*  Configure PIO as peripheral (D16/D31) */
	ice_wr32(ice, AT91_BASE_PIOC + PIO_ASR, 0xffff0000);
	ice_wr32(ice, AT91_BASE_PIOC + PIO_PDR, 0xffff0000);
	ice_wr32(ice, AT91_BASE_PIOC + PIO_PUER, 0xffff0000);

	chips = 2;
	cols = 8;
	rows = 11;
	banks = 4;

	/* config to 32 bits bus (2 chips) */
	at91_sdram_config(ice, chips, cols, rows, banks);

	/* probing the number of chips */
	ice_wr32(ice, mem, PATTERN_A);
	ice_wr32(ice, mem + 4, PATTERN_B);
	ice_rd32(ice, mem, &probe);
	if (probe == PATTERN_A) {
		chips = 2;
	} else {
		at91_sdram_config(ice, 1, cols, rows, banks);
		ice_wr32(ice, mem, PATTERN_A);
		ice_wr32(ice, mem + 4, PATTERN_B);
		ice_rd32(ice, mem, &data);
		if (data == PATTERN_A) {
			chips = 1;
		} else{
			return -1;
		}
	}

	/* probing the memory geometry */
	/* number of columns */
	for (i = 8; i <= 11; i++) {
		at91_sdram_config(ice, chips, i, rows, banks);
		ice_wr32(ice, mem, 0);
		probe = (1 << i) * chips / 4;
		ice_wr32(ice, mem + (4 * probe), PATTERN_A);
		ice_rd32(ice, mem, &data);
		if (data != 0)
			break;
		cols = i;
	}

	/* number of rows */
	for (i = 11; i <= 13; i++) {
		at91_sdram_config(ice, chips, cols, i, banks);
		ice_wr32(ice, mem, 0);
		probe = (1 << cols) * (1 << i) * chips / 4;
		ice_wr32(ice, mem + (4 * probe), PATTERN_A);
		ice_rd32(ice, mem, &data);
		if (data != 0)
			break;
		rows = i;
	}

	/* number of banks */
	at91_sdram_config(ice, chips, cols, cols, 4);
	ice_wr32(ice, mem, 0);
	probe = (1 << cols) * (1 << i) * chips * banks / 8;
	ice_wr32(ice, mem + (4 * probe), PATTERN_A);
	ice_rd32(ice, mem, &data);
	if (data == 0)
		banks = 4;
	else
		banks = 2;

	at91_sdram_config(ice, chips, cols, rows, banks);

	fprintf(f, "SDRAM: %d chips, %d cols, %d rows, %d banks, %d MiB\n",
		   chips, cols, rows, banks,
		   ((1 << rows) * (1 << cols) * chips * banks * 2) / (1024 * 1024));

	return 0;
}




int at91sam9_on_init(FILE * f, const ice_drv_t * ice, ice_mem_entry_t * mem)
{
	uint32_t fmain_khz;
	uint32_t fpll_khz;
	uint32_t tck_khz;
	uint32_t data;
	uint32_t mclk;
	int pll_mul;
	int pll_div;
	int again;

	/* ATTENTION: all clock frequencies are in KiloHertz to avoid
	 * multiplication overflow ... */
	/* main clock (KHz) configure 
	 *
	 * We select the main clock as 100MHz
	 */
	mclk = 100000;
	fpll_khz = mclk * 2;

#if 0
	fprintf(f, "- Reset peripherals ...\n");
	/* reset the peripherals */
	ice_wr32(ice, AT91_BASE_RSTC + RSTC_CR, 
			  RSTC_KEY(0xa5) | RSTC_PERRST);

	fprintf(f, "- Watchdog disable ...\n");
	/* Disable the watchdog */
	ice_wr32(ice,  AT91_BASE_WDTC + WDTC_WDMR, WDTC_WDDIS);

	fprintf(f, "- Set master clock to slow clock ...\n");
	/* 0. Set the master clock to the slow clock : */
	ice_wr32(ice, AT91_BASE_PMC + PMC_MCKR, PMC_CSS_SLOW_CLK);

	fprintf(f, "- Main oscillator disable ...\n");
	/* disable the Main Oscillator */
	ice_wr32(ice, AT91_BASE_CKGR + CKGR_MOR, 0x00000000);
	/* disable the PLL A */
	ice_wr32(ice, AT91_BASE_CKGR + CKGR_PLLAR, 0x00000000);

	fprintf(f, "- Waiting for slow clock ...\n");
	for (again = 16; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_MCKRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: slow clock fail!\n", __func__);
			return -1;
		}
	}
#endif

	fprintf(f, "- Main oscillator enable...\n");
	/* 1. Enabling the Main Oscillator: */
	ice_wr32(ice, AT91_BASE_CKGR + CKGR_MOR, CKGR_OSCOUNT(32) | CKGR_MOSCEN);

	/* wait for main startup time expiration */
	for (again = 16; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_MOSCS)
			break;
		if (again == 0) {
			fprintf(f, "# Main oscillator startup fail!\n");
	//		return -1;
		}
	}


	/* 2. Checking the Main Oscillator Frequency: */
	for (again = 16; ; again--) {
		ice_rd32(ice, AT91_BASE_CKGR + CKGR_MCFR, &data);
		if (data & CKGR_MAINRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: main oscillator startup fail!\n", __func__);
			return -1;
		}
	}

	fmain_khz = (((data & CKGR_MAINF) * (32768 / 16)) + 1000) / 1000;

	fprintf(f, "- Main oscillator detected frequency: %d.%03d MHz\n", 
			fmain_khz / 1000, fmain_khz % 1000);

	fprintf(f, "- Switching to main clock ...\n");
	/* Master clock = Main Clock */
	ice_wr32(ice, AT91_BASE_PMC + PMC_MCKR, 
			 PMC_CSS_MAIN_CLK | PMC_PRES_CLK | PMC_MDIV_2);
	
	for (again = 16; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_MCKRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: master clock set fail!\n", __func__);
			return -1;
		}
	}

	if (fmain_khz > 19500) {
		fmain_khz = 20000;
		pll_mul = 20;
		pll_div = 2;
	} else {
		if (fmain_khz > 18000) {
			fmain_khz = 18432;
			pll_mul = 727;
			pll_div = 67;
		} else {
			if (fmain_khz > 16200) {
				fmain_khz = 16368;
				pll_mul = 892;
				pll_div = 73;
			} else {
				fmain_khz = 12000;
				pll_mul = 50;
				pll_div = 3;
			}
		}
	}


	tck_khz = fmain_khz / 8;

	fprintf(f, "- Increasing the JTAG frequency to: %d.%03d MHz...\n", 
			tck_khz / 1000, tck_khz % 1000);
	jtag_tck_freq_set(tck_khz * 1000);

    fpll_khz = (fmain_khz * pll_mul) / pll_div;
	fprintf(f, "- Fmain=%d.%03d Fpll=%d.%03d mul=%d div=%d\n", 
			fmain_khz / 1000, fmain_khz % 1000, 
			fpll_khz / 1000, fpll_khz % 1000, 
			pll_mul, pll_div);


	/*
     * fpll = (fmain * pll_mul) / pll_div
	 *
	 * pll_mul = (fusb * n * pll_div) / fmain
	 *
	 */

	/* 3. Setting PLL and divider: */
	/* configure the PLL A */
	fprintf(f, "- Configuring PLL A ...\n");
	ice_wr32(ice, AT91_BASE_CKGR + CKGR_PLLAR, 
			  CKGR_PLLMUL(pll_mul) | CKGR_PLLCOUNT(40) | 
			  CKGR_PLLDIV(pll_div) | CKGR_SRCA | CKGR_OUTA_2);

	/* Wait the pll lock (until PMC Status register LOCK bit is set) */
	for (again = 256; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_LOCKA)
			break;
		if (again == 0) {
			fprintf(f, " %s: pll lock fail!\n", __func__);
			return -1;
		}
	}

	/* 4. Selection of Master Clock and Processor Clock */
/*
   If a new value for CSS field corresponds to PLL Clock,
     – Program the PRES field in the PMC_MCKR register.
     – Wait for the MCKRDY bit to be set in the PMC_SR register.
     – Program the CSS field in the PMC_MCKR register.
     – Wait for the MCKRDY bit to be set in the PMC_SR register.
   If a new value for CSS field corresponds to Main Clock or Slow Clock,
     – Program the CSS field in the PMC_MCKR register.
     – Wait for the MCKRDY bit to be set in the PMC_SR register.
     – Program the PRES field in the PMC_MCKR register.
     – Wait for the MCKRDY bit to be set in the PMC_SR register
	 */

	DCC_LOG(LOG_TRACE, "master and processor clock clock selection...");

	fprintf(f, "- Switching to PLL A clock ...\n");
	ice_wr32(ice, AT91_BASE_PMC + PMC_MCKR, 
			 PMC_CSS_PLLA_CLK | PMC_PRES_CLK | PMC_MDIV_2);

	for (again = 256; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_MCKRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: master clock set fail!\n", __func__);
			return -1;
		}
	}

#if 0
	tck_khz = fpll_khz / 8;
	fprintf(f, "- Increasing the JTAG frequency to: %d.%03d MHz...\n", 
			tck_khz / 1000, tck_khz % 1000);
	jtag_tck_freq_set(tck_khz * 1000);
#endif

	fprintf(f, "- Configure nRST pin to generate a reset...\n");
	/* configure the NRST pint to generate a reset */
	ice_wr32(ice, AT91_BASE_RSTC + RSTC_MR, 
			  RSTC_KEY(0xa5) | RSTC_URSTEN);

	fprintf(f, "- Bus matrix remap...\n");
	ice_wr32(ice, AT91_BASE_MATRIX + MATRIX_MRCR, 
			 MATRIX_RCA926I | MATRIX_RCA926D);

	fprintf(f, "- NOR flash configuration, (EBI CS0, 16bits)...\n");
	at91_cs0_init(f, ice);

	fprintf(f, "- SDRAM configuration...\n");
	at91_sdram_init(f, ice);

	return 0;
}

int at91sam9_on_halt(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	/* Disable the watchdog */
	return ice_wr32(ice, AT91_BASE_WDTC + WDTC_WDMR, WDTC_WDDIS);
}

int at91sam9_reset(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem)
{
	int again;
	uint32_t data;

	DCC_LOG(LOG_TRACE, "master clock set to slow clock...");
	/* 0. Set the master clock to the slow clock : */
	ice_wr32(ice, AT91_BASE_PMC + PMC_MCKR, 0x00000000);

	for (again = 4096; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_MCKRDY)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "master clock not ready!");
			return -1;
		}
	}

	DCC_LOG(LOG_TRACE, "peripheral reset...");
	/* peripheral reset */
	ice_wr32(ice, AT91_BASE_RSTC + RSTC_CR, 
			  RSTC_KEY(0xa5) | RSTC_PERRST);

	DCC_LOG(LOG_TRACE, "core reset...");
	/* Processor reset */
	ice_wr32(ice, AT91_BASE_RSTC + RSTC_CR, 
			  RSTC_KEY(0xa5) | RSTC_PROCRST);

//	__os_sleep(100);

	return 0;
}

int at9sam9260_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t cidr;

	/* Chip identification.... */
	ice_rd32(ice, AT91_BASE_DBGU + DBGU_CIDR, &cidr);

	DCC_LOG1(LOG_TRACE, "CIDR=0x%08x\n", cidr);
	fprintf(f, "cidr=0x%08x ", cidr);


	fprintf(f, "- CHIP ID: ver=%d proc=%d arch=0x%02x nvtp=%d ext=%d\n", 
			CIDR_VER(cidr), CIDR_EPROC(cidr), 
			CIDR_ARCH(cidr), CIDR_NVPTYP(cidr), CIDR_EXT(cidr));

	return ((cidr & AT91SAM9260_CHIP_ID_MASK) == AT91SAM9260_CHIP_ID) ? 1 : 0;
}

const struct target_cpu at91sam9_cpu = {
	.family = "AT91SAM9",
	.model = "---",
	.vendor = "ATMEL",
	.irlength = 4,
	.idmask = 0xfffff0ff,
	.idcomp = 0x0792603f,
	.sym = at91sam9_sym
};

const struct armice_cfg at91sam9_cfg = {
	.endianness = LITTLE_ENDIAN,
	.work_addr = 0x00200000,
	.work_size = MEM_KiB(4),
};

