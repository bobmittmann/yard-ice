#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <libgen.h>

#include "debug.h"

#ifdef _WIN32
#include <windows.h>
static void sleep(sec) {
	Sleep(1000 * sec);
}

int usleep(useconds_t useconds) {
	Sleep(useconds / 1000);
	return 0;
}
#endif

#include "serial.h"
#include "xmodem.h"

void serial_rx_task(struct serial_dev * sp)
{
	char buf[1];
	int c;
#ifndef _WIN32
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGQUIT);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
#endif

	for (;;) {
		if ((c = serial_recv(sp, buf, 1, 500)) == 0) {
			printf("!");
			fflush(stdout);
			continue;
		}

		
		if (c < 0) {
			printf("??");
			fflush(stdout);
			sleep(1);
			continue;
		}

		printf("%02x", c);
		fflush(stdout);
	}
}

void serial_rx_start(struct serial_dev * sp)
{
	pthread_t thread;
	int ret;

	if ((ret = pthread_create(&thread, NULL, 
							  (void * (*)(void *))serial_rx_task, 
							  (void *)sp)) < 0) {
		fprintf(stderr, "%s: pthread_create() failed: %s", 
				__func__, strerror(ret));
	}
}

struct serial_dev * win_serial_open(const char * com_port);

int ymodem_send(const char * port, int fcnt, char * pathname[]) 
{

	struct serial_config cfg = {
		.baudrate = 115200,
		.databits = 8,
		.parity = 0,
		.stopbits = 1,
		.flowctrl = 0
	};
	struct serial_dev * ser;
	struct xmodem_send * sy;
	uint8_t buf[512];
	int ret;
	int i;

	printf("-- serial port: '%s'\n", port);
	fflush(stdout);

	DBG(DBG_TRACE, "1. win_serial_open() ...");
	if ((ser = win_serial_open(port)) == NULL) {
		return 1;
	}

	DBG(DBG_TRACE, "2. xmodem_send_alloc() ...");
	if ((sy = xmodem_send_alloc()) == NULL) {
		return 1;
	}

	DBG(DBG_TRACE, "3. serial_config_set() ...");
	serial_config_set(ser, &cfg);

	xmodem_send_open(sy, ser, MODE_YMODEM);

	for (i = 0; i < fcnt; ++i) {
		unsigned int fsize;
		unsigned int rem;
		char * fname;
		FILE * f;
		int len;

		if ((f = fopen(pathname[i], "rb")) == NULL) {
			fprintf(stderr, "ERROR: %s: open(): %s.\n",
					__func__, strerror(errno));
			return -1;
		}

		fname = basename(pathname[i]);
		fseek(f, 0, SEEK_END);
		fsize = ftell(f);
		fseek(f, 0, SEEK_SET);
		rem = fsize;

		if ((ret = xmodem_send_start(sy, fname, fsize)) < 0) {
			fprintf(stderr, "ERROR: %s: xmodem_send_start().\n", __func__);
			return ret;
		}

		while (rem) {
			if ((len = fread(buf, 1, sizeof(buf), f)) <= 0) {
				fprintf(stderr, "ERROR: %s: fread(): %s.\n",
						__func__, strerror(errno));
				fclose(f);
				return len;
			}

			if ((ret = xmodem_send_loop(sy, buf, len)) < 0) {
				DBG(DBG_WARNING, "xmodem_send_loop() failed!");
				fclose(f);
				return ret;
			}

			rem -= len;
		}

		if ((ret = xmodem_send_eot(sy)) < 0) {
			DBG(DBG_WARNING, "xmodem_send_eot() failed!");
			fclose(f);
			return ret;
		}

		fclose(f);
	}

	if ((ret = xmodem_send_close(sy)) < 0) {
		DBG(DBG_WARNING, "xmodem_send_close() failed!");
		return ret;
	}

	return 0;
}

