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
 * @file davinci.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <crc.h>
#include <stdlib.h>

#include "target.h"
#include "armice.h"
#include "script.h"
#include "davinci/dm36x.h"
#include "fmt.h"
#include "nand.h"
#include "target/davinci.h"

/* write into PRTCSS register */
int davinci_prtcss_wr(const struct ice_drv * ice, int reg, 
					  uint8_t * data, int len)
{
	uint32_t ctrl;
	uint32_t mask;
	uint32_t buf;
	int tmo;
	int i;

	for (tmo = 1000;;) {
		ice_rd32(ice, DM36X_PRTCIF_BASE + PRTCIF_CTRL, &ctrl);
		if ((ctrl & CTRL_BUSY) == 0)
			break;
		if (--tmo == 0)
			return -1;
	}

	if (len >  8)
		len = 8;

	mask = ~(0xffffffff << len);

	if (len > 4) {
		ctrl = CTRL_SIZE_8 | CTRL_WR | CTRL_BENU(mask >> 4) | 
			CTRL_BENL(mask & 0xf) | CTRL_ADRS(reg);
	} else {
		ctrl = CTRL_SIZE_4 | CTRL_WR | CTRL_BENU(0) | 
			CTRL_BENL(mask) | CTRL_ADRS(reg);
	}

	ice_wr32(ice, DM36X_PRTCIF_BASE + PRTCIF_CTRL, ctrl);

	/* write the lower bytes */
	buf = 0;
	for (i = 0; i < (len > 4 ? 4 : len); i++)
		buf = (buf << 8) | data[i];
	ice_wr32(ice, DM36X_PRTCIF_BASE + PRTCIF_LDATA, buf);

	if (len > 4) {
		buf = 0;
		for (; i < len; i++)
			buf = (buf << 8) | data[i];
		/* write the upper bytes */
		ice_wr32(ice, DM36X_PRTCIF_BASE + PRTCIF_UDATA, buf);
	}

	return len;
}

/* Read from PRTCSS registers */
int davinci_prtcss_rd(const struct ice_drv * ice, int reg, 
					  uint8_t * data, int len)
{
	uint32_t ctrl;
	uint32_t mask;
	uint32_t buf;
	int tmo;
	int i;

	for (tmo = 1000;;) {
		ice_rd32(ice, DM36X_PRTCIF_BASE + PRTCIF_CTRL, &ctrl);
		if ((ctrl & CTRL_BUSY) == 0)
			break;
		if (--tmo == 0)
			return -1;
	}

	if (len >  8)
		len = 8;

	mask = ~(0xffffffff << len);

	if (len > 4) {
		ctrl = CTRL_SIZE_8 | CTRL_RD | CTRL_BENU(mask >> 4) | 
			CTRL_BENL(mask & 0xf) | CTRL_ADRS(reg);
	} else {
		ctrl = CTRL_SIZE_4 | CTRL_RD | CTRL_BENU(0) | 
			CTRL_BENL(mask) | CTRL_ADRS(reg);
	}

	ice_wr32(ice, DM36X_PRTCIF_BASE + PRTCIF_CTRL, ctrl);

	/* read the lower bytes */
	ice_rd32(ice, DM36X_PRTCIF_BASE + PRTCIF_LDATA, &buf);
	for (i = 0; i < (len > 4 ? 4 : len); i++) {
		data[i] = buf & 0xff;
		buf >>= 8;
	}

	if (len > 4) {
		/* read the upper bytes */
		ice_rd32(ice, DM36X_PRTCIF_BASE + PRTCIF_UDATA, &buf);
		for (; i < len; i++) {
			data[i] = buf & 0xff;
			buf >>= 8;
		}
	}

	return len;
}

#if 0
int davinci_on_init(FILE * f, const ice_drv_t * ice, ice_mem_entry_t * mem)
{
	uint8_t rtc_ctrl[1];

	fprintf(f, "Davinci init script...\n");

	davinci_prtcss_rd(ice, PRTCSS_RTC_CTRL, rtc_ctrl, 1);

	fprintf(f, "PRTCSS_RTC_CTRL=%02x\n", *rtc_ctrl);

	if (*rtc_ctrl & CTRL_WEN) {
		fprintf(f, "- PRTCSS WDT enabled, disabling it...\n");
		while (*rtc_ctrl & CTRL_WDTBUSY) {
			davinci_prtcss_rd(ice, PRTCSS_RTC_CTRL, rtc_ctrl, 1);
		}
		*rtc_ctrl &= ~CTRL_WEN; 
		davinci_prtcss_wr(ice, PRTCSS_RTC_CTRL, rtc_ctrl, 1);
	}

	return 0;
}
#endif

