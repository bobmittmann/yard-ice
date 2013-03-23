/* 
 * Copyright(c) 2009-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file sys/usb.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_USB_H__
#define __SYS_USB_H__

#include <stdint.h>

enum usb_state {
	USB_STATE_ATTACHED,
	USB_STATE_POWERED,
	USB_STATE_DEFAULT,
	USB_STATE_ADDRESS,
	USB_STATE_CONFIGURED,
	USB_STATE_SUSPENDED
};

struct usb_cfg;

/*
           +------------+
           | ATTACHED   |
           +------------+
               ^   |
  hub reset or |   |
  deconfigured |   |
               |   v
           +------------+    +------------+
  power -->| POWERED    |<-->| SUSPENDED  |
  fail     +------------+    +------------+
                   |
                   |
                   | reset
                   v
           +------------+    +------------+
  reset -->| DEFAULT    |<-->| SUSPENDED  |
           +------------+    +------------+
                   |
                   | address 
                   | assigned
                   v
           +------------+    +------------+
           | ADDRESS    |<-->| SUSPENDED  |
           +------------+    +------------+
               ^   |
  device       |   | device
  deconfigured |   | configured
               |   v
           +------------+    +------------+
           | CONFIGURED |<-->| SUSPENDED  |
           +------------+    +------------+
*/

struct usb_request {
	/* Characteristics of the request */
    uint8_t type;
	/* Particular request */
    uint8_t request;
	/* Request-specific parameter */
    uint16_t value;
	/* Request-specific parameter */
    uint16_t index;
	/* Length of data for the data phase */
    uint16_t length;
} __attribute__((__packed__));

/* Standard Device Requests 
   These are the standard request defined for a SETUP transaction. Please refer
   to Section 9.4 of the USB 2.0 specification (usb_20.pdf) for more
   information.  */
enum usb_std_request {
	/* Returns the status for the specified recipient. */
	USB_GET_STATUS = 0x00,
	/* Disables a specific feature of the device */
	USB_CLEAR_FEATURE = 0x01,
	/* Enables a specific feature of the device */
	USB_SET_FEATURE = 0x03,
	/* Sets the device address for subsequent accesses */
	USB_SET_ADDRESS = 0x05,
	/* Returns the specified descriptor if it exists */
	USB_GET_DESCRIPTOR = 0x06,
	/* Updates existing descriptors or creates new descriptors */
	USB_SET_DESCRIPTOR = 0x07,
	/* Returns the current configuration value of the device */
	USB_GET_CONFIGURATION = 0x08,
	/* Sets the configuration of the device */
	USB_SET_CONFIGURATION = 0x09,
	/* Returns the specified alternate setting for an interface */
	USB_GET_INTERFACE = 0x0a,
	/* Selects an alternate setting for the selected interface */
	USB_SET_INTERFACE = 0x0b,
	/* Sets and reports an endpoint synchronization frame */
	USB_SYNCH_FRAME = 0x0c,
};

/* Descriptor Types */
enum usb_descriptor_type {
	/* Device descriptor */
	USB_DESCRIPTOR_DEVICE = 0x01,
	/* Configuration descriptor */
	USB_DESCRIPTOR_CONFIGURATION = 0x02,
	/* String descriptor */
	USB_DESCRIPTOR_STRING = 0x03,
	/* Interface descriptor */
	USB_DESCRIPTOR_INTERFACE = 0x04,
	/* Endpoint descriptor */
	USB_DESCRIPTOR_ENDPOINT = 0x05,
	/* Device qualifier descriptor */
	USB_DESCRIPTOR_DEVICE_QUALIFIER = 0x06,
	/* Other speed configuration descriptor */
	USB_DESCRIPTOR_OTHER_SPEED_CONFIGURATION = 0x07,
	/* Interface power descriptor */
	USB_DESCRIPTOR_INTERFACE_POWER = 0x08
};

/* Standard Feature Selectors */
enum usb_std_selector {
	/* Halt feature of an endpoint */
	USB_ENDPOINT_HALT = 0x00,
	/* Remote wake-up feature of the device */
	USB_DEVICE_REMOTE_WAKEUP = 0x01,
	/* USB test mode */
	USB_TEST_MODE = 0x02
};

/* the receipient of req.type */
enum usb_recipient {
	/* Recipient is the whole device */
	USB_RECIPIENT_DEVICE = 0x00,
	/* Recipient is an interface */
	USB_RECIPIENT_INTERFACE = 0x01,
	/* Recipient is an endpoint */
	USB_RECIPIENT_ENDPOINT = 0x02
};

