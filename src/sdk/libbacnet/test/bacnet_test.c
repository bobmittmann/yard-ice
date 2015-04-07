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
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/serial.h>
#include <sys/shell.h>
#include <sys/tty.h>
#include <tcpip/net.h>
#include <thinkos.h>
#include <bacnet/bacnet-ptp.h>
#include <bacnet/bacnet-mstp.h>
#include <bacnet/bacnet-dl.h>
#include <sys/usb-cdc.h>

#include <sys/dcclog.h>

#include "board.h"
#include "lattice.h"

#include "npdu.h"
#include "address.h"
#include "device.h"
#include "bactext.h"
#include "bacerror.h"

/* some demo stuff needed */
#include "handlers.h"
#include "client.h"

#define VERSION_NUM "0.1"
#define VERSION_DATE "Mar, 2015"

const char * version_str = "BACnet Demo " \
							VERSION_NUM " - " VERSION_DATE;
const char * copyright_str = "(c) Copyright 2015 - Bob Mittmann";

const char * shell_prompt(void)
{
	return "[WEBSRV]$ ";
}

void shell_greeting(FILE * f) 
{
	fprintf(f, "\n%s", version_str);
	fprintf(f, "\n%s\n\n", copyright_str);
}

extern const struct shell_cmd shell_cmd_tab[];
extern const uint8_t ice40lp384_bin[];
extern const unsigned int sizeof_ice40lp384_bin;
#if 0
void network_init(void)
{
	struct ifnet * ifn;
	in_addr_t ip_addr;
	in_addr_t netmask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	char s[64];
	char s1[16];
	char s2[16];
	char * env;
	/* TODO: random initial mac address */
	uint8_t ethaddr[6] = { 0x1c, 0x95, 0x5d, 0x00, 0x00, 0x80};
	uint64_t esn;
	int dhcp;

	esn = *((uint64_t *)STM32F_UID);
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);

	ethaddr[0] = ((esn >>  0) & 0xfc) | 0x02; /* Locally administered MAC */
	ethaddr[1] = ((esn >>  8) & 0xff);
	ethaddr[2] = ((esn >> 16) & 0xff);
	ethaddr[3] = ((esn >> 24) & 0xff);
	ethaddr[4] = ((esn >> 32) & 0xff);
	ethaddr[5] = ((esn >> 40) & 0xff);

	printf("* mac addr: %02x-%02x-%02x-%02x-%02x-%02x\n ", 
		   ethaddr[0], ethaddr[1], ethaddr[2],
		   ethaddr[3], ethaddr[4], ethaddr[5]);

	DCC_LOG(LOG_TRACE, "tcpip_init().");
	tcpip_init();

//	if ((env = getenv("IPCFG")) == NULL) {
	if (1) {
		printf("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.10.128 255.255.255.0 192.168.10.254 0");
		/* set the default configuration */
		setenv("IPCFG", s, 1);
	} else {
		strcpy(s, env);
	}

	if (!inet_aton(strtok(s, " ,"), (struct in_addr *)&ip_addr)) {
		DCC_LOG(LOG_WARNING, "inet_aton() failed.");
		return;
	}

	if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&netmask)) {
		if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&gw_addr)) {
			dhcp = strtoul(strtok(NULL, ""), NULL, 0);
		}
	}

	/* initialize the Ethernet interface */
	/* configure the ip address */
	ifn = ethif_init(ethaddr, ip_addr, netmask);
