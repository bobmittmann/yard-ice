/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
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
 * @brief YARD-ICE UART console
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/usb-dev.h>


#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

/* TX packet buffer descriptor */
struct stm32f_usb_tx_pktbuf {
	uint32_t addr;
	uint32_t count;
};

/* RX packet buffer descriptor */
struct stm32f_usb_rx_pktbuf {
	uint32_t addr;
	uint32_t count: 9;
	uint32_t num_block: 5;
	uint32_t blsize: 1;
};

/* Generic packet buffer descriptor */
struct stm32f_usb_pktbuf {
	union {
		struct {
			/* single buffer entry */
			struct stm32f_usb_tx_pktbuf tx;
			struct stm32f_usb_rx_pktbuf rx;
		};
		/* double buffer TX */
		struct stm32f_usb_tx_pktbuf dtx[2]; 
		/* double buffer RX */
		struct stm32f_usb_rx_pktbuf drx[2];
	};
};

/* Endpoint control */
struct stm32f_usb_ep {
	struct stm32f_usb_pktbuf * pktbuf;
	uint32_t * rx_buf;
	uint16_t rx_max;
	union {
		usb_class_on_ep_rx_t on_rx;
		usb_class_on_setup_t on_setup;
	};
};

#define USB_DRIVER_EP_MAX 8

/* USB Driver */
struct stm32f_usb_drv {
	struct stm32f_usb_ep ep[USB_DRIVER_EP_MAX];
	uint8_t ep_cnt;
	usb_class_t * cl;
	const struct usb_descriptor_device * dsc_dev;
};

struct usb_desc_set {
	struct usb_descriptor_device device;
	struct usb_descriptor_configuration conf;
};

struct usb_descriptor {
	uint8_t length;              
	uint8_t type;      
} __attribute__((__packed__));

static struct usb_descriptor * 
	usb_desc_next(struct usb_descriptor * dsc) 
{
	return (struct usb_descriptor *)(((uint8_t *)dsc) + dsc->length);  
}

const char * ep_type_nm[] = {
	"CONTROL",
	"ISOCHRONOUS",
	"BULK",
	"INTERRUPT"
};

int usb_desc_device_parse(const struct usb_descriptor_device * dv)
{
	/* make sur the descriptor is a device descriptor */
	if (dv->type != USB_DESCRIPTOR_DEVICE)
		return -1;
	if (dv->length != sizeof(struct usb_descriptor_device))
		return -1;
	dv = (struct usb_descriptor_device *)dv;

	/* Control endpoint 0 max. packet size */
	DCC_LOG1(LOG_TRACE, "EP0 IN/OUT CONTROL maxpacketsize=%d...", 
			 dv->max_pkt_sz0);

	return dv->max_pkt_sz0;
};

int usb_desc_config_parse(const struct usb_descriptor_configuration * cfg)
{
	struct usb_descriptor * dsc;
	struct usb_descriptor * end;

	if (cfg->type != USB_DESCRIPTOR_CONFIGURATION)
		return -1;
	if (cfg->length != sizeof(struct usb_descriptor_configuration))
		return -1;

	/* skip the configuration descriptor */
	dsc = usb_desc_next((struct usb_descriptor *) cfg);
	/* get a pointer to the end of the configuration */
	end = (struct usb_descriptor *)(((uint8_t *)cfg) + cfg->total_length);

	/* search for endpoints */
	while (dsc < end) {
		if (dsc->type == USB_DESCRIPTOR_ENDPOINT) {
			struct usb_descriptor_endpoint * e;
			e = (struct usb_descriptor_endpoint *)dsc;

			if (e->length != sizeof(struct usb_descriptor_endpoint))
				return -1;

			DCC_LOG4(LOG_TRACE, "EP%d %s %s maxpacketsize=%d...", 
					e->endpointaddress & 0x7f, 
					(e->endpointaddress & USB_ENDPOINT_IN) ? "IN" : "OUT", 
					ep_type_nm[e->attributes], 
					e->maxpacketsize);
		}
		dsc = usb_desc_next(dsc);
	}

	return 0;
};



