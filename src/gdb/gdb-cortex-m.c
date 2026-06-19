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
 * @file gdb-cortex-m.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include "gdb-i.h"

/* -------------------------------------------------------------------------
 * Core files
 * ------------------------------------------------------------------------- */

#if (GDB_ENABLE_QXFER_FEATURES) 
const char feature_target_core_xml[] = 
	"<target>\n"
		"<architecture>arm</architecture>\n"
		"<feature name=\"org.gnu.gdb.arm.m-profile\">\n"
			"<reg name=\"r0\" bitsize=\"32\"/>\n"
			"<reg name=\"r1\" bitsize=\"32\"/>\n"
			"<reg name=\"r2\" bitsize=\"32\"/>\n"
			"<reg name=\"r3\" bitsize=\"32\"/>\n"
			"<reg name=\"r4\" bitsize=\"32\"/>\n"
			"<reg name=\"r5\" bitsize=\"32\"/>\n"
			"<reg name=\"r6\" bitsize=\"32\"/>\n"
			"<reg name=\"r7\" bitsize=\"32\"/>\n"
			"<reg name=\"r8\" bitsize=\"32\"/>\n"
			"<reg name=\"r9\" bitsize=\"32\"/>\n"
			"<reg name=\"r10\" bitsize=\"32\"/>\n"
			"<reg name=\"r11\" bitsize=\"32\"/>\n"
			"<reg name=\"r12\" bitsize=\"32\"/>\n"
			"<reg name=\"sp\" bitsize=\"32\" type=\"data_ptr\"/>\n"
			"<reg name=\"lr\" bitsize=\"32\"/>\n"
			"<reg name=\"pc\" bitsize=\"32\" type=\"code_ptr\"/>\n"
			"<reg name=\"xpsr\" bitsize=\"32\" regnum=\"25\"/>\n"
		"</feature>\n"
};
const char feature_target_vfp_xml[] = 
		"<feature name=\"org.gnu.gdb.arm.vfp\">\n"
			"<reg name=\"d0\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d1\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d2\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d3\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d4\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d5\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d6\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d7\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d8\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d9\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d10\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d11\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d12\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d13\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d14\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"d15\" bitsize=\"64\" type=\"ieee_double\"/>\n"
			"<reg name=\"fpscr\" bitsize=\"32\" type=\"int\" group=\"float\"/>\n"
		"</feature>\n"
const char feature_target_closing_xml[] =
	"</target>";

void feature_target_generate(void * arg, uint8_t buf, int max)
{
	if (debugger
}

#endif

#if (GDB_ENABLE_QXFER_MEMORY_MAP) 
const char memory_map_xml[] = 
"<memory-map>"
"<memory type=\"flash\" start=\"0x8000000\" length=\"0x100000\">"
"<property name=\"blocksize\">2048</property>"
"</memory>"
"<memory type=\"flash\" start=\"0x8100000\" length=\"0x100000\">"
"<property name=\"blocksize\">2048</property>"
"</memory>"
"<memory type=\"ram\" start=\"0x20000000\" length=\"0x80000\"/>"
"<memory type=\"ram\" start=\"0x10000000\" length=\"0x10000\"/>"
"</memory-map>";
#endif

#if (GDB_ENABLE_QXFER_THREADS) 
const char threads_xml[] = 
"<?xml version=\"1.0\"?>"
"<threads>"
    "<thread id=\"%d\" core=\"0\" name=\"%s\">"
    "</thread>"
"</threads>"
#endif
dfa
int target_file_read(const char * name, char * dst, 
					  unsigned int offs, unsigned int size)
{
	char * src;
	unsigned int len;
	unsigned int cnt;
	unsigned int i;

	if (prefix(name, "target.xml")) {
		src = (char *)target_xml;
		len = sizeof(target_xml) - 1;
		DCC_LOG(LOG_TRACE, "target.xml");
#if (GDB_ENABLE_QXFER_MEMORY_MAP) 
	} else if (prefix(name, "memmap.xml")) {
		src = (char *)memory_map_xml;
		len = sizeof(memory_map_xml) - 1;
#endif
	} else
		return -1;

	if (offs >= len)
		return 0;

	DCC_LOG3(LOG_TRACE, "offs=%d len=%d size=%d", offs, len, size);

	cnt = len - offs;
	if (cnt > size)
		cnt = size;

	src += offs;

	for (i = 0; i < cnt; ++i)
		dst[i] = src[i];

	return cnt;
}

