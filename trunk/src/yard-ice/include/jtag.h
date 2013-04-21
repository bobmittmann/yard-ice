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
 * @file jtag.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __JTAG_H__
#define __JTAG_H__

/* JTAG TAP States */
#define JTAG_TAP_DREXIT2    0x00 /* Exit2-DR */
#define JTAG_TAP_DREXIT1    0x01 /* Exit1-DR */
#define JTAG_TAP_DRSHIFT    0x02 /* Shift-DR */
#define JTAG_TAP_DRPAUSE    0x03 /* Pause-DR */
#define JTAG_TAP_IRSELECT   0x04 /* Select-IR-Scan */
#define JTAG_TAP_DRUPDATE   0x05 /* Update-DR */
#define JTAG_TAP_DRCAPTURE  0x06 /* Capture-DR */
#define JTAG_TAP_DRSELECT   0x07 /* Select-DR-Scan */
#define JTAG_TAP_IREXIT2    0x08 /* Exit2-IR */
#define JTAG_TAP_IREXIT1    0x09 /* Exit1-IR */
#define JTAG_TAP_IRSHIFT    0x0a /* Shift-IR */
#define JTAG_TAP_IRPAUSE    0x0b /* Pause-IR */
#define JTAG_TAP_IDLE       0x0c /* Run-Test-Idle */
#define JTAG_TAP_RUN_TEST   0x0c /* Run-Test-Idle */
#define JTAG_TAP_IRUPDATE   0x0d /* Update-IR */
#define JTAG_TAP_IRCAPTURE  0x0e /* Capture-IR */
#define JTAG_TAP_RESET      0x0f /* Test-Logic-Reset */

#if 0
	constant TAP_DREXIT2   : tap_state_t := 16#00#; -- Exit2-DR
	constant TAP_DREXIT1   : tap_state_t := 16#01#; -- Exit1-DR
	constant TAP_DRSHIFT   : tap_state_t := 16#02#; -- Shift-DR
	constant TAP_DRPAUSE   : tap_state_t := 16#03#; -- Pause-DR
	constant TAP_IRSELECT  : tap_state_t := 16#04#; -- Select-IR-Scan
	constant TAP_DRUPDATE  : tap_state_t := 16#05#; -- Update-DR
	constant TAP_DRCAPTURE : tap_state_t := 16#06#; -- Capture-DR
	constant TAP_DRSELECT  : tap_state_t := 16#07#; -- Select-DR-Scan
	constant TAP_IREXIT2   : tap_state_t := 16#08#; -- Exit2-IR
	constant TAP_IREXIT1   : tap_state_t := 16#09#; -- Exit1-IR
	constant TAP_IRSHIFT   : tap_state_t := 16#0A#; -- Shift-IR
	constant TAP_IRPAUSE   : tap_state_t := 16#0B#; -- Pause-IR
	constant TAP_IDLE      : tap_state_t := 16#0C#; -- Run-Test-Idle
	constant TAP_IRUPDATE  : tap_state_t := 16#0D#; -- Update-IR
	constant TAP_IRCAPTURE : tap_state_t := 16#0E#; -- Capture-IR
	constant TAP_RESET     : tap_state_t := 16#0F#; -- Test-Logic-Reset
#endif

/* JTAG IDCODE */
#define JTAG_IDCODE_VERSION(ID) (((ID) >> 28) & 0x0F)
#define JTAG_IDCODE_PART_NUMBER(ID) (((ID) >> 12) & 0xFFFF)
#define JTAG_IDCODE_MANUFACTURER(ID) (((ID) >> 1) & 0x07FF)
#define JTAG_IDCODE_MARKER(ID) ((ID) & 0x0001)

#ifdef __ASSEMBLER__

#else /* __ASSEMBLER__ */

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t jtag_idcode_t;

