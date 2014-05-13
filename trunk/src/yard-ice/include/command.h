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
 * @file command.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <stdint.h> 
#include <stdio.h> 
#include <sys/shell.h>

struct lt_entry_int {
	const char * name;
	int val;
};

extern const struct shell_cmd cmd_tab[];

#ifdef __cplusplus
extern "C" {
#endif

int cmd_beep(FILE * f, int argc, char ** argv);

int cmd_info(FILE * f, int argc, char ** argv);

int cmd_probe(FILE * f, int argc, char ** argv);

int cmd_step(FILE * f, int argc, char ** argv);

int cmd_run(FILE * f, int argc, char ** argv);

int cmd_goto(FILE * f, int argc, char ** argv);

int cmd_show_context(FILE * f, int argc, char ** argv);

int cmd_show_status(FILE * f, int argc, char ** argv);

int cmd_init(FILE * f, int argc, char ** argv);

int cmd_halt(FILE * f, int argc, char ** argv);

int cmd_help(FILE * f, int argc, char ** argv);

int cmd_reset(FILE * f, int argc, char ** argv);

int cmd_connect(FILE * f, int argc, char ** argv);

int cmd_release(FILE * f, int argc, char ** argv);

int cmd_disassemble(FILE * f, int argc, char ** argv);

int cmd_eint(FILE * f, int argc, char ** argv);

int cmd_dint(FILE * f, int argc, char ** argv);

int cmd_breakpoint(FILE * f, int argc, char ** argv);

int cmd_watchpoint(FILE * f, int argc, char ** argv);

int cmd_bp_clear(FILE * f, int argc, char ** argv);

int cmd_delete(FILE * f, int argc, char ** argv);

int cmd_cfgerase(FILE *f, int argc, char ** argv);

int cmd_ipcfg(FILE *f, int argc, char ** argv);

int cmd_target(FILE *f, int argc, char ** argv);

int cmd_mem_read(FILE * f, int argc, char ** argv);

int cmd_mem_write(FILE * f, int argc, char ** argv);

int cmd_mem_erase(FILE * f, int argc, char ** argv);

int cmd_meminfo(FILE * f, int argc, char ** argv);

int cmd_version(FILE *f, int argc, char ** argv);

int cmd_get(FILE *f, int argc, char ** argv);

int cmd_set(FILE *f, int argc, char ** argv);

int cmd_let(FILE *f, int argc, char ** argv);

int cmd_unset(FILE *f, int argc, char ** argv);

int cmd_test(FILE *f, int argc, char ** argv);

int cmd_ice_test(FILE *f, int argc, char ** argv);

int cmd_ice_info(FILE *f, int argc, char ** argv);

int cmd_config(FILE *f, int argc, char ** argv);

int show_hex32(FILE * f, uint32_t  addr, const void * buf, int count);

int hexdump32(FILE * f, uint32_t addr, int size);

int cmd_tcp_recv(FILE * f, int argc, char ** argv);

int cmd_tcp_send(FILE * f, int argc, char ** argv);

int cmd_trst(FILE * f, int argc, char ** argv);

int cmd_nrst(FILE * f, int argc, char ** argv);

int cmd_tap(FILE * f, int argc, char ** argv);

int cmd_power(FILE * f, int argc, char ** argv);

int cmd_relay(FILE * f, int argc, char ** argv);

int cmd_irscan(FILE * f, int argc, char ** argv);

int cmd_drscan(FILE * f, int argc, char ** argv);

int cmd_idcode(FILE * f, int argc, char ** argv);

/* Devkit external commands */

int cmd_close(FILE *f, int argc, char ** argv);

int cmd_ifconfig(FILE *f, int argc, char ** argv);

int cmd_arp(FILE *f, int argc, char ** argv);

int cmd_route(FILE *f, int argc, char ** argv);

int cmd_netstat(FILE *f, int argc, char ** argv);

int cmd_ping(FILE *f, int argc, char ** argv);

int cmd_reboot(FILE *f, int argc, char ** argv);

int cmd_tcpstat(FILE *f, int argc, char ** argv);

int cmd_var(FILE *f, int argc, char ** argv);

int cmd_sleep(FILE *f, int argc, char ** argv);

int cmd_os(FILE *f, int argc, char ** argv);

int cmd_thread(FILE * f, int argc, char ** argv);

int cmd_bb_scan(FILE *f, int argc, char ** argv);

int cmd_scrub(FILE * f, int argc, char ** argv);

int cmd_runtest(FILE * f, int argc, char ** argv);

int cmd_irpause(FILE * f, int argc, char ** argv);

int cmd_drpause(FILE * f, int argc, char ** argv);

int cmd_enable(FILE * f, int argc, char ** argv);

int cmd_disable(FILE * f, int argc, char ** argv);

int cmd_i2c(FILE * f, int argc, char ** argv);

int cmd_trace(FILE * f, int argc, char ** argv);

int exec(FILE * f, char * line);

void show_val(FILE * f, uint32_t val);


int lookup_int_val(const struct lt_entry_int * table, int val);


#ifdef __cplusplus
}
#endif

#endif /* __COMMAND_H__ */


