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
 * @file nand.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __NAND_H__
#define __NAND_H__

#include "memory.h"
#include "ice_drv.h"
#include "module.h"

/* NAND flash commands */
#define NAND_LO_PAGE          0x00
#define NAND_HI_PAGE          0x01
#define NAND_EXTRA_PAGE       0x50

#define NAND_LOCK             0x2a
#define NAND_UNLOCK_START     0x23
#define NAND_UNLOCK_END       0x24

#define NAND_READ_PAGE        0x00
#define NAND_READ_30H         0x30
#define NAND_RANDOM_READ_PAGE 0x05
#define NAND_RANDOM_READ_E0H  0xe0

#define	NAND_RDID             0x90
#define NAND_RDID2            0x65
#define NAND_RDIDADD          0x00

#define	NAND_RESET            0xff
#define	NAND_STATUS           0x70
#define NAND_RDY              0x40

#define	NAND_PGRM_START       0x80
#define	NAND_RANDOM_PGRM      0x85
#define NAND_PGRM_END         0x10
#define	NAND_PGM_FAIL         0x01

#define	NAND_BERASEC1         0x60
#define	NAND_BERASEC2         0xd0


/* NAND ONFI commands */
#define NAND_ONFIRDIDADD        0x20
#define NANDONFI_RDPARAMPAGE    0xec
#define NANDONFI_ID 0x49464e4f

/* Status Output */
#define NAND_STATUS_WRITEREADY  0xc0
#define NAND_STATUS_ERROR       0x01
#define NAND_STATUS_READY       0x40
#define NAND_STATUS_PROTECTED   0x80

#define NAND_MAX_PAGE_SIZE      8192  

#define NAND_BUS_8_BITS 0
#define NAND_BUS_16_BITS 1

#include <stdint.h>

/******************************************************************************
 * NAND chip geometry info
 *****************************************************************************/
struct nand_info_entry {
	/* device ID */
	uint8_t id;              
	/* number of blocks address bits - 1 */
	uint8_t blk_bits:4;          
	/* number of page address bits - 1 */
	uint8_t pg_bits:4;
	/* page size (bytes) include spare */
	uint16_t pg_size:15;
	/* bus width */
	uint16_t bus_width:1;
};

typedef struct nand_info_entry nand_info_entry_t;

/******************************************************************************
 * NAND Chip
 *****************************************************************************/

/*
   NAND chip information
   Holds geometry and operational information */
struct nand_chip {
	/* CHIP IO base addres */
	uint32_t iobase;
	/* ONFI device compatible */
	uint16_t onfi:1;
	/* Data bus width */
	uint16_t bus_width:1;
	/* Large page devices */
	uint16_t pg_large:1;
	/* command timeout in ms */
	uint16_t tmo_ms;

	/* NAND manufacturer ID */
	uint8_t mfg_id;
	/* NAND device ID */
	uint8_t dev_id;
	/* Number of blocks */
	uint16_t num_blocks;

	/* Pages per block */
	uint16_t pgs_per_blk;
	/* Data size (Page size less spare bytes) */
	uint16_t data_size;

	/* Spare bytes in the page */
	uint16_t spare_size;
	/* Number of Column address cycles */
	uint8_t col_addr_cnt;    
	/* Number of Row address cycles */
	uint8_t row_addr_cnt;    

	/* ECC start offset in the spare area */
	uint16_t ecc_offs;
};

typedef struct nand_chip nand_chip_t;

/******************************************************************************
 * NAND IO
 *****************************************************************************/

struct nand_dev;
typedef struct nand_dev nand_dev_t;

typedef int (* nand_init_t)(const void * arg, nand_dev_t * nand, 
							uint32_t iobase, int bus_width);

typedef int (* nand_cmd_t)(const void * arg, nand_chip_t * chip, 
						   unsigned int cmd);

typedef int (* nand_addr_t)(const void * arg, nand_chip_t * chip, 
							unsigned int addr);

typedef int (* nand_read_t)(const void * arg, nand_chip_t * chip, 
							void * buf, int len);

typedef int (* nand_write_t)(const void * arg, nand_chip_t * chip, 
							 const void * buf, int len);

typedef int (* nand_ready_wait_t)(const void * arg, nand_chip_t * chip);

typedef int (* nand_chip_select_t)(const void * arg, nand_chip_t * chip, 
								   int id);

/* NAND IO operations */
struct nand_io_op {
	nand_init_t init;
	nand_cmd_t cmd;
	nand_addr_t addr;
	nand_read_t read;
	nand_write_t write;
	nand_ready_wait_t ready_wait;
	nand_chip_select_t chip_select;
};

typedef struct nand_io_op nand_io_op_t;

/******************************************************************************
 * NAND ECC
 *****************************************************************************/

typedef void (*nand_ecc_enable_t)(const void * arg, nand_chip_t * chip);

typedef void (*nand_ecc_disable_t)(const void * arg, nand_chip_t * chip);

typedef int (*nand_ecc_correct_t)(const void * arg, nand_chip_t * chip, 
									   void * data, uint16_t * ecc);

typedef void (*nand_ecc_calculate_t)(const void * arg, nand_chip_t * chip, 
									 void * data, uint16_t * ecc);