//	ifn = loopif_init(ip_addr, netmask);

	ifn_getname(ifn, s);
	ifn_ipv4_get(ifn, &ip_addr, &netmask);
	printf("* netif %s: %s, %s\n", s, 
		   inet_ntop(AF_INET, (void *)&ip_addr, s1, 16),
		   inet_ntop(AF_INET, (void *)&netmask, s2, 16));

	if (gw_addr != INADDR_ANY) {
		/* add the default route (gateway) to ethif */
		ipv4_route_add(INADDR_ANY, INADDR_ANY, gw_addr, ifn);
		printf("* default route gw: %s\n", 
			   inet_ntop(AF_INET, (void *)&gw_addr, s1, 16));
	}

	if (dhcp) {
#if 0
		/* configure the initial ip address */
		dhcp_start();
		/* schedule the interface to be configured through dhcp */
		dhcp_ifconfig(ethif, dhcp_callback);
		tracef("DHCP started.\n");
#endif
	}
}
#endif
void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	/* USART5 TX */
	stm32_gpio_mode(UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART5_TX, GPIO_AF8);
	/* USART5 RX */
	stm32_gpio_mode(UART5_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART5_RX, GPIO_AF8);

	/* USART6_TX */
	stm32_gpio_mode(UART6_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART6_TX, GPIO_AF7);
	/* USART6_RX */
	stm32_gpio_mode(UART6_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART6_RX, GPIO_AF7);

	/* IO init */
	stm32_gpio_mode(RS485_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(RS485_RX, GPIO_AF7);

	stm32_gpio_mode(RS485_TX, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(RS485_TX, GPIO_AF7);

#ifdef RS485_CK
	stm32_gpio_mode(RS485_CK, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(RS485_CK, GPIO_AF7);
#endif

#ifdef RS485_TRIG
	stm32_gpio_mode(RS485_TRIG, INPUT, PULL_UP);
#endif

#ifdef RS485_TXEN
	stm32_gpio_mode(RS485_TXEN, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_set(RS485_TXEN);
#endif

#ifdef RS485_LOOP
	stm32_gpio_mode(RS485_LOOP, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_set(RS485_LOOP);
#endif

#ifdef RS485_MODE
	stm32_gpio_mode(RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_set(RS485_MODE);
#endif

	lattice_ice40_configure(ice40lp384_bin, sizeof_ice40lp384_bin);
}

/* converted command line arguments */
uint32_t Target_Device_Object_Instance = BACNET_MAX_INSTANCE;
/* Process identifier for matching replies */
uint32_t Target_Device_Process_Identifier = 0;
/* the invoke id is needed to filter incoming messages */
uint8_t Request_Invoke_ID = 0;
/* MAC and SNET address of target */
BACNET_ADDRESS Target_Address;
/* indication of error, reject, or abort */
bool Error_Detected = false;
/* data used in COV subscription request */
BACNET_SUBSCRIBE_COV_DATA *COV_Subscribe_Data = NULL;
/* flags to signal early termination */
bool Notification_Detected = false;
bool Simple_Ack_Detected = false;
bool Cancel_Requested = false;

void MyErrorHandler(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    BACNET_ERROR_CLASS error_class,
    BACNET_ERROR_CODE error_code)
{
	DCC_LOG(LOG_TRACE, "...");
    if (address_match(&Target_Address, src) &&
        (invoke_id == Request_Invoke_ID)) {
        printf("BACnet Error: %s: %s\r\n",
            bactext_error_class_name((int) error_class),
            bactext_error_code_name((int) error_code));
        Error_Detected = true;
    }
}

void MyAbortHandler(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t abort_reason,
    bool server)
{
    (void) server;
	DCC_LOG(LOG_TRACE, "...");
    if (address_match(&Target_Address, src) &&
        (invoke_id == Request_Invoke_ID)) {
        printf("BACnet Abort: %s\r\n",
            bactext_abort_reason_name((int) abort_reason));
        Error_Detected = true;
    }
}

void MyRejectHandler(
    BACNET_ADDRESS * src,
    uint8_t invoke_id,
    uint8_t reject_reason)
{
	DCC_LOG(LOG_TRACE, "...");
    if (address_match(&Target_Address, src) &&
        (invoke_id == Request_Invoke_ID)) {
        printf("BACnet Reject: %s\r\n",
            bactext_reject_reason_name((int) reject_reason));
        Error_Detected = true;
    }
}

void My_Unconfirmed_COV_Notification_Handler(
    uint8_t * service_request,
    uint16_t service_len,
    BACNET_ADDRESS * src)
{
	DCC_LOG(LOG_TRACE, "...");
    handler_ucov_notification(service_request, service_len, src);
    Notification_Detected = true;
}

void My_Confirmed_COV_Notification_Handler(
    uint8_t * service_request,
    uint16_t service_len,
    BACNET_ADDRESS * src,
    BACNET_CONFIRMED_SERVICE_DATA * service_data)
{
	DCC_LOG(LOG_TRACE, "...");
    handler_ccov_notification(service_request, service_len, src, service_data);
    Notification_Detected = true;
}

void MyWritePropertySimpleAckHandler(
    BACNET_ADDRESS * src,
    uint8_t invoke_id)
{
	DCC_LOG(LOG_TRACE, "...");
    if (address_match(&Target_Address, src) &&
        (invoke_id == Request_Invoke_ID)) {
        printf("SubscribeCOV Acknowledged!\r\n");
        Simple_Ack_Detected = true;
    }
}

static void Init_Service_Handlers( void)
{
    Device_Init(NULL);
    /* we need to handle who-is to support dynamic device binding */
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_IS, handler_who_is);
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_HAS, handler_who_has);
    /* handle i-am to support binding to other devices */
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_I_AM, handler_i_am_bind);
    /* set the handler for all the services we don't implement */
    /* It is required to send the proper reject message... */
    apdu_set_unrecognized_service_handler_handler
        (handler_unrecognized_service);
    /* Set the handlers for any confirmed services that we support. */
    /* We must implement read property - it's required! */
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROPERTY,
        handler_read_property);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROP_MULTIPLE,
        handler_read_property_multiple);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_WRITE_PROPERTY,
        handler_write_property);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE,
        handler_write_property_multiple);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_RANGE,
        handler_read_range);
