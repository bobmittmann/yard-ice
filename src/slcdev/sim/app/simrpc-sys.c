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

#include <thinkos.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <time.h>
#include <string.h>
#include <sys/shell.h>
#include <sys/file.h>
#include <sys/null.h>
#include <sys/dcclog.h>

#include "board.h"
#include "simlnk.h"
#include "simrpc.h"
#include "version.h"
#include "simrpc_svc.h"
#include "slcdev.h"

extern const struct shell_cmd cmd_tab[];

void simrpc_appinfo_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct simrpc_appinfo inf;

	DCC_LOG(LOG_TRACE, "...");

	inf.uptime = time(NULL);
	inf.version.major = VERSION_MAJOR;
	inf.version.minor = VERSION_MINOR;
	inf.version.build = VERSION_BUILD;
	inf.version.timestamp = VERSION_TIMESTAMP;

	simrpc_send(SIMRPC_REPLY_OK(hdr), &inf, sizeof(inf));
}

void simrpc_cfginfo_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct slcdev_cfg_info * cfg;
	struct simrpc_cfginfo inf;

	if ((cfg = config_info_get()) == NULL) {
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EINVAL);
		return;
	}

	strncpy(inf.tag, const_str(cfg->tag), CFGINFO_TAG_MAX);
	inf.tag[CFGINFO_TAG_MAX] = '\0'; 

	strncpy(inf.author, const_str(cfg->author), CFGINFO_AUTHOR_MAX);
	inf.author[CFGINFO_AUTHOR_MAX] = '\0'; 

	strncpy(inf.desc, const_str(cfg->desc), CFGINFO_DESC_MAX);
	inf.desc[CFGINFO_DESC_MAX] = '\0'; 

	inf.version.major = cfg->version[0];
	inf.version.minor = cfg->version[1];
	inf.version.build = cfg->version[2];

	simrpc_send(SIMRPC_REPLY_OK(hdr), &inf, sizeof(inf));
}

void simrpc_dbinfo_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct db_info * db;
	struct simrpc_dbinfo inf;

	if ((db = db_info_get()) == NULL) {
		simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_EINVAL);
		return;
	}

	strncpy(inf.desc, const_str(db->desc), DBINFO_DESC_MAX);
	inf.desc[DBINFO_DESC_MAX] = '\0'; 

	inf.version.major = db->version[0];
	inf.version.minor = db->version[1];
	inf.version.build = db->version[2];

	simrpc_send(SIMRPC_REPLY_OK(hdr), &inf, sizeof(inf));
}

struct rpc_str {
	char buf[SIMRPC_DATA_MAX];
	unsigned int len;
};

int simrpc_write(struct rpc_str * str, const char * buf, int cnt)
{
	int rem = SIMRPC_DATA_MAX - str->len;
	int n = cnt;

	if (n > rem)
		n = rem;

	memcpy(&str->buf[str->len], buf, n);
	str->len += n;

	return cnt;
}

const struct fileop simrpc_fops = {
	.write = (void *)simrpc_write,
	.read = (void *)null_read,
	.flush = (void *)null_flush,
	.close = (void *)null_close
};


void simrpc_shellexec_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	char * line = (char *)data;
	struct rpc_str str;
	struct file rpc_file = {
		.data = &str,
		.op = &simrpc_fops
	};
	struct file * f = &rpc_file;
	struct shell_cmd * cmd;
	int ret;

	str.len = 0;
	line[cnt] = '\0';

	if ((cmd = cmd_lookup(cmd_tab, line)) == NULL) {
		fprintf(f, "Command not found!\n");
	} else {
		ret = cmd_exec(f, cmd, line);
		if ((ret < 0) && (ret !=  SHELL_ABORT)) {
			fprintf(f, "Error: %d\n", -ret);
		} 
	}

	simrpc_send(SIMRPC_REPLY_OK(hdr), str.buf, str.len);
}

int js(FILE * f, char * script, unsigned int len);

