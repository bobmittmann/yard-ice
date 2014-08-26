/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/shell.h>
#include <sys/tty.h>
#include <sys/param.h>
#include <xmodem.h>
#include <thinkos.h>

#include <slcdev.h>

#include <sys/dcclog.h>
#include <sys/delay.h>

#include <microjs.h>

#include "flashfs.h"
#include "isink.h"

extern const struct shell_cmd cmd_tab[];

/*****************************************************************************
 * Help
 *****************************************************************************/

int cmd_help(FILE *f, int argc, char ** argv)
{
	struct shell_cmd * cmd;

	if (argc > 2)
		return -1;

	if (argc > 1) {
		if ((cmd = cmd_lookup(argv[1], cmd_tab)) == NULL) {
			fprintf(f, " Not found: '%s'\n", argv[1]);
			return -1;
		}

		fprintf(f, "  %s, %s - %s\n", cmd->name, cmd->alias, cmd->desc);
		fprintf(f, "  usage: %s %s\n\n", argv[1], cmd->usage);

		return 0;
	}

	fprintf(f, "\n Command:   Alias:  Desciption: \n");
	for (cmd = (struct shell_cmd *)cmd_tab; cmd->callback != NULL; cmd++) {
		fprintf(f, "  %-10s %-4s   %s\n", cmd->name, cmd->alias, cmd->desc);
	}

	return 0;
}

/*****************************************************************************
 * FPGA
 *****************************************************************************/

int flash_xmodem_recv(FILE * f, uint32_t offs, unsigned int size)
{
	struct comm_dev comm;
	struct file * raw;
	uint8_t buf[128];
	unsigned int cnt;
	unsigned int rem;
	int ret;

	union {
		struct xmodem_rcv rx;
		struct xmodem_snd sx;
	} xmodem;

	raw = ftty_lowlevel(f);

	comm.arg = raw->data;
	comm.op.send = (int (*)(void *, const void *, unsigned int))raw->op->write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))raw->op->read;

	DCC_LOG(LOG_TRACE, ".................................");

	xmodem_rcv_init(&xmodem.rx, &comm, XMODEM_RCV_CRC);

	cnt = 0;
	rem = size;
	do {
		unsigned int n;

		if ((ret = xmodem_rcv_loop(&xmodem.rx, buf, 128)) < 0) {
			DCC_LOG1(LOG_ERROR, "ret=%d", ret);
			return ret;
		}

		if (rem == 0) {
			xmodem_rcv_cancel(&xmodem.rx);
			break;
		}

		n = MIN(rem, ret); 
		stm32_flash_write(offs, buf, n);
		cnt += n;
		offs += n;
		rem -= n;
	} while (ret > 0);

	return cnt;
}

int cmd_rx(FILE * f, int argc, char ** argv)
{
	uint32_t blk_offs;
	uint32_t blk_size;
	struct fs_dirent entry;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (!fs_dirent_lookup(argv[1], &entry)) {
		fprintf(f, "invalid file: \"%s\"\n", argv[1]);
		return SHELL_ERR_ARG_INVALID;
	}

	blk_offs = entry.offs;
	blk_size = entry.blk_size;

	fprintf(f, "Erasing block: 0x%06x, %d bytes...\n", blk_offs, blk_size);
	if (stm32_flash_erase(blk_offs, blk_size) < 0) {
		fprintf(f, "stm32f_flash_erase() failed!\n");
		return -1;
	};

	fprintf(f, "RX waiting to receive.");
	if (flash_xmodem_recv(f, blk_offs, blk_size) < 0) {
		fprintf(f, "flash_xmodem_recv() failed!\n");
		return -1;
	}

	return 0;
}

int cmd_cat(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;
	char * cp;
	int cnt;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (!fs_dirent_lookup(argv[1], &entry)) {
		fprintf(f, "invalid file: \"%s\"\n", argv[1]);
		return SHELL_ERR_ARG_INVALID;
	}

	cp = (char *)(STM32_MEM_FLASH + entry.offs);
	DCC_LOG2(LOG_TRACE, "cp=0x%08x len=%d", cp, strlen(cp));

	for (cnt = 0; cnt < entry.size; ) { 
		int n;

		n = strlen(cp);
		if (n > 0) {
			fprintf(f, "%s\n", cp);
		} else
			n = 1;

		cnt += n;
		cp += n;
	}

	return 0;
}

