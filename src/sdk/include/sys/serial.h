/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file sys/serial.h
 * @brief YARD-ICE 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_SERIAL_H__
#define __SYS_SERIAL_H__

/* Number of data bits */
#define SERIAL_DATABITS_6 (6 << 0)
#define SERIAL_DATABITS_7 (7 << 0)
#define SERIAL_DATABITS_8 (8 << 0)
#define SERIAL_DATABITS_9 (9 << 0)
#define SERIAL_DATABITS_MASK (0x0f << 0)

/* Parity bit type */
#define SERIAL_PARITY_NONE (0 << 4)
#define SERIAL_PARITY_EVEN (1 << 4)
#define SERIAL_PARITY_ODD  (2 << 4)
#define SERIAL_PARITY_MARK (3 << 4)
#define SERIAL_PARITY_SPACE (4 << 4)
#define SERIAL_PARITY_MASK (0x0f << 4)

/* Number of stop bits */
#define SERIAL_STOPBITS_1 (1 << 8)
#define SERIAL_STOPBITS_2 (2 << 8)
#define SERIAL_STOPBITS_1_5 (3 << 8)
#define SERIAL_STOPBITS_0_5 (4 << 8)
#define SERIAL_STOPBITS_MASK (0x0f << 8)

/* Flow control bits */
#define SERIAL_FLOWCTRL_NONE (0 << 12)
#define SERIAL_FLOWCTRL_RTSCTS (1 << 12)
#define SERIAL_FLOWCTRL_XONXOFF (2 << 12)
#define SERIAL_FLOWCTRL_MASK (0x0f << 12)

/* Common character frame options */
#define SERIAL_8N1 (SERIAL_DATABITS_8 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_1) 
#define SERIAL_8N2 (SERIAL_DATABITS_8 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_2) 
#define SERIAL_7N1 (SERIAL_DATABITS_7 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_1) 
#define SERIAL_7N2 (SERIAL_DATABITS_7 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_2) 
#define SERIAL_8E1 (SERIAL_DATABITS_8 | SERIAL_PARITY_EVEN | SERIAL_STOPBITS_1) 
#define SERIAL_8E2 (SERIAL_DATABITS_8 | SERIAL_PARITY_EVEN | SERIAL_STOPBITS_2) 
#define SERIAL_7E1 (SERIAL_DATABITS_7 | SERIAL_PARITY_EVEN | SERIAL_STOPBITS_1) 
#define SERIAL_7E2 (SERIAL_DATABITS_7 | SERIAL_PARITY_EVEN | SERIAL_STOPBITS_2) 
#define SERIAL_8O1 (SERIAL_DATABITS_8 | SERIAL_PARITY_ODD | SERIAL_STOPBITS_1) 
#define SERIAL_8O2 (SERIAL_DATABITS_8 | SERIAL_PARITY_ODD | SERIAL_STOPBITS_2) 
#define SERIAL_7O1 (SERIAL_DATABITS_7 | SERIAL_PARITY_ODD | SERIAL_STOPBITS_1) 
#define SERIAL_7O2 (SERIAL_DATABITS_7 | SERIAL_PARITY_ODD | SERIAL_STOPBITS_2) 
#define SERIAL_9N1 (SERIAL_DATABITS_9 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_1) 
#define SERIAL_9N2 (SERIAL_DATABITS_9 | SERIAL_PARITY_NONE | SERIAL_STOPBITS_2) 

#include <stdint.h>
#include <stdbool.h>

/* character encoding and baud rate */
struct serial_config {
	uint32_t baudrate;
	uint8_t databits;
	uint8_t parity;
	uint8_t stopbits;
	uint8_t flowctrl;
};

/* Convert from the config structure to de encoded flag bits */
#define CFG_TO_FLAGS(CFG) ((CFG)->databits + (CFG)->parity +\
						  ((CFG)->stopbits << 8))

/* modem control bits */
struct serial_control {
	uint8_t dtr :1;
	uint8_t rts :1;
	/* sending line break */
	uint8_t brk :1;
};

struct serial_status {
	/* modem status bits */
	uint8_t dsr :1;
	uint8_t ri :1;
	uint8_t dcd :1;
	uint8_t cts :1;
	/* receiving line break */
	uint8_t brk :1;
};

