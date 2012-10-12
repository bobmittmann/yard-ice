/* 
 * File:	 usb-test.c
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


#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/mii.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/usb.h>
#include <sys/cdc.h>

int hexdump32(uint32_t addr, int count);

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
	struct cdc_line_coding line;
} __attribute__((__packed__));

#define EP0 0
//#define EP0_FIFO_SIZE 8
#define EP0_FIFO_SIZE 64

#define EP_OUT 1
#define EP_OUT_SIZE 64

#define EP_IN  2
#define EP_IN_SIZE 64 

#define EP_INT 3
#define EP_INT_SIZE 64

#define RX_FIFO_SIZE 128
#define TX0_FIFO_SIZE 64
#define TX1_FIFO_SIZE 128
#define TX2_FIFO_SIZE 0
#define TX3_FIFO_SIZE 0

#define DFIFIO_ADDR (STM32F_BASE_OTG_FS +  0x20000)
#define DFIFIO (uint32_t *)DFIFIO_ADDR

const struct cdc_desc_set cdc_desc = {
	{
		/* Size of this descriptor in bytes */
		sizeof(struct usb_descriptor_device),
			/* DEVICE descriptor type */
			USB_DESCRIPTOR_DEVICE,
			/* USB specification release number */
			USB2_00,
//			USB1_10,
			/* Class code */
			USB_CLASS_COMMUNICATION,
			/* Subclass code */
			0x00,
			/* Protocol code */
			0x00,
			/* Control endpoint 0 max. packet size */
			EP0_FIFO_SIZE,
			/* Vendor ID */
			USB_VENDOR_ATMEL,
			/* Product ID */
			0x6119,
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
			EP_INT_SIZE,
			/* Interval for polling endpoint (ms) */
			1
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
			EP_OUT_SIZE,
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
			EP_IN_SIZE,
			/* Interval for polling endpoint for data transfers */
			0x0
		}
	}, {
		115200,
		0,
		0,
		8
	} 
};
#if 0
struct usb_cdc {
	uint8_t rx_bank: 1;
	uint8_t set_addr: 1;
	volatile uint8_t dte: 1;
	volatile uint8_t cd: 1;
	volatile uint8_t lc: 1;

	uint16_t line_coding;

	volatile uint8_t ping;
	volatile uint8_t pong;

	uint8_t addr;

	uint8_t rx_count;
	/* descriptor buffer pointers */
	uint8_t tx_pos;
	uint8_t tx_rem;
};
#endif

struct usb_dev {
	struct stm32f_otg_fs * otg_fs;

	uint8_t rx_count;
	/* ep0 tx info */
	uint8_t tx_rem;
	uint8_t tx_len;
	uint8_t * tx_ptr;
	
	uint32_t setup[2];
};

#define OTG_FS_DP PA, 12
#define OTG_FS_DM PA, 11
#define OTG_FS_VBUS PA, 9
#define OTG_FS_ID PA, 10

static void core_reset(struct stm32f_otg_fs * otg_fs)
{
	/* Wait for AHB master IDLE state. */
	while (!(otg_fs->grstctl & OTG_FS_AHBIDL)) {
		udelay(3);
	}

	/* Core Soft Reset */
	otg_fs->grstctl = OTG_FS_CSRST;
	do {
		udelay(3);
	} while (otg_fs->grstctl & OTG_FS_CSRST);

	/* Wait for 3 PHY Clocks*/
	udelay(3);

	/* AHB configuration */
	otg_fs->gahbcfg = OTG_FS_TXFELVL | OTG_FS_GINTMSK; 
}

static void txfifo_flush(struct stm32f_otg_fs * otg_fs, int num)
{
	otg_fs->grstctl = OTG_FS_TXFFLSH | OTG_FS_TXFIFO_SET(num);
	do {
		udelay(3);
	} while (otg_fs->grstctl & OTG_FS_TXFFLSH);
	/* Wait for 3 PHY Clocks*/
	udelay(3);
}


void usb_device_init(struct usb_dev * usb)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	uint32_t depctl;
	int i;

	usb->otg_fs = otg_fs;
	usb->tx_ptr = NULL;
	usb->tx_rem = 0;


	printf(" - Enabling USB FS clock...\n");
	rcc->ahb2enr |= RCC_OTGFSEN;

	/* - Force device mode 
	   - Full Speed serial transceiver select */
	otg_fs->gusbcfg = OTG_FS_TRDT_SET(5) | OTG_FS_FDMOD | OTG_FS_PHYSEL;

	/* Reset after a PHY select and set Device mode */
	core_reset(otg_fs);

	/* Restart the Phy Clock */
	otg_fs->pcgcctl = 0;

	/* Flush the FIFOs */
