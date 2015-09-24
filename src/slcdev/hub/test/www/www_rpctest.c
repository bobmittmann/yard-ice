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
	char s[129];
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
	if ((n = simrpc_file_read(sp, s, 128)) < 0) {
		fprintf(f, "simrpc_file_read() failed!\n");
	} else {
		s[n] = '\0';
		fprintf(f, "\"%s\"\n", s);
	}
	stop = profclk_get();
	fprintf(f, "simrpc_file_read(): %d us.\n", profclk_us(stop - start));

	if (simrpc_file_close(sp) < 0) {
		fprintf(f, "simrpc_file_close() failed!\n");
	}

	simrpc_close(sp);
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
	simrpc_mem_lock(sp, addr, cnt);

	if ((n = simrpc_mem_read(sp, buf, cnt)) < 0) {
		fprintf(f, "simrpc_mem_write() failed!\n");
	} else {
		show_hex8(f, addr, buf, n);
	}

	if (simrpc_mem_unlock(sp, addr, cnt) < 0) {
		fprintf(f, "simrpc_mem_unlock() failed!\n");
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
    case 1:
    	rpc_test_file_write(f, port, fname);
    	break;
    case 2:
    	rpc_test_file_read(f, port, fname);
    	break;
    case 3:
    	rpc_test_mem_read(f, port, addr, size);
    	break;
    }

   	return 0;
}

const char file_saved_html[] = "<!DOCTYPE html><html><body>\r\n"
		"File saved.\r\n"
		"</body></html>\r\n";


int rpc_test_file_load(struct httpctl * ctl, int port, char * fname)
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

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
		return -1;
	}

	simrpc_set_timeout(sp, 500);

	if (simrpc_file_create(sp, "cfg.js") < 0) {
		WARN("simrpc_file_create() failed!");
		simrpc_close(sp);
		return -1;
	}

    while ((n = http_multipart_recv(ctl, buf, 512)) > 0) {
    	start = profclk_get();
    	if (simrpc_file_write(sp, buf, n) < 0) {
    		WARN("simrpc_file_write() failed!");
    		simrpc_close(sp);
    		return -1;
    	}
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

    httpd_200(ctl->tp, TEXT_HTML);
    return http_send(ctl, file_saved_html, sizeof(file_saved_html) - 1);

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
	int port = 1;
	char * fname = "cfg.js";

    port = atoi(http_query_lookup(http, "port"));
//    DBG("input file: '%s'", http_query_lookup(http, "fname"));

    return rpc_test_file_load(http, port, fname);
}