typedef enum {
	JTAG_OK = 0,
	JTAG_ERR_UNDEF = -201,
	JTAG_ERR_PARM_INVALID = -202,
	JTAG_ERR_HARDWARE = -203,
	JTAG_ERR_INVALID_TAP = -204,
	JTAG_ERR_IR_SCAN = -205,
	JTAG_ERR_TOO_MANY_TAPS = -206,
	JTAG_ERR_ARM_DR_SCAN_N = -207,
	JTAG_ERR_ARM_SYNC_TIMEOUT = -208,
	JTAG_ERR_ARM_DCC_TIMEOUT = -209,
	JTAG_ERR_ARM_POLL_STOP = -210,
	JTAG_ERR_ADI_ACK_INVALID = -211,
	JTAG_ERR_ADI_WAIT_TIMEOUT = -212
} jtag_erro_t;

struct jtag_tap {
	uint8_t pos;
	uint8_t irlen;
	uint8_t preirlen;
	uint8_t precnt;
	uint8_t posirlen;
	uint8_t poscnt;
	uint32_t irmask;
	jtag_idcode_t idcode;
	jtag_idcode_t idmask;
	jtag_idcode_t idcomp;
};

typedef struct jtag_tap jtag_tap_t;

typedef uint32_t * jtag_vec_t;

extern const char * const jtag_state_name[];

typedef enum {
	JTAG_TAP_CLASS_UNDEF = 0, /* Undefined or unknown class */
	JTAG_TAP_CLASS_CPU_CORE = 1, /* Processor Core */
	JTAG_TAP_CLASS_JRC = 2 /* JTAG Router Controller */
} jtag_tap_class_t;

#define JTAG_TAP_NAME_MAX 15
#define JTAG_TAP_MODEL_MAX 7
#define JTAG_TAP_VENDOR_MAX 15

struct jtag_tap_info {
	char name[JTAG_TAP_NAME_MAX + 1];
	char model[JTAG_TAP_MODEL_MAX + 1];
	char vendor[JTAG_TAP_VENDOR_MAX + 1];
	jtag_tap_class_t class;
	uint8_t irlength;
	uint32_t idmask;
	uint32_t idcomp;
};

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Primitive JTAG operations
 */
int jtag_start(void);

void jtag_tap_reset(void);

void jtag_run_test(int n, unsigned int final_state);

int jtag_ir_scan(const jtag_vec_t in, jtag_vec_t out, 
				 int vlen, unsigned int final_state);

int jtag_dr_scan(const jtag_vec_t in, jtag_vec_t out, 
				 int vlen, unsigned int final_state);

void jtag_ir_pause(int n, unsigned int final_state);

void jtag_dr_pause(int n, unsigned int final_state);

int jtag_ir_stop(int state);

int jtag_dr_stop(int state);

/* 
 * Move to a defined state
 */
int jtag_goto_state(unsigned int state);

/* 
 * Get the current TAP state
 */
int jtag_state(void);

/**
 * Control the TRST line.
 * @param set 1 - assert,  0 -  deassert 
 * @returns JTAG_OK on success, or a negative code if an error occured.
 */
int jtag_trst(bool assert);

/**
 * Control the nRST line.
 * @param set 1 - assert,  0 -  deassert 
 * @returns JTAG_OK on success, or a negative code if an error occured.
 */
int jtag_nrst(bool assert);

int jtag_nrst_pulse(unsigned int ms);

/**
 * Enable or disable the JTAG interface loopback mode. When in loopback 
 * mode the interfce will connect TDO to TDI, for testing porpouses.
 * @returns JTAG_OK on success, or a negative code if an error occured.
 */
int jtag_if_loopback(int enable);

/**
 * Reset the JTAG interface.
 * @returns JTAG_OK on success, or an error code in case of failure.
 */
int jtag_if_reset(void);

/**
 * Configure the JTAG interface clock to a specified frequency in Hz.
 * @returns JTAG_OK on success, or an error code in case of failure.
 */
int jtag_tck_freq_set(unsigned int tck_freq);