//	USB_OTG_FlushTxFifo(pdev , 0x10); 
//	USB_OTG_FlushRxFifo(pdev);

	/* Clear all pending Device Interrupts */
	otg_fs->diepmsk = 0;
	otg_fs->doepmsk = 0;
	otg_fs->daint = 0xffffffff;
	otg_fs->daintmsk = 0;
	for (i = 0; i < 4; i++) {
		depctl = otg_fs->inep[i].diepctl;
		depctl = (depctl & OTG_FS_EPENA) ? OTG_FS_EPDIS | OTG_FS_SNAK : 0;
		otg_fs->inep[i].diepctl = depctl;
		otg_fs->inep[i].dieptsiz = 0;
		otg_fs->inep[i].diepint = 0xff;
	}
	for (i = 0; i < 4; i++) {
		depctl = otg_fs->outep[i].doepctl;
		depctl = (depctl & OTG_FS_EPENA) ? OTG_FS_EPDIS | OTG_FS_SNAK : 0;
		otg_fs->outep[i].doepctl = depctl;
		otg_fs->outep[i].doeptsiz = 0;
		otg_fs->outep[i].doepint = 0xff;
	}

	/* Enable wakeup interrupt */
	otg_fs->gintmsk = OTG_FS_WUIM;

	/* Enable Cortex interrupts */
	cm3_irq_enable(STM32F_IRQ_OTG_FS);

	/* The application must perform the following steps to initialize the 
	   core as a device on power-up or after a mode change from host to device.
	1. Program the following fields in the OTG_FS_DCFG register:
	– Device speed
	- Non-zero-length status OUT handshake */
	otg_fs->dcfg = OTG_FS_PFIVL_80 | OTG_FS_DSPD_FULL;

	/* 2. Program the OTG_FS_GINTMSK register to unmask the 
	   following interrupts:
	   – USB reset
	   – Enumeration done
	   – Early suspend
	   – USB suspend
	   – SOF */
	otg_fs->gintmsk |= OTG_FS_USBRSTM | OTG_FS_ENUMDNEM | OTG_FS_ESUSPM | 
		OTG_FS_USBSUSPM; //| OTG_FS_SOFM;

	/* 3. Program the VBUSBSEN bit in the OTG_FS_GCCFG register to enable VBUS 
	   sensing in “B” device mode and supply the 5 volts across the pull-up 
	   resistor on the DP line. */
	otg_fs->gccfg = OTG_FS_VBUSBSEN | OTG_FS_PWRDWN;

	/* 4. Wait for the USBRST interrupt in OTG_FS_GINTSTS. It indicates that 
	   a reset has been detected on the USB that lasts for about 10 ms on 
	   receiving this interrupt. Wait for the ENUMDNE interrupt in 
	   OTG_FS_GINTSTS.
	   This interrupt indicates the end of reset on the USB. On receiving this 
	   interrupt, the application must read the OTG_FS_DSTS register to 
	   determine the enumeration speed and perform the steps listed in 
	   Endpoint initialization on enumeration completion on page 1035.
	   At this point, the device is ready to accept SOF packets and perform 
	   control transfers on control endpoint 0. */
}

