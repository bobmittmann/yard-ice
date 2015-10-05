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
#include <thinkos.h>
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

void rpc_test_safe(FILE * f, int port)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		fprintf(f, "simrpc_open() failed!\n");
	} else {
		if (simrpc_exec(sp, SIMRPC_EXEC_KEY('S', 'A', 'F', 'E')) < 0) {
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

	n = sprintf(s, "The quick brown fox jumps over the lazy dog!\n");

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
	if (simrpc_file_open(sp, fname) < 0) {
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
		simrpc_set_timeout(sp, 500);
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

    INF("rpc_test.cgi: code=%d port=%d", code, port);

    fprintf(f, "This is a test!\n");
    fprintf(f, "Code = %d\n", code);
    fprintf(f, "Port = %d\n", port);

    switch (code) {

    case 11:
    	rpc_test_safe(f, port);
    	break;
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

const char ok_html[] = "<!DOCTYPE html><html><body>\r\n"
		"OK\r\n"
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

	if (simrpc_file_create(sp, fname) < 0) {
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

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
		httpd_200(http->tp, TEXT_HTML);
	    return http_send(http, error_html, sizeof(error_html) - 1);
	}

	simrpc_set_timeout(sp, 500);

	if (simrpc_file_create(sp, fname) < 0) {
		WARN("simrpc_file_create() failed!");
		simrpc_close(sp);
		return -1;
	}

    while ((n = http_content_recv(http, buf, 512)) > 0) {
    	if (simrpc_file_write(sp, buf, n) < 0) {
    		WARN("simrpc_file_write() failed!");
    		simrpc_close(sp);
    		return -1;
    	}
    }

	if (n < 0) {
		WARN("http_content_recv() failed!");
	}

	if (simrpc_file_close(sp) < 0) {
		WARN("simrpc_file_close() failed!");
	}

	simrpc_close(sp);

    httpd_200(http->tp, TEXT_HTML);
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

int simrpc_stdout_get(struct simrpc_pcb * sp, void * buf, unsigned int max);

int get_stdout_cgi(struct httpctl * http)
{
	struct simrpc_pcb * sp;
	unsigned int daddr;
	char buf[HTML_MAX];
	int port;
	int n;

	port = atoi(http_query_lookup(http, "port"));
	daddr = port;

    httpd_200(http->tp, TEXT_PLAIN);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
	    return http_send(http, error_html, sizeof(error_html) - 1);
	}

	n = simrpc_stdout_get(sp, buf, sizeof(buf));
	if (n == 0) {
//		DBG("simrpc_stdout_flush()...");
//		simrpc_stdout_flush(sp);
//		thinkos_sleep(50);
//		n = simrpc_stdout_get(sp, buf, sizeof(buf));
	}

	while (n > 0) {
		http_send(http, buf, n);
		n = simrpc_stdout_get(sp, buf, sizeof(buf));
	}

	if (n < 0) {
		WARN("simrpc_stdout_get() failed!");
	}

	simrpc_close(sp);
	return 0;
}

int get_status_cgi(struct httpctl * http)
{
	struct simrpc_kernelinfo kinf;
	struct simrpc_appinfo ainf;
	struct simrpc_pcb * sp;
	unsigned int daddr;
	char s[HTML_MAX];
	char * cp;
	int port;
	int n;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

	DBG("get_status_cgi: port=%d", port);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
		return rpc_json_error(http, 0, "simrpc_open() failed");
	}

	simrpc_set_timeout(sp, 50);

   	httpd_200(http->tp, APPLICATION_JSON);

	if (simrpc_kernelinfo_get(sp, &kinf) < 0) {
		WARN("simrpc_kernelinfo_get() failed!");
	 	n = snprintf(s, HTML_MAX, "{\"state\": \"Offline\"}\r\n");
	} else {
		cp = s;
	 	cp += snprintf(cp, HTML_MAX, "{\"state\": \"Online\", \"kernel\": {"
	 			"\"ticks\": %d, \"version\": { \"major\": %d, "
	 			"\"minor\": %d, \"build\": %d }}",
				kinf.ticks, kinf.version.major,
				kinf.version.minor, kinf.version.build);
		if (simrpc_appinfo_get(sp, &ainf) < 0) {
			WARN("simrpc_appinfo() failed!");
		 	cp += snprintf(cp, HTML_MAX, "}\r\n");
		} else {
		 	cp += snprintf(cp, HTML_MAX, ", \"app\": {"
		 		"\"uptime\": %d, \"version\": { \"major\": %d, "
	 			"\"minor\": %d, \"build\": %d }}}\r\n",
				ainf.uptime, ainf.version.major,
				ainf.version.minor, ainf.version.build);
		}
	 	n = cp - s;
	}
	simrpc_close(sp);

	return http_send(http, s, n);
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

int rpc_shellexec(struct httpctl * http, unsigned int daddr, const char * cmd)
{
	struct simrpc_pcb * sp;
	char s[SIMRPC_DATA_MAX];
    int n;

	INF("daddr=0x%02x cmd='%s'", daddr, cmd);
    httpd_200(http->tp, TEXT_HTML);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
		n = sprintf(s, "#ERROR: simrpc_open() failed!\n");
	} else {
		simrpc_set_timeout(sp, 5000);
		if ((n = simrpc_shellexec(sp, cmd, s, 512)) < 0) {
			WARN("simrpc_shellexec() failed!");
			n = sprintf(s, "#ERROR: simrpc_shellexec() failed!\n");
		}
		simrpc_close(sp);
	}

	return http_send(http, s, n);
}

int rpc_exec_cgi(struct httpctl * http)
{
	unsigned int daddr;
    char * cmd;
    int port;

    port = atoi(http_query_lookup(http, "port"));
    cmd = http_query_lookup(http, "cmd");
	daddr = port;

	return rpc_shellexec(http, daddr, cmd);
}

int rpc_js_cgi(struct httpctl * http)
{
	struct simrpc_pcb * sp;
	unsigned int daddr;
	char script[512];
	char * cp = script;
    int port;
    int rem;
    int cnt;
    int n;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

    httpd_200(http->tp, TEXT_HTML);

    rem = sizeof(script) - 1;

    while (rem) {
    	int n;
    	n = http_content_recv(http, cp, rem);

    	if (n < 0) {
    		WARN("http_content_recv() failed!");
    		return n;
    	}

    	if (n == 0)
    		break;

    	cp += n;
    	cnt += n;
    	rem -= n;
    }

    script[cnt] = '\0';
    printf("<<--\n%s\n-->>\n", script);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
		http_send(http, error_html, sizeof(error_html) - 1);
	} else {
		simrpc_set_timeout(sp, 1000);
		if ((n = simrpc_jsexec(sp, script, script, cnt)) < 0) {
			WARN("simrpc_jsexec() failed!");
			http_send(http, error_html, sizeof(error_html) - 1);
		} else {
			http_send(http, ok_html, sizeof(ok_html) - 1);
		}
		simrpc_close(sp);
	}

	return 0;
}

/*---------------------------------------------------------------------------
  Configuration
  ---------------------------------------------------------------------------*/

int cfg_load_cgi(struct httpctl * http)
{
	int port;
	char * fname = "cfg.js";

    port = atoi(http_query_lookup(http, "port"));

    return rpc_test_file_load(http, port, fname);
}

int cfg_compile_cgi(struct httpctl * http)
{
	struct simrpc_pcb * sp;
	unsigned int daddr;
	uint32_t clk_start;
	uint32_t clk_stop;
	char s[512];
    int port;
    int n;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

    httpd_200(http->tp, TEXT_HTML);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
		n = sprintf(s, "#ERROR: simrpc_open() failed!\n");
	} else {
		simrpc_set_timeout(sp, 10000);
		clk_start = profclk_get();
		n = simrpc_cfg_compile(sp, s, sizeof(s));
		clk_stop = profclk_get();
		if (n < 0) {
			WARN("simrpc_cfg_compile() failed, with code=%d after %d us!",
					n, profclk_us(clk_stop - clk_start));
			n = sprintf(s, "#ERROR: simrpc_cfg_compile() failed!\r\n");
		} else {
			INF("configuration compiled in %d us.", profclk_us(clk_stop - clk_start));
		}

		simrpc_close(sp);
	}

	return http_send(http, s, n);
}