#if defined(BACFILE)
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_ATOMIC_READ_FILE,
        handler_atomic_read_file);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_ATOMIC_WRITE_FILE,
        handler_atomic_write_file);
#endif
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_REINITIALIZE_DEVICE,
        handler_reinitialize_device);
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_UTC_TIME_SYNCHRONIZATION,
        handler_timesync_utc);
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_TIME_SYNCHRONIZATION,
        handler_timesync);
    /* handle communication so we can shutup when asked */
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL,
        handler_device_communication_control);
    /* handle the data coming back from private requests */
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_PRIVATE_TRANSFER,
        handler_unconfirmed_private_transfer);
#if defined(INTRINSIC_REPORTING)
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_ACKNOWLEDGE_ALARM,
        handler_alarm_ack);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_GET_EVENT_INFORMATION,
        handler_get_event_information);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_GET_ALARM_SUMMARY,
        handler_get_alarm_summary);
#endif /* defined(INTRINSIC_REPORTING) */

    apdu_set_confirmed_handler(SERVICE_CONFIRMED_SUBSCRIBE_COV,
        handler_cov_subscribe);
//    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_COV_NOTIFICATION,
 //       handler_ucov_notification);

    /* handle the data coming back from COV subscriptions */
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_COV_NOTIFICATION,
        My_Confirmed_COV_Notification_Handler);

    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_COV_NOTIFICATION,
        My_Unconfirmed_COV_Notification_Handler);

    /* handle the Simple ack coming back from SubscribeCOV */
    apdu_set_confirmed_simple_ack_handler(SERVICE_CONFIRMED_SUBSCRIBE_COV,
        MyWritePropertySimpleAckHandler);
    /* handle any errors coming back */
//    apdu_set_error_handler(SERVICE_CONFIRMED_SUBSCRIBE_COV, MyErrorHandler);
    apdu_set_abort_handler(MyAbortHandler);
    apdu_set_reject_handler(MyRejectHandler);
}

void bacnet_init(void)
{
	address_init();

	Init_Service_Handlers();

}

FILE * serial_tty_open(struct serial_dev * serdev)
{
	struct tty_dev * tty;
	FILE * f_raw;

	f_raw = serial_fopen(serdev);
	tty = tty_attach(f_raw);
	return tty_fopen(tty);
}

FILE * usb_tty_open(usb_cdc_class_t * cdc)
{
	struct tty_dev * tty;
	FILE * f_raw;

	f_raw = usb_cdc_fopen(cdc);
	tty = tty_attach(f_raw);
	return tty_fopen(tty);
}


int __attribute__((noreturn)) shell_task(FILE * term)
{
	for (;;) {
		shell(term, shell_prompt, shell_greeting, shell_cmd_tab);
	}
}

struct bn_ptp_bundle {
	FILE * term;
	struct bacnet_ptp_lnk * ptp; 
};

int __attribute__((noreturn)) bacnet_task(struct bn_ptp_bundle * p)
{
	FILE * term = p->term;
	struct bacnet_ptp_lnk * ptp = p->ptp; 

	for (;;) {
		DCC_LOG1(LOG_WARNING, "<%d> Console shell!", thinkos_thread_self());
		shell(term, shell_prompt, shell_greeting, shell_cmd_tab);

		/* BACnet protocol... */
		DCC_LOG1(LOG_WARNING, "<%d> BACnet PtP Data Link!", 
				 thinkos_thread_self());
		bacnet_ptp_inbound(ptp);
		bacnet_ptp_loop(ptp);
	}
}

uint32_t tty1_stack[1024];

const struct thinkos_thread_inf tty1_inf = {
	.stack_ptr = tty1_stack, 
	.stack_size = sizeof(tty1_stack), 
	.priority = 32,
	.thread_id = 8, 
	.paused = 0,
	.tag = "TTY1"
};

uint32_t tty2_stack[1024];

const struct thinkos_thread_inf tty2_inf = {
	.stack_ptr = tty2_stack, 
	.stack_size = sizeof(tty2_stack), 
	.priority = 32,
	.thread_id = 7, 
	.paused = 0,
	.tag = "TTY2"
};

struct bacnet_ptp_lnk ptp1; 
struct bacnet_ptp_lnk ptp2; 
struct bacnet_mstp_lnk mstp1; 