int ymodem_recv(const char * port) 
{
	struct serial_config cfg = {
		.baudrate = 115200,
		.databits = 8,
		.parity = 0,
		.stopbits = 1,
		.flowctrl = 0
	};
	struct serial_dev * ser;
	struct xmodem_recv * ry;
	uint8_t buf[512];
	FILE * f = NULL;
	int ret;

	printf("-- serial port: '%s'\n", port);
	fflush(stdout);

	DBG(DBG_TRACE, "1. win_serial_open() ...");
	if ((ser = win_serial_open(port)) == NULL) {
		return 1;
	}

	DBG(DBG_TRACE, "2. xmodem_recv_alloc() ...");
	if ((ry = xmodem_recv_alloc()) == NULL) {
		return 1;
	}

	DBG(DBG_TRACE, "3. serial_config_set() ...");
	serial_config_set(ser, &cfg);

	DBG(DBG_TRACE, "4. xmodem_recv_init() ...");
	xmodem_recv_init(ry, ser, FCS_CRC, MODE_YMODEM);

	DBG(DBG_TRACE, "5. xmodem_recv_loop() ...");
	while ((ret = xmodem_recv_loop(ry, buf, sizeof(buf))) >= 0) {
		int len;

		DBG(DBG_INFO, "xmodem_recv_loop() ret = %d.", ret);

		len = ret;
		if (len == 0) {
			if (ry->fsize == 0) {
				printf("End of transfer\n");
				fflush(stdout);
				break;
			}
			printf("-- file='%s' size=%d\n", ry->fname, ry->fsize);
			fflush(stdout);
			if (f != NULL) 
				fclose(f);

			if ((f = fopen(ry->fname, "wb")) == NULL) {
				fprintf(stderr, "ERROR: %s: open(): %s.\n",
						__func__, strerror(errno));
				return -1;
			}
		} else {
//			printf(" - data=%d\n", len);
//			fflush(stdout);
			if ((ret = fwrite(buf, len, 1, f)) != 1) {
				fprintf(stderr, "ERROR: %s: fwrite(): %s.\n",
						__func__, strerror(errno));
				fclose(f);
				return ret;
			}

		}
	}

	if (f != NULL) 
		fclose(f);

	DBG(DBG_TRACE, "xmodem_recv_loop() ret = %d.", ret);

	return ret;
}

/* -------------------------------------------------------------------------
 * Application startup
 * ------------------------------------------------------------------------- 
 */

#define APP_NAME "ymodem_test"
#define VERSION_MAJOR 3
#define VERSION_MINOR 0

static char * progname;


static void show_usage(void)
{
	fprintf(stderr, "Usage: %s [OPTION...]\n", progname);
	fprintf(stderr, "  -h  \tShow this help message\n");
	fprintf(stderr, "  -v  \tShow version\n");
	fprintf(stderr, "  -p PORT \tSet comm PORT\n");
	fprintf(stderr, "  -d \tEnable DAV/RPC protocol over stdout/stdin\n");
	fprintf(stderr, "\n");
}

static void show_version(void)
{
	fprintf(stderr, "%s %d.%d\n", APP_NAME, VERSION_MAJOR, VERSION_MINOR);
}

static void parse_err(char * opt)
{
	fprintf(stderr, "%s: invalid option %s\n", progname, opt);
}

void err_opt(char * opt)
{
	fprintf(stderr, "%s: invalid option %s\n", progname, opt);
}

void err_arg(int opt, char * arg)
{
	fprintf(stderr, "%s: invalid argument -%c %s\n", progname, opt, arg);
}

#define CMD_LINE_MAX 1023

int main(int argc, char *argv[]) 
{
#if defined(_WIN32) || defined(__CYGWIN__)
	const char * port = "COM1";
#else
	const char * port = "/dev/ttyS11";
#endif
	int fcnt;
	char * fnam[64];
	extern char *optarg;	/* getopt */
	extern int optind;	/* getopt */
	int c;

	/* the program name start just after the last slash */
	if ((progname = (char *)strrchr(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;

	/* parse the command line options */
	while ((c = getopt(argc, argv, "dvhp:")) > 0) {
		switch (c) {
		case 'v':
			show_version();
			return 0;
		case 'h':
			show_usage();
			return 1;
		case 'p':
			port = optarg;
			break;
		default:
			parse_err(optarg);
			return 2;
		}
	}

	if (optind < argc) {
		fcnt = 0;
		do {
			fnam[fcnt++] = argv[optind];
			optind++;
		} while (optind < argc);

		return ymodem_send(port, fcnt, fnam); 
	}

	return ymodem_recv(port); 
}