void usb_on_reset(struct stm32f_otg_fs * otg_fs)
{
	uint32_t addr;
	uint32_t siz;
	int i;

	/* Clear the Remote Wake-up Signaling */
	otg_fs->dctl &= ~OTG_FS_RWUSIG;

	/* Flush the Tx FIFO */
	txfifo_flush(otg_fs, 0);

	/* Clear all pending interrupts */
	otg_fs->daint = 0xffffffff;
	for (i = 0; i < 4; i++) {
		otg_fs->inep[i].diepint = 0xff;
		otg_fs->outep[i].doepint = 0xff;
	}
	
	/* Reset Device Address */
	otg_fs->dcfg &= ~OTG_FS_DAD_MSK;

	/* Endpoint initialization on USB reset */

	/* 1. Set the NAK bit for all OUT endpoints
	   – SNAK = 1 in OTG_FS_DOEPCTLx (for all OUT endpoints) */
	otg_fs->outep[EP0].doepctl = OTG_FS_SNAK;
	otg_fs->outep[EP_OUT].doepctl = OTG_FS_SNAK;

	/* 2. Unmask the following interrupt bits
	   – INEP0 = 1 in OTG_FS_DAINTMSK (control 0 IN endpoint)
	   – OUTEP0 = 1 in OTG_FS_DAINTMSK (control 0 OUT endpoint)
	   – STUP = 1 in DOEPMSK
	   – XFRC = 1 in DOEPMSK
	   – XFRC = 1 in DIEPMSK
	   – TOC = 1 in DIEPMSK */
	otg_fs->daintmsk = OTG_FS_IEPM0 | OTG_FS_OEPM0;
	otg_fs->doepmsk = OTG_FS_STUPM | OTG_FS_XFRCM | OTG_FS_EPDM;
	otg_fs->diepmsk = OTG_FS_XFRCM | OTG_FS_TOM | OTG_FS_EPDM;

	/* 3. Set up the Data FIFO RAM for each of the FIFOs
	   – Program the OTG_FS_GRXFSIZ register, to be able to receive 
	   control OUT data and setup data. If thresholding is not enabled, 
	   at a minimum, this must be equal to 1 max packet size of 
	   control endpoint 0 + 2 Words (for the status of the control OUT 
	   data packet) + 10 Words (for setup packets).
	   - Program the OTG_FS_TX0FSIZ register (depending on the FIFO number 
	   chosen) to be able to transmit control IN data. At a minimum, this 
	   must be equal to 1 max packet size of control endpoint 0. */

	/* EP0 and EP2 RX fifo memory allocation */
	addr = 0;
	siz = RX_FIFO_SIZE;
	otg_fs->grxfsiz = siz;

	/* EP0 TX fifo memory allocation */
	addr += siz;
	siz = TX0_FIFO_SIZE;
	otg_fs->dieptxf0 = OTG_FS_TX0FD_SET(siz) | OTG_FS_TX0FSA_SET(addr);

	/* EP1 TX fifo memory allocation */
	addr += siz;
	siz = TX1_FIFO_SIZE;
	otg_fs->dieptxf1 = OTG_FS_INEPTXFD_SET(siz) | OTG_FS_INEPTXSA_SET(addr);

	/* EP2 TX fifo memory allocation */
	addr += siz;
	siz = TX2_FIFO_SIZE;
	otg_fs->dieptxf2 = OTG_FS_INEPTXFD_SET(siz) | OTG_FS_INEPTXSA_SET(addr);

	/* EP3 TX fifo memory allocation */
	addr += siz;
	siz = TX3_FIFO_SIZE;
	otg_fs->dieptxf3 = OTG_FS_INEPTXFD_SET(siz) | OTG_FS_INEPTXSA_SET(addr);

	/*  4. Program the following fields in the endpoint-specific registers 
	   for control OUT endpoint 0 to receive a SETUP packet
	   – STUPCNT = 3 in OTG_FS_DOEPTSIZ0 (to receive up to 3 back-to-back 
	   SETUP packets)
	   At this point, all initialization required to receive SETUP packets 
	   is done. */
	otg_fs->outep[0].doeptsiz = OTG_FS_STUPCNT_SET(3) | 
		OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(8 * 3);

	otg_fs->gintmsk |=  OTG_FS_IEPINTM | OTG_FS_OEPINTM | 
		OTG_FS_IISOIXFRM | OTG_FS_IISOOXFRM | OTG_FS_RXFLVLM;

	printf(" - USB reseted.\n");
}

void usb_on_enum_done(struct stm32f_otg_fs * otg_fs)
{
	uint32_t diepctl;
	uint32_t dsts;

	/* Endpoint initialization on enumeration completion 
	   1. On the Enumeration Done interrupt (ENUMDNE in OTG_FS_GINTSTS), 
	   read the OTG_FS_DSTS register to determine the enumeration speed. */
	dsts = otg_fs->dsts;
	
	/* 2. Program the MPSIZ field in OTG_FS_DIEPCTL0 to set the maximum 
	   packet size. This step configures control endpoint 0. The maximum 
	   packet size for a control endpoint depends on the enumeration speed. */

	diepctl = otg_fs->inep[0].diepctl;
	diepctl &= ~OTG_FS_MPSIZ_MSK;
	if ((dsts & OTG_FS_ENUMSPD_MSK) == OTG_FS_ENUMSPD_FULL) { 
		diepctl |= OTG_FS_MPSIZ_64;
		printf(" - USB full speed\n");
	} else {
		diepctl |= OTG_FS_MPSIZ_8;
		printf(" - USB speed ???\n");
	}
	otg_fs->inep[0].diepctl = diepctl;

	/* At this point, the device is ready to receive SOF packets and is 
	   configured to perform control transfers on control endpoint 0. */

	/*  Clear global IN NAK */
	otg_fs->dctl |= OTG_FS_CGINAK;

}

void usb_on_set_addr(struct stm32f_otg_fs * otg_fs, int addr)
{
	uint32_t dcfg;
	/*  Endpoint initialization on SetAddress command */

	/* This section describes what the application must do when it 
	   receives a SetAddress command in a SETUP packet.
	   1. Program the OTG_FS_DCFG register with the device address 
	   received in the SetAddress command */
	dcfg = otg_fs->dcfg; 
	dcfg &= ~OTG_FS_DAD_MSK;
	dcfg |= OTG_FS_DAD_SET(addr);
	otg_fs->dcfg = dcfg;

	/* 2. Program the core to send out a status IN packet */
}

