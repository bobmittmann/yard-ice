/*
 * rpc_test.c
 *
 *  Created on: Sep 23, 2015
 *      Author: bmittmann
 */
#include <stdlib.h>
#include <stdio.h>
#include <simrpc.h>
#include <trace.h>
#include <hexdump.h>
#include <sys/file.h>
#include <sys/null.h>
#include "profclk.h"
#include "www.h"

void rpc_test_reset(FILE * f, int port)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
	} else {
		if (simrpc_reboot(sp) < 0) {
			fprintf(f, "simrpc_reboot() failed!\n");
		}
		simrpc_close(sp);
	}
}

void rpc_test_exec(FILE * f, int port)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
	} else {
		if (simrpc_exec(sp, SIMRPC_EXEC_KEY('A', 'P', 'P', '\0')) < 0) {
			fprintf(f, "simrpc_exec() failed!\n");
		}
		simrpc_close(sp);
	}
}

void rpc_test_suspend(FILE * f, int port)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
	} else {
		if (simrpc_suspend(sp) < 0) {
			fprintf(f, "simrpc_suspend() failed!\n");
		}
		simrpc_close(sp);
	}
}

void rpc_test_resume(FILE * f, int port)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
	} else {
		if (simrpc_resume(sp) < 0) {
			fprintf(f, "simrpc_resume() failed!\n");
		}
		simrpc_close(sp);
	}
}

void rpc_test_file_write(FILE * f, int port, char * fname)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;
	char s[128];
	uint32_t start;
	uint32_t stop;
	int n;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
		return;
	}

	simrpc_set_timeout(sp, 500);

	start = profclk_get();
	if (simrpc_file_create(sp, fname) < 0) {
		fprintf(f, "simrpc_file_create() failed!\n");
		simrpc_close(sp);
		return;
	}
	stop = profclk_get();
	fprintf(f, "simrpc_file_create(): %d us.\n", profclk_us(stop - start));

	n = sprintf(s, "The quick brown fox jumps over the lazy dog!");

	start = profclk_get();
	if (simrpc_file_write(sp, s, n) < 0) {
		fprintf(f, "simrpc_file_write() failed!\n");
	}
	stop = profclk_get();
	fprintf(f, "simrpc_file_write(): %d us.\n", profclk_us(stop - start));

	if (simrpc_file_close(sp) < 0) {
		fprintf(f, "simrpc_file_close() failed!\n");
	}

	simrpc_close(sp);
}

void rpc_test_file_read(FILE * f, int port, char * fname)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;
	char s[512];
	uint32_t start;
	uint32_t stop;
	int n;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
		return;
	}

	start = profclk_get();
	if (simrpc_file_open(sp, "cfg.js") < 0) {
		fprintf(f, "simrpc_file_create() failed!\n");
		simrpc_close(sp);
		return;
	}
	stop = profclk_get();
	fprintf(f, "simrpc_file_open(): %d us.\n", profclk_us(stop - start));


	start = profclk_get();
	while ((n = simrpc_file_read(sp, s, 128)) > 0) {
		fwrite(s, n, 1, f);
	}

	if (n < 0) {
		fprintf(f, "\n\n#ERROR: simrpc_file_read() failed!\n");
	}

	stop = profclk_get();
	fprintf(f, "simrpc_file_read(): %d us.\n", profclk_us(stop - start));

	if (simrpc_file_close(sp) < 0) {
		fprintf(f, "simrpc_file_close() failed!\n");
	}

	simrpc_close(sp);
}

void rpc_test_file_remove(FILE * f, int port, char * fname)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
	} else {
		if (simrpc_file_unlink(sp, fname) < 0) {
			fprintf(f, "simrpc_file_unlink() failed!\n");
		}
		simrpc_close(sp);
	}
}