/*
 Upon system and power-on reset, the first operation the application software should perform
 is to provide all required clock signals to the USB peripheral and subsequently de-assert its
 reset signal so to be able to access its registers. The whole initialization sequence is
 hereafter described.
 As a first step application software needs to activate register macrocell clock and de-assert
 macrocell specific reset signal using related control bits provided by device clock
 management logic.
 After that, the analog part of the device related to the USB transceiver must be switched on
 using the PDWN bit in CNTR register, which requires a special handling. This bit is intended
 to switch on the internal voltage references that supply the port transceiver. This circuit has
 a defined startup time (tSTARTUP specified in the datasheet) during which the behavior of the
 USB transceiver is not defined. It is thus necessary to wait this time, after setting the PDWN
 bit in the CNTR register, before removing the reset condition on the USB part (by clearing
 the FRES bit in the CNTR register). Clearing the ISTR register then removes any spurious
 pending interrupt before any other macrocell operation is enabled.
 At system reset, the microcontroller must initialize all required registers and the packet
 buffer description table, to make the USB peripheral able to properly generate interrupts and
 data transfers. All registers not specific to any endpoint must be initialized according to the
 needs of application software (choice of enabled interrupts, chosen address of packet
 buffers, etc.). Then the process continues as for the USB reset case (see further
 paragraph). */

/* configure a RX descriptor */
static int pktbuf_rx_cfg(struct stm32f_usb_rx_pktbuf * rx, 
						 unsigned int addr, unsigned int mxpktsz)
{
	int sz = mxpktsz + 2; /* alloc 2 extra bytes for CRC */

	if (sz < 63) {
		rx->num_block = sz >> 1;
		rx->blsize = 0;
	} else {
		/* round up to a multiple of 32 */
		sz = (sz + 0x1f) & ~0x1f;
		rx->num_block = (sz >> 5) - 1;
		rx->blsize = 1;
	}

	rx->addr = addr;
	rx->count = 0;

	return sz;
}

/* configure a TX descriptor */
static int pktbuf_tx_cfg(struct stm32f_usb_tx_pktbuf * tx, 
						 unsigned int addr, unsigned int mxpktsz)
{
	tx->addr = addr;
	tx->count = 0;

	return mxpktsz;
}

#define USB_FS_DP STM32F_GPIOA, 12
#define USB_FS_DM STM32F_GPIOA, 11
#define USB_FS_VBUS STM32F_GPIOB, 12

void stm32f_usb_io_init(void)
{
	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);

	stm32f_gpio_mode(USB_FS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(USB_FS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	/* PB12: External Pull-up */
	stm32f_gpio_mode(USB_FS_VBUS, INPUT, 0);
	stm32f_gpio_set(USB_FS_VBUS);
}

void stm32f_usb_vbus_connect(bool connect)
{
	if (connect)
		stm32f_gpio_mode(USB_FS_VBUS, OUTPUT, PUSH_PULL | SPEED_LOW);
	else
		stm32f_gpio_mode(USB_FS_VBUS, INPUT, 0);
#if 0
	if (connect)
		stm32f_gpio_mode(USB_FS_VBUS, ALT_FUNC, SPEED_LOW);
	else
		stm32f_gpio_mode(USB_FS_VBUS, INPUT, 0);
#endif
}

void stm32f_usb_power_on(struct stm32f_usb * usb)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	DCC_LOG(LOG_TRACE, "Enabling USB device clock...");

	stm32f_usb_vbus_connect(true);

	rcc->apb1enr |= RCC_USBEN;

	/* USB power ON */
	usb->cntr = USB_FRES;
	/* Wait tSTARTUP time ... */
	udelay(2);
	/* Removing the reset condition */
	usb->cntr = 0;

	/* Removing any spurious pending interrupts */
	usb->istr = 0;

	/* Enable Reset, SOF  and Wakeup interrupts */
	usb->cntr = USB_WKUP | USB_SUSPM | USB_RESETM;

	/* enable Cortex interrupts */
	cm3_irq_enable(STM32F_IRQ_USB_LP);

	cm3_irq_enable(STM32F_IRQ_USB_HP);

}

