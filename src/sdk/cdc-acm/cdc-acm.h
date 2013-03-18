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


#include <stdlib.h>
#include <stdio.h>
#include <sys/usb.h>
#include <sys/cdc.h>

#define EP0 0
#define EP0_TX_FIFO 0
#define EP0_MAX_PKT_SIZE 8
#define EP0_FIFO_SIZE 128

#define EP_OUT 1
#define EP_OUT_TX_FIFO 1
#define EP_OUT_MAX_PKT_SIZE 64
#define EP_OUT_FIFO_SIZE 512

#define EP_IN  2
#define EP_IN_TX_FIFO 0
#define EP_IN_MAX_PKT_SIZE 64
#define EP_IN_FIFO_SIZE 512

#define EP_INT 3
#define EP_INT_TX_FIFO 2
#define EP_INT_MAX_PKT_SIZE 64
#define EP_INT_FIFO_SIZE 128

struct usb_descriptor_set_cdc {
	struct usb_descriptor_configuration cfg;
	struct usb_descriptor_interface comm_if;
	struct cdc_header_descriptor hdr;
	struct cdc_call_management_descriptor cm;
	struct cdc_abstract_control_management_descriptor acm;
	struct cdc_union_1slave_descriptor un;
	struct usb_descriptor_endpoint ep_int;
	struct usb_descriptor_interface if_data;
	struct usb_descriptor_endpoint ep_out;
	struct usb_descriptor_endpoint ep_in;
} __attribute__((__packed__));

struct cdc_desc_set {
	struct usb_descriptor_device device;
	struct usb_descriptor_set_cdc conf;
} __attribute__((__packed__));

extern const struct cdc_desc_set cdc_acm_desc;

const struct usb_descriptor_endpoint * cdc_acm_ep[4];

#define CDC_STOP_BITS_1   (0 << 0)
#define CDC_STOP_BITS_1_5 (1 << 0)
#define CDC_STOP_BITS_2   (2 << 0)
#define CDC_STOP_BITS_ERR (3 << 0)
#define CDC_CHAR_FORMAT_SET(VAL)  (((VAL) & 0x3) << 0)
#define CDC_CHAR_FORMAT_GET(VAL)  (((VAL) >> 0) & 0x3)
#define CDC_STOP_BITS_MAX 2

#define CDC_PARITY_NONE   (0 << 2)
#define CDC_PARITY_ODD    (1 << 2)
#define CDC_PARITY_EVEN   (2 << 2)
#define CDC_PARITY_MARK   (3 << 2)
#define CDC_PARITY_SPACE  (4 << 2)
#define CDC_PARITY_ERR    (7 << 2)
#define CDC_PARITY_SET(VAL)  (((VAL) & 0x7) << 2)
#define CDC_PARITY_GET(VAL)  (((VAL) >> 2) & 0x7)
#define CDC_PARITY_MAX    4

#define CDC_DATA_BITS_5   (0 << 5)
#define CDC_DATA_BITS_6   (1 << 5)
#define CDC_DATA_BITS_7   (2 << 5)
#define CDC_DATA_BITS_8   (3 << 5)
#define CDC_DATA_BITS_16  (4 << 5)
#define CDC_DATA_BITS_ERR (7 << 5)
#define CDC_DATA_BITS_SET(VAL)  data_bit_lut[(VAL) & 0x1f]
#define CDC_DATA_BITS_GET(VAL)  data_bit_rev_lut[((VAL) >> 5) & 0x7]
#define CDC_DATA_BITS_MAX 16