int cmd_ls(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;

	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	memset(&entry, 0, sizeof(entry));

	while (fs_dirent_get_next(&entry)) {
		fprintf(f, "0x%06x %6d %6d %s\n", entry.offs, 
				entry.blk_size, entry.size, entry.name);
	}

	return 0;
}


int cmd_trig(FILE * f, int argc, char ** argv)
{
	unsigned int addr;
	bool mod = false;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (argc == 2) {
		addr = strtoul(argv[1], NULL, 0);
		if (addr > 319)
			return SHELL_ERR_ARG_INVALID;

		if (addr >= 160) {
			mod = true;
			addr -= 160;
		}

		trig_addr_set(mod, addr);
	}

	addr = trig_addr_get();

	fprintf(f, "Trigger address: %d\n", addr);

	return 0;
}

int cmd_enable(FILE * f, int argc, char ** argv)
{
	unsigned int addr;
	bool sens = false;
	bool mod = false;
	bool all = false;
	int i;

	if (argc == 1) {
		struct ss_device * dev;

		fprintf(f, "Sensosrs:");
		for (i = 1; i < 160; ++i) {
			dev = dev_sim_sensor_lookup(i);
			if (dev->enabled)
				fprintf(f, " %3d", i);
		}

		fprintf(f, "\nModules:");
		for (i = 1; i < 160; ++i) {
			dev = dev_sim_module_lookup(i);
			if (dev->enabled)
				fprintf(f, " %3d", i);
		}
		fprintf(f, "\n");
	}

	if (argc == 2) {
		if (strcmp(argv[1], "all") == 0) {
			all = true;
		} else if ((strcmp(argv[1], "sens") == 0) || 
			(strcmp(argv[1], "s") == 0)) {
			sens = true;
		} else if (strcmp(argv[1], "mod") == 0) {
			mod = true;
		}
	}

	if (all) {
		if ((!mod) && (!sens)) {
			mod = true;
			sens = true;
		}

		if (sens) {
			for (addr = 1; addr < 160; ++addr)
				dev_sim_enable(false, addr);
			fprintf(f, "All sensors enabled\n");
		}

		if (mod) {
			for (addr = 1; addr < 160; ++addr)
				dev_sim_enable(true, addr);
			fprintf(f, "All modules enabled\n");
		}

		return 0;
	}

	for (i = 1; i < argc; ++i) {
		addr = strtoul(argv[i], NULL, 0);
		if ((addr < 1) || (addr > 319))
			return SHELL_ERR_ARG_INVALID;

		if (sens) {
			fprintf(f, "Sensor %d enabled\n", addr);
			dev_sim_enable(false, addr);
		} 
		if (mod) {
			fprintf(f, "Module %d enabled\n", addr);
			dev_sim_enable(true, addr);
		}
	}

	return 0;
}

int cmd_disable(FILE * f, int argc, char ** argv)
{
	unsigned int addr;
	bool sens = false;
	bool mod = false;
	bool all = false;
	int i;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc == 2) {
		if (strcmp(argv[1], "all") == 0) {
			all = true;
		} else if ((strcmp(argv[1], "sens") == 0) || 
			(strcmp(argv[1], "s") == 0)) {
			sens = true;
		} else if (strcmp(argv[1], "mod") == 0) {
			mod = true;
		}
	}

	if (all) {
		if ((!mod) && (!sens)) {
			mod = true;
			sens = true;
		}

		if (sens) {
			for (addr = 1; addr < 160; ++addr)
				dev_sim_disable(false, addr);
			fprintf(f, "All sensors disabled\n");
		}

		if (mod) {
			for (addr = 1; addr < 160; ++addr)
				dev_sim_disable(true, addr);
			fprintf(f, "All modules disabled\n");
		}

		return 0;
	}

	for (i = 1; i < argc; ++i) {
		addr = strtoul(argv[i], NULL, 0);
		if ((addr < 1) || (addr > 319))
			return SHELL_ERR_ARG_INVALID;

		if (sens) {
			fprintf(f, "Sensor %d disabled\n", addr);
			dev_sim_disable(false, addr);
		} 
		if (mod) {
			fprintf(f, "Module %d disabled\n", addr);
			dev_sim_disable(true, addr);
		}
	}

	return 0;
}

int cmd_self_test(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	lamp_test();

	return 0;
}

void isink_test(void);