void stm32f_usb_power_off(struct stm32f_usb * usb)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	usb->cntr = USB_FRES;
	/* Removing any spurious pending interrupts */
	usb->istr = 0;

	stm32f_usb_vbus_connect(false);

	usb->cntr = USB_FRES | USB_PDWN;

	DCC_LOG(LOG_TRACE, "Disabling USB device clock...");
	rcc->apb1enr &= ~RCC_USBEN;
}


#if 0
	/* Configure the endpoints and allocate packet buffers */
	for (i = 0; i < cnt; i++) {
		int mxpktsz = epi[i].mxpktsz;
		uint32_t epr;

		/* set the endpoint descriptor pointer */
		drv->ep[i].desc = desc;
		/* set the endpoint rx/tx/setup callback */
		drv->ep[i].on_rx = epi[i].on_rx;
		drv->ep[i].rx_buf = NULL;
		drv->ep[i].rx_max = 0;
		/* Set EP address */
		epr = USB_EA_SET(epi[i].addr);
		/* Allocate packet buffers */
		switch (epi[i].type)
		{
		case ENDPOINT_TYPE_CONTROL:
			epr |= USB_EP_CONTROL;
			/* allocate single buffers for TX and RX */
			DCC_LOG2(LOG_TRACE, "EP[%d]: CONTROL -> 0x%08x", i, desc);
			stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
			stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
			break;

		case ENDPOINT_TYPE_ISOCHRONOUS:
			epr |= USB_EP_ISO | USB_EP_DBL_BUF;
			/* allocate double buffers for TX or RX */
			DCC_LOG2(LOG_TRACE, "EP[%d]: CONTROL -> 0x%08x", i, desc);
			/* allocate double buffers for TX or RX */
			if ((epi[i].addr & USB_ENDPOINT_IN) == 0) {
				stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
				stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
			} else {
				stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
				stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
			}
			break;

		case ENDPOINT_TYPE_BULK:
			epr |= USB_EP_BULK | USB_EP_DBL_BUF;
			DCC_LOG2(LOG_TRACE, "EP[%d]: BULK -> 0x%08x", i, desc);
			/* allocate double buffers for TX or RX */
			if ((epi[i].addr & USB_ENDPOINT_IN) == 0) {
				stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
				stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
			} else {
				stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
				stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
			}
			break;

		case ENDPOINT_TYPE_INTERRUPT:
			epr |= USB_EP_INTERRUPT;
			/* allocate single buffers for TX or RX */
			DCC_LOG2(LOG_TRACE, "EP[%d]: CONTROL -> 0x%08x", i, desc);
			stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
			stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
			break;
		}

		usb->epr[i] = epr;
	}

	drv->ep_cnt = cnt;


	return 0;
#endif

int stm32f_usb_dev_connect(struct stm32f_usb_drv * drv)
{
	return 0;
}

int stm32f_usb_dev_disconnect(struct stm32f_usb_drv * drv)
{
	return 0;
}

/* Endpoint initialization
The first step to initialize an endpoint is to write appropriate values to the
ADDRn_TX/ADDRn_RX registers so that the USB peripheral finds the data to be
transmitted already available and the data to be received can be buffered. The EP_TYPE
bits in the USB_EPnR register must be set according to the endpoint type, eventually using
the EP_KIND bit to enable any special required feature. On the transmit side, the endpoint
must be enabled using the STAT_TX bits in the USB_EPnR register and COUNTn_TX must
be initialized. For reception, STAT_RX bits must be set to enable reception and
COUNTn_RX must be written with the allocated buffer size using the BL_SIZE and
NUM_BLOCK fields. Unidirectional endpoints, except Isochronous and double-buffered bulk
endpoints, need to initialize only bits and registers related to the supported direction. Once
the transmission and/or reception are enabled, register USB_EPnR and locations
ADDRn_TX/ADDRn_RX, COUNTn_TX/COUNTn_RX (respectively), should not be modified
by the application software, as the hardware can change their value on the fly. When the
data transfer operation is completed, notified by a CTR interrupt event, they can be
accessed again to re-enable a new operation. */

