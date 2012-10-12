/*
 *
 * File:	uboot.c
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:	
 * Copyright(c) 2008-2011 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <stdlib.h>
#include <stdbool.h>
#include <crc.h>
#include <stdio.h>
#include <sys/ice-comm.h>

#include "sas_cmd.h"

#define SAS_PKT_LEN_MAX 1024

struct sas_conn {
	struct stm32f_usart * us;
};

typedef struct sas_conn sas_conn_t;


unsigned int sas_pkt_send(sas_conn_t * conn, int wake, 
						  uint8_t * pkt, unsigned int len)
{
	struct stm32f_usart * us = conn->us;
	int i;

	stm32f_usart_putc(us, pkt[0] | (wake ? 0x100 : 0));

	for (i = 1; i < len; i++) {
		stm32f_usart_putc(us, pkt[i]);
	}

	return len;
}

unsigned int sas_catu16(uint8_t * pkt, unsigned int len, uint16_t val)
{
	pkt[len] = val & 0xff;
	pkt[len + 1] = (val >> 8) & 0xff;

	return len + 2;
}

unsigned int sas_catu32(uint8_t * pkt, unsigned int len, uint32_t val)
{
	pkt[len] = val & 0xff;
	pkt[len + 1] = (val >> 8) & 0xff;
	pkt[len + 2] = (val >> 12) & 0xff;
	pkt[len + 3] = (val >> 16) & 0xff;

	return len + 4;
}

unsigned int sas_catcrc(uint8_t * pkt, unsigned int len)
{
	unsigned int crc;

	crc = crc16ccitt(0, pkt, len);
	return sas_catu16(pkt, len, crc);
}

unsigned int sas_mkhdr(uint8_t * pkt, uint8_t addr, uint8_t cmd)
{
	pkt[0] = addr;
	pkt[1] = cmd;
	return 2;	
}


/* general poll */
void sas_general_poll(sas_conn_t * conn, int addr)
{
	uint8_t pkt[SAS_PKT_LEN_MAX];

	pkt[0] = (addr & 0x7f) | 0x80;
	sas_pkt_send(conn, 1, pkt, 1);
}


/* Type R host message */
void sas_send_current_credits(sas_conn_t * conn, int addr)
{
	uint8_t pkt[SAS_PKT_LEN_MAX];
	unsigned int len;

	len = sas_mkhdr(pkt, addr, 0x1a);
	sas_pkt_send(conn, 1, pkt, len);
}

void sas_rom_signature_poll(sas_conn_t * conn, int addr)
{
	uint8_t pkt[SAS_PKT_LEN_MAX];
	unsigned int len;
	uint16_t seed;

	seed = rand() & 0xffff;

	len = sas_mkhdr(pkt, addr, SAS_CMD_ROM_SIGNATURE);
	len = sas_catu16(pkt, len, seed);
	len = sas_catcrc(pkt, len);

	sas_pkt_send(conn, 1, pkt, len);
}

void sas_startup(sas_conn_t * conn, int addr)
{
	uint8_t pkt[SAS_PKT_LEN_MAX];
	unsigned int len;

	len = sas_mkhdr(pkt, addr, SAS_CMD_STARTUP);
	len = sas_catcrc(pkt, len);

	sas_pkt_send(conn, 1, pkt, len);
}

void sas_shutdown(sas_conn_t * conn, int addr)
{
	uint8_t pkt[SAS_PKT_LEN_MAX];
	unsigned int len;

	len = sas_mkhdr(pkt, addr, SAS_CMD_SHUTDOWN);
	len = sas_catcrc(pkt, len);

	sas_pkt_send(conn, 1, pkt, len);
}

void sas_en_dis_rt_event_report(sas_conn_t * conn, int addr, bool flag)
{
	uint8_t pkt[SAS_PKT_LEN_MAX];
	unsigned int len;

	len = sas_mkhdr(pkt, addr, SAS_CMD_EN_DIS_RT_EVENT_REPORT);
	pkt[len++] = flag ? 1 : 0;
	len = sas_catcrc(pkt, len);

	sas_pkt_send(conn, 1, pkt, len);
}

void sas_rom_signature_reply(sas_conn_t * conn, int addr)
{
	uint8_t pkt[SAS_PKT_LEN_MAX];
	unsigned int len;
	uint16_t seed;

	seed = rand() & 0xffff;

	len = sas_mkhdr(pkt, addr, SAS_CMD_ROM_SIGNATURE);
	len = sas_catu16(pkt, len, seed);
	len = sas_catcrc(pkt, len);

	sas_pkt_send(conn, 0, pkt, len);
}


void sas_open(sas_conn_t * conn, struct stm32f_usart * us)
{
	conn->us = us;;
}

FILE * stdout = (FILE *)&ice_comm_file;
FILE * stdin = (FILE *)&ice_comm_file;
FILE * stderr = (FILE *)&ice_comm_file;

void stm32f_uart5_isr(void)
{
	struct stm32f_usart * us = STM32F_USART5;
	uint32_t sr = us->sr;
	int c;

	if (sr & USART_RXNE) {
		c = us->dr;
		printf("[%03x]", c);
	}	
}

void serial_cfg(void)
{
	struct stm32f_usart * us = STM32F_USART5;

	cm3_irq_enable(STM32F_IRQ_UART5);
	/* enable RX interrupt */
	us->cr1 |= USART_RXNEIE;
}

int main(int argc, char ** argv)
{
	struct stm32f_usart * us = STM32F_USART5;
	sas_conn_t conn;
	int cnt = 0;
	unsigned int addr = 0x45;

	cm3_udelay_calibrate();

	printf("Serial open\n");
	stm32f_usart_open(us, 19200, SERIAL_9N1);
	printf("Serial config\n");
	serial_cfg();

	sas_open(&conn, us);
/*
	for (;;) {
		printf("Count=%d\n", ++cnt);
		delay(1);
	}
*/
	for (;;) {
		printf("Count=%d\n", ++cnt);

		printf(" - General Poll\n");
		sas_general_poll(&conn, 1);
		udelay(5000);	
		sas_general_poll(&conn, addr);
		udelay(500000);	

		printf(" - Enable RT event report\n");
		sas_en_dis_rt_event_report(&conn, addr, true);
		udelay(500000);	

		printf(" - Startup\n");
		sas_startup(&conn, addr);
		udelay(500000);	

		printf(" - ROM signature\n");
		sas_rom_signature_poll(&conn, addr);
		udelay(500000);	

		printf(" - Send current credits\n");
		sas_send_current_credits(&conn, addr);
		udelay(500000);	

		printf(" - Shutdown\n");
		sas_shutdown(&conn, addr);
		udelay(500000);	

		printf(" - Disable RT event report\n");
		sas_en_dis_rt_event_report(&conn, addr, false);
		udelay(500000);	

		printf("Wait.. \n");
		delay(2);	
	}

	return 0;
}