struct serial_error {
	uint8_t ovr :1;
	uint8_t par :1;
	uint8_t frm :1;
};

struct serial_dev;

enum {
	SERIAL_IOCTL_ENABLE = 0,
	SERIAL_IOCTL_DISABLE,
	SERIAL_IOCTL_DRAIN,
	SERIAL_IOCTL_FLUSH
};

#define SERIAL_RX_EN 1
#define SERIAL_TX_EN 2

struct serial_op {
	int (* send)(void *, const void *, unsigned int);
	int (* recv)(void *, void *, unsigned int, unsigned int);
	int (* drain)(void *);
	int (* close)(void *);
	int (* conf_get)(void *, struct serial_config *);
	int (* conf_set)(void *, const struct serial_config *);
	int (* ioctl)(void *, int , unsigned int );
};

struct serial_dev {
	void * drv;
	const struct serial_op * op;
};

extern inline int serial_send(struct serial_dev * dev, const void * buf, 
							  unsigned int len) {
	return dev->op->send(dev->drv, buf, len);
}

extern inline int serial_recv(struct serial_dev * dev, void * buf, 
							  unsigned int len, unsigned int msec) {
	return dev->op->recv(dev->drv, buf, len, msec);
}

extern inline int serial_drain(struct serial_dev * dev) {
	return dev->op->drain(dev->drv);
}

extern inline int serial_close(struct serial_dev * dev){
	return dev->op->close(dev->drv);
}

extern inline int serial_config_get(struct serial_dev * dev, 
									struct serial_config * cfg){
	return dev->op->conf_get(dev->drv, cfg);
}

extern inline int serial_config_set(struct serial_dev * dev, 
									const struct serial_config * cfg){
	return dev->op->conf_set(dev->drv, cfg);
}

extern inline int serial_ioctl(struct serial_dev * dev, 
							   int opt, unsigned int arg) {
	return dev->op->ioctl(dev->drv, opt, arg);
}

#if 0
extern inline int serial_drain(struct serial_dev * dev) {
	return dev->op->ioctl(dev->drv, SERIAL_IOCTL_DRAIN, 0);
}
#endif

extern inline int serial_flush(struct serial_dev * dev) {
	return dev->op->ioctl(dev->drv, SERIAL_IOCTL_FLUSH, 0);
}


extern inline int serial_enable(struct serial_dev * dev) {
	return dev->op->ioctl(dev->drv, SERIAL_IOCTL_ENABLE, 
						  SERIAL_RX_EN | SERIAL_TX_EN);
}

extern inline int serial_disable(struct serial_dev * dev) {
	return dev->op->ioctl(dev->drv, SERIAL_IOCTL_DISABLE, 
						  SERIAL_RX_EN | SERIAL_TX_EN);
}

extern inline int serial_rx_enable(struct serial_dev * dev) {
	return dev->op->ioctl(dev->drv, SERIAL_IOCTL_ENABLE, SERIAL_RX_EN);
}

extern inline int serial_rx_disable(struct serial_dev * dev) {
	return dev->op->ioctl(dev->drv, SERIAL_IOCTL_DISABLE, SERIAL_RX_EN);
}

#ifdef __cplusplus
extern "C" {
#endif

int serial_send(struct serial_dev * dev, const void * buf, 
				unsigned int len);

int serial_recv(struct serial_dev * dev, void * buf, 
				unsigned int len, unsigned int msec);

int serial_drain(struct serial_dev * dev);

int serial_close(struct serial_dev * dev);

int serial_config_get(struct serial_dev * dev, struct serial_config * cfg);

int serial_config_set(struct serial_dev * dev, 
					  const struct serial_config * cfg);
int serial_ioctl(struct serial_dev * dev, int opt, unsigned int arg);


int serial_enable(struct serial_dev * dev);

int serial_disable(struct serial_dev * dev);

int serial_rx_disable(struct serial_dev * dev);

int serial_rx_enable(struct serial_dev * dev);


struct file * serial_fopen(struct serial_dev * dev);

bool is_serial(struct file * f); 

/*
int serial_control_get(struct serial_dev * dev, struct serial_control * ctrl);

int serial_status_set(struct serial_dev * dev, struct serial_status * stat);
*/

#ifdef __cplusplus
}
#endif

#endif /* __SYS_SERIAL_H__ */

