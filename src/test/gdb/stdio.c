/* 
 * File:	 stdio.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#include <sys/stm32f.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/tty.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thinkos.h>
#include <sys/dcclog.h>

#include <sys/shell.h>

const struct file stm32_uart_file = {
	.data = STM32_UART5, 
	.op = &stm32_usart_fops 
};

void stdio_init(void)
{
	struct serial_dev * console;
//	struct usb_cdc_class * cdc;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

//	f_raw = (FILE *)&console_file;
#if 1
	console = stm32f_uart5_serial_init(115200, SERIAL_8N1);
	f_raw = serial_fopen(console);
#endif
#if 0
//	usb_cdc_sn_set(*((uint64_t *)STM32F_UID));
	cdc = usb_cdc_init(&stm32f_otg_fs_dev, 
					   cdc_acm_def_str, 
					   cdc_acm_def_strcnt);
	f_raw = usb_cdc_fopen(cdc);
#endif
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = (struct file *)&stm32_uart_file;
	stdout = f_tty;
	stdin = f_tty;
}

#if 0
/*****************************************************************************
 * Help
 *****************************************************************************/

extern const struct shell_cmd shell_cmd_tab[];

int cmd_help(FILE *f, int argc, char ** argv)
{
	const struct shell_cmd * cmd;

	if (argc > 2)
		return -1;

	if (argc > 1) {
		if ((cmd = cmd_lookup(shell_cmd_tab, argv[1])) == NULL) {
			fprintf(f, " Not found: '%s'\n", argv[1]);
			return -1;
		}

		fprintf(f, "  %s, %s - %s\n", cmd->name, cmd->alias, cmd->desc);
		fprintf(f, "  usage: %s %s\n\n", argv[1], cmd->usage);

		return 0;
	}

	fprintf(f, "\n COMMAND:   ALIAS:  DESCIPTION: \n");
	for (cmd = shell_cmd_tab; cmd->callback != NULL; cmd++) {
		fprintf(f, "  %-10s %-4s   %s\n", cmd->name, cmd->alias, cmd->desc);
	}

	return 0;
}

int cmd_test(FILE * f, int argc, char ** argv)
{
	fprintf(f, 
"| 0123456789abcdefg | 0123456789ABCDEFG | 0123456789abcdefg | 0123456789ABCDEF |"
"| 0123456789abcdefg | 0123456789ABCDEFG | 0123456789abcdefg | 0123456789ABCDEF |");
	return 0;
}

int cmd_fault(FILE * f, int argc, char ** argv)
{
	volatile uint32_t * ptr = (uint32_t *)(0x0);
	uint32_t x;
	int i;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (strcmp(argv[1], "r") == 0) {
		fprintf(f, "Invalid read access\n");
		for (i = 0; i < (16 << 4); ++i) {
			fprintf(f, "0x%08x\n", (uintptr_t)ptr);
			x = *ptr;
			(void)x;
			ptr += 0x10000000 / (2 << 4);
		}
	};

	if (strcmp(argv[1], "w") == 0) {
		fprintf(f, "Invalid write access\n");
		for (i = 0; i < (16 << 4); ++i) {
			fprintf(f, "0x%08x\n", (uintptr_t)ptr);
			*ptr = 0;
			ptr += 0x10000000 / (2 << 4);
		}
	};


	return 0;
}

const struct shell_cmd shell_cmd_tab[] = {
	{ cmd_echo, "echo", "", 
		"[STRING]...", "Echo the STRING(s) to terminal" },

	{ cmd_get, "get", "", 
		"VAR", "get environement variable" },

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

	{ cmd_osinfo, "sys", "os", 
		"", "show OS status" },

	{ cmd_oscheck, "check", "ck", 
		"", "check OS sanity" },

	{ cmd_reboot, "reboot", "rst", "", 
		"reboot system" },

	{ cmd_set, "set", "", 
		"VAR EXPR", "set environement variable" },

	{ cmd_thread, "thread", "th", 
		"[ID]", "show thread status" },

	{ cmd_memxxd, "xxd", "x", "ADDR [COUNT]", 
		"prints a hexdump of a memory block" },

	{ cmd_test, "test", "t", "", 
		"Test" },

	{ cmd_fault, "fault", "f", "", 
		"Generate a fault" },

	{ NULL, "", "", NULL, NULL }
};


const char * shell_prompt(void)
{
	return "[TEST]$ ";
}

extern const char * version_str;
extern const char * copyright_str;

void shell_greeting(FILE * f) 
{
	fprintf(f, "\n%s", version_str);
	fprintf(f, "\n%s\n\n", copyright_str);
}

int test_shell(FILE * f)
{
	return shell(f, shell_prompt, shell_greeting, shell_cmd_tab);
}

#endif

