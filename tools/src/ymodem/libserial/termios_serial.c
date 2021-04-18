/*
 * @file	termios_serial.c
 * @brief	*NIX termios library serial wrapper
 * @author	Robinson Mittmann (bobmittmann@gmail.com)
 *
 */

#if !defined(_WIN32) && !defined(__CYGWIN__)

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdint.h>
#include <termios.h>
#include <assert.h>
#include <sys/ioctl.h>

#include "serial.h"

#define SERIAL_DEV_RX_BUF_LEN 8

/* termios serial device */
struct serial_dev {
	struct termios save_term;
	struct termios term;
	int fd;
	struct {
		unsigned int pos;
		unsigned int cnt;
		uint8_t buf[SERIAL_DEV_RX_BUF_LEN];
	} rx;
};

int serial_getchar(struct serial_dev * dev, int tmo)
{
	struct timeval tv;
	fd_set rfds;
	int fd;
	int len;
	int ret;
	int c;

	assert(dev != NULL);

	fd = dev->fd;

	if (dev->rx.cnt == 0) {
		/* no buffered data */
		tv.tv_sec = 0;
		tv.tv_usec = tmo * 1000;
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		ret = select(fd + 1, &rfds, NULL, NULL, &tv);

		if (ret < 0) {
			if (errno == EINTR) {
				fprintf(stderr, "%s: select() call interrupted.\n", __func__);
				fflush(stderr);
			} else {
				fprintf(stderr, "%s: select() failed: %s.\n", 
						__func__, strerror(errno));

				fprintf(stderr, "%s: fd=%d tv_sec=%d tv_usec=%d tmo=%d\n", 
						__func__, fd, (int)tv.tv_sec, (int)tv.tv_usec, tmo);
				fflush(stderr);
			}
			return SERIAL_ERR;
		}

		if (ret == 0) {
			/* timeout */
			return SERIAL_TMO;
		}


		if ((len = read(fd, dev->rx.buf, SERIAL_DEV_RX_BUF_LEN)) <= 0) {
			fprintf(stderr, "%s: read() failed: %s.\n", 
					__func__, strerror(errno));
			fflush(stderr);
			return SERIAL_ERR;
		}

		dev->rx.cnt = len;
		dev->rx.pos = 0;
	}

	c = dev->rx.buf[dev->rx.pos++];
	if (dev->rx.pos == dev->rx.cnt)
		dev->rx.cnt = 0;

	return c;
}

int serial_putchar(struct serial_dev * dev, int c)
{
	uint8_t buf[1];
	int len;

	assert(dev != NULL);

	buf[0] = c;
	if ((len = write(dev->fd, buf, 1)) < 0) {
		fprintf(stderr, "%s: write() failed: %s.\n", 
				__func__, strerror(errno));
		return SERIAL_ERR;
	}

	return 0;
}

/*
int serial_flush(int fd, int ms)
{
	unsigned char buf[64];

	tcflush(fd, TCIOFLUSH);

	if (ms > 0) 
		while (serial_recv(fd, buf, 64, ms) > 0);

	return 0;
}
*/


int serial_write(struct serial_dev * dev, const void * buf, 
				 unsigned int len)
{
	int fd;
	int n;

	assert(dev != NULL);

	fd = dev->fd;

	if ((n = write(fd, buf, len)) < 0) {
		perror("write():");
		return n;
	}

	tcdrain(fd);

	return n;
}

int serial_read(struct serial_dev * dev, char * buf, 
				unsigned int max, unsigned int tmo_msec)
{
	struct timeval tv;
	fd_set rfds;
	int retval;
	int len;
	int fd;

	assert(dev != NULL);

	fd = dev->fd;

	tv.tv_sec = 0;
	tv.tv_usec = tmo_msec * 1000;
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	
	retval = select(fd + 1, &rfds, NULL, NULL, &tv);

	if (retval == -1) {
		perror("select():");
		return retval;
	}

	if (retval == 0) {
		return 0;
	}

	if ((len = read(fd, buf, max)) < 0) {
		perror("read():");
		return len;
	}

	return len;
}

int serial_dtr_set(struct serial_dev * dev, int on)
{
	int arg;
	
	assert(dev != NULL);

	arg = TIOCM_DTR;

	if (on)
		return ioctl(dev->fd, TIOCMBIS, &arg);

	return ioctl(dev->fd, TIOCMBIC, &arg);
}

int serial_cts_get(struct serial_dev * dev, int on)
{
	int status;
	
	assert(dev != NULL);

	ioctl(dev->fd, TIOCMGET, &status); 
	
	return (status & TIOCM_CTS) ? 1 : 0;
}


