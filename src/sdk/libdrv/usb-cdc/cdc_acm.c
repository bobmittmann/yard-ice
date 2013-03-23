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


#include "cdc_acm.h"

#define ATMEL_VCOM_PRODUCT_ID 0x6119
#define ST_VCOM_PRODUCT_ID 0x5740

const struct cdc_desc_set cdc_acm_desc = {
	{
		/* Size of this descriptor in bytes */
		sizeof(struct usb_descriptor_device),
		/* DEVICE descriptor type */
		USB_DESCRIPTOR_DEVICE,
		/* USB specification release number */
//			USB2_00,
		USB1_10,
		/* Class code */
		USB_CLASS_COMMUNICATION,
		/* Subclass code */
		0x00,
		/* Protocol code */
		0x00,
		/* Control endpoint 0 max. packet size */
		EP0_MAX_PKT_SIZE,
		/* Vendor ID */
		USB_VENDOR_ST,
		/* Product ID */
		ST_VCOM_PRODUCT_ID,
		/* Device release number */
		0x0001,
		/* Index of manu. string descriptor */
		0x00,
		/* Index of prod. string descriptor */
		0x00,
		/* Index of S.N.  string descriptor */
		0x00,
		/* Number of possible configurations */
		0x01
	}, {
		/* Configuration 1 descriptor */ 
		{
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_configuration),
			/* CONFIGURATION descriptor type */
			USB_DESCRIPTOR_CONFIGURATION,
			/* Total length of data returned for this configuration */
			sizeof(struct usb_descriptor_set_cdc),
			/* Number of interfaces for this configuration */
			2,
			/* Value to use as an argument for the 
			   Set Configuration request to select this configuration */
			1,
			/* Index of string descriptor describing this configuration */
			0,
			/* Configuration characteristics - attributes */
			USB_CONFIG_SELF_NOWAKEUP,
			/* Maximum power consumption of the device */
			USB_POWER_MA(100)
		},
		/* Communication Class Interface Descriptor Requirement */ 
		{
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_interface),
			/* INTERFACE descriptor type */
			USB_DESCRIPTOR_INTERFACE,
			/* Number of this interface */
			0,
			/* Value used to select this alternate setting */
			0,
			/* Number of endpoints used by this interface 
			   (excluding endpoint zero) */
			1,
			/* Class code */
			CDC_INTERFACE_COMMUNICATION,
			/* Sub-class */
			CDC_ABSTRACT_CONTROL_MODEL,
			/* Protocol code: (V.25ter, Common AT commands)*/
			CDC_PROTOCOL_COMMON_AT_COMMANDS, 
			/* Index of string descriptor describing this interface */
			0 
		}, 
		/* Header Functional Descriptor */ 
		{
			/* Size of this descriptor in bytes */
			sizeof(struct cdc_header_descriptor),
			/* CS_INTERFACE descriptor type */
			CDC_CS_INTERFACE,
			/* Header functional descriptor subtype */
			CDC_HEADER,
			/* USB CDC specification release version */
			CDC1_10
		},
		/* Call Management Functional Descriptor */ 
		{
			/* Size of this descriptor in bytes */
			sizeof(struct cdc_call_management_descriptor),
			/* CS_INTERFACE descriptor type */
			CDC_CS_INTERFACE,
			/* Call management functional descriptor subtype */
			CDC_CALL_MANAGEMENT,
			/* The capabilities that this configuration supports */
			1, /* D1 + D0 */
			/* Interface number of the data class 
			   interface used for call management */
			1
		},
		/* Abstract Control Management Functional Descriptor */ 
		{
			/* Size of this descriptor in bytes */
			sizeof(struct cdc_abstract_control_management_descriptor),
			/* CS_INTERFACE descriptor type */
			CDC_CS_INTERFACE,
			/* Abstract control management functional descriptor subtype */
			CDC_ABSTRACT_CONTROL_MANAGEMENT,
			/* Capabilities supported by this configuration */
			0x06
		},
		/* Union Functional Descriptor */ 
		{ 
			{
				/* Size of this descriptor in bytes */
				sizeof(struct cdc_union_1slave_descriptor),
				/* CS_INTERFACE descriptor type */
				CDC_CS_INTERFACE,
				/* Union functional descriptor subtype */
				CDC_UNION,
				/* The interface number designated as master */
				0
			},
			/* The interface number designated as first slave */
			{ 1 }
		},
		/* Endpoint 3 descriptor */ 
		{
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_endpoint),
			/* ENDPOINT descriptor type */
			USB_DESCRIPTOR_ENDPOINT,
			/* Address of the endpoint on the USB device */
			USB_ENDPOINT_IN + EP_INT,
			/* Endpoint attributes when configured */
			ENDPOINT_TYPE_INTERRUPT,
			/* Maximum packet size this endpoint is capable 
			   of sending or receiving */
			EP_INT_MAX_PKT_SIZE,
			/* Interval for polling endpoint (ms) */
			10
		},
		/* Data Class Interface Descriptor Requirement */ 
		{
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_interface),
			/* INTERFACE descriptor type */
			USB_DESCRIPTOR_INTERFACE,
			/* Number of this interface */
			1,
			/* Value used to select this alternate setting */
			0,
			/* Number of endpoints used by this interface 
			   (excluding endpoint zero) */
			2,
			/* Class code */
			CDC_INTERFACE_DATA,
			/* Sub-class */
			0,
			/* Protocol code */
			0,
			/* Index of string descriptor describing this interface */
			0 
		},
		/* First alternate setting */ 
		/* Endpoint 1 descriptor */ 
		{
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_endpoint),
			/* ENDPOINT descriptor type */
			USB_DESCRIPTOR_ENDPOINT,
			/* Address of the endpoint on the USB device */
			USB_ENDPOINT_OUT + EP_OUT,
			/* Endpoint attributes when configured */
			ENDPOINT_TYPE_BULK,
			/* Maximum packet size this endpoint is capable of 
			   sending or receiving */
			EP_OUT_MAX_PKT_SIZE,
			/* Interval for polling endpoint for data transfers */
			0x00
		},
		/* Endpoint 2 descriptor */ 
		{
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_endpoint),
			/* ENDPOINT descriptor type */
			USB_DESCRIPTOR_ENDPOINT,
			/* Address of the endpoint on the USB device */
			USB_ENDPOINT_IN + EP_IN,
			/* Endpoint attributes when configured */
			ENDPOINT_TYPE_BULK,
			/* Maximum packet size this endpoint is capable of 
			   sending or receiving */
			EP_IN_MAX_PKT_SIZE,
			/* Interval for polling endpoint for data transfers */
			0x0
		}
	}
};

const struct usb_descriptor_endpoint * cdc_acm_ep[4] = {
	NULL,
	&cdc_acm_desc.conf.ep_out,
	&cdc_acm_desc.conf.ep_in,
	&cdc_acm_desc.conf.ep_int
};