void usb_on_set_configt(struct stm32f_otg_fs * otg_fs, int id)
{
	if (id == 0) {
		/* Diable endpoints interrupts */
		otg_fs->daintmsk &= ~(OTG_FS_IEPM(EP_IN) | OTG_FS_OEPM(EP_OUT));

		/* Disable endpoints */
		otg_fs->outep[EP_OUT].doepctl &= ~OTG_FS_USBAEP;
		otg_fs->outep[EP_OUT].doepctl &= ~OTG_FS_USBAEP;

		otg_fs->outep[EP_IN].doepctl |= OTG_FS_EPDIS | OTG_FS_SNAK;
		return;
	}
	/* Endpoint activation
	   This section describes the steps required to activate a device 
	   endpoint or to configure an existing device endpoint to a 
	   new type.
	   1. Program the characteristics of the required endpoint into 
	   the following fields of the OTG_FS_DIEPCTLx register (for IN or 
	   bidirectional endpoints) or the OTG_FS_DOEPCTLx register (for 
	   OUT or bidirectional endpoints).
	   – Maximum packet size
	   – USB active endpoint = 1
	   – Endpoint start data toggle (for interrupt and bulk endpoints)
	   – Endpoint type
	   – TxFIFO number */
	/* 2. Once the endpoint is activated, the core starts decoding the 
	   tokens addressed to that endpoint and sends out a valid 
	   handshake for each valid token received for the
	   endpoint. */

	/* Activate OUT endpoint */
	otg_fs->outep[EP_OUT].doepctl = OTG_FS_MPSIZ_SET(64) |
		OTG_FS_USBAEP | OTG_FS_SD0PID | OTG_FS_EPTYP_BULK | 
		OTG_FS_TXFNUM_SET(0);

//	otg_fs->outep[EP_OUT].doeptsiz = OTG_FS_STUPCNT_SET(3) | 
//		OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(64);

	/* Activate IN endpoint */
	otg_fs->inep[EP_IN].diepctl = OTG_FS_MPSIZ_SET(64) |
		OTG_FS_USBAEP | OTG_FS_SD0PID | OTG_FS_EPTYP_BULK |
		OTG_FS_TXFNUM_SET(1);

	/* Enable endpoints interrupts */
	otg_fs->daintmsk |= OTG_FS_IEPM(EP_IN) | OTG_FS_OEPM(EP_OUT);

	/* Prepare to receive */
	otg_fs->outep[EP_OUT].doeptsiz = OTG_FS_PKTCNT_SET(1) | 
		OTG_FS_XFRSIZ_SET(64);
	/* EP enable */
	otg_fs->outep[EP_OUT].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;

}

static void usb_ep0_tx(struct usb_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint8_t * cp;
	int free;
	int len;
	int i;
	uint32_t deptsiz;

	free = otg_fs->inep[0].dtxfsts * 4;

	printf(" - free=%d\n", free);
	len = dev->tx_len;
	
	deptsiz = otg_fs->inep[0].dieptsiz;
	printf(" - pkts=%d size=%d\n", OTG_FS_PKTCNT_GET(deptsiz),
		   OTG_FS_XFRSIZ_GET(deptsiz)); 

//	while ((dev->tx_len > 0) && (free >= len)) {
	if ((len > 0) && (free >= len)) {

		printf("Tx: (%d)\n", len);

		cp = dev->tx_ptr;
		for (i = 0; i < len; i += 4) {
			uint32_t data;
			data = cp[0] + (cp[1] << 8) + (cp[2] << 16) + (cp[3] << 24);
			otg_fs->dfifo[0].push = data;
			cp += 4;
		}	

		dev->tx_ptr += len;
		dev->tx_rem -= len;
		dev->tx_len -= len;
	}
}

void usb_ep0_tx_start(struct usb_dev * dev, void * ptr, int len)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t depctl;
	uint32_t deptsiz;

	dev->tx_ptr = (uint8_t *)ptr;
	dev->tx_rem = len;

	depctl = otg_fs->inep[0].diepctl;
	deptsiz = otg_fs->inep[0].dieptsiz;

	if (len > 0) {
		dev->tx_len = MIN(len, EP0_FIFO_SIZE);
		deptsiz = OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(dev->tx_len); 
	} else {
		dev->tx_len = 0;
		deptsiz = OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(0); 
	}
	otg_fs->inep[0].dieptsiz = deptsiz;

	depctl |= OTG_FS_EPENA | OTG_FS_CNAK;
	otg_fs->inep[0].diepctl = depctl; 

	if (len > 0) {
		/* umask FIFO empty interrupt */
		otg_fs->diepempmsk |= (1 << EP0);
	}
}