struct speed_code {
	long speed;
	int code;
};

static struct speed_code speedtab[] = {
	{ 50, B50 },
	{ 75, B75 },
	{ 110, B110 },
	{ 134, B134 },
	{ 150, B150 },
	{ 200, B200 },
	{ 300, B300 },
	{ 600, B600 },
	{ 1200, B1200 },
	{ 1800, B1800 },
	{ 2400, B2400 },
	{ 4800, B4800 },
	{ 9600, B9600 },
	{ 19200, B19200 },
	{ 38400, B38400 },
	{ 57600, B57600 },
	{ 115200, B115200 },
	{ 230400, B230400 },
	{ 460800, B460800 },
	{ 0, 0 },
};

static int baud_code(long speed)
{
	struct speed_code *sp;

	for (sp = speedtab; sp->speed; sp++)
		if (sp->speed == speed)
			return (sp->code);

	return 0;
}


int serial_config_set(struct serial_dev * dev, 
					  const struct serial_config * cfg)
{
	struct termios term;
	int fd;

	assert(dev != NULL);
	assert(cfg != NULL);

	fd = dev->fd;

	if (tcgetattr(fd, &term) < 0) {
		fprintf(stderr, "%s: tcgetattr(%d) failed: %s.\n", 
				__func__, fd, strerror(errno));
		return -1;
	}

	term.c_cflag = CREAD | HUPCL | CLOCAL;
	term.c_iflag = IGNBRK;

	switch(cfg->databits) {
		case 8 : term.c_cflag |= CS8;
			break;
		case 7 : term.c_cflag |= CS7;
			break;
		case 6 : term.c_cflag |= CS6;
			break;
		case 5 : term.c_cflag |= CS5;
			break;
	}

	if (cfg->stopbits == 2) {
		term.c_cflag |= CSTOPB;
	}

	switch(cfg->parity) {
		case _PARITY_NONE: term.c_iflag |= IGNPAR;
			break;
		case _PARITY_ODD: term.c_iflag |= PARODD;
		case _PARITY_EVEN: term.c_cflag |= PARENB;
			break;
	}

	switch (cfg->flowctrl) {
		case _FLOWCTRL_XONXOFF:
			term.c_iflag |= IXON | IXOFF;
			break;
		case _FLOWCTRL_RTSCTS:
			term.c_cflag |= CRTSCTS;
			break;
		default:
			term.c_iflag |= IXANY;
			break;
	}

	term.c_lflag = 0;
	term.c_oflag = 0;

	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;

	cfsetispeed(&term, baud_code(cfg->baudrate));
	cfsetospeed(&term, baud_code(cfg->baudrate));

	if (tcsetattr(fd, TCSANOW, &term) < 0) {
		fprintf(stderr, "%s: tcsetattr() failed: %s.\n", 
				__func__, strerror(errno));
		return -1;
	}

	return 0;
}

struct serial_dev * serial_open(const char * line)
{
	struct serial_dev * dev;
	int flags;
	int fd;

	dev = (struct serial_dev*)malloc(sizeof(struct serial_dev));
	if (dev == NULL) {
		fprintf(stderr, "%s: malloc() failed: %s.\n", 
				__func__, strerror(errno));
		return NULL;
	}

	if ((fd = open(line, O_RDWR | O_NONBLOCK)) < 0) {
		fprintf(stderr, "%s: open(\"%s\") failed: %s.\n", 
				__func__, line, strerror(errno));
		free(dev);
		return NULL;
	}

	/* prevent the file descriptor being inherited by another process */ 
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		flags = 0;
	fcntl(fd, F_SETFD, (flags | FD_CLOEXEC) & ~O_NONBLOCK);

	if (isatty(fd) == 0) {
		fprintf(stderr, "%s: '%s' is not a serial device.\n", __func__, line);
		free(dev);
		close(fd);
		return NULL;
	}

	/* store the TTY configuration */
	if (tcgetattr(fd, &dev->save_term) < 0) {
		fprintf(stderr, "%s: tcgetattr(%d) failed: %s.\n", 
				__func__, fd, strerror(errno));
		free(dev);
		close(fd);
		return NULL;
	}

	dev->fd = fd;
	/* clear buffer */
	dev->rx.pos = 0;
	dev->rx.cnt = 0;

	return dev;
}

int serial_close(struct serial_dev * dev)
{
	assert(dev != NULL);

	if (tcsetattr(dev->fd, TCSANOW, &dev->save_term) < 0) {
		fprintf(stderr, "%s: tcsetattr() failed: %s.\n", 
				__func__, strerror(errno));
	}
	close(dev->fd);
	free(dev);

	return 0;
}

#endif /* !_WIN32 */