int cfg_getinfo_cgi(struct httpctl * http)
{
	struct simrpc_cfginfo inf;
	struct simrpc_pcb * sp;
	unsigned int daddr;
	char s[HTML_MAX];
	int port;
	int n;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

	DBG("cfg_getinfo.cgi: port=%d", port);

   	httpd_200(http->tp, APPLICATION_JSON);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
	 	n = snprintf(s, HTML_MAX, "{\"error\": \"simrpc_open() failed!\"}\r\n");
	} else {
		if (simrpc_cfginfo_get(sp, &inf) < 0) {
			WARN("simrpc_cfginfo_get() failed!");
		 	n = snprintf(s, HTML_MAX, "{\"error\": \"simrpc_cfginfo_get() failed!\"}\r\n");
		} else {
		 	n = snprintf(s, HTML_MAX, "{\"tag\": \"%s\", \"author\": \"%s\", "
		 			"\"desc\":\"%s\", \"version\": { \"major\": %d, "
		 			"\"minor\": %d, \"build\": %d }}",
					inf.tag, inf.author, inf.desc, inf.version.major,
					inf.version.minor, inf.version.build);
		}
		simrpc_close(sp);
	}

	return http_send(http, s, n);
}

/*---------------------------------------------------------------------------
  Database
  ---------------------------------------------------------------------------*/