void usb_on_recv(struct usb_dev * dev, int ep, int len)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t data;
	int i;

	printf(" - Rx: (%d)", len);
	for (i = 0; i < len; i += 4) {
		data = otg_fs->dfifo[ep].pop;
		printf(" %08x", data);
	}
	printf("\n");

	/* Prepare to receive more */
	otg_fs->outep[ep].doeptsiz = OTG_FS_PKTCNT_SET(1) | 
		OTG_FS_XFRSIZ_SET(64);
	/* EP enable */
	otg_fs->outep[ep].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
}

void usb_on_setup(struct usb_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	unsigned int data;
	int req_type;
	int request;
	int value;
	int	index;
	int desc;
	int len;

	req_type = (dev->setup[0] >> 0) & 0xff;
	request = (dev->setup[0] >> 8) & 0xff;
	value = (dev->setup[0] >> 16) & 0xffff;
	index = (dev->setup[1] >> 0) & 0xffff;
	len = (dev->setup[1] >> 16) & 0xffff;

	if (req_type & 0x80) {
		printf("Setup: In\n");
		/* XXX: change direction */
	} else {
		printf("Setup: Out\n");
	}

	/* Handle supported standard device request Cf 
	   Table 9-3 in USB specification Rev 1.1 */

	switch ((request << 8) | req_type) {

	case STD_GET_DESCRIPTOR:
		desc = value >> 8;

		if (desc == USB_DESCRIPTOR_DEVICE) {	
			/* Return Device Descriptor */
			len = MIN(sizeof(struct usb_descriptor_device), len);
			printf("GetDesc: Device: len=%d\n", len);
			usb_ep0_tx_start(dev, (void *)&cdc_desc, len);
			break;
		} 
		
		if (desc == USB_DESCRIPTOR_CONFIGURATION)	{
			/* Return Configuration Descriptor */
			len = MIN(sizeof(struct usb_descriptor_set_cdc), len);
			printf("GetDesc: Config: len=%d\n", len);
			usb_ep0_tx_start(dev, (void *)&cdc_desc.conf, len);
			break;
		} 

		printf("GetDesc: %d ?\n", desc);
		goto stall;

	case STD_SET_ADDRESS:
		printf("SetAddr: %d\n", value);
		usb_on_set_addr(otg_fs, value);
		goto send_zlp;

	case STD_SET_CONFIGURATION:
		printf("SetCfg: %d\n", value);
		usb_on_set_configt(otg_fs, value);
		goto send_zlp;

	case STD_GET_CONFIGURATION:
		printf("GetCfg\n");
		data = 0;
//		data = (udp->glb_stat & UDP_CONFG) ? 1 : 0;
		goto send_word;

	case STD_GET_STATUS_INTERFACE:
		printf("GetStIf\n");
		data = 0;
		goto send_word;

	case STD_GET_STATUS_ZERO:
		printf("GetStZr\n");
		data = 0;
		goto send_word;

	case STD_GET_STATUS_ENDPOINT:
		index &= 0x0f;
		printf("GetStEpt:%d\n", index);
#if 0
		if ((udp->glb_stat & UDP_CONFG) && (index <= 3)) {
			data = (udp->csr[index] & UDP_EPEDS) ? 0 : 1;
			goto send_word;
		} 

		if ((udp->glb_stat & UDP_FADDEN) && (index == 0)) {
			data = (udp->csr[index] & UDP_EPEDS) ? 0 : 1;
			goto send_word;
		}
#endif
		goto stall;

	case STD_SET_FEATURE_INTERFACE:
		printf("SetIf\n");
		goto send_zlp;

	case STD_SET_FEATURE_ENDPOINT:
		printf("SetEpt\n");
		index &= 0x0f;
#if 0
		if ((value == 0) && index && (index <= 3)) {
			udp->csr[index] = 0;
			goto send_zlp;
		}
#endif
		goto stall;

	case STD_CLEAR_FEATURE_ZERO:
		printf("ClrZr\n");
		goto stall;

	case STD_CLEAR_FEATURE_INTERFACE:
		printf("ClrIf\n");
		goto send_zlp;

	case STD_CLEAR_FEATURE_ENDPOINT:
		index &= 0x0f;
		printf("ClrEpt: %d\n", index);
#if 0
		if ((value == 0) && index && (index <= 3)) {
			if (index == EP_OUT)
				udp->csr[EP_OUT] =
					(UDP_EPEDS | UDP_EPTYPE_BULK_OUT);

			else if (index == EP_IN)
				udp->csr[EP_IN] =
					(UDP_EPEDS | UDP_EPTYPE_BULK_IN);

#if USB_CDC_ENABLE_STATE
			else if (index == EP_INT)
				udp->csr[EP_INT] = 
					(UDP_EPEDS | UDP_EPTYPE_INT_IN);
#endif

			goto send_zlp;
		}
#endif
		goto stall;

		/* handle CDC class requests */
	case SET_LINE_CODING:
		printf("CDC SetLn: idx=%d val=%d len=%d ", index, value, len);
		goto send_zlp;

	case GET_LINE_CODING:
		printf("CDC GetLn");
		len = MIN(sizeof(struct cdc_line_coding), len);
		usb_ep0_tx_start(dev, (void *)&cdc_desc.line, len);
		break;

	case SET_CONTROL_LINE_STATE:
		printf("CDC SetCtrl: idx=%d val=%d len=%d ", index, value, len);
		goto send_zlp;

	default:
		printf("CDC t=%x r=%x v=%x i=%d l=%d\n", 
				req_type, request, value, index, len); 
		goto stall;
	}

	return;

send_zlp:
	printf("Send: ZLP\n");
	otg_fs->inep[0].dieptsiz = OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(0); 
	otg_fs->inep[0].diepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
	return;

stall:
	printf("Stall!\n");
	otg_fs->inep[0].diepctl |= OTG_FS_EPDIS | OTG_FS_STALL;
	return;

send_word:
	printf("Send: 0x%04x\n", data);
	dev->tx_rem = 0;
	otg_fs->dfifo[0].push = data;
	otg_fs->inep[0].dieptsiz = OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(2); 
	otg_fs->inep[0].diepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
	return;
}