void rpc_test_mem_read(FILE * f, int port, uint32_t addr, unsigned int cnt)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;
	uint8_t buf[512];
	int n;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
		return;
	}

	if (simrpc_mem_lock(sp, addr, cnt) < 0) {
		fprintf(f, "simrpc_mem_lock() failed!\n");
	} else {
		while (cnt > 0) {
			if ((n = simrpc_mem_read(sp, buf, cnt)) < 0) {
				fprintf(f, "simrpc_mem_read() failed!\n");
				break;
			} else {
				show_hex8(f, addr, buf, n);
			}
			cnt -= n;
			addr += n;
		}
		if (simrpc_mem_unlock(sp, addr, cnt) < 0) {
			fprintf(f, "simrpc_mem_unlock() failed!\n");
		}
	}

	simrpc_close(sp);
}

void rpc_test_mem_erase(FILE * f, int port, uint32_t addr, unsigned int size)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;
	int n;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
		return;
	}
	if (simrpc_mem_lock(sp, addr, size) < 0) {
		fprintf(f, "simrpc_mem_lock() failed!\n");
	} else {
		if ((n = simrpc_mem_erase(sp, 0, size)) < 0) {
			fprintf(f, "simrpc_mem_erase() failed!\n");
		}
		if (simrpc_mem_unlock(sp, addr, size) < 0) {
			fprintf(f, "simrpc_mem_unlock() failed!\n");
		}
	}
	simrpc_close(sp);
}

void rpc_test_mem_crc(FILE * f, int port, uint32_t addr, unsigned int size)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;
	uint32_t crc32;
	int n;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
		return;
	}
	if (simrpc_mem_lock(sp, addr, size) < 0) {
		fprintf(f, "simrpc_mem_lock() failed!\n");
	} else {
		if ((n = simrpc_mem_crc32(sp, 0, size, &crc32)) < 0) {
			fprintf(f, "simrpc_mem_crc32() failed!\n");
		} else {
			fprintf(f, "CRC32 = 0x%08x\n", crc32);
		}
		if (simrpc_mem_unlock(sp, addr, size) < 0) {
			fprintf(f, "simrpc_mem_unlock() failed!\n");
		}
	}

	simrpc_close(sp);
}


const struct fileop http_fops = {
	.write = (void *)http_send,
	.read = (void *)null_read,
	.flush = (void *)null_flush,
	.close = (void *)null_close
};

int rpc_test_cgi(struct httpctl * http)
{
	struct file http_file = {
		.data = http,
		.op = &http_fops
	};
	struct file * f = &http_file;
    int code;
    int port;
    uint32_t addr;
    char * fname;
    int size;

   	httpd_200(http->tp, TEXT_PLAIN);

   	code = atoi(http_query_lookup(http, "code"));
    port = atoi(http_query_lookup(http, "port"));
    addr = strtoul(http_query_lookup(http, "addr"), NULL, 0);
    size = atoi(http_query_lookup(http, "size"));
    fname = http_query_lookup(http, "fname");

    INF("code=%d port=%d", port);

    fprintf(f, "This is a test!\n");
    fprintf(f, "Code = %d\n", code);
    fprintf(f, "Port = %d\n", port);

    switch (code) {

    case 10:
    	rpc_test_exec(f, port);
    	break;
    case 1:
    	rpc_test_reset(f, port);
    	break;
    case 2:
    	rpc_test_suspend(f, port);
    	break;
    case 3:
    	rpc_test_resume(f, port);
    	break;

    case 4:
    	rpc_test_file_write(f, port, fname);
    	break;
    case 5:
    	rpc_test_file_read(f, port, fname);
    	break;
    case 6:
    	rpc_test_file_remove(f, port, fname);
    	break;

    case 7:
    	rpc_test_mem_read(f, port, addr, size);
    	break;
    case 8:
    	rpc_test_mem_erase(f, port, addr, size);
    	break;
    case 9:
    	rpc_test_mem_crc(f, port, addr, size);
    	break;
    }

   	return 0;
}

const char file_saved_html[] = "<!DOCTYPE html><html><body>\r\n"
		"File saved.\r\n"
		"</body></html>\r\n";