#if ENABLE_DAVINCI_TEST

int davinci_pll_info(FILE * f, const ice_drv_t * ice, 
					 uint32_t pll_base, int nsys)
{
	uint32_t data;
	uint32_t pllm;
	uint32_t prediv;
	uint32_t postdiv;
	uint32_t plln;
	uint32_t fosc = 24000000;
	uint32_t fvco;
	uint32_t fpll;
	uint32_t div;
	char s[32];

	fprintf(f, " - PLL_ADDR=0x%08x, FOSC=%s\n", pll_base, fmt_freq(s, fosc));

	ice_rd32(ice, pll_base + PLL_PID, &data);
	if (data != DM36X_PLL_PID) {
		fprintf(f, "# invalid PID: %08x\n", data);
		return -1;
	}

	ice_rd32(ice, pll_base + PLL_PLLM, &pllm);
	ice_rd32(ice, pll_base + PLL_PREDIV, &prediv);

	pllm *= 2;
	plln = (prediv & PREDIV_MASK) + 1; 
	fvco = (uint64_t)((uint64_t)fosc * (uint64_t)pllm) / plln;

	ice_rd32(ice, pll_base + PLL_POSTDIV, &postdiv);
	fpll = fvco / ((postdiv & PLLDIV_MASK) + 1); 

	fprintf(f, " - M=%d N=%d FVCO=%s ", pllm, plln, fmt_freq(s, fvco));
	
	fprintf(f, "FPLL=%s\n", fmt_freq(s, fpll));

	ice_rd32(ice, pll_base + PLL_PLLDIV1, &div);
	fprintf(f, " - SYSCLK1=%s\n", 
			fmt_freq(s, fpll / ((div & PLLDIV_MASK) + 1)));

	ice_rd32(ice, pll_base + PLL_PLLDIV2, &div);
	fprintf(f, " - SYSCLK2=%s\n", 
			fmt_freq(s, fpll / ((div & PLLDIV_MASK) + 1)));

	ice_rd32(ice, pll_base + PLL_PLLDIV3, &div);
	fprintf(f, " - SYSCLK3=%s\n", 
			fmt_freq(s, fpll / ((div & PLLDIV_MASK) + 1)));

	ice_rd32(ice, pll_base + PLL_PLLDIV4, &div);
	fprintf(f, " - SYSCLK4=%s\n", 
			fmt_freq(s, fpll / ((div & PLLDIV_MASK) + 1)));

	ice_rd32(ice, pll_base + PLL_PLLDIV5, &div);
	fprintf(f, " - SYSCLK5=%s\n",
			fmt_freq(s, fpll / ((div & PLLDIV_MASK) + 1)));

	if (nsys == 5)
		return 0;

	ice_rd32(ice, pll_base + PLL_PLLDIV6, &div);
	fprintf(f, " - SYSCLK6=%s\n", 
			fmt_freq(s, fpll / ((div & PLLDIV_MASK) + 1)));

	if (nsys == 6)
		return 0;

	ice_rd32(ice, pll_base + PLL_PLLDIV7, &div);
	fprintf(f, " - SYSCLK7=%s\n", 
			fmt_freq(s, fpll / ((div & PLLDIV_MASK) + 1)));

	if (nsys == 7)
		return 0;

	ice_rd32(ice, pll_base + PLL_PLLDIV8, &div);
	fprintf(f, "- SYSCLK8=%s\n", fmt_freq(s, fpll / ((div & PLLDIV_MASK) + 1)));

	if (nsys == 8)
		return 0;

	ice_rd32(ice, pll_base + PLL_PLLDIV9, &div);
	fprintf(f, "- SYSCLK9=%s\n", fmt_freq(s, fpll / ((div & PLLDIV_MASK) + 1)));

	return 0;
}



