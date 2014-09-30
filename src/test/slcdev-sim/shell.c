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
#include "profclk.h"

#include "slcdev-lib.h"

extern const struct shell_cmd cmd_tab[];

/* -------------------------------------------------------------------------
 * Help
 * ------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------
 * Filesystem
 * ------------------------------------------------------------------------- */

int cmd_rx(FILE * f, int argc, char ** argv)
{
	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;


	fprintf(f, "XMODEM ... ");
	if ((fs_xmodem_recv(f, argv[1])) < 0) {
		fprintf(f, "fs_xmodem_recv() failed!\r\n");
		return -1;
	}

	return 0;
}

int cmd_rm(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (!fs_dirent_lookup(argv[1], &entry)) {
		fprintf(f, "invalid file: \"%s\"\r\n", argv[1]);
		return SHELL_ERR_ARG_INVALID;
	}

	fs_file_unlink(&entry);

	return 0;
}

int cmd_cat(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;
	struct fs_file * fp;
	int i;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (!fs_dirent_lookup(argv[1], &entry)) {
		fprintf(f, "invalid file: \"%s\"\r\n", argv[1]);
		return SHELL_ERR_ARG_INVALID;
	}

	fp = entry.fp;

	for (i = 0; i < fp->size; ++i) { 
		int c = fp->data[i];
		fputc(c, f);
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
		fprintf(f, "0x%06x %6d 0x%04x %6d %s\r\n", 
				entry.blk_offs, entry.blk_size, 
				entry.fp->crc, entry.fp->size, entry.name);
	}

	return 0;
}

/* -------------------------------------------------------------------------
 * Trigger
 * ------------------------------------------------------------------------- */

int cmd_trig(FILE * f, int argc, char ** argv)
{
	unsigned int addr;
	bool mod = false;

	if (argc > 3)
		return SHELL_ERR_EXTRA_ARGS;

	if (argc > 1) {
		if (argc == 2)
			return SHELL_ERR_ARG_MISSING;

		if ((strcmp(argv[1], "sens") == 0) || 
			(strcmp(argv[1], "s") == 0)) {
			mod = false;
		} else if ((strcmp(argv[1], "mod") == 0) ||
			  (strcmp(argv[1], "m") == 0)) {
			mod = true;
		} else
			return SHELL_ERR_ARG_INVALID;

		addr = strtoul(argv[2], NULL, 0);
		if (addr > 159)
			return SHELL_ERR_ARG_INVALID;

		trig_addr_set(mod, addr);
	} else {
		trig_addr_get(&mod, &addr);
	}

	fprintf(f, "Trigger: %s %d\n", mod ? "module" : "sensor", addr);

	return 0;
}

/* remove uncofigured devices from the list */
void dev_lst_remove_unconfigured(uint32_t sb[], uint32_t mb[])
{
	struct ss_device * dev;
	unsigned int addr;

	for (addr = 0; addr < 160; ++addr) {
		dev = dev_sim_lookup(false, addr);
		if (!dev->cfg)
			sb[addr / 32] &= ~1;
		dev = dev_sim_lookup(true, addr);
		if (!dev->cfg)
			mb[addr / 32] &= ~1;
	}
}