void usb_on_rxflvl(struct usb_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t grxstsp;
	int ep;
	int len;
	int stat;

	/* 1. On catching an RXFLVL interrupt (OTG_FS_GINTSTS register), 
	   the application must read the Receive status pop 
	   register (OTG_FS_GRXSTSP). */
	grxstsp = otg_fs->grxstsp;

	/* 2. The application can mask the RXFLVL interrupt (in OTG_FS_GINTSTS) 
	   by writing to RXFLVL = 0 (in OTG_FS_GINTMSK), until it has read the 
	   packet from the receive FIFO. */
	otg_fs->gintmsk &= ~OTG_FS_RXFLVLM;

	len = OTG_FS_BCNT_GET(grxstsp);
	ep = OTG_FS_EPNUM_GET(grxstsp);
	stat = OTG_FS_PKTSTS_GET(grxstsp); 
	printf("- RX, ep=%d len=%d status=%d\n", ep, len, stat);

	/* 3. If the received packet’s byte count is not 0, the byte count 
	   amount of data is popped from the receive Data FIFO and stored in 
	   memory. If the received packet byte count is 0, no data is popped 
	   from the receive data FIFO. */

	switch (grxstsp & OTG_FS_PKTSTS_MSK) {
	case OTG_FS_PKTSTS_GOUT_NACK:
		printf("<GOUT_NACK>");
		break;
	case OTG_FS_PKTSTS_DATA_UPDT:
		printf("<DATA_UPDT>");
		if (len) {
			usb_on_recv(dev, ep, len);
		}
		break;
	case OTG_FS_PKTSTS_XFER_COMP:
		printf("<XFER_COMP>");
		break;
	case OTG_FS_PKTSTS_SETUP_COMP:
		printf("<SETUP_COMP>");
		break;
	case OTG_FS_PKTSTS_SETUP_UPDT:
		printf("<SETUP_UPDT>");
	   /* Copy the setup packet received in FIFO into the setup buffer in RAM */
		dev->setup[0] = otg_fs->dfifo[0].pop;
		dev->setup[1] = otg_fs->dfifo[0].pop;
		break;
	}	

	/* 5. After the data payload is popped from the receive FIFO, the 
	   RXFLVL interrupt (OTG_FS_GINTSTS) must be unmasked. */
	otg_fs->gintmsk |= OTG_FS_RXFLVLM;
}

void usb_on_inepint0(struct usb_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t diepint;
	uint32_t msk;

	msk = otg_fs->diepmsk;
	/* add the Transmit FIFO empty bit to the mask */
	msk |= ((otg_fs->diepempmsk >> 0) & 0x1) << 7;
	diepint = otg_fs->inep[0].diepint & msk;

	if (diepint & OTG_FS_TXFE) {
		printf("<TXFE>");
		usb_ep0_tx(dev);
		/* clear interrupt */
		otg_fs->inep[0].diepint = OTG_FS_TXFE;
	}
	if (diepint & OTG_FS_INEPNE) {
		printf("<INEPNE>");
		/* clear interrupt */
		otg_fs->inep[0].diepint = OTG_FS_INEPNE;
	}
	if (diepint & OTG_FS_ITTXFE) {
		printf("<ITTXFE>");
		/* clear interrupt */
		otg_fs->inep[0].diepint = OTG_FS_ITTXFE;
	}
	if (diepint & OTG_FS_TOC) {
		printf("<TOC>");
		/* clear interrupt */
		otg_fs->inep[0].diepint = OTG_FS_TOC;
	}
	if (diepint & OTG_FS_EPDISD) {
		printf("<EPDISD>");
		/* clear interrupt */
		otg_fs->inep[0].diepint = OTG_FS_EPDISD;
	}
	if (diepint & OTG_FS_XFRC) {
		printf("<XFRC>");
		/* mask FIFO empty interrupt */
		otg_fs->diepempmsk &= ~(1 << EP0);
		/* clear interrupt */
		otg_fs->inep[0].diepint = OTG_FS_XFRC;
		/* TX complete */
		if (dev->tx_rem)
			usb_ep0_tx_start(dev, dev->tx_ptr, dev->tx_rem);
		else {
			/* Prepare to receive */
			otg_fs->outep[0].doeptsiz = OTG_FS_STUPCNT_SET(3) | 
				OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(64);
			/* EP enable */
			otg_fs->outep[0].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
		}
	}
}