/* This is a bitmask that when applied to the EPR register
 * will NOT change its value except possibly for the address
 */
#define EPR_INVARIANT (USB_CTR_RX | USB_CTR_TX)

void stm32f_usb_dev_ep_init(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb_pktbuf * pktbuf;
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t epr;
	uint32_t ea;
	unsigned int addr;

//	desc = drv->ep[ep_id].desc;

	epr = usb->epr[ep_id];
	ea = USB_EA_GET(epr);
	(void)ea;
	DCC_LOG2(LOG_TRACE, "epr=0x%04x btable=0x%08x", epr, usb->btable);

	pktbuf = (struct stm32f_usb_pktbuf *)STM32F_USB_PKTBUF;

	epr |= USB_EP_CONTROL;
	/* allocate single buffers for TX and RX */
	addr = 0;
	pktbuf_tx_cfg(&pktbuf->tx, addr, 8);
	pktbuf_rx_cfg(&pktbuf->rx, addr, 8);


	/* Set the bits we want to toggle */
	epr = epr ^ (USB_RX_VALID | USB_TX_NAK);

	/* clear the correct transfer bits */
	epr &= ~(USB_CTR_RX | USB_CTR_TX);

	usb->epr[ep_id] = epr;

//	epr = usb->epr[ep_id];
	DCC_LOG2(LOG_TRACE, "epr=0x%04x,0x%04x", epr, usb->epr[ep_id]);
}

/* USB reset (RESET interrupt)
When this event occurs, the USB peripheral is put in the same conditions it is left by the
system reset after the initialization described in the previous paragraph: communication is
disabled in all endpoint registers (the USB peripheral will not respond to any packet). As a
response to the USB reset event, the USB function must be enabled, having as USB
address 0, implementing only the default control endpoint (endpoint address is 0 too). This
is accomplished by setting the Enable Function (EF) bit of the USB_DADDR register and
initializing the EP0R register and its related packet buffers accordingly. During USB
enumeration process, the host assigns a unique address to this device, which must be
written in the ADD[6:0] bits of the USB_DADDR register, and configures any other
necessary endpoint.
When a RESET interrupt is received, the application software is responsible to enable again
the default endpoint of USB function 0 within 10mS from the end of reset sequence which
triggered the interrupt. */

#define PKTBUF_BUF_BASE (8 * 8)

/******************************************************************************/
/*                            Endpoint register                               */
/******************************************************************************/
/* bit positions */
#define EP_CTR_RX      (0x8000) /* EndPoint Correct TRansfer RX */
#define EP_DTOG_RX     (0x4000) /* EndPoint Data TOGGLE RX */
#define EPRX_STAT      (0x3000) /* EndPoint RX STATus bit field */
#define EP_SETUP       (0x0800) /* EndPoint SETUP */
#define EP_T_FIELD     (0x0600) /* EndPoint TYPE */
#define EP_KIND        (0x0100) /* EndPoint KIND */
#define EP_CTR_TX      (0x0080) /* EndPoint Correct TRansfer TX */
#define EP_DTOG_TX     (0x0040) /* EndPoint Data TOGGLE TX */
#define EPTX_STAT      (0x0030) /* EndPoint TX STATus bit field */
#define EPADDR_FIELD   (0x000F) /* EndPoint ADDRess FIELD */

/* EndPoint REGister MASK (no toggle fields) */
#define EPREG_MASK     (EP_CTR_RX|EP_SETUP|EP_T_FIELD|EP_KIND|EP_CTR_TX|EPADDR_FIELD)

/* EP_TYPE[1:0] EndPoint TYPE */
#define EP_TYPE_MASK   (0x0600) /* EndPoint TYPE Mask */
#define EP_BULK        (0x0000) /* EndPoint BULK */
#define EP_CONTROL     (0x0200) /* EndPoint CONTROL */
#define EP_ISOCHRONOUS (0x0400) /* EndPoint ISOCHRONOUS */
#define EP_INTERRUPT   (0x0600) /* EndPoint INTERRUPT */
#define EP_T_MASK      (~EP_T_FIELD & EPREG_MASK)