int cmd_isink(FILE * f, int argc, char ** argv)
{
	unsigned int mode = 0;
	unsigned int rate = 0;
	unsigned int pre = 50;
	unsigned int pulse = 1000;

	if (argc > 5)
		return SHELL_ERR_EXTRA_ARGS;

	if (argc > 1) {
		mode = strtoul(argv[1], NULL, 0);
		if (mode > 25)
			return SHELL_ERR_ARG_INVALID;
		if (argc > 2) {
			rate = strtoul(argv[2], NULL, 0);
			if (rate > 3)
				return SHELL_ERR_ARG_INVALID;
			if (argc > 3) {
				pulse = strtoul(argv[3], NULL, 0);
				if (argc > 4)
					pre = strtoul(argv[4], NULL, 0);
			}
		}

		isink_mode_set(mode | (rate << 5));
		isink_pulse(pre, pulse);
		udelay(pulse / 2);
		isink_stop();
	} else {
		isink_test();
	}
	
	return 0;
}

int cmd_slewrate(FILE * f, int argc, char ** argv)
{
	unsigned int rate = 0;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	rate = strtoul(argv[1], NULL, 0);

	isink_slewrate_set(rate);

	return 0;
}

int cmd_dbase(FILE * f, int argc, char ** argv)
{
	struct json_file json;
	bool erase = false;
	bool compile = false;
	int i;

	if (argc == 1)
		device_db_dump(f);

	for (i = 1; i < argc; ++i) {
		if ((strcmp(argv[i], "compile") == 0) || 
			(strcmp(argv[i], "c") == 0)) {
			compile = true;
		} else if ((strcmp(argv[i], "erase") == 0) || 
			(strcmp(argv[i], "e") == 0)) {
			erase = true;
		} else
			return SHELL_ERR_ARG_INVALID;
	}

	if (erase) {
		fprintf(f, "Erasing...\n");
		device_db_erase();
	}

	if (compile) {
		json_file_get(FLASH_BLK_DB_JSON_OFFS, &json);
		if (device_db_sanity_check(&json)) {
			fprintf(f, "Up-to-date.\n");
		} else {
			fprintf(f, "Compiling...\n");
			if (device_db_compile(&json) < 0) {
				printf("Parse error!\n");
				return -1;
			}
		}
	}

	return 0;
}

int cmd_config(FILE * f, int argc, char ** argv)
{
	struct json_file json;
	bool erase = false;
	bool compile = false;
	bool load = false;
	bool save = false;
	int i;

	if (argc == 1) 
		return config_show_info(f);

	for (i = 1; i < argc; ++i) {
		if ((strcmp(argv[i], "compile") == 0) || 
			(strcmp(argv[i], "c") == 0)) {
			compile = true;
		} else if ((strcmp(argv[i], "erase") == 0) || 
			(strcmp(argv[i], "e") == 0)) {
			erase = true;
		} else if ((strcmp(argv[i], "load") == 0) || 
			(strcmp(argv[i], "l") == 0)) {
			load = true;
		} else if ((strcmp(argv[i], "save") == 0) || 
			(strcmp(argv[i], "s") == 0)) {
			save = true;
			erase = true;
		} else
			return SHELL_ERR_ARG_INVALID;
	}

	json_file_get(FLASH_BLK_CFG_JSON_OFFS, &json);

	slcdev_stop();

	if (compile) {
		if (config_sanity_check(&json)) {
			fprintf(f, "Up-to-date.\n");
		} else {
			fprintf(f, "Compiling...\n");
			if (config_compile(&json) < 0) {
				printf("Parse error!\n");
				return -1;
			}

		}
	}

	if (erase) {
		fprintf(f, "Erasing...\n");
		config_erase();
	}

	if (save) {
		printf("Saving...\n");
		config_save(&json);
	}

	if (load) {
		printf("Loading...\n");
		config_save(&json);
	}

	slcdev_resume();

	return 0;
}

int device_dump(FILE * f, int addr);
int device_attr_set(int addr, const char * name, const char * val);
int device_attr_print(FILE * f, int addr, const char * name);

int cmd_module(FILE * f, int argc, char ** argv)
{
	int addr;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 4)
		return SHELL_ERR_EXTRA_ARGS;

	addr = strtoul(argv[1], NULL, 0);
	if (addr > 160)
		return SHELL_ERR_ARG_INVALID;

	addr += 160;

	if (argc > 2) {
		if (argc > 3)
			return device_attr_set(addr, argv[2], argv[3]);
		return device_attr_print(f, addr, argv[2]);
	} else {
		device_dump(f, addr);
	}

	return 0;
}