/* Class Codes */
/* These are the class codes approved by the USB-IF organization. 
   http://www.usb.org/developers/defined_class */
enum usb_class {
	/* Indicates that the class information is determined by the interface 
	   descriptor. */
	USB_CLASS_DEVICE = 0x00,
	/* Audio capable devices */
	USB_CLASS_AUDIO = 0x01,
	/* Communication devices */
	USB_CLASS_COMMUNICATION = 0x02,
	/* Human-interface devices */
	USB_CLASS_HID = 0x03,
	/* Human-interface devices requiring real-time physical feedback */
	USB_CLASS_PHYSICAL = 0x05,
	/* Still image capture devices */
	USB_CLASS_STILL_IMAGING = 0x06,
	/* Printer devices */
	USB_CLASS_PRINTER = 0x07,
	/* Mass-storage devices */
	USB_CLASS_MASS_STORAGE = 0x08,
	/* Hub devices */
	USB_CLASS_HUB = 0x09,
	/* Raw-data communication device */
	USB_CLASS_CDC_DATA = 0x0a,
	/* Smartcards devices */
	USB_CLASS_SMARTCARDS = 0x0b,
	/* Protected content devices */
	USB_CLASS_CONTENT_SECURITY = 0x0d,
	/* Video recording devices */
	USB_CLASS_VIDEO = 0x0e,
	/* Devices that diagnostic devices */
	USB_CLASS_DIAGNOSTIC_DEVICE = 0xdc,
	/* Wireless controller devices */
	USB_CLASS_WIRELESS_CONTROLLER = 0xe0,
	/* Miscellaneous devices */
	USB_CLASS_MISCELLANEOUS = 0xef,
	/* Application-specific class code */
	USB_CLASS_APPLICATION_SPECIFIC = 0xfe,
	/* Vendor-specific class code */
	USB_CLASS_VENDOR_SPECIFIC = 0xff
};

/* Device is bus-powered and does not support remote wakeup */
#define USB_CONFIG_BUS_NOWAKEUP    0x80

/* Device is self-powered and does not support remote wakeup */
#define USB_CONFIG_SELF_NOWAKEUP   0xC0

/* Device is bus-powered and supports remote wakeup */
#define USB_CONFIG_BUS_WAKEUP      0xA0

/* Device is self-powered and supports remote wakeup */
#define USB_CONFIG_SELF_WAKEUP     0xE0

/* Power consumption for the Configuration descriptor */
#define USB_POWER_MA(MA)           (MA >> 1)

/* Defines an OUT endpoint */
#define USB_ENDPOINT_OUT           (0 << 7)

/* Defines an IN endpoint */
#define USB_ENDPOINT_IN            (1 << 7)

/* These are the four possible transfer type values for the bmAttributes
        field of an endpoint descriptor. */
enum enpoint_type {
	/* Defines a CONTROL endpoint */
	ENDPOINT_TYPE_CONTROL = 0x00,
	/* Defines a ISOCHRONOUS endpoint */
	ENDPOINT_TYPE_ISOCHRONOUS = 0x01,
	/* Defines a BULK endpoint */
	ENDPOINT_TYPE_BULK = 0x02,
	/* Defines an INTERRUPT endpoint */
	ENDPOINT_TYPE_INTERRUPT = 0x03
};

struct usb_descriptor_device {
	/* Size of this descriptor in bytes */
	uint8_t length;              
	/* DEVICE descriptor type */
	uint8_t type;      
	/* USB specification release number */
	uint16_t usb_release;
	/* Class code */
	uint8_t dev_class;
	/* Subclass code */
	uint8_t dev_subclass;
	/* Protocol code */
	uint8_t dev_proto;
	/* Control endpoint 0 max. packet size */
	uint8_t max_pkt_sz0;
	/* Vendor ID */
	uint16_t vendor_id;
	/* Product ID */
	uint16_t product_id;
	/* Device release number */
	uint16_t dev_release;
	/* Index of manu. string descriptor */
	uint8_t manufacturer;
	/* Index of prod. string descriptor */
	uint8_t product;
	/* Index of S.N.  string descriptor */
	uint8_t serial_num;
	/* Number of possible configurations */
	uint8_t num_of_conf;
} __attribute__((__packed__));