const char error_html[] = "<!DOCTYPE html><html><body>\r\n"
		"ERROR!\r\n"
		"</body></html>\r\n";

int rpc_test_file_load(struct httpctl * http, int port, char * fname)
{
	struct simrpc_pcb * sp;
	unsigned int daddr;
    char buf[512];
    int cnt = 0;
	int n;
	uint32_t start;
	uint32_t stop;

	daddr = port;

	INF("port=%d fname='%s'", port, fname);
//    httpd_200(ctl->tp, TEXT_HTML);
    httpd_200(http->tp, TEXT_PLAIN);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
	    return http_send(http, error_html, sizeof(error_html) - 1);
	}

	simrpc_set_timeout(sp, 500);

	if (simrpc_file_create(sp, "cfg.js") < 0) {
		WARN("simrpc_file_create() failed!");
		simrpc_close(sp);
		return -1;
	}

    while ((n = http_multipart_recv(http, buf, 512)) > 0) {
    	start = profclk_get();
    	if (simrpc_file_write(sp, buf, n) < 0) {
    		WARN("simrpc_file_write() failed!");
    		simrpc_close(sp);
    		return -1;
    	}
        http_send(http, buf, n);
    	stop = profclk_get();
    	DBG("simrpc_file_write(): %d us.", profclk_us(stop - start));
    	cnt += n;
    }

	if (n < 0) {
		WARN("http_multipart_recv() failed!");
	}

    DBG("file size=%d", cnt);

	if (simrpc_file_close(sp) < 0) {
		WARN("simrpc_file_close() failed!");
	}

	simrpc_close(sp);

    return 0;
}

int db_load_cgi(struct httpctl * http)
{
	int port = 1;
	char * fname = "db.js";

    port = atoi(http_query_lookup(http, "port"));
//    DBG("input file: '%s'", http_query_lookup(http, "fname"));

    return rpc_test_file_load(http, port, fname);
}

int cfg_load_cgi(struct httpctl * http)
{
	int port;
	char * fname = "cfg.js";

    port = atoi(http_query_lookup(http, "port"));
//    DBG("input file: '%s'", http_query_lookup(http, "fname"));

    return rpc_test_file_load(http, port, fname);
}

int file_write_cgi(struct httpctl * http)
{
	struct simrpc_pcb * sp;
	unsigned int daddr;
    char buf[512];
	char * fname;
	int port;
	int n;

    port = atoi(http_query_lookup(http, "port"));
    fname = http_query_lookup(http, "fname");

	daddr = port;
	INF("port=%d fname='%s'", port, fname);

    httpd_200(http->tp, TEXT_HTML);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
	    return http_send(http, error_html, sizeof(error_html) - 1);
	}

	simrpc_set_timeout(sp, 500);

	if (simrpc_file_create(sp, fname) < 0) {
		WARN("simrpc_file_create() failed!");
		simrpc_close(sp);
		return -1;
	}

    while ((n = http_multipart_recv(http, buf, 512)) > 0) {
    	if (simrpc_file_write(sp, buf, n) < 0) {
    		WARN("simrpc_file_write() failed!");
    		simrpc_close(sp);
    		return -1;
    	}
    }

	if (n < 0) {
		WARN("http_multipart_recv() failed!");
	}

	if (simrpc_file_close(sp) < 0) {
		WARN("simrpc_file_close() failed!");
	}

	simrpc_close(sp);
	return http_send(http, file_saved_html, sizeof(file_saved_html) - 1);

}

int file_read_cgi(struct httpctl * http)
{
	struct simrpc_pcb * sp;
	unsigned int daddr;
	char buf[512];
	char * fname;
	int port;
	int n;


	port = atoi(http_query_lookup(http, "port"));
	fname = http_query_lookup(http, "fname");

	daddr = port;
	INF("port=%d fname='%s'", port, fname);

    httpd_200(http->tp, TEXT_PLAIN);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
	    return http_send(http, error_html, sizeof(error_html) - 1);
	}

	simrpc_set_timeout(sp, 500);

	if (simrpc_file_open(sp, fname) < 0) {
		WARN("simrpc_file_open() failed!");
		simrpc_close(sp);
		return -1;
	}

	while ((n = simrpc_file_read(sp, buf, sizeof(buf))) > 0) {
		http_send(http, buf, n);
	}

	if (n < 0) {
		WARN("simrpc_file_read() failed!");
	}

	if (simrpc_file_close(sp) < 0) {
		WARN("simrpc_file_close() failed!");
	}

	simrpc_close(sp);
	return 0;
}