int dev_lst_cmd_parse(FILE * f, int argc, char ** argv,
					  uint32_t sb[], uint32_t mb[])
{
	struct ss_device * dev;
	bool sens = false;
	bool mod = false;
	bool all = false;
	bool grp = false;
	unsigned int addr;
	int k;
	int i;

	memset(sb, 0, 160 / 8);
	memset(mb, 0, 160 / 8);

	if ((argc == 2) && (strcmp(argv[1], "all") == 0)) {
		all = true;
		mod = true;
		sens = true;
	} else {
		if (argc < 3)
			return SHELL_ERR_ARG_MISSING;

		if ((strcmp(argv[1], "sens") == 0) || 
			(strcmp(argv[1], "s") == 0)) {
			sens = true;
		} else if ((strcmp(argv[1], "mod") == 0) ||
			(strcmp(argv[1], "m") == 0)) {
			mod = true;
		} else if ((strcmp(argv[1], "grp") == 0) ||
			(strcmp(argv[1], "g") == 0)) {
			grp = true;
		} else {
			return SHELL_ERR_ARG_INVALID;
		}
		if ((argc == 3) && (strcmp(argv[2], "all") == 0))
			all = true;
	}

	if (all & !grp) {
		if (sens) {
			for (i = 0; i < (160 / 32); ++i)
				sb[i] = 0xffffffff;
			fprintf(f, "  All sensors\n");
		}
		if (mod) {
			for (i = 0; i < (160 / 32); ++i)
				mb[i] = 0xffffffff;
			fprintf(f, "  All modules\n");
		}
		return 0;
	}

	if (grp) {

		if (all) {
			for (addr = 0; addr < 160; ++addr) {
				dev = dev_sim_lookup(false, addr);
				if ((dev->grp[0] != 0) || (dev->grp[1] != 0) ||
					(dev->grp[2] != 0) || (dev->grp[3] != 0))
					sb[addr / 32] |= 1 << (addr % 32);

				dev = dev_sim_lookup(true, addr);
				if ((dev->grp[0] != 0) || (dev->grp[1] != 0) ||
					(dev->grp[2] != 0) || (dev->grp[3] != 0))
					mb[addr / 32] |= 1 << (addr % 32);
			}
			fprintf(f, "  All groups\n");
		} else {
			for (k = 2; k < argc; ++k) {
				unsigned int g = strtoul(argv[k], NULL, 0);

				if ((g < 1) || (g > 256))
					return SHELL_ERR_ARG_INVALID;

				for (addr = 0; addr < 160; ++addr) {
					dev = dev_sim_lookup(false, addr);
					if ((dev->grp[0] == g) || (dev->grp[1] == g) ||
						(dev->grp[2] == g) || (dev->grp[3] == g))
						sb[addr / 32] |= 1 << (addr % 32);

					dev = dev_sim_lookup(true, addr);
					if ((dev->grp[0] == g) || (dev->grp[1] == g) ||
						(dev->grp[2] == g) || (dev->grp[3] == g))
						mb[addr / 32] |= 1 << (addr % 32);
				}
				fprintf(f, "  Group %d\n", g);
			}
		}
		return 0;
	}

	for (k = 2; k < argc; ++k) {
		addr = strtoul(argv[k], NULL, 0);

		if ((addr < 0) || (addr > 159))
			return SHELL_ERR_ARG_INVALID;

		if (sens) {
			sb[addr / 32] |= (1 << (addr % 32));
			fprintf(f, "  Sensor %d\n", addr);
		}

		if (mod) {
			mb[addr / 32] |= (1 << (addr % 32));
			fprintf(f, "  Module %d\n", addr);
		}
	}

	return 0;

}

int cmd_enable(FILE * f, int argc, char ** argv)
{
	uint32_t sb[160 / 32];
	uint32_t mb[160 / 32];
	int ret;

	if (argc == 1) {
		struct ss_device * dev;
		unsigned int addr;

		/* Print enabled status flag of all the devices */

		fprintf(f, "Sensors:");
		for (addr = 0; addr < 160; ++addr) {
			dev = dev_sim_lookup(false, addr);
			if (dev->enabled)
				fprintf(f, " %3d", addr);
		}

		fprintf(f, "\nModules:");
		for (addr = 0; addr < 160; ++addr) {
			dev = dev_sim_lookup(true, addr);
			if (dev->enabled)
				fprintf(f, " %3d", addr);
		}
		fprintf(f, "\n");

		return 0;
	}

	if ((ret = dev_lst_cmd_parse(f, argc, argv, sb, mb)) < 0)
		return ret;

	dev_lst_remove_unconfigured(sb, mb);

	dev_sim_multiple_enable(sb, mb);

	fprintf(f, "Enabled.\n");

	return 0;
}