struct usb_descriptor_configuration {
	/* Size of this descriptor in bytes */
	uint8_t length;
	/* CONFIGURATION descriptor type */
	uint8_t type;
	/* Total length of data returned for this configuration */
	uint16_t total_length;
	/* Number of interfaces for this configuration */
	uint8_t num_interfaces;
	/* Value to use as an argument for the Set Configuration request to */
	/* select this configuration */
	uint8_t configuration_value;
	/* Index of string descriptor describing this configuration */
	uint8_t configuration;
	/* Configuration characteristics */
	uint8_t attributes;
	/* Maximum power consumption of the device */
	uint8_t max_power;
} __attribute__((__packed__));

/* standard interface descriptor. Used to describe a specific interface
   of a configuration.
   see usb_20.pdf - Section 9.6.5 */
struct usb_descriptor_interface {
	/* Size of this descriptor in bytes */
	uint8_t length;
	/* INTERFACE descriptor type */
	uint8_t type;
	/* Number of this interface */
	uint8_t number;
	/* Value used to select this alternate setting */
	uint8_t alt_setting;
	/* Number of endpoints used by this interface (excluding endpoint zero) */
	uint8_t num_endpoints;
	/* Class code */
	uint8_t ceclass;
	/* Sub-class */
	uint8_t esubclass;
	/* Protocol code */
	uint8_t protocol;
	/* Index of string descriptor describing this interface */
	uint8_t interface;
} __attribute__((__packed__));

/* standard endpoint descriptor. It contains the necessary information 
   for the host to determine the bandwidth required by the endpoint.
   see usb_20.pdf - Section 9.6.6 */
struct usb_descriptor_endpoint {
	/* Size of this descriptor in bytes */
	uint8_t length;
	/* ENDPOINT descriptor type */
	uint8_t type;
	/* Address of the endpoint on the USB device described by this descriptor */
	uint8_t endpointaddress;
	/* Endpoint attributes when configured */
	uint8_t attributes;
	/* Maximum packet size this endpoint is capable of sending or receiving */
	uint16_t maxpacketsize;
	/* Interval for polling endpoint for data transfers */
	uint8_t interval;
} __attribute__((__packed__));

/* device qualifier structure provide information on a high-speed
   capable device if the device was operating at the other speed.
   see usb_20.pdf - Section 9.6.2 */
struct usb_descriptor_device_qualifier {
	/* Size of this descriptor in bytes */
	uint8_t length;
	/* DEVICE_QUALIFIER descriptor type */
	uint8_t descriptortype;
	/* USB specification release number */
	uint16_t usb;
	/* Class code */
	uint8_t deviceclass;
	/* Sub-class code */
	uint8_t devicesubclass;
	/* Protocol code */
	uint8_t deviceprotocol;
	/* Control endpoint 0 max. packet size */
	uint8_t maxpacketsize0;
	/* Number of possible configurations */
	uint8_t numconfigurations;
	/* Reserved for future use, must be 0 */
	uint8_t reserved;
} __attribute__((__packed__));

/* represents the string descriptor zero, 
   used to specify the languages supported by the device. This 
   structure only define one language ID. 
   see usb_20.pdf - Section 9.6.7 - Table 9.15 */

struct usb_language_id {
	/* Size of this descriptor in bytes */
	uint8_t length;
	/* STRING descriptor type */
	uint8_t type;
	/* LANGID code zero */
	uint16_t langid;
} __attribute__((__packed__));

/* USB 2.0 specification code */
#define USB2_00                     0x0200
#define USB1_10                     0x0110

/* ATMEL Vendor ID */
#define USB_VENDOR_ATMEL            0x03EB
#define USB_VENDOR_ST               0x0483


/* USB standard request code */
#define STD_GET_STATUS_ZERO           0x0080
#define STD_GET_STATUS_INTERFACE      0x0081
#define STD_GET_STATUS_ENDPOINT       0x0082

#define STD_CLEAR_FEATURE_ZERO        0x0100
#define STD_CLEAR_FEATURE_INTERFACE   0x0101
#define STD_CLEAR_FEATURE_ENDPOINT    0x0102

#define STD_SET_FEATURE_ZERO          0x0300
#define STD_SET_FEATURE_INTERFACE     0x0301
#define STD_SET_FEATURE_ENDPOINT      0x0302

#define STD_SET_ADDRESS               0x0500
#define STD_GET_DESCRIPTOR            0x0680
#define STD_SET_DESCRIPTOR            0x0700
#define STD_GET_CONFIGURATION         0x0880
#define STD_SET_CONFIGURATION         0x0900
#define STD_GET_INTERFACE             0x0A81
#define STD_SET_INTERFACE             0x0B01
#define STD_SYNCH_FRAME               0x0C82





#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SYS_USB_H__ */

