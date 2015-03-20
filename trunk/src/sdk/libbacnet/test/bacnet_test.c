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

#include <sys/dcclog.h>

#include "board.h"
#include "lattice.h"

#include "dcc.h"
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

struct bacnet_ptp_lnk * ptp_lnk;

int datalink_send_pdu(BACNET_ADDRESS * dest, BACNET_NPDU_DATA * npdu_data,
					  uint8_t * pdu, unsigned pdu_len)
{
	DCC_LOG(LOG_INFO, "...");
	return bacnet_ptp_send(ptp_lnk, pdu, pdu_len);
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

uint16_t datalink_receive(BACNET_ADDRESS * src, uint8_t * pdu,
						  uint16_t max_pdu, unsigned timeout)
{
	return 0;
}

void datalink_cleanup(void)
{
	DCC_LOG(LOG_TRACE, "...");
	return;
}

void datalink_get_broadcast_address(BACNET_ADDRESS * dest)
{
	DCC_LOG(LOG_INFO, "...");

    dest->mac_len = 0;
    dest->net = 0;
    dest->len = 0;
}

void datalink_get_my_address(BACNET_ADDRESS * my_address)
{
	DCC_LOG(LOG_INFO, "...");

    my_address->mac_len = 0;
    my_address->net = 0;
    my_address->len = 0;        
}

void datalink_set_interface(char *ifname)
{
	DCC_LOG(LOG_TRACE, "...");
	return;
}

void datalink_set(char *datalink_string)
{
	DCC_LOG(LOG_TRACE, "...");
	return;
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

#if 0
    /* initialize objects */
    Device_Init(NULL);

    /* set up our confirmed service unrecognized service handler - required! */
    apdu_set_unrecognized_service_handler_handler
        (handler_unrecognized_service);
    /* we need to handle who-is to support dynamic device binding */
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_IS, handler_who_is);
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_HAS, handler_who_has);
    /* Set the handlers for any confirmed services that we support. */
    /* We must implement read property - it's required! */
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROPERTY,
        handler_read_property);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_PROP_MULTIPLE,
        handler_read_property_multiple);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_REINITIALIZE_DEVICE,
        handler_reinitialize_device);
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_WRITE_PROPERTY,
        handler_write_property);
    /* handle communication so we can shutup when asked */
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL,
        handler_device_communication_control);

    /* handle i-am to support binding to other devices */
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_I_AM, handler_i_am_bind);

    /* handle the data coming back from COV subscriptions */
    apdu_set_confirmed_handler(SERVICE_CONFIRMED_COV_NOTIFICATION,
        My_Confirmed_COV_Notification_Handler);

    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_COV_NOTIFICATION,
        My_Unconfirmed_COV_Notification_Handler);

    /* handle the Simple ack coming back from SubscribeCOV */
    apdu_set_confirmed_simple_ack_handler(SERVICE_CONFIRMED_SUBSCRIBE_COV,
        MyWritePropertySimpleAckHandler);
    /* handle any errors coming back */
    apdu_set_error_handler(SERVICE_CONFIRMED_SUBSCRIBE_COV, MyErrorHandler);
    apdu_set_abort_handler(MyAbortHandler);
    apdu_set_reject_handler(MyRejectHandler);
#endif
}

FILE * serial_tty_open(struct serial_dev * serdev)
{
	struct tty_dev * tty;
	FILE * f_raw;

	f_raw = serial_fopen(serdev);
	tty = tty_attach(f_raw);
	return tty_fopen(tty);
}


int __attribute__((noreturn)) shell_task(FILE * term)
{
	for (;;) {
		shell(term, shell_prompt, shell_greeting, shell_cmd_tab);
	}
}

uint32_t shell_stack[512];

const struct thinkos_thread_inf shell_inf = {
	.stack_ptr = shell_stack, 
	.stack_size = sizeof(shell_stack), 
	.priority = 32,
	.thread_id = 7, 
	.paused = 0,
	.tag = "SHELL"
};

int main(int argc, char ** argv)
{
	struct serial_dev * ser5;
	struct serial_dev * ser1;
	FILE * term1;
	FILE * term5;
	uint8_t pdu[512];
	uint16_t pdu_len;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. stm32f_nvram_env_init().");
	stm32f_nvram_env_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "4. console serial init");
	ser5 = stm32f_uart5_serial_init(57600, SERIAL_8N1);
//	ser5 = stm32f_uart5_serial_init(115200, SERIAL_8N1);
	ser1 = stm32f_uart1_serial_init(460800, SERIAL_8N1);

	term1 = serial_tty_open(ser1);
	term5 = serial_tty_open(ser5);

	DCC_LOG(LOG_TRACE, "4. stdio_init().");
	stderr = term1;
	stdout = term1;
	stdin = term1;

	DCC_LOG(LOG_TRACE, "5. starting HTTP workers...");
	thinkos_thread_create_inf((void *)shell_task, (void *)term1, &shell_inf);


	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" BACnet Test\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	network_init();

	bacnet_init();

	DCC_LOG(LOG_TRACE, "5. starting console shell...");

	for (;;) {
		DCC_LOG(LOG_WARNING, "Console shell!");
	
		shell(term5, shell_prompt, shell_greeting, shell_cmd_tab);

		/* BACnet protocol... */
		DCC_LOG(LOG_WARNING, "BACnet Data Link Connection!");
		ptp_lnk = bacnet_ptp_inbound(ser5);

		while ((pdu_len = bacnet_ptp_recv(ptp_lnk, pdu, 512)) >= 0) {
	       	npdu_handler(NULL, pdu, pdu_len);
		}

		DCC_LOG(LOG_WARNING, "BACnet Data Link Connection!");

	}

	return 0;
}