int cmd_disable(FILE * f, int argc, char ** argv)
{
	uint32_t sb[160 / 32];
	uint32_t mb[160 / 32];
	int ret;

	if ((ret = dev_lst_cmd_parse(f, argc, argv, sb, mb)) < 0)
		return ret;

	dev_lst_remove_unconfigured(sb, mb);

	dev_sim_multiple_disable(sb, mb);

	fprintf(f, "Disabled.\n");

	return 0;
}

int cmd_alarm(FILE * f, int argc, char ** argv)
{
	uint32_t sb[160 / 32];
	uint32_t mb[160 / 32];
	unsigned int lvl;
	int ret;

	if (argc == 1) {
		struct ss_device * dev;
		unsigned int addr;

		/* Print enabled status flag of all the devices */

		fprintf(f, "Sensors:");
		for (addr = 0; addr < 160; ++addr) {
			dev = dev_sim_lookup(false, addr);
			if (dev->alm)
				fprintf(f, " %3d", addr);
		}

		fprintf(f, "\nModules:");
		for (addr = 0; addr < 160; ++addr) {
			dev = dev_sim_lookup(true, addr);
			if (dev->alm)
				fprintf(f, " %3d", addr);
		}
		fprintf(f, "\n");

		return 0;
	}

	if (argc < 3)
		return SHELL_ERR_ARG_MISSING;

	lvl = strtoul(argv[1], NULL, 0);

	/* consume one parameter */
	argv++;
	argc--;

	/* fill in the bitmaps with the remaining of the command line */
	if ((ret = dev_lst_cmd_parse(f, argc, argv, sb, mb)) < 0)
		return ret;

	dev_lst_remove_unconfigured(sb, mb);

	/* set the alarm level for the selected devices */
	dev_sim_multiple_alarm_set(sb, mb, lvl);

	fprintf(f, "Alarm %d.\n", lvl);

	return 0;
}

int cmd_trouble(FILE * f, int argc, char ** argv)
{
	uint32_t sb[160 / 32];
	uint32_t mb[160 / 32];
	unsigned int lvl;
	int ret;

	if (argc == 1) {
		struct ss_device * dev;
		unsigned int addr;

		/* Print enabled status flag of all the devices */

		fprintf(f, "Sensors:");
		for (addr = 0; addr < 160; ++addr) {
			dev = dev_sim_lookup(false, addr);
			if (dev->tbl)
				fprintf(f, " %3d", addr);
		}

		fprintf(f, "\nModules:");
		for (addr = 0; addr < 160; ++addr) {
			dev = dev_sim_lookup(true, addr);
			if (dev->tbl)
				fprintf(f, " %3d", addr);
		}
		fprintf(f, "\n");

		return 0;
	}
	if (argc < 3)
		return SHELL_ERR_ARG_MISSING;

	lvl = strtoul(argv[1], NULL, 0);

	/* consume one parameter */
	argv++;
	argc--;

	/* fill in the bitmaps with the remaining of the command line */
	if ((ret = dev_lst_cmd_parse(f, argc, argv, sb, mb)) < 0)
		return ret;

	/* set the alarm level for the selected devices */
	dev_sim_multiple_trouble_set(sb, mb, lvl);

	fprintf(f, "Trouble %d.\n", lvl);

	return 0;
}


int cmd_dbase(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;
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

	slcdev_stop();

	if (erase) {
		fprintf(f, "Erasing...\n");
		fs_dirent_get(&entry, FLASHFS_DB_BIN);
		fs_file_unlink(&entry);
		/* uncofigure all devices */
		dev_sim_uncofigure_all();
	}

	if (compile) {
		fs_dirent_get(&entry, FLASHFS_DB_JSON);

		if (!device_db_need_update(entry.fp)) {
			fprintf(f, "Up-to-date.\n");
		} else {
			fprintf(f, "Compiling...\n");
			if (!device_db_compile(entry.fp)) {
				printf("Parse error!\n");
				return -1;
			}
		}
	}

	slcdev_resume();

	return 0;
}