int davinci_sys_test(FILE * f, const ice_drv_t * ice)
{
	uint32_t clkctl;

	fprintf(f, "== DM36x PLL test == \n\n");
	fprintf(f, "- PLL 1\n");
	davinci_pll_info(f, ice, DM36X_PLL1_BASE, 9);
	fprintf(f, "- PLL 2\n");
	davinci_pll_info(f, ice, DM36X_PLL2_BASE, 5);
	fprintf(f, "\n");

	ice_rd32(ice, DM36X_SYSTEM_BASE + SYS_PERI_CLKCTL, &clkctl);

	fprintf(f, "- CLKCTL: 0x%08x\n", clkctl);

	fprintf(f, "- ARM926 clock: %s\n", 
			clkctl & PERI_CLKCTL_ARMCLKS ? "PLLC2 SYSCLK2" : "PLLC1 SYSCLK2");

	fprintf(f, "- DDR2 clock: %s\n", 
			clkctl & PERI_CLKCTL_DDRCLKS ? "PLLC2 SYSCLK3" : "PLLC1 SYSCLK7");

	return 0;
}

int davinci_on_halt(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	/* Disable the watchdog */
	return 0;
}

int davinci_reset(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem)
{

	return 0;
}

int davinci_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	return 0;
}


int prtcss_test(FILE * f, const struct ice_drv * ice)
{
	uint32_t tmo_top;
	uint32_t tmo_cmp;
	uint8_t tm[8];
	uint8_t sec;
	int i;

	fprintf(f, "== DM36x PRTCSS test == \n\n");

	davinci_prtcss_rd(ice, PRTCSS_RTC_SEC, tm, 5);

	for (i = 0; i < 15; i++) {
		tmo_top = __os_ms_ticks() + 2000;
		do {
			tmo_cmp = __os_ms_ticks();
			if ((int32_t)(tmo_cmp - tmo_top) < 0) {
				fprintf(f, "#ERROR: davinci_prtcss_rd() polling timeout!\n");
				return -1;
			}
			sec = tm[0];
			if (davinci_prtcss_rd(ice, PRTCSS_RTC_SEC, tm, 5) < 0) {
				fprintf(f, "#ERROR: davinci_prtcss_rd() fail!\n");
				return -1;
			}
		} while (sec == tm[0]);

		fprintf(f, " - RTC: %0d %02x:%02x:%02x\n", 
				(tm[4] << 8) + tm[3], tm[2], tm[1], tm[0]);
	}

	return 0;
}

#include "hexdump.h"

int show_hex32(FILE * f, uint32_t  addr, const void * buf, int count);

int test_nand_write(FILE * f, nand_dev_t * nand, int block, int n)
{
	uint8_t buf[nand->chip->data_size];
	int start = block * nand->chip->pgs_per_blk;
	int ret;
	int pg;
	int i;

	fprintf(f, " - Writing block %d ...\n", block);

	for (pg = start; pg < (start + n); pg++) {
		for (i = 0; i < nand->chip->data_size; i++) {
			buf[i] = rand();
		}
//		fprintf(f, "   - %4d -> CRC=%04x\n", pg,
//				crc16ccitt(0, buf, nand->chip->data_size));

		if ((ret = nand_page_write(nand, pg, buf)) < 0) {
			fprintf(f, "#ERROR: nand_page_write(), ");
			return ret;
		}
	}

//	show_hex8(f, 0, buf, nand->chip->data_size);
	return 0;
}

int test_nand_read(FILE * f, nand_dev_t * nand, int block, int n)
{
	uint8_t buf[nand->chip->data_size];
	int start = block * nand->chip->pgs_per_blk;
	int ret = 0;
	int pg;

	fprintf(f, " - Reading block %d ...\n", block);
	for (pg = start; pg < (start + n); pg++) {
		ret = nand_page_read(nand, pg, buf);
//		fprintf(f, "   - %4d -> CRC=%04x\n", pg,
//				crc16ccitt(0, buf, nand->chip->data_size));

		if (ret < 0) {
			fprintf(f, "#ERROR: nand_page_read()\n");
			break;
		}
	}
//	show_hex8(f, 0, buf, nand->chip->data_size);
	return ret;
}