void usb_on_inepint(struct usb_dev * dev, int id)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t diepint;
	uint32_t msk;

	msk = otg_fs->diepmsk;
	/* add the Transmit FIFO empty bit to the mask */
	msk |= ((otg_fs->diepempmsk >> id) & 0x1) << 7;
	diepint = otg_fs->inep[id].diepint & msk;

	if (diepint & OTG_FS_TXFE) {
		printf("<TXFE>");
		/* clear interrupt */
		otg_fs->inep[id].diepint = OTG_FS_TXFE;
	}
	if (diepint & OTG_FS_INEPNE) {
		printf("<INEPNE>");
		/* clear interrupt */
		otg_fs->inep[id].diepint = OTG_FS_INEPNE;
	}
	if (diepint & OTG_FS_ITTXFE) {
		printf("<ITTXFE>");
		/* clear interrupt */
		otg_fs->inep[id].diepint = OTG_FS_ITTXFE;
	}
	if (diepint & OTG_FS_TOC) {
		printf("<TOC>");
		/* clear interrupt */
		otg_fs->inep[id].diepint = OTG_FS_TOC;
	}
	if (diepint & OTG_FS_EPDISD) {
		printf("<EPDISD>");
		/* clear interrupt */
		otg_fs->inep[id].diepint = OTG_FS_EPDISD;
	}
	if (diepint & OTG_FS_XFRC) {
		printf("<XFRC>");
		/* mask FIFO empty interrupt */
		otg_fs->diepempmsk &= ~(1 << EP0);
		/* clear interrupt */
		otg_fs->inep[id].diepint = OTG_FS_XFRC;
	}
}


void usb_on_oepint0(struct usb_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t doepint;

	doepint = otg_fs->outep[0].doepint & otg_fs->doepmsk;

	if (doepint & OTG_FS_XFRC) {
		printf("<XFRC>"); 
		/* clear interrupt */
		otg_fs->outep[0].doepint = OTG_FS_XFRC;
	}
	if (doepint & OTG_FS_EPDISD) {
		printf("<EPDISD>"); 
		/* clear interrupt */
		otg_fs->outep[0].doepint = OTG_FS_EPDISD;
	}
	if (doepint & OTG_FS_STUP) {
		printf("<STUP>"); 
		usb_on_setup(dev);
		/* clear interrupt */
		otg_fs->outep[0].doepint = OTG_FS_STUP;
	}
}

void usb_on_oepint(struct usb_dev * dev, int id)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t doepint;

	doepint = otg_fs->outep[id].doepint & otg_fs->doepmsk;

	if (doepint & OTG_FS_XFRC) {
		printf("<XFRC>"); 
		/* clear interrupt */
		otg_fs->outep[id].doepint = OTG_FS_XFRC;
	}
	if (doepint & OTG_FS_EPDISD) {
		printf("<EPDISD>"); 
		/* clear interrupt */
		otg_fs->outep[id].doepint = OTG_FS_EPDISD;
	}
	if (doepint & OTG_FS_STUP) {
		printf("<STUP>"); 
		/* clear interrupt */
		otg_fs->outep[id].doepint = OTG_FS_STUP;
	}
}

void usb_on_inep(struct usb_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t ep_intr;

	ep_intr = (otg_fs->daint & otg_fs->daintmsk);

	if (ep_intr & OTG_FS_IEPINT0) {
		printf("{0}");
		usb_on_inepint0(dev);
	};

	if (ep_intr & OTG_FS_IEPINT1) {
		printf("{1}");
		usb_on_inepint(dev, 1);
	};

	if (ep_intr & OTG_FS_IEPINT2) {
		printf("{2}");
		usb_on_inepint(dev, 2);
	};

	if (ep_intr & OTG_FS_IEPINT3) {
		printf("{3}");
		usb_on_inepint(dev, 3);
	};
	return;

}