#define _SetEPType(bEpNum,wType) (_SetENDPOINT(bEpNum,\ ((_GetENDPOINT(bEpNum) & EP_T_MASK) | wType)))

/* STAT_TX[1:0] STATus for TX transfer */
#define EP_TX_DIS      (0x0000) /* EndPoint TX DISabled */
#define EP_TX_STALL    (0x0010) /* EndPoint TX STALLed */
#define EP_TX_NAK      (0x0020) /* EndPoint TX NAKed */
#define EP_TX_VALID    (0x0030) /* EndPoint TX VALID */
#define EPTX_DTOG1     (0x0010) /* EndPoint TX Data TOGgle bit1 */
#define EPTX_DTOG2     (0x0020) /* EndPoint TX Data TOGgle bit2 */
#define EPTX_DTOGMASK  (EPTX_STAT|EPREG_MASK)

/* STAT_RX[1:0] STATus for RX transfer */
#define EP_RX_DIS      (0x0000) /* EndPoint RX DISabled */
#define EP_RX_STALL    (0x1000) /* EndPoint RX STALLed */
#define EP_RX_NAK      (0x2000) /* EndPoint RX NAKed */
#define EP_RX_VALID    (0x3000) /* EndPoint RX VALID */
#define EPRX_DTOG1     (0x1000) /* EndPoint RX Data TOGgle bit1 */
#define EPRX_DTOG2     (0x2000) /* EndPoint RX Data TOGgle bit1 */
#define EPRX_DTOGMASK  (EPRX_STAT|EPREG_MASK)


void set_ep_type(struct stm32f_usb * usb, int ep_id, int type)
{
	uint16_t epr;      

	epr = usb->epr[ep_id] & EP_T_MASK;
	
	epr |= type;

	usb->epr[ep_id] = epr;
}

void set_ep_txstat(struct stm32f_usb * usb, int ep_id, int stat)
{
	uint16_t epr;      

	epr = usb->epr[ep_id] & EPTX_DTOGMASK;

	/* toggle first bit ? */    
	if (stat & EPTX_DTOG1)      
		epr ^= EPTX_DTOG1;       
	/* toggle second bit ?  */         
	if (stat & EPTX_DTOG2)      
		epr ^= EPTX_DTOG2;

	usb->epr[ep_id] = epr;
} 

void stm32f_usb_dev_ep0_init(struct stm32f_usb_drv * drv, int mxpktsz)
{
	struct stm32f_usb_pktbuf * pktbuf;
	struct stm32f_usb * usb = STM32F_USB;
//	uint32_t epr;
	unsigned int addr;
	unsigned int sz;

	/* packet buffer  */
	pktbuf = (struct stm32f_usb_pktbuf *)STM32F_USB_PKTBUF;
	addr = PKTBUF_BUF_BASE;

	DCC_LOG1(LOG_TRACE, "usb=0x%08x...", usb);

	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);
	set_ep_type(usb, 0, EP_CONTROL);
	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);
	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);
	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);
	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);
	set_ep_txstat(usb, 0, EP_TX_NAK);
	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);

	/* allocate single buffers for TX and RX */
//	sz = pktbuf_tx_cfg(&pktbuf[0].tx, addr + mxpktsz, mxpktsz);
//	sz = pktbuf_rx_cfg(&pktbuf[0].rx, addr + sz, mxpktsz);

	/* Set the bits we want to toggle */
//	epr = epr ^ (USB_RX_VALID | USB_TX_NAK);

	/* clear the correct transfer bits */
//	epr &= ~(USB_CTR_RX | USB_CTR_TX);

//	usb->epr[0] = epr;

	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);
}