/* NAND ECC operations */
struct nand_ecc_op {
	nand_ecc_enable_t enable;
	nand_ecc_disable_t disable;
	nand_ecc_correct_t correct;
	nand_ecc_calculate_t calculate;
};

typedef struct nand_ecc_op nand_ecc_op_t;

/******************************************************************************
 * NAND Bad Block 
 *****************************************************************************/

/* Mark spare bytes of page */
typedef void (* nand_bb_mark_spare_t)(const void * arg, nand_chip_t * chip, 
									  uint8_t * spare);

/* Check spare bytes of page */
typedef uint32_t (* nand_bb_check_spare_t)(const void * arg, 
										   nand_chip_t * chip, uint8_t * spare);

/* NAND Bad Block operations */
struct nand_bb_op {
	nand_bb_mark_spare_t mark_spare;
	nand_bb_check_spare_t check_spare;
};

typedef struct nand_bb_op nand_bb_op_t;

/******************************************************************************
 * NAND Device 
 *****************************************************************************/

/* NAND device */
/* This structure holds information related to the NAND controller */
struct nand_dev {
	/* device private data */
	void * arg;
	/* NAND IO base addres */
	uint32_t iobase;
	/* low level IO operations */
	const nand_io_op_t * io;
	/* ECC operations */
	const nand_ecc_op_t * ecc;
	/* Bad Block operations */
	const nand_bb_op_t * bb;
	/* maximum number of bytes per operation */
	uint16_t data_max;     
	/* maximum number of spare bytes per operation */
	uint16_t spare_max;    
	/* flags */
	uint8_t ecc_en:1;
	uint8_t bb_mark_en:1;
	uint8_t bb_chk_en:1;
	uint8_t is_open:1;
	uint8_t bus_width:1;
	/* number of chips attahced to the controller */
	uint8_t chip_cnt;
	/* current chip selected */
	uint8_t chip_sel;
	/* NAND chip information */
	nand_chip_t * chip;
};

//typedef struct nand_dev nand_dev_t;

/*****************************************************************************/

static inline void nand_row_addr(const void * arg, nand_chip_t * chip, 
								const nand_io_op_t * io, uint32_t page) {
	switch (chip->row_addr_cnt) {
	case 4:
		io->addr(arg, chip, page);
		page >>= 8;
	case 3:
		io->addr(arg, chip, page);
		page >>= 8;
	case 2:
		io->addr(arg, chip, page);
		page >>= 8;
	default:
		io->addr(arg, chip, page);
	}
}

static inline void nand_col_addr(const void * arg, nand_chip_t * chip, 
								const nand_io_op_t * io, int offs) {
	if (chip->bus_width == NAND_BUS_16_BITS)
		offs >>= 1;
	io->addr(arg, chip, offs);
	if (chip->col_addr_cnt == 2) {
		offs >>= 8;
		io->addr(arg, chip, offs);
	}
}

static inline int nand_status(const void * arg, nand_chip_t * chip, 
							  const nand_io_op_t * io) {
	uint8_t buf[1];
	io->cmd(arg, chip, NAND_STATUS);
	io->read(arg, chip, buf, 1); 
	return *buf;
}

/* Reset the NAND device */
static inline int nand_reset(const void * arg, nand_chip_t * chip, 
							 const nand_io_op_t * io) {
	io->cmd(arg, chip, NAND_RESET);
	return io->ready_wait(arg, chip);
}

extern const struct ice_mem_oper nand_mem_oper;

extern const struct module_def nand_module;

#ifdef __cplusplus
extern "c" {
#endif

nand_dev_t *  nand_dev_register(const ice_drv_t * ice, unsigned int dev_id,
								uint32_t iobase, int bus_width,
								const nand_io_op_t * io, 
								 const nand_ecc_op_t * ecc);

nand_dev_t *  nand_dev_select(unsigned int dev_id);

nand_dev_t * nand_dev_sel_get(void);

nand_chip_t *  nand_chip_probe(nand_dev_t * nand, unsigned int chip_id);

int nand_chip_init(nand_dev_t * nand, nand_chip_t *  chip);

nand_dev_t * nand_dev_get(unsigned int dev_id);

nand_chip_t * nand_chip_get(unsigned int dev_id, unsigned int chip_id);


/*********************************************************************/

int nand_page_read(nand_dev_t * nand, int page, void * buf);

int nand_page_write(nand_dev_t * nand, int page, const void * buf);

int nand_spare_read(nand_dev_t * nand, int page, void * buf);

int nand_spare_write(nand_dev_t * nand, int page, const void * buf);

int nand_block_erase(nand_dev_t * nand, int block);

/*********************************************************************/

int nand_bb_check(nand_dev_t * nand, int block);

/*********************************************************************/

nand_dev_t *  nand_mem_map(const ice_drv_t * ice, uint32_t iobase,
					  const nand_io_op_t * io, 
					  const nand_ecc_op_t * ecc, 
					  const nand_bb_op_t * bb);

int mod_nand_start(void);

#ifdef __cplusplus
}
#endif

#endif /* __NAND_H__ */