int dm365_nand_test(FILE * f, const struct ice_drv * ice)
{
#if 0
	nand_chip_t chip;
	nand_dev_t nand;
	uint32_t size_mb;
	struct timespec start;
	struct timespec end;
	uint32_t ms;
	int size;
	int block;
	int pgs;

	fprintf(f, "== DM36x NAND test == \n\n");

	/* Chip configuration */
	if (nand_chip_init(ice, &chip, &dm365_nand_io8, DM365_CE0_BASE) < 0) {
		fprintf(f, "#ERROR: nand_chip_init()!\n");
		return -1;
	}

		if ((nand = nand_dev_register(ice, 0, DM365_CE0_BASE, NAND_BUS_8_BITS,
								 &dm365_nand_io8, &dm365_nand_ecc)) == NULL) {
			fprintf(f, " # nand_dev_register() fail!!!\n");
			ret = -1;
		} else {
			if ((chip = nand_chip_probe(ice, nand, 0)) == NULL) {
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



	if (nand_detect(ice, &chip, &dm365_nand_io8) < 0) {
		fprintf(f, "#ERROR: nand_detect()!\n");
		return -1;
	}

	fprintf(f, " - Manufacturer: 0x%02x, Device: 0x%02x\n",
			chip.mfg_id, chip.dev_id);

	fprintf(f, " - %d blocks, %d pages/block, %d+%d bytes/page\n", 
			chip.num_blocks, chip.pgs_per_blk, 
			chip.data_size, chip.spare_size);

	size_mb = ((chip.pgs_per_blk * chip.num_blocks) >> 9) * 
		(chip.data_size >> 8);

	fprintf(f, " - %d Mbits\n", size_mb);

	/* NAND operations */

	if (nand_open(&nand, ice, &chip, &dm365_nand_io8, &dm365_nand_ecc, 
				  &dm365_nand_bb) < 0) {
		fprintf(f, "#ERROR: nand_open()!\n");
		return -1;
	}

//	for (i =0; i < 256; i++) {
//		test_nand_read(f, &nand, i);
//	}

	pgs = chip.pgs_per_blk;
//	pgs = 1;

	block = rand() % chip.num_blocks;
//	block = 1069;

	if (nand_block_erase(&nand, block) < 0) {
		fprintf(f, "#ERROR: nand_block_erase()!\n");
		return -1;
	}

	size = chip.data_size * pgs;

	clock_gettime(CLOCK_REALTIME, &start);

	test_nand_write(f, &nand, block, pgs);

	clock_gettime(CLOCK_REALTIME, &end);
	ms = ((end.tv_sec - start.tv_sec) * 1000) + 
		((end.tv_nsec - start.tv_nsec) / 1000000);
	fprintf(f, " - size=%d tm=%d[ms] speed=%d[bytes/sec]\n", 
			size, ms, (size * 1000) / ms);

	clock_gettime(CLOCK_REALTIME, &start);

	test_nand_read(f, &nand, block, pgs);

	clock_gettime(CLOCK_REALTIME, &end);
	ms = ((end.tv_sec - start.tv_sec) * 1000) + 
		((end.tv_nsec - start.tv_nsec) / 1000000);
	fprintf(f, " - size=%d tm=%d[ms] speed=%d[bytes/sec]\n", 
			size, ms, (size * 1000) / ms);
#endif
	return 0;
}

int davinci_test(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem, uint32_t val)
{
	fprintf(f, "\n");
	switch (val) {
	case 1:
		return prtcss_test(f, ice);
	case 2:
		return davinci_sys_test(f, ice);
		break;
	case 3:
		dm365_nand_test(f, ice);
		fprintf(f, "\n");
		break;
	default:
		fprintf(f, "# Valid test numbers:\n");
		fprintf(f, "  1 - PRTCSS (clock)\n");
		fprintf(f, "  2 - PLL\n");
		fprintf(f, "  3 - NAND detect\n");
		return -1;
	};

	return 0;
}

#endif /* ENABLE_DAVINCI_TEST */

const struct target_sym davinci_sym[] = {
	{0, "", 0, 0} 
};

const struct target_cpu davinci_cpu = {
	.family = "DAVINCI",
	.model = "v5t",
	.vendor = "TI",
	.irlength = 4,
	.idmask = 0xfffff0ff,
	.idcomp = 0x0792602f,
	.sym = davinci_sym
};


const struct armice_cfg davinci_cfg = {
	.endianness = LITTLE_ENDIAN,
	.work_addr = 0x00014000,
	.work_size = MEM_KiB(16)
};