void stm32f_usb_dev_reset(struct stm32f_usb_drv * drv)
{
	struct stm32f_usb * usb = STM32F_USB;
	int mxpktsz;

	DCC_LOG(LOG_TRACE, "...");

	/* set the btable address */
	usb->btable = 0x000;

	mxpktsz = usb_desc_device_parse(drv->dsc_dev);
		
	/* initializes EP0 */
	stm32f_usb_dev_ep0_init(drv, mxpktsz);

	/* Enable the device and set the address to 0 */
	usb->daddr = USB_EF + 0;

	/* Enable Correct transfer and other interrupts */
	usb->cntr |= USB_CTRM | USB_PMAOVRM | USB_ERRM | USB_SUSPM;
}

void stm32f_usb_dev_enumerate(struct stm32f_usb_drv * drv)
{
	struct stm32f_usb * usb = STM32F_USB;

	(void)usb;
}

/*
 * The CTR_RX event is serviced by first
   determining the transaction type (SETUP bit in the USB_EPnR register); the application
   software must clear the interrupt flag bit and get the number of received bytes reading the
   COUNTn_RX location inside the buffer description table entry related to the endpoint being
 processed. After the received data is processed, the application software should set the
STAT_RX bits to ‘11 (Valid) in the USB_EPnR, enabling further transactions. While the
STAT_RX bits are equal to ‘10 (NAK), any OUT request addressed to that endpoint is
NAKed, indicating a flow control condition: the USB host will retry the transaction until it
succeeds. It is mandatory to execute the sequence of operations in the above mentioned
order to avoid losing the notification of a second OUT transaction addressed to the same
endpoint following immediately the one which triggered the CTR interrupt.
 */


void stm32f_usb_dev_ep_out(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_pktbuf * desc;
	struct stm32f_usb_ep * ep;
	uint32_t epr;
	int cnt;
	int ea;

	epr = usb->epr[ep_id];
	ep = &drv->ep[ep_id];
	ea = USB_EA_GET(epr);
	(void)ea;

	if (epr & USB_EP_DBL_BUF) {
		/* double buffer */
		/* select the descriptor according to the data toggle bit */
		desc = &ep->pktbuf[(epr & USB_DTOG_RX) ? 0: 1];
	} else {
		/* single buffer */
		/* select the RX descriptor */
		desc = &ep->pktbuf[1];
	}

	/* Data received */
	cnt = desc->rx.count;

	if (ep->rx_buf != NULL) {
		uint16_t * src;
		uint16_t * dst;
		int i;
		/* copy data to destination buffer */
		dst = (uint16_t *)ep->rx_buf;
		src = (uint16_t *)STM32F_USB_PKTBUF + (desc->rx.addr / 2);
		for (i = 0; i < (cnt + 1) / 2; i++)
			dst[i] = src[i];
	}

	if (epr & USB_SETUP) {
		DCC_LOG2(LOG_TRACE, "SETUP EP%d, cnt=%d", ea, cnt);
		/* call class endpoint callback */
		ep->on_setup(drv->cl, ep->rx_buf, cnt);
	} else {
		DCC_LOG2(LOG_TRACE, "OUT EP%d, cnt=%d", ea, cnt);
		/* call class endpoint callback */
		ep->on_rx(drv->cl, ep->rx_buf, cnt);
	}

	/* Set the endpoint as valid to continue receiving */
	epr = epr ^ USB_RX_VALID; /* adjust the bits we want to toggle */
	epr &= ~(USB_CTR_RX); 	/* clear the CTR_RX bit */
	usb->epr[ep_id] = epr;
}

void stm32f_usb_dev_ep_in(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;

	(void)usb;

	DCC_LOG1(LOG_TRACE, "IN %d", ep_id);
}

/* prepares to receive */
int stm32f_usb_ep_rx_setup(struct stm32f_usb_drv * drv, int ep_id,
		uint32_t * buf, unsigned int len)
{
	struct stm32f_usb_ep * ep;

	ep = &drv->ep[ep_id];
	ep->rx_buf = buf;

	return 0;
}


void stm32f_usb_dev_ep0_setup(struct stm32f_usb_drv * drv,
		struct stm32f_usb_pktbuf * desc)
{
	struct stm32f_usb * usb = STM32F_USB;