char msg[64];
uint8_t rcv_buf[512];

#if THINKOS_STDERR_FAULT_DUMP
const struct file stm32_uart_file = {
	.data = STM32_UART5, 
	.op = &stm32_usart_fops 
};
#endif

int main(int argc, char ** argv)
{
	struct serial_dev * ser1;
	struct serial_dev * ser2;
	struct serial_dev * ser5;
	FILE * term1;
	FILE * term2;
	struct bn_ptp_bundle bdl1;
	struct bn_ptp_bundle bdl2;
	unsigned int mstp_addr;
	char * env;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. stm32f_nvram_env_init().");
	stm32f_nvram_env_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(31));

	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "4. serial ports init");
//	ser5 = stm32f_uart5_serial_init(57600, SERIAL_8N1);
	ser5 = stm32f_uart5_serial_init(115200, SERIAL_8N1);
//	ser5 = stm32f_uart5_serial_init(38400, SERIAL_8N1);
//	ser1 = stm32f_uart1_serial_init(460800, SERIAL_8N1);
	ser1 = stm32f_uart1_serial_init(500000, SERIAL_8N1);
//	ser1 = stm32f_uart1_serial_dma_init(500000, SERIAL_8N1);
//	ser1 = stm32f_uart1_serial_dma_init(460800, SERIAL_8N1);

	DCC_LOG(LOG_TRACE, "13. usb_cdc_init()");
	usb_cdc_sn_set(*((uint64_t *)STM32F_UID));
	usb_cdc_init(&stm32f_otg_fs_dev, 
				 cdc_acm_def_str, 
				 cdc_acm_def_strcnt);
	
	ser2 = (struct serial_dev *)&cdc_acm_serial_dev;

	term1 = serial_tty_open(ser5);
//	term1 = serial_tty_open(ser1);
	term2 = serial_tty_open(ser2);

	DCC_LOG(LOG_TRACE, "4. stdio_init().");
#if THINKOS_STDERR_FAULT_DUMP
	stderr = (struct file *)&stm32_uart_file;
#else
	stderr = term1;
#endif
	stdout = stderr;
	stdin = stderr;

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" BACnet Test\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

//	network_init();

	bacnet_init();

	bacnet_dl_init();

	DCC_LOG(LOG_TRACE, "5. starting BACnet PtP links...");
	bacnet_ptp_init(&ptp1, "PtP1", ser5);
	bacnet_ptp_init(&ptp2, "PtP2", ser2);

	if ((env = getenv("MSTP")) == NULL) {
		char s[16];
		mstp_addr = 8;
		/* default configuration */
		sprintf(s, "%d", mstp_addr);
		/* set the default configuration */
		setenv("MSTP", s, 1);
	} else {
		mstp_addr = strtoul(env, NULL, 0);
	}

	DCC_LOG1(LOG_TRACE, "5. BACnet MS/TP link addr: %d ...", mstp_addr);
	bacnet_mstp_init(&mstp1, "MS/TP1", mstp_addr, ser1);
//	bacnet_mstp_init(&mstp1, "MS/TP1", mstp_addr, ser5);

	DCC_LOG(LOG_TRACE, "6. TTY threads...");
	bdl1.ptp = &ptp1;
	bdl1.term = term1;
	thinkos_thread_create_inf((void *)bacnet_task, (void *)&bdl1, &tty1_inf);
	bdl2.ptp = &ptp2;
	bdl2.term = term2;
	thinkos_thread_create_inf((void *)bacnet_task, (void *)&bdl2, &tty2_inf);

#if 0
	{
		int i;
		uint32_t old_clk = thinkos_clock();
		int n;

		for (i = 0;; ++i) {
			uint32_t clk;
			int32_t dt;
			int ret;
	
			clk = thinkos_clock();
			dt = clk - old_clk;
			old_clk = clk;
			DCC_LOG1(LOG_TRACE, "-- %4d ----------------------------", dt);
		
			ret = serial_recv(ser1, rcv_buf, 512, 100);
			(void)ret;
//			if (ret != THINKOS_ETIMEDOUT)
//				thinkos_sleep(0);
		
			n = sprintf(msg, "%2d %6d.%03d "
						"The quick brown fox jumps over the lazy dog!\r\n", 
						 mstp_addr, clk / 1000, clk % 1000); 
			serial_send(ser1, msg, n);
		}
	}
#endif

	thinkos_sleep(100);
	DCC_LOG(LOG_TRACE, "7. BACnet MS/TP...");
	bacnet_mstp_start(&mstp1);
	bacnet_mstp_loop(&mstp1);
	

	return 0;
}



