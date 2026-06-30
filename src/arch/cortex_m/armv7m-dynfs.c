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
 * @file gdb-tcp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <fixpt.h>
#include "dynfs.h"
#include "target.h"

const char feature_target_core_xml[] = 
	"<?xml version=\"1.0\"?>\n"
	"<target>\n"
	"\t<architecture>arm</architecture>\n"
	"\t<feature name=\"org.gnu.gdb.arm.m-profile\">\n"
	"\t\t<reg name=\"r0\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r1\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r2\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r3\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r4\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r5\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r6\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r7\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r8\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r9\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r10\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r11\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"r12\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"sp\" bitsize=\"32\" type=\"data_ptr\"/>\n"
	"\t\t<reg name=\"lr\" bitsize=\"32\"/>\n"
	"\t\t<reg name=\"pc\" bitsize=\"32\" type=\"code_ptr\"/>\n"
	"\t\t<reg name=\"xpsr\" bitsize=\"32\" regnum=\"25\"/>\n"
	"\t</feature>\n";

const char feature_target_vfp_xml[] = 
	"\t<feature name=\"org.gnu.gdb.arm.vfp\">\n"
	"\t\t<reg name=\"d0\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d1\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d2\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d3\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d4\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d5\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d6\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d7\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d8\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d9\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d10\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d11\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d12\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d13\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d14\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"d15\" bitsize=\"64\" type=\"ieee_double\"/>\n"
	"\t\t<reg name=\"fpscr\" bitsize=\"32\" type=\"int\" group=\"float\"/>\n"
	"\t</feature>\n";

const char feature_target_close_xml[] =
	"</target>\n";

int arm_v7m_target_xml_generate(void * arg, char * buf, size_t size)
{
	struct target_info * tgt = (struct target_info *)arg;
	char * cp = (char *)buf;

	cp = stpcpy(cp, feature_target_core_xml);
	if (tgt->arch->fpu != NULL) {
		cp = stpcpy(cp, feature_target_vfp_xml);
	}
	cp = stpcpy(cp, feature_target_close_xml);

	return cp - buf;
}

const char memory_map_open_xml[] = 
	"<?xml version=\"1.0\"?>\n"
	"<memory-map>\n";
const char memory_flash_xml[] = 
	"\t<memory type=\"flash\" start=\"0x%08x\" length=\"0x%08x\">\n"
	"\t\t<property name=\"blocksize\">%d</property>\n"
	"\t</memory>\n";
const char memory_ram_xml[] = 
	"\t<memory type=\"ram\" start=\"0x%08x\" length=\"0x%08x\"/>\n";
const char memory_map_close_xml[] = "</memory-map>\n";

int arm_v7m_memory_map_generate(void * arg, char * buf, size_t size)
{
	struct target_info * tgt = (struct target_info *)arg;
	char * cp = (char *)buf;
	ice_mem_entry_t * e; 
	uint32_t memsize;
	uint32_t memstart;

	cp = stpcpy(cp, memory_map_open_xml);
	if ((e = ice_mem_by_name(tgt->mem, "flash")) != NULL) {
		memstart = e->addr.base + e->addr.offs;
		memsize = e->blk.count * e->blk.size;
		cp += sprintf(cp, memory_flash_xml, memstart, memsize, e->blk.size);
	}
	if ((e = ice_mem_by_name(tgt->mem, "sram")) != NULL) {
		memstart = e->addr.base + e->addr.offs;
		memsize = e->blk.count * e->blk.size;
		cp += sprintf(cp, memory_ram_xml, memstart, memsize);
	}
	cp = stpcpy(cp, memory_map_close_xml);

	return cp - buf;
}

#if (GDB_ENABLE_QXFER_THREADS) 
const char threads_open_xml[] = 
	"<?xml version=\"1.0\"?>"
	"<threads>";

const char threads_entry_xml[] = 
	"\t<thread id=\"%d\" core=\"0\" name=\"%s\"/>";

const char threads_close_xml[] = 
	"</threads>";
#endif


const struct dynfs_file_def arm_v7m_dynfs_fsdef[] = {
	{ .inode = 1, .fname = "target.xml", .data = (void *)feature_target_core_xml, 
		.size = sizeof(feature_target_core_xml) +
			sizeof(feature_target_vfp_xml) +
			sizeof(feature_target_close_xml),
		.generate = arm_v7m_target_xml_generate },
	{ .inode = 2, .fname = "memory_map.xml", .data = (void *)memory_map_open_xml, 
		.size = sizeof(memory_map_open_xml) +
		sizeof(memory_flash_xml) +
		4 * sizeof(memory_ram_xml) +
		sizeof(memory_map_close_xml),
		.generate = arm_v7m_memory_map_generate},
	{ .inode = 0, .fname = "", .data = NULL, .generate = NULL }
};