int cmd_config(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;
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
			erase = true;
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
		} else
			return SHELL_ERR_ARG_INVALID;
	}

	fs_dirent_get(&entry, FLASHFS_CFG_JSON);

	slcdev_stop();

	if (erase) {
		struct fs_dirent bin;
		fprintf(f, "Erasing...\n");
		fs_dirent_get(&bin, FLASHFS_CFG_BIN);
		fs_file_unlink(&bin);
	}

	if (compile) {
		if (config_is_sane() && !config_need_update(entry.fp)) {
			fprintf(f, "Up-to-date.\n");
		} else {
			fprintf(f, "Compiling...\n");
			if (config_compile(entry.fp) < 0) {
				fprintf(f, "# Error!\n");
				return -1;
			}

		}
	}

	if (save) {
		fprintf(f, "Saving...\n");
		if (config_save(entry.fp) < 0) {
			fprintf(f, "# Error!\n");
			return -1;
		}
	}

	if (load) {
		fprintf(f, "Loading...\n");
		if (config_load() < 0) {
			fprintf(f, "# Error!\n");
			return -1;
		}
	}

	slcdev_resume();

	return 0;
}

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

	if (argc > 2) {
		if (argc > 3)
			return device_attr_set(true, addr, argv[2], argv[3]);
		return device_attr_print(f, true, addr, argv[2]);
	} else {
		device_dump(f, true, addr);
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
			return device_attr_set(false, addr, argv[2], argv[3]);
		return device_attr_print(f, false, addr, argv[2]);
	} else {
		device_dump(f, false, addr);
	}

	return 0;
}

int cmd_group(FILE * f, int argc, char ** argv)
{
	int g;

	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	for (g = 1; g < 256; ++g) {
		struct ss_device * dev;
		int addr;
		int n = 0;
		int k = 0;

		for (addr = 0; addr < 160; ++addr) {
			dev = dev_sim_lookup(false, addr);
			if ((dev->grp[0] == g) || (dev->grp[1] == g) ||
				(dev->grp[2] == g) || (dev->grp[3] == g)) {
				if (n++ == 0)
					fprintf(f, "\nGroup %d:", g);
				if (k++ == 0)
					fprintf(f, "\n  Sensors:");
				fprintf(f, "%4d", addr);
			}
		}

		k = 0;
		for (addr = 0; addr < 160; ++addr) {
			dev = dev_sim_lookup(true, addr);
			if ((dev->grp[0] == g) || (dev->grp[1] == g) ||
				(dev->grp[2] == g) || (dev->grp[3] == g)) {
				if (n++ == 0)
					fprintf(f, "\nGroup %d:", g);
				if (k++ == 0)
					fprintf(f, "\n  Modules:");
				fprintf(f, "%4d", addr);
			}
		}
		if (n) 
			fprintf(f, "\n");
	}


	return 0;
}

static int shell_pw_sel(FILE * f, int argc, char ** argv, int n)
{
	struct db_dev_model * mod;
	struct ss_device * dev;
	int addr;
	int sel;

	if (argc < 3)
		return SHELL_ERR_ARG_MISSING;

	addr = strtoul(argv[1], NULL, 0);
	if (addr > 160)
		return SHELL_ERR_ARG_INVALID;

	sel = strtoul(argv[2], NULL, 0);
	if (sel > 16)
		return SHELL_ERR_ARG_INVALID;

	dev = dev_sim_lookup(false, addr);

	if ((mod = db_dev_model_by_index(db_info_get(), dev->model)) == NULL)
		return SHELL_ERR_ARG_INVALID;

	switch (n) { 
	case 1:
		dev->pw1 = device_db_pw_lookup(mod->pw1, sel);
		break;
	case 2:
		dev->pw2 = device_db_pw_lookup(mod->pw2, sel);
		break;
	case 3:
		dev->pw3 = device_db_pw_lookup(mod->pw3, sel);
		break;
	case 4:
		dev->pw4 = device_db_pw_lookup(mod->pw4, sel);
		break;
	case 5:
		dev->pw5 = device_db_pw_lookup(mod->pw5, sel);
		break;
	}

	return 0;
}