int get_status_cgi(struct httpctl * http)
{
	struct kernelinfo inf;
	struct simrpc_pcb * sp;
	unsigned int daddr;
	char s[HTML_MAX];
	int port;
	int n;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

	INF("port=%d", port);

   	httpd_200(http->tp, APPLICATION_JSON);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
	 	n = snprintf(s, HTML_MAX, "{\"state\": \"Error\"}\r\n");
    	http_send(http, s, n);
	} else {
		if (simrpc_kernelinfo(sp, &inf) < 0) {
			WARN("simrpc_kernelinfo() failed!");
		 	n = snprintf(s, HTML_MAX, "{\"state\": \"Offline\"}\r\n");
	    	http_send(http, s, n);
		} else {
		 	n = snprintf(s, HTML_MAX, "{\"state\": \"Online\", \"kernel\": {"
		 			"\"ticks\": %d, ", inf.ticks);
	    	http_send(http, s, n);
		 	n = snprintf(s, HTML_MAX, "\"version\": { \"major\": %d, "
		 			"\"minor\": %d, \"build\": %d }}}\r\n",
					inf.version.major, inf.version.minor, inf.version.build);
	    	http_send(http, s, n);
		}
		simrpc_close(sp);
	}

   	return 0;
}

/*---------------------------------------------------------------------------
  Simulator ports firmware upgrade
  ---------------------------------------------------------------------------*/

#define SIMPORT_APP_ADDR 0x08004000
#define SIMPORT_APP_SIZE (64 * 1024)

int firmware_load_cgi(struct httpctl * http)
{
	uint32_t addr = SIMPORT_APP_ADDR;
	unsigned int size = SIMPORT_APP_SIZE;
	struct simrpc_pcb * sp;
	unsigned int daddr;
    int port;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

	INF("port=%d addr=%08x size=%d", port, addr, size);
    httpd_200(http->tp, TEXT_HTML);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
	    return http_send(http, error_html, sizeof(error_html) - 1);
	}

	if (simrpc_suspend(sp) < 0) {
		WARN("simrpc_suspend() failed!");
		simrpc_close(sp);
	    return http_send(http, error_html, sizeof(error_html) - 1);
	}

	if (simrpc_mem_lock(sp, addr, size) < 0) {
		WARN("simrpc_mem_lock() failed!");
		simrpc_close(sp);
	    return http_send(http, error_html, sizeof(error_html) - 1);
	}

	simrpc_set_timeout(sp, 5000);

	if (simrpc_mem_erase(sp, 0, size) < 0) {
		WARN("simrpc_mem_erase() failed!");
		http_send(http, error_html, sizeof(error_html) - 1);
	} else {
	    char buf[512];
	    int cnt = 0;
		int n;

		while ((n = http_multipart_recv(http, buf, 512)) > 0) {
			if (simrpc_mem_write(sp, buf, n) < 0) {
				WARN("simrpc_file_write() failed!");
			}
			cnt += n;
		}

		if (n < 0) {
			WARN("http_multipart_recv() failed!");
		}

		(void)cnt;
	    DBG("file size=%d", cnt);

		http_send(http, file_saved_html, sizeof(file_saved_html) - 1);
	}

	if (simrpc_mem_unlock(sp, addr, size) < 0) {
		WARN("simrpc_mem_unlock() failed!");
	}

	if (simrpc_reboot(sp) < 0) {
		WARN("simrpc_reboot() failed!");
	}

	simrpc_close(sp);

    return 0;
}