void usb_on_outep(struct usb_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t ep_intr;

	ep_intr = (otg_fs->daint & otg_fs->daintmsk);

	if (ep_intr & OTG_FS_OEPINT0) {
		printf("{0}"); 
		usb_on_oepint0(dev);
	};

	if (ep_intr & OTG_FS_OEPINT1) {
		printf("{1}"); 
		usb_on_oepint(dev, 1);
	};

	if (ep_intr & OTG_FS_OEPINT2) {
		printf("{2}"); 
		usb_on_oepint(dev, 2);
	};

	if (ep_intr & OTG_FS_OEPINT3) {
		printf("{3}"); 
		usb_on_oepint(dev, 3);
	};
}


struct usb_dev usb_dev;

void stm32f_otg_fs_isr(void)
{
	static int sof = 0;
	struct usb_dev * dev = &usb_dev;
	struct stm32f_otg_fs * otg_fs = dev->otg_fs;
	uint32_t gintsts;

	gintsts = otg_fs->gintsts;

	if (gintsts & OTG_FS_RXFLVL) {
		printf("<RXFLVL>\n"); 
		usb_on_rxflvl(dev);
	}

	if (gintsts & OTG_FS_SOF) {
//		printf("[SOF]"); 
//		printf(">"); 
		sof++;
	}

	if (gintsts & OTG_FS_WKUPINT) {
		printf("<WKUPINT> [ATTACHED]\n"); 
		/* disable resume/wakeup interrupts */
	}

	if (gintsts & OTG_FS_USBRST ) {
		/* end of bus reset */
		printf("<USBRST> [DEFAULT]\n"); 
		usb_on_reset(otg_fs);
		dev->rx_count = 0;
		dev->tx_rem = 0;
	}

	if (gintsts & OTG_FS_ENUMDNE) {
		printf("<ENUMDNE>"); 
		usb_on_enum_done(otg_fs);
	}

	if (gintsts & OTG_FS_ESUSP) {
		printf("<ESUSP>"); 
	}

	if (gintsts & OTG_FS_USBSUSP) {
		printf("<USBSUSP>"); 
	}

	if (gintsts & OTG_FS_IEPINT) {
		printf("<IEPINT>"); 
		usb_on_inep(dev);
	}

	if (gintsts & OTG_FS_OEPINT) {
		printf("<OEPINT>"); 
		usb_on_outep(dev);
	}

	if (gintsts & OTG_FS_IISOIXFR) {
		printf("<IISOIXFR>"); 
	}

	if (gintsts & OTG_FS_INCOMPISOOUT) {
		printf("<INCOMPISOOUT>"); 
	}

	if (gintsts & OTG_FS_MMIS) {
		printf("<MMIS>"); 
	}

	/* clear pending interrupts */
	otg_fs->gintsts = gintsts;
}

void usb_io_init(void)
{
	printf(" - Enabling GPIO clock...\n");
	stm32f_gpio_clock_en(PA);

	printf(" - Configuring GPIO pins...\n");

	stm32f_gpio_af(OTG_FS_DP, GPIO_AF10);
	stm32f_gpio_af(OTG_FS_DM, GPIO_AF10);
	stm32f_gpio_af(OTG_FS_VBUS, GPIO_AF10);
	stm32f_gpio_af(OTG_FS_ID, GPIO_AF10);

	stm32f_gpio_mode(OTG_FS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(OTG_FS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(OTG_FS_VBUS, ALT_FUNC, SPEED_LOW);
	stm32f_gpio_mode(OTG_FS_ID, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
}

void usb_enumaration_wait(struct stm32f_otg_fs * otg_fs)
{
	while (!(otg_fs->gintsts & OTG_FS_ENUMDNE)) {
		printf(".");
		udelay(100000);
	}
	printf(" - Enumaration speed: %d.\n", OTG_FS_ENUMSPD_GET(otg_fs->dsts));
	printf(" - USB Enumeration done.\n");
}

void usb_reset_wait(struct stm32f_otg_fs * otg_fs)
{
	while (!(otg_fs->gintsts & OTG_FS_USBRST)) {
		printf(".");
		udelay(100000);
	}
	printf(" - USB reset done.\n");
}

int main(int argc, char ** argv)
{
	cm3_udelay_calibrate();
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);

	printf("\n");
	printf("-------------------------------------------------------------------------\n");
	printf("- USB test\n");
	printf("-------------------------------------------------------------------------\n");
	printf("\n");

	usb_io_init();
	usb_device_init(&usb_dev);
//	usb_setup();

	for (;;) {
/*		if ((len = eth_recv(frame, 2048)) > 0) {
			printf("+");
			eth_input((uint8_t *)frame, len);
		} */
	}

	return 0;
}


