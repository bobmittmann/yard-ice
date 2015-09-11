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

#include "slcdev-lib.h"

extern const struct shell_cmd cmd_tab[];
extern const char * version_str;
extern const char * copyright_str;

/* -------------------------------------------------------------------------
 * Help
 * ------------------------------------------------------------------------- */

int cmd_help(FILE *f, int argc, char ** argv)
{
	struct shell_cmd * cmd;

	if (argc > 2)
		return -1;

	if (argc > 1) {
		if ((cmd = cmd_lookup(cmd_tab, argv[1])) == NULL) {
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

	fprintf(f, "\n");

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

int cmd_bacnet(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	return SHELL_ABORT;
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
 * Environment
 * ------------------------------------------------------------------------- */

int cmd_verbose(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	usr.verbose = true;

	fprintf(f, "Verbose mode.\n");

	return 0;
}

int cmd_quiet(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	usr.verbose = false;

	fprintf(f, "Quiet mode.\n");

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

		trig_addr_set(addr);
		if (mod)
			trig_module_set(true);
		else
			trig_sensor_set(true);

	} else {
		addr = trig_addr_get();
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
		dev = sensor(addr);
		if (!dev->cfg)
			sb[addr / 32] &= ~(1 << (addr % 32));

		dev = module(addr);
		if (!dev->cfg)
			mb[addr / 32] &= ~(1 << (addr % 32));
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
				dev = sensor(addr);
				if ((dev->grp[0] != 0) || (dev->grp[1] != 0) ||
					(dev->grp[2] != 0) || (dev->grp[3] != 0))
					sb[addr / 32] |= 1 << (addr % 32);

				dev = module(addr);
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
					dev = sensor(addr);
					if ((dev->grp[0] == g) || (dev->grp[1] == g) ||
						(dev->grp[2] == g) || (dev->grp[3] == g))
						sb[addr / 32] |= 1 << (addr % 32);

					dev = module(addr);
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
			dev = sensor(addr);
			if (dev->enabled)
				fprintf(f, " %3d", addr);
		}

		fprintf(f, "\nModules:");
		for (addr = 0; addr < 160; ++addr) {
			dev = module(addr);
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
			dev = sensor(addr);
			if (dev->alm)
				fprintf(f, " %3d", addr);
		}

		fprintf(f, "\nModules:");
		for (addr = 0; addr < 160; ++addr) {
			dev = module(addr);
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
			dev = sensor(addr);
			if (dev->tbl)
				fprintf(f, " %3d", addr);
		}

		fprintf(f, "\nModules:");
		for (addr = 0; addr < 160; ++addr) {
			dev = module(addr);
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

void db_cfg_purge(void)
{
	struct fs_dirent entry;

	/* uncofigure all devices */
	dev_sim_uncofigure_all();

	/* Erase database */
	fs_dirent_get(&entry, FLASHFS_DB_BIN);
	fs_file_unlink(&entry);

	/* Erase config */
	fs_dirent_get(&entry, FLASHFS_CFG_BIN);
	fs_file_unlink(&entry);

	/* Erase strings */
	const_strbuf_purge();

	/* Initialize symbol table */
	symtab_init(slcdev_symbuf, sizeof(slcdev_symbuf));
}

int cmd_dbase(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;
	bool erase = false;
	bool compile = false;
	bool xfer = false;
	bool dump = false;
	int i;

	if (argc == 1) {
		fprintf(f, "Device Model Database:\n");
		return device_db_info(f);
	}

	for (i = 1; i < argc; ++i) {
		if ((strcmp(argv[i], "compile") == 0) || 
			(strcmp(argv[i], "c") == 0)) {
			compile = true;
			erase = true;
		} else if ((strcmp(argv[i], "erase") == 0) || 
			(strcmp(argv[i], "e") == 0)) {
			erase = true;
		} else if ((strcmp(argv[i], "xfer") == 0) || 
			(strcmp(argv[i], "x") == 0)) {
			xfer = true;
			erase = true;
			compile = true;
		} else if ((strcmp(argv[i], "dump") == 0) || 
			(strcmp(argv[i], "d") == 0)) {
			dump = true;
		} else
			return SHELL_ERR_ARG_INVALID;
	}

	slcdev_stop();

	if (xfer) {
		fprintf(f, "XMODEM receive: '%s'... ", "db.js");
		if ((fs_xmodem_recv(f, "db.js")) < 0) {
			fprintf(f, "fs_xmodem_recv() failed!\r\n");
			return -1;
		}
		fprintf(f, "\n");
	}

	if (erase) {
		fprintf(f, "Erasing DB...\n");
		db_cfg_purge();
	}

	if (compile) {
		fs_dirent_get(&entry, FLASHFS_DB_JSON);

		if (!device_db_need_update(entry.fp)) {
			fprintf(f, "Up-to-date.\n");
		} else {
			struct microjs_rt * rt;

			fprintf(f, "Compiling...\n");
			if (!device_db_compile(entry.fp)) {
				printf("Parse error!\n");
				return -1;
			}

			if (SLCDEV_VERBOSE()) {
				rt = symtab_rt_get((struct symtab *)slcdev_symbuf);
				fprintf(f, " - data: %d of %d\n", 
						rt->data_sz, sizeof(slcdev_vm_data));
				fprintf(f, " - stack: %d of %d\n", 
						rt->stack_sz, sizeof(slcdev_vm_stack));
			}
		}
		
		device_db_info(f);
	}

	if (dump)
		device_db_dump(f);

	return 0;
}

int cmd_config(FILE * f, int argc, char ** argv)
{
	bool xfer = false;
	bool erase = false;
	bool compile = false;
	bool load = false;
	bool restart = false;
	int i;

	if (argc == 1) {
		fprintf(f, "Simulation Configuration:\n");
		return config_show_info(f);
	}

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
		} else if ((strcmp(argv[i], "xfer") == 0) || 
			(strcmp(argv[i], "x") == 0)) {
			xfer = true;
			erase = true;
			compile = true;
		} else
			return SHELL_ERR_ARG_INVALID;
	}

	if (xfer) {
		slcdev_stop();
		fprintf(f, "XMODEM receive: '%s'... ", "cfg.js");
		if ((fs_xmodem_recv(f, "cfg.js")) < 0) {
			fprintf(f, "fs_xmodem_recv() failed!\r\n");
			return -1;
		}
		fprintf(f, "\n");
	}

	if (erase) {
		struct fs_dirent bin;
		fprintf(f, "Erasing...\n");
		fs_dirent_get(&bin, FLASHFS_CFG_BIN);
		fs_file_unlink(&bin);
	}

	if (compile) {
		struct fs_dirent json;

		fs_dirent_get(&json, FLASHFS_CFG_JSON);

		if (config_is_sane() && !config_need_update(json.fp)) {
			fprintf(f, "Up-to-date.\n");
		} else {
			struct microjs_rt * rt;
			slcdev_stop();
			fprintf(f, "Compiling...\n");
			if (config_compile(json.fp) < 0) {
				struct fs_dirent bin;
				fprintf(f, "# Error!\n");
				/* purge the invalid config */
				fs_dirent_get(&bin, FLASHFS_CFG_BIN);
				fs_file_unlink(&bin);
				return -1;
			}

			if (SLCDEV_VERBOSE()) {
				rt = symtab_rt_get((struct symtab *)slcdev_symbuf);
				fprintf(f, " - data: %d of %d\n", 
						rt->data_sz, sizeof(slcdev_vm_data));
				fprintf(f, " - stack: %d of %d\n", 
						rt->stack_sz, sizeof(slcdev_vm_stack));
			}

			fprintf(f, "Saving...\n");
			if (config_save(json.fp) < 0) {
				fprintf(f, "# Error!\n");
				return -1;
			}
			restart = true;
		}
		config_show_info(f);
	}

	if (load) {
		fprintf(f, "Loading...\n");
		slcdev_stop();
		if (config_load() < 0) {
			fprintf(f, "# Error!\n");
			return -1;
		}
		restart = true;
	}

	if (restart) {
		/* restart simulation */
		thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_SIM_START);
	}

	return 0;
}

int cmd_reboot(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	cm3_sysrst();

	return 0;
}

int cmd_sym(FILE * f, int argc, char ** argv)
{
	struct symtab * symtab = (struct symtab *)slcdev_symbuf; /* symbols */

	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	return symtab_dump(f, symtab);
}

#if 0
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

	dev = sensor(addr);

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


int cmd_js(FILE * f, int argc, char ** argv)
{
	struct fs_dirent entry;
	char * script;
	int len;

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

	return js(f, script, len);
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


int cmd_self_test(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	lamp_test();

	return 0;
}

#endif

int cmd_isink(FILE * f, int argc, char ** argv)
{
	unsigned int mode = 2;
	unsigned int rate = 1;
	unsigned int pre = 50;
	unsigned int pulse = 200;

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
	}

	if (pulse < pre)
		pulse = pre;

	fprintf(f, "PW pulse: mode=%d rate=%d pulse=%dus pre=%dus ...\n",
			mode, rate, pulse, pre);

	isink_mode_set(mode | (rate << 5));
	thinkos_sleep(2);
	isink_pulse(pre, pulse);
	
	return 0;
}

void uart_xflash(void * uart, uint32_t offs, uint32_t len);

int cmd_xflash(FILE * f, int argc, char ** argv)
{
	uint32_t offs = 0x00000;
	uint32_t size = 0x00000;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	do {
		if ( (strcmp(argv[1], "firm") == 0) ||
			 (strcmp(argv[1], "f") == 0)) {
			offs = 0;
			size = 56 * 1024;
			break;;
		} 
		return SHELL_ERR_ARG_INVALID;
	} while (0);

	fprintf(f, "Shutdown...\n");
	fflush(f);

	slcdev_sim_stop();
	slcdev_sleep();
	isink_sleep();
	io_shutdown();

	fprintf(f, "Firmware update %d bytes...\n", size);
	fflush(f);

	uart_xflash(STM32_USART2, offs, size);

	return 0;
}

int cmd_sim(FILE * f, int argc, char ** argv)
{
	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if ((strcmp(argv[1], "stop") == 0) || 
		(strcmp(argv[1], "s") == 0)) {
		thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_SIM_STOP);
		fprintf(f, "Simulation paused...\n");
	} else if ((strcmp(argv[1], "resume") == 0) || 
		(strcmp(argv[1], "r") == 0)) {
		thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_SIM_RESUME);
		fprintf(f, "Simulation resumed...\n");
	} else
		return SHELL_ERR_ARG_INVALID;

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
			dev = sensor(addr);
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
			dev = module(addr);
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

int cmd_version(FILE *f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	fprintf(f, "%s\n", version_str);
	fprintf(f, "%s\n", copyright_str);

	return 0;
}


const struct shell_cmd cmd_tab[] = {

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

#if 0
	{ cmd_self_test, "selftest", "st", "", "Self test" },

	{ cmd_slewrate, "slewrate", "sr", "[VALUE]", "Current slewrate set" },

	{ cmd_run, "run", "", "script", "run compiled code" },

	{ cmd_js, "js", "", "script", "javascript" },

	{ cmd_module, "module", "mod", "<addr> [attr [VAL]]", 
		"get/set module attribute" },

	{ cmd_sensor, "sensor", "sens", "<addr> [attr [VAL]]", 
		"get/set sensor attribute" },

	{ cmd_str, "str", "", "", "dump string pool" },

	{ cmd_pw2, "pw2", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW2 value" },

	{ cmd_pw3, "pw3", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW3 value" },

	{ cmd_pw4, "pw4", "", "<addr> [set [VAL]] | [lookup [SEL]]>", 
		"get set PW4 value" },

#endif
	{ cmd_bacnet, "BACnet", "", "", 
		"Start BACnet Data Link Connection" },


	{ cmd_alarm, "alarm", "alm", "[[LVL] [<sens|mod|grp>[N1 .. N6]|all]", 
		"set/get alarm level" },

	{ cmd_cat, "cat", "", "<filename>", 
		"display file content" },

	{ cmd_config, "config", "cfg", "[compile|erase|load|xfer]", 
		"configuration options" },

	{ cmd_dbase, "dbase", "db", "[compile|dump|erase|xfer]", 
		"manage device database" },

	{ cmd_disable, "disable", "d", "[<sens|mod|grp>[N1 .. N6]|all] ", 
		"device disable" },

	{ cmd_enable, "enable", "e", "[<sens|mod|grp>[N1 .. N6]|all] ", 
		"device enable" },

	{ cmd_group, "group", "grp", "", 
		"show group information" },

	{ cmd_isink, "isink", "isk", "[MODE [RATE [PULSE [PRE]]]]]", 
		"isink pulse" },

	{ cmd_ls, "ls", "", "", 
		"list files" },

	{ cmd_quiet, "quiet", "q", "", 
		"enable quiet mode" },

	{ cmd_reboot, "reboot", "rst", "", 
		"reboot system" },

	{ cmd_rm, "rm", "", "<FILENAME>", 
		"remove files" },

	{ cmd_rx, "rx", "", "<FILENAME>", 
		"Xmodem file receive" },

	{ cmd_sim, "sim", "", "[stop|resume]", 
		"stop/resume simulation" },

	{ cmd_sym, "sym", "", "", 
		"JS symbol table dump" },

	{ cmd_trig, "trig", "t", "[ADDR]", 
		"trigger module address get/set" },

	{ cmd_trouble, "trouble", "tbl", "[[LVL] [<sens|mod|grp>[N1 .. N6]|all]", 
		"set/get trouble level" },

	{ cmd_verbose, "verbose", "v", "", 
		"enable verbose mode" },

	{ cmd_version, "version", "ver", "", 
		"show version information" },

	{ cmd_xflash, "xflash", "xf", "firm", 
		"upgrade firmware using Xmodem" },

	{ NULL, "", "", NULL, NULL }
};