int cmd_sensor(FILE * f, int argc, char ** argv)
{
	int addr;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 4)
		return SHELL_ERR_EXTRA_ARGS;

	addr = strtoul(argv[1], NULL, 0);
	if (addr > 160)
		return SHELL_ERR_ARG_INVALID;

	if (argc > 2) {
		if (argc > 3)
			return device_attr_set(addr, argv[2], argv[3]);
		return device_attr_print(f, addr, argv[2]);
	} else {
		device_dump(f, addr);
	}

	return 0;
}

int cmd_pw2(FILE * f, int argc, char ** argv)
{
	return 0;
}

int cmd_pw3(FILE * f, int argc, char ** argv)
{
	struct ss_device * dev;
	struct db_dev_model * mod;
	int addr;
	int sel;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 3)
		return SHELL_ERR_EXTRA_ARGS;

	addr = strtoul(argv[1], NULL, 0);
	if (addr > 319)
		return SHELL_ERR_ARG_INVALID;

	sel = strtoul(argv[1], NULL, 0);
	if (sel > 16)
		return SHELL_ERR_ARG_INVALID;

	dev = &ss_dev_tab[addr];

	if ((mod = db_dev_model_by_index(dev->model)) == NULL) {
		DCC_LOG1(LOG_WARNING, "invalid model: %d", dev->model);
		return SHELL_ERR_ARG_INVALID;
	}

	dev->pw3 = device_db_pw3_lookup(mod, sel, dev->tbias);

	return 0;
}

int cmd_dev(FILE * f, int argc, char ** argv)
{
	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if ((strcmp(argv[1], "stop") == 0) || 
		(strcmp(argv[1], "s") == 0)) {
		slcdev_stop();
	} else if ((strcmp(argv[1], "resume") == 0) || 
		(strcmp(argv[1], "r") == 0)) {
		slcdev_resume();
	} else
		return SHELL_ERR_ARG_INVALID;

	return 0;
}


int cmd_pw4(FILE * f, int argc, char ** argv)
{
	return 0;
}

int cmd_pw5(FILE * f, int argc, char ** argv)
{
	return 0;
}

int cmd_str(FILE * f, int argc, char ** argv)
{
	int ret = 0;
	int i;

	if (argc == 1)
		return microjs_str_pool_dump(&microjs_str_const);

	for (i = 1; i < argc; ++i) {
		if ((ret = const_str_write(argv[i], strlen(argv[i]))) < 0)
			break;
	}

	return ret;
}

int cmd_reboot(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	cm3_sysrst();

	return 0;
}


const struct shell_cmd cmd_tab[] = {

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

	{ cmd_rx, "rx", "r", "FILENAME", "XMODEM file receive" },

	{ cmd_trig, "trig", "t", "[ADDR]", "Trigger module address get/set" },

	{ cmd_enable, "enable", "e", "ADDR0 .. ADDR7", "Device enable" },

	{ cmd_disable, "disable", "d", "ADDR0 .. ADDR7", "Device disable" },

	{ cmd_self_test, "selftest", "st", "", "Self test" },

	{ cmd_isink, "isink", "i", "[MODE [PRE [PULSE]]]", "Self test" },

	{ cmd_slewrate, "slewrate", "sr", "[VALUE]", "Current slewrate set" },

	{ cmd_dbase, "dbase", "db", "[compile|stat]", "device database" },

	{ cmd_config, "config", "cfg", "[compile|erase|load|save]", 
		"configuration options" },

	{ cmd_cat, "cat", "", "<filename>", "display file content" },

	{ cmd_module, "module", "mod", "<addr> [attr [VAL]]", 
		"get/set module attribute" },

	{ cmd_sensor, "sensor", "sens", "<addr> [attr [VAL]]", 
		"get/set sensor attribute" },

	{ cmd_ls, "ls", "", "<filename>", "list files" },

	{ cmd_pw2, "pw2", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW2 value" },

	{ cmd_pw3, "pw3", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW3 value" },

	{ cmd_pw4, "pw4", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW4 value" },

	{ cmd_pw5, "pw5", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW4 value" },

	{ cmd_str, "str", "", "", "dump string pool" },

	{ cmd_dev, "dev", "", "[start|stop]", "dump string pool" },

	{ cmd_reboot, "reboot", "rst", "", "reboot" },

	{ NULL, "", "", NULL, NULL }
};