int db_load_cgi(struct httpctl * http)
{
	int port = 1;
	char * fname = "db.js";

    port = atoi(http_query_lookup(http, "port"));

    return rpc_test_file_load(http, port, fname);
}

int db_compile_cgi(struct httpctl * http)
{
	struct simrpc_pcb * sp;
	unsigned int daddr;
	uint32_t clk_start;
	uint32_t clk_stop;
	char s[512];
    int port;
    int n;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

    httpd_200(http->tp, TEXT_HTML);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
		n = sprintf(s, "#ERROR: simrpc_open() failed!\r\n");
	} else {
		simrpc_set_timeout(sp, 10000);
		clk_start = profclk_get();
		n = simrpc_db_compile(sp, s, sizeof(s));
		clk_stop = profclk_get();
		if (n < 0) {
			WARN("simrpc_db_compile() failed, with code=%d after %d us!",
					n, profclk_us(clk_stop - clk_start));
			n = sprintf(s, "#ERROR: simrpc_db_compile() failed!\r\n");
		} else {
			INF("database compiled in %d us.", profclk_us(clk_stop - clk_start));
		}
		simrpc_close(sp);
	}

	return http_send(http, s, n);
}

int db_getinfo_cgi(struct httpctl * http)
{
	struct simrpc_dbinfo inf;
	struct simrpc_pcb * sp;
	unsigned int daddr;
	char s[HTML_MAX];
	int port;
	int n;

    port = atoi(http_query_lookup(http, "port"));
	daddr = port;

	DBG("db_getinfo_cgi port=%d", port);

   	httpd_200(http->tp, APPLICATION_JSON);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
	 	n = snprintf(s, HTML_MAX, "{\"error\": \"simrpc_open() failed!\"}\r\n");
	} else {
		if (simrpc_dbinfo_get(sp, &inf) < 0) {
			WARN("simrpc_dbinfo_get() failed!");
		 	n = snprintf(s, HTML_MAX, "{\"error\": \"simrpc_dbinfo_get() failed!\"}\r\n");
		} else {
		 	n = snprintf(s, HTML_MAX,
		 			"{\"desc\":\"%s\", \"version\": { \"major\": %d, "
		 			"\"minor\": %d, \"build\": %d }}",
					inf.desc, inf.version.major,
					inf.version.minor, inf.version.build);
		}
		simrpc_close(sp);
	}

	return http_send(http, s, n);
}

int sim_getattr_cgi(struct httpctl * http)
{
	struct simrpc_attr_req req;
	struct simrpc_pcb * sp;
	unsigned int daddr;
	char s[HTML_MAX];
	uint32_t val[128];
	int port;
	int ret;
	int n;

    port = atoi(http_query_lookup(http, "port"));
    req.dev_type = atoi(http_query_lookup(http, "dev_type"));
    req.attr_offs = atoi(http_query_lookup(http, "attr_offs"));
    req.base_addr = atoi(http_query_lookup(http, "base_addr"));
    req.count = atoi(http_query_lookup(http, "count"));
	daddr = port;

	INF("port=%d type=%d count=%d", port, req.dev_type, req.count);

   	httpd_200(http->tp, APPLICATION_JSON);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
	 	n = snprintf(s, HTML_MAX, "{\"error\": \"simrpc_open() failed!\"}\r\n");
	 	ret = http_send(http, s, n);
	} else {
		int size;
		int cnt;

		if ((size = simrpc_attr_get(sp, &req, val)) < 0) {
			WARN("simrpc_attr_get() failed!");
		 	n = snprintf(s, HTML_MAX, "{\"error\": \"simrpc_attr_get() failed!\"}\r\n");
		 	ret = http_send(http, s, n);
		} else if ((cnt = (size / sizeof(uint32_t))) < 1) {
			WARN("simrpc_attr_get() invalid!");
		 	n = snprintf(s, HTML_MAX, "{\"error\": \"simrpc_attr_get() invalid!\"}\r\n");
		 	ret = http_send(http, s, n);
		} else {
			char * cp = s;
			int i;

			/* format header and first attribute */
		 	cp += snprintf(cp, HTML_MAX, "{ \"attr\":[%d", val[0]);

		 	/* encode remaining attributes */
			for (i = 1; i < cnt; ++i) {
				cp += snprintf(cp, HTML_MAX, ",%d", val[i]);
				if ((n = (cp - s)) > (HTML_MAX - 16)) {
					/* flush */
					http_send(http, s, n);
					cp = s;
				}
			}

			cp += snprintf(cp, HTML_MAX, "] }\r\n");
			n = (cp - s);
			ret = http_send(http, s, n);
		}
		simrpc_close(sp);
	}

	return ret;
}
