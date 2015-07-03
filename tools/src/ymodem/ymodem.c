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

int ymodem_test(const char * port, int fcnt, const char * fnam[]) 
{

	struct serial_config cfg = {
		.baudrate = 9600,
		.databits = 8,
		.parity = 0,
		.stopbits = 1,
		.flowctrl = 0
	};
	struct serial_dev * ser;
	int n;

	DBG(DBG_TRACE, "starting MODEM test ...");

	printf("\n==== Serial test! ====\n");

	printf(" 2. Oppening serial port: '%s'\n", port);
	fflush(stdout);

	ser = win_serial_open(port);
	if (ser == NULL) {
		return 1;
	}

	serial_config_set(ser, &cfg);

	serial_rx_start(ser);

	for (n = 0; n < 10000000; ++n) {
#ifdef _WIN32
		Sleep(250);
#else
		usleep(50000);
#endif
		printf(".");	
		fflush(stdout);	
//		serial_putchar(ser, test_msg[n % sizeof(test_msg)]);
	}

	return 0;
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
	fprintf(stderr, "  -q  \tQuiet\n");
	fprintf(stderr, "  -n  \tDon't start capturing\n");
	fprintf(stderr, "  -r DIR \tSet root directory to DIR\n");
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
	const char * fnam[128];
	extern char *optarg;	/* getopt */
	extern int optind;	/* getopt */
	int c;

	/* the program name start just after the last slash */
	if ((progname = (char *)strrchr(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;

	/* parse the command line options */
	while ((c = getopt(argc, argv, "dvhqnr:")) > 0) {
		switch (c) {
		case 'v':
			show_version();
			return 0;
		case 'h':
			show_usage();
			return 1;
		default:
			parse_err(optarg);
			return 2;
		}
	}

	fcnt = 0;
	if (optind < argc) {
		fnam[fcnt++] = argv[optind];
		optind++;
	}

	return ymodem_test(port, fcnt, fnam); 
}