	(void)usb;

	DCC_LOG(LOG_TRACE, "SETUP 0");
}

void stm32f_usb_dev_ep0_out(struct stm32f_usb_drv * drv,
		struct stm32f_usb_pktbuf * desc)
{
	struct stm32f_usb * usb = STM32F_USB;

	(void)usb;

	DCC_LOG(LOG_TRACE, "OUT 0");
}

void stm32f_usb_dev_ep0_in(struct stm32f_usb_drv * drv,
		struct stm32f_usb_pktbuf * desc)
{
	struct stm32f_usb * usb = STM32F_USB;

	(void)usb;

	DCC_LOG(LOG_TRACE, "IN 0");
}

/*
 * Arguments:
 *
 *   drv: pointer to the driver runtime structure.
 *
 *   ep: list of endpoint configuration information. The first item on this list
 *       must be the endpoint 0.
 *
 *  cnt: number of items in the list.
 */

int stm32f_usb_dev_init(struct stm32f_usb_drv * drv, usb_class_t * cl,
		const struct usb_descriptor_device * dsc)
{
	struct stm32f_usb * usb = STM32F_USB;
//	uint32_t stack = STM32F_USB_PKTBUF_SIZE;
//	struct stm32f_usb_pktbuf * desc;
	
//	desc = (struct stm32f_usb_pktbuf *)STM32F_USB_PKTBUF;
	drv->cl = cl;
	drv->dsc_dev = dsc;

	/* Initialize IO pins */
	stm32f_usb_io_init();

	stm32f_usb_power_off(usb);

	stm32f_usb_power_on(usb);

	return 0;
}











/* Private USB device driver data */
struct stm32f_usb_drv stm32f_usb_drv0;


/* USB high priority ISR */
void stm32f_can1_tx_usb_hp_isr(void)
{
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t sr;

	sr = usb->istr;
	(void)sr;
}


/* USB low priority ISR */
void stm32f_can1_rx0_usb_lp_isr(void)
{
	struct stm32f_usb_drv * drv = &stm32f_usb_drv0;
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t sr;

	sr = usb->istr;
	(void)sr;

	if (sr & USB_SOF) {
		DCC_LOG(LOG_TRACE, "SOF");
	}

	if (sr & USB_RESET) {
		DCC_LOG(LOG_TRACE, "RESET");
		usb->istr = ~USB_RESET;
		stm32f_usb_dev_reset(drv);
	}

	if (sr & USB_SUSP) {
		DCC_LOG(LOG_TRACE, "SUSP");
	}

	if (sr & USB_WKUP) {
		DCC_LOG(LOG_TRACE, "WKUP");
	}

	if (sr & USB_ERR) {
		DCC_LOG(LOG_TRACE, "ERR");
	}

	if (sr & USB_PMAOVR) {
		DCC_LOG(LOG_TRACE, "PMAOVR");
	}

	if (sr & USB_CTR) {
		int ep_id;

		ep_id = USB_EP_ID_GET(sr);

		DCC_LOG1(LOG_TRACE, "CTR ep_id=%d", ep_id);
		if (sr & USB_DIR)
			stm32f_usb_dev_ep_out(drv, ep_id);
		else
			stm32f_usb_dev_ep_in(drv, ep_id);
	}

	/* clear interrupts */
	usb->istr = ~sr;
}

/* USB device operations */
const struct usb_dev_ops stm32f_usb_ops = {
	.dev_init = (usb_dev_init_t)stm32f_usb_dev_init,
	.connect = (usb_dev_connect_t)stm32f_usb_dev_connect,
	.disconnect = (usb_dev_disconnect_t)stm32f_usb_dev_disconnect,
	.ep_rx_setup = (usb_dev_ep_rx_setup_t)stm32f_usb_ep_rx_setup
};

/* USB device driver */
const struct usb_dev stm32f_usb_dev = {
	.priv = (void *)&stm32f_usb_drv0,
	.op = &stm32f_usb_ops
};