void simrpc_jsexec_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	char * line = (char *)data;
	struct rpc_str str;
	struct file rpc_file = {
		.data = &str,
		.op = &simrpc_fops
	};
	struct file * f = &rpc_file;
	FILE * tmp = stdout;

	str.len = 0;
	stdout = f;
	if (cnt < SIMRPC_DATA_MAX)
		line[cnt] = '\0';
	js(f, line, cnt);
	stdout = tmp;

	simrpc_send(SIMRPC_REPLY_OK(hdr), str.buf, str.len);
}

void simrpc_cfgcompile_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct rpc_str str;
	struct file rpc_file = {
		.data = &str,
		.op = &simrpc_fops
	};
	struct file * f = &rpc_file;
	FILE * tmp = stdout;
	struct fs_dirent json;

	str.len = 0;
	stdout = f;

	fs_dirent_get(&json, FLASHFS_CFG_JSON);
	if (config_is_sane() && !config_need_update(json.fp)) {
		fprintf(f, "Up-to-date.\n");
	} else {
		slcdev_stop();
		struct fs_dirent bin;

		DCC_LOG(LOG_TRACE, "pausing simulation!");
		slcdev_sim_stop();

		/* Erase current configuration */
		fs_dirent_get(&bin, FLASHFS_CFG_BIN);
		fs_file_unlink(&bin);

		if (config_compile(json.fp) < 0) {
			DCC_LOG(LOG_WARNING, "config_compile() failed!");
			fprintf(f, "# Error!\n");
			/* purge the invalid config */
			fs_dirent_get(&bin, FLASHFS_CFG_BIN);
			fs_file_unlink(&bin);
		} else {
			struct microjs_rt * rt;

			rt = symtab_rt_get((struct symtab *)slcdev_symbuf);
			fprintf(f, " - data: %d of %d\n", 
					rt->data_sz, sizeof(slcdev_vm_data));
			fprintf(f, " - stack: %d of %d\n", 
					rt->stack_sz, sizeof(slcdev_vm_stack));

			if (config_save(json.fp) < 0) {
				DCC_LOG(LOG_WARNING, "config_save() failed!");
				fprintf(f, "# Error: config_save()!\n");
			} else if (config_load() < 0) {
				DCC_LOG(LOG_WARNING, "config_load() failed!");
				fprintf(f, "# Error: config_load()!\n");
			} else {
				DCC_LOG(LOG_TRACE, "restarting simulation!");
				slcdev_sim_resume();
			}
		}
	}

	stdout = tmp;

	simrpc_send(SIMRPC_REPLY_OK(hdr), str.buf, str.len);
}

void db_cfg_purge(void);

void simrpc_dbcompile_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct rpc_str str;
	struct file rpc_file = {
		.data = &str,
		.op = &simrpc_fops
	};
	struct file * f = &rpc_file;
	FILE * tmp = stdout;
	struct fs_dirent json;

	str.len = 0;
	stdout = f;

	fs_dirent_get(&json, FLASHFS_DB_JSON);
	if (!device_db_need_update(json.fp)) {
		fprintf(f, "Up-to-date.\n");
	} else {
		struct fs_dirent bin;

		slcdev_stop();

		/* uncofigure all devices */
		dev_sim_uncofigure_all();

		/* Erase database */
		fs_dirent_get(&bin, FLASHFS_DB_BIN);
		fs_file_unlink(&bin);

		/* Erase config */
		fs_dirent_get(&bin, FLASHFS_CFG_BIN);
		fs_file_unlink(&bin);

		/* Erase strings */
		const_strbuf_purge();

		/* Initialize symbol table */
		symtab_init(slcdev_symbuf, sizeof(slcdev_symbuf));

		if (!device_db_compile(json.fp)) {
			printf("Parse error!\n");
		} else {
			struct microjs_rt * rt;
			rt = symtab_rt_get((struct symtab *)slcdev_symbuf);
			fprintf(f, " - data: %d of %d\n", 
					rt->data_sz, sizeof(slcdev_vm_data));
			fprintf(f, " - stack: %d of %d\n", 
					rt->stack_sz, sizeof(slcdev_vm_stack));
		}
	}

	stdout = tmp;

	simrpc_send(SIMRPC_REPLY_OK(hdr), str.buf, str.len);
}