/**
 * Configure the JTAG interface to use RTCK
 * @returns JTAG_OK on success, or an error code in case of failure.
 */
int jtag_rtck_enable(void);

int jtag_rtck_disable(void);

int jtag_rtck_freq_set(unsigned int tck_freq);

unsigned int jtag_rtck_freq_get(void);

/**
 * Get the JTAG interface current TCK frequency.
 * @returns The TCK frequency in Hz.
 */
unsigned int jtag_tck_freq_get(void);

/*
 Current  Final
 State    State     Default State Path
 RESET    RESET     At least one TCK cycle applied with TMS = 1
 RESET    IDLE      RESET-IDLE
 RESET    DRPAUSE   RESET-IDLE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
 RESET    IRPAUSE   RESET-IDLE-DRSELECT-IRSELECT-IRCAPTURE-IREXIT1-IRPAUSE
 IDLE     RESET     IDLE-DRSELECT-IRSELECT-RESET
 IDLE     IDLE      At least one TCK cycle applied with TMS = 0
 IDLE     DRPAUSE   IDLE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
 IDLE     IRPAUSE   IDLE-DRSELECT-IRSELECT-IRCAPTURE-IREXIT1-IRPAUSE
 DRPAUSE  RESET     DRPAUSE-DREXIT2-DRUPDATE-DRSELECT-IRSELECT-RESET
 DRPAUSE  IDLE      IDLE-DRPAUSE-DREXIT2-DRUPDATE-IDLE
 DRPAUSE  DRPAUSE   At least one TCK cycle applied with TMS = 0
 DRPAUSE  IRPAUSE   DRPAUSE-DREXIT2-DRUPDATE-DRSELECT-
                    IRSELECT-IRCAPTURE-IREXIT1-IRPAUSE
 IRPAUSE  RESET     IRPAUSE-IREXIT2-IRUPDATE-DRSELECT-IRSELECT-RESET
 IRPAUSE  IDLE      IRPAUSE-IREXIT2-IRUPDATE-IDLE
 IRPAUSE  DRPAUSE   IRPAUSE-IREXIT2-IRUPDATE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
 IRPAUSE  IRPAUSE   At least one TCK cycle applied with TMS = 0
*/

int jtag_move_to(int state);

int jtag_wait(int state);

/**
  * Remove all taps...
  */
int jtag_tap_purge(void);

int jtag_tap_insert(jtag_tap_t ** tap_ptr, unsigned int chain_pos, 
					unsigned int ir_len, uint32_t id_code);

/**
 *  Add a new generic TAP to the JTAG controller
 *  @param ir_len The instruction register length 
 *  @returns Return the index of the TAP in the chain, or a negative 
 *  error code if an error occurred.
 */
int jtag_tap_add(jtag_tap_t ** tap_ptr, unsigned int ir_len, uint32_t id_code);

/**
 *  Get the number of TAPs in the chain
 *  @returns Return the number of TAPS in the chain
 */
int jtag_tap_tell(void);

int jtag_tap_get(jtag_tap_t ** tap_ptr, unsigned int chain_pos);

/**
 *  Select (activate) a TAP in the chain
 *  @param chain_pos The position in the chain as returned by jtag_tap_add().
 *  @returns Return a pointer to a jtag_tap structure
 */
int jtag_tap_select(jtag_tap_t * tap_ptr);

int jtag_tap_idcode(jtag_tap_t * tap, jtag_idcode_t * idcode);

int jtag_tdo_probe(int enable);

int jtag_vcc(int enable);

int jtag_relay(int enable);

int jtag_pwr_ctrl(int on);


int jtag_chain_probe(uint8_t * irlen, int max, unsigned int * cnt_ptr);

int jtag_chain_init(uint8_t * irlen, int cnt);

char * jtag_strerror(int errno);


int jtag_drv_init(void);

int jtag_drv_done(void);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __TAG_H__ */