int cmd_pw2(FILE * f, int argc, char ** argv)
{
	return shell_pw_sel(f, argc, argv, 2);
}

int cmd_pw3(FILE * f, int argc, char ** argv)
{
	return shell_pw_sel(f, argc, argv, 3);
}

int cmd_pw4(FILE * f, int argc, char ** argv)
{
	return shell_pw_sel(f, argc, argv, 4);
}

int cmd_sim(FILE * f, int argc, char ** argv)
{
	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if ((strcmp(argv[1], "stop") == 0) || 
		(strcmp(argv[1], "s") == 0)) {
		slcdev_event_raise(SLC_EV_SIM_STOP);
	} else if ((strcmp(argv[1], "resume") == 0) || 
		(strcmp(argv[1], "r") == 0)) {
		slcdev_event_raise(SLC_EV_SIM_RESUME);
	} else
		return SHELL_ERR_ARG_INVALID;

	return 0;
}

int cmd_reboot(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	cm3_sysrst();

	return 0;
}

int cmd_js(FILE * f, int argc, char ** argv)
{
	struct symtab * symtab = (struct symtab *)slcdev_symbuf; /* symbols */
	uint8_t code[512]; /* compiled code */
	uint32_t sdtbuf[64]; /* compiler buffer */
	int32_t stack[16]; /* exec stack */
	struct microjs_sdt * microjs; 
	struct microjs_rt rt;
	struct microjs_vm vm; 
	struct symstat symstat;
	struct fs_dirent entry;
	uint32_t start_clk;
	uint32_t stop_clk;
	int code_sz;
	char * script;
	int len;
	int ret;


	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (fs_dirent_lookup(argv[1], &entry)) {
		script = (char *)entry.fp->data;
		len = entry.fp->size;
	} else {
		script = argv[1];
		len = strlen(argv[1]);
	}

	profclk_init();

	/* initialize compiler */
	microjs = microjs_sdt_init(sdtbuf, sizeof(sdtbuf), symtab);

	symstat = symtab_state_save(symtab);

	microjs_sdt_begin(microjs, code, sizeof(code));

	start_clk = profclk_get();
	if ((ret = microjs_compile(microjs, script, len)) < 0) {
		symtab_state_rollback(symtab, symstat);
		fprintf(f, "# compile error: %d\n", -ret);
		microjs_sdt_error(f, microjs, ret);
		return -1;
	}
	if ((ret = microjs_sdt_end(microjs, &rt)) < 0) {
		symtab_state_rollback(symtab, symstat);
		fprintf(f, "# compile error: %d\n", -ret);
		microjs_sdt_error(f, microjs, ret);
		return -1;
	}
	stop_clk = profclk_get();

	code_sz = ret;
	fprintf(f, " - Compile time: %d us.\n", profclk_us(stop_clk - start_clk));
	fprintf(f, " - code: %d\n", code_sz);
	fprintf(f, " - data: %d of %d\n", rt.data_sz, sizeof(slcdev_vm_data));
	fprintf(f, " - stack: %d of %d\n", rt.stack_sz, sizeof(stack));

	if (rt.data_sz > sizeof(slcdev_vm_data)) {
		fprintf(f, "# data overlow. %d bytes required\n", rt.data_sz);
		return -1;
	}

	if (rt.stack_sz > sizeof(stack)) {
		fprintf(f, "# stack overflow. %d bytes required\n", rt.stack_sz);
		return -1;
	}

#if MICROJS_TRACE_ENABLED
	microjs_vm_tracef = f;
#endif

	/* initialize virtual machine instance */
	microjs_vm_init(&vm, &rt, NULL, slcdev_vm_data, stack);

	start_clk = profclk_get();
	if ((ret = microjs_exec(&vm, code, code_sz)) != 0){
		fprintf(f, "# exec error: %d\n", ret);
		return -1;
	}
	stop_clk = profclk_get();
	fprintf(f, "Exec time: %d us.\n", profclk_us(stop_clk - start_clk));

	return 0;
}


