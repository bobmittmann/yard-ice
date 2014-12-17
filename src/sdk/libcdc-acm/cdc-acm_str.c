/* 
 * File:	 cdc_acm.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#include "cdc-acm.h"
#include <sys/param.h>
#include <string.h>
#include <sys/dcclog.h>

#define LANG_STR_SZ              4
/* LangID = 0x0409: U.S. English */
const uint8_t cdc_acm_lang_str[LANG_STR_SZ] = {
	LANG_STR_SZ, USB_DESCRIPTOR_STRING,
	0x09, 0x04
};

#define VENDOR_STR_SZ            38
const uint8_t cdc_acm_vendor_str[VENDOR_STR_SZ] = {
	VENDOR_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Manufacturer: "STMicroelectronics" */
	'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0, 'l', 0, 
	'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0, 'c', 0, 's', 0
};


#define PRODUCT_STR_SZ           44
const uint8_t cdc_acm_product_str[PRODUCT_STR_SZ] = {
	PRODUCT_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Product name: "STM32 Virtual ComPort" */
	'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'V', 0, 'i', 0, 'r', 0, 
	't', 0, 'u', 0, 'a', 0, 'l', 0, ' ', 0, 'C', 0, 'o', 0, 'm', 0, 'P', 0, 
	'o', 0, 'r', 0, 't', 0,
};


#define SERIAL_STR_SZ            26
uint8_t cdc_acm_serial_str[SERIAL_STR_SZ] = {
	SERIAL_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Serial number: "0000000000001" */
	'0', 0, '0', 0, '0', 0, '0', 0, '0', 0, 
	'0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '1', 0
};

#define INTERFACE_STR_SZ         16
const uint8_t cdc_acm_interface_str[INTERFACE_STR_SZ] = {
	INTERFACE_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Interface 0: "ST VCOM" */
	'S', 0, 'T', 0, ' ', 0, 'V', 0, 'C', 0, 'O', 0, 'M', 0
};

const uint8_t * const cdc_acm_def_str[] = {
	cdc_acm_lang_str,
	cdc_acm_vendor_str,
	cdc_acm_product_str,
	cdc_acm_serial_str,
	cdc_acm_interface_str
};

const uint8_t cdc_acm_def_strcnt = sizeof(cdc_acm_def_str) / sizeof(uint8_t *);

void usb_cdc_sn_set(uint64_t sn)
{
	char s[24];
	char * cp;
	int c;
	int i;
	int n;

	DCC_LOG2(LOG_INFO, "ESN: %08x %08x", (uint32_t)sn, (uint32_t)(sn >> 32LL));

	n = sprintf(s, "%llu", sn);
	cp = s + n - 1;

	for (i = (SERIAL_STR_SZ / 2) - 1; i >= 0; --i) {
		if (cp < s)
			break;
		c = *cp--;
		cdc_acm_serial_str[i * 2] = c;
	}
}