int cmd_run(FILE * f, int argc, char ** argv)
{
	struct microjs_vm vm; 
	struct microjs_rt rt;
	int32_t stack[16]; /* local stack */
	uint8_t * code;
	int ret;

	if (argc < 3)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 3)
		return SHELL_ERR_EXTRA_ARGS;

	DCC_LOG(LOG_TRACE, "db_js_lookup()...");

	if ((code = db_js_lookup(argv[1], argv[2])) == NULL)
		return SHELL_ERR_ARG_INVALID;

	DCC_LOG1(LOG_TRACE, "code=%08x.", code);

	rt.data_sz = sizeof(slcdev_vm_data);
	rt.stack_sz = sizeof(stack);

	/* initialize virtual machine instance */
	microjs_vm_init(&vm, &rt, NULL, slcdev_vm_data, stack);

	DCC_LOG(LOG_TRACE, "microjs_exec...");

	if ((ret = microjs_exec(&vm, code, 4096)) != 0) {
		fprintf(f, "# exec error: %d\n", ret);
		return -1;
	}

	fprintf(f, "\n");

	return 0;
}

int cmd_str(FILE * f, int argc, char ** argv)
{
	int ret = 0;
	int i;

	if (argc == 1)
		return const_strbuf_dump(f);

	for (i = 1; i < argc; ++i) {
		if ((ret = const_str_add(argv[i], strlen(argv[i]))) < 0)
			break;
	}

	return ret;
}
#if 0

int cmd_self_test(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	lamp_test();

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

#endif

const struct shell_cmd cmd_tab[] = {

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

#if 0
	{ cmd_self_test, "selftest", "st", "", "Self test" },

	{ cmd_isink, "isink", "i", "[MODE [PRE [PULSE]]]", "Self test" },

	{ cmd_slewrate, "slewrate", "sr", "[VALUE]", "Current slewrate set" },

#endif
	{ cmd_str, "str", "", "", "dump string pool" },

	{ cmd_rx, "rx", "r", "FILENAME", "XMODEM file receive" },

	{ cmd_cat, "cat", "", "<filename>", "display file content" },

	{ cmd_ls, "ls", "", "<filename>", "list files" },

	{ cmd_rm, "rm", "", "<filename>", "remove files" },

	{ cmd_trig, "trig", "t", "[ADDR]", "Trigger module address get/set" },

	{ cmd_enable, "enable", "e", "[<sens|mod|grp>[N1 .. N6]|all] ", 
		"Device enable" },

	{ cmd_disable, "disable", "d", "ADDR0 .. ADDR7", "Device disable" },

	{ cmd_dbase, "dbase", "db", "[compile|stat]", "device database" },

	{ cmd_config, "config", "cfg", "[compile|erase|load|save]", 
		"configuration options" },

	{ cmd_module, "module", "mod", "<addr> [attr [VAL]]", 
		"get/set module attribute" },

	{ cmd_sensor, "sensor", "sens", "<addr> [attr [VAL]]", 
		"get/set sensor attribute" },

	{ cmd_group, "group", "grp", "", "show group information" },

	{ cmd_alarm, "alarm", "alm", "", "alarm" },

	{ cmd_trouble, "trouble", "tbl", "", "trouble" },

	{ cmd_pw2, "pw2", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW2 value" },

	{ cmd_pw3, "pw3", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW3 value" },

	{ cmd_pw4, "pw4", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW4 value" },

	{ cmd_sim, "sim", "", "[start|stop]", "" },
	
	{ cmd_js, "js", "", "script", "javascript" },

	{ cmd_run, "run", "", "script", "run compiled code" },

	{ cmd_reboot, "reboot", "rst", "", "reboot" },

	{ NULL, "", "", NULL, NULL }
};

