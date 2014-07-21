/* 
 * File:	 usb-cdc.c
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
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/dcclog.h>

#include "cdc_acm.h"

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#define OTG_FS_IRQ_LVL IRQ_PRIORITY_REGULAR

#define USB_CDC_ENABLE_STATE 1

#define RX_FIFO_SIZE EP_OUT_FIFO_SIZE
#define TX0_FIFO_SIZE EP0_FIFO_SIZE
#define TX1_FIFO_SIZE 0
#define TX2_FIFO_SIZE EP_IN_FIFO_SIZE
#define TX3_FIFO_SIZE EP_INT_FIFO_SIZE

#define DFIFIO_ADDR (STM32F_BASE_OTG_FS +  0x20000)
#define DFIFIO (uint32_t *)DFIFIO_ADDR

struct ep_tx_ctrl {
	uint8_t ep;
	uint8_t pkts;
	uint16_t max;
	volatile uint32_t len;
	uint8_t * ptr;
};

struct ep_rx_ctrl {
	uint8_t ep;
	uint8_t rem;
	volatile uint32_t len;
	uint32_t data;
	uint32_t tmr;
};


struct usb_cdc {
	/* modem bits */
//	volatile uint8_t dtr: 1;
//	volatile uint8_t rts: 1;
//	volatile uint8_t dcd: 1;
//	volatile uint8_t dsr: 1;

	volatile uint8_t status; /* modem status lines */
	volatile uint8_t control; /* modem control lines */

	uint8_t lsst; /* local (set) serial state */ 
	uint8_t rsst; /* remote (acked) serail state */

	struct cdc_line_coding lc;
};

struct usb_cdc_dev {
	/* class specific block */
	struct usb_cdc cdc;

	volatile uint8_t state;
	
	int8_t rx_ev; /* RX event */
	int8_t tx_ev; /* TX event */

	int8_t tx_lock; /* TX lock */
	int8_t tx_lock_ev; /* TX lock/unlock event */

	int8_t ctrl_ev; /* Control event */
	uint8_t ctrl_rcv; /* control message received count */
	uint8_t ctrl_ack; /* control message acknowledge count */

	/* ep0 tx ctrl */
	struct ep_tx_ctrl ep0_tx;
	/* ep2 tx ctrl */
	struct ep_tx_ctrl ep2_tx;
	/* ep1 rx ctrl */
	struct ep_rx_ctrl ep1_rx;

	uint32_t setup_buf[2];
	uint32_t pkt_buf[4];
};

static void ep_tx_ctrl_init(struct ep_tx_ctrl * ctrl, 
							unsigned int ep, unsigned int max)
{
	ctrl->ep = ep;
	ctrl->max = max;
	ctrl->len = 0;
	ctrl->ptr = NULL;
}

static void ep_rx_ctrl_init(struct ep_rx_ctrl * ctrl, unsigned int ep)
{
	ctrl->ep = ep;
	ctrl->rem = 0;
	ctrl->len = 0;
	ctrl->data = 0;
}

static void cdc_init(struct usb_cdc * cdc)
{
	/* Modem control lines */
	cdc->control = 0;
	/* Modem status lines */
	cdc->status = 0;

	cdc->lsst = 0;
	cdc->rsst = 0;

	/* Data terminal rate in bits per second */
	cdc->lc.dwDTERate = 115200;
	/* Number of stop bits */
	cdc->lc.bCharFormat = 0;
	/* Parity bit type */
	cdc->lc.bParityType = 0;
	/* Number of data bits */
	cdc->lc.bDataBits = 8;
}

#define OTG_FS_DP STM32F_GPIOA, 12
#define OTG_FS_DM STM32F_GPIOA, 11
#define OTG_FS_VBUS STM32F_GPIOA, 9
#define OTG_FS_ID STM32F_GPIOA, 10

static void otg_fs_io_init(void)
{
	DCC_LOG(LOG_MSG, "Enabling GPIO clock...");
	stm32f_gpio_clock_en(STM32F_GPIOA);

	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

	stm32f_gpio_af(OTG_FS_DP, GPIO_AF10);
	stm32f_gpio_af(OTG_FS_DM, GPIO_AF10);
	stm32f_gpio_af(OTG_FS_VBUS, GPIO_AF10);
	stm32f_gpio_af(OTG_FS_ID, GPIO_AF10);

	stm32f_gpio_mode(OTG_FS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(OTG_FS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(OTG_FS_VBUS, ALT_FUNC, SPEED_LOW);
	stm32f_gpio_mode(OTG_FS_ID, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
}


#if DEBUG
static const char * const eptyp_nm[] = {
	"CTRL",
	"ISOC",
	"BULK",
	"INT"
};
#endif

void stm32f_otg_fs_ep_enable(struct stm32f_otg_fs * otg_fs, unsigned int addr,
							 unsigned int type, unsigned int mpsiz)
{
	int ep = addr & 0x7f;
	int input = addr & 0x80;
	uint32_t depctl;

	DCC_LOG3(LOG_TRACE, "ep=%d %s %s", ep,
			 input ? "IN" : "OUT", eptyp_nm[type]);

	depctl = input ? otg_fs->inep[ep].diepctl : otg_fs->outep[ep].doepctl;

	depctl &= ~(OTG_FS_MPSIZ_MSK | OTG_FS_EPTYP_MSK | OTG_FS_TXFNUM_MSK);

	/* Endpoint activation
	   This section describes the steps required to activate a device
	   endpoint or to configure an existing device endpoint to a
	   new type.
	   1. Program the characteristics of the required endpoint into
	   the following fields of the OTG_FS_DIEPCTLx register (for IN or
	   bidirectional endpoints) or the OTG_FS_DOEPCTLx register (for
	   OUT or bidirectional endpoints).
	   â€“ Maximum packet size
	   â€“ USB active endpoint = 1
	   â€“ Endpoint start data toggle (for interrupt and bulk endpoints)
	   â€“ Endpoint type
	   â€“ TxFIFO number */

	depctl |= OTG_FS_MPSIZ_SET(mpsiz);
	depctl |= OTG_FS_EPTYP_SET(type);
	depctl |= OTG_FS_SD0PID | OTG_FS_USBAEP;

	/* XXX: mask FIFO empty interrupt, maybe this should
	   be performed elsewhere. */
	otg_fs->diepempmsk &= ~(1 << ep);

	if (input) {
		/* Activate IN endpoint */
		otg_fs->inep[ep].diepctl = depctl | OTG_FS_TXFNUM_SET(ep);

		/* Enable endpoint interrupt */
		otg_fs->daintmsk |= OTG_FS_IEPM(ep);

	} else {
		uint32_t rxfsiz;
		uint32_t pktcnt;

		/* Activate OUT endpoint */
		otg_fs->outep[ep].doepctl = depctl;

		rxfsiz = otg_fs->grxfsiz * 4;
		pktcnt = rxfsiz / mpsiz;

		/* Prepare EP_OUT to receive */
		otg_fs->outep[ep].doeptsiz = OTG_FS_PKTCNT_SET(pktcnt) |
			OTG_FS_XFRSIZ_SET(pktcnt * mpsiz);
		/* EP enable */
		otg_fs->outep[ep].doepctl = depctl | OTG_FS_EPENA | OTG_FS_CNAK;

		/* Enable endpoint interrupt */
		otg_fs->daintmsk |= OTG_FS_OEPM(ep);
	}

	/* 2. Once the endpoint is activated, the core starts decoding the
	   tokens addressed to that endpoint and sends out a valid
	   handshake for each valid token received for the
	   endpoint. */
}

static void otg_fs_fifo_config(struct stm32f_otg_fs * otg_fs)
{
	uint32_t addr;
	uint32_t siz;

	/* EP0 and EP2 RX fifo memory allocation */
	addr = 0;
	siz = RX_FIFO_SIZE;
	otg_fs->grxfsiz = siz / 4;

	/* EP0 TX fifo memory allocation */
	addr += siz;
	siz = TX0_FIFO_SIZE;
	otg_fs->dieptxf0 = OTG_FS_TX0FD_SET(siz / 4) | 
		OTG_FS_TX0FSA_SET(addr / 4);

	/* EP1 TX fifo memory allocation */
	addr += siz;
	siz = TX1_FIFO_SIZE;
	otg_fs->dieptxf1 = OTG_FS_INEPTXFD_SET(siz / 4) | 
		OTG_FS_INEPTXSA_SET(addr / 4);

	/* EP2 TX fifo memory allocation */
	addr += siz;
	siz = TX2_FIFO_SIZE;
	otg_fs->dieptxf2 = OTG_FS_INEPTXFD_SET(siz / 4) | 
		OTG_FS_INEPTXSA_SET(addr / 4);

	/* EP3 TX fifo memory allocation */
	addr += siz;
	siz = TX3_FIFO_SIZE;
	otg_fs->dieptxf3 = OTG_FS_INEPTXFD_SET(siz / 4) | 
		OTG_FS_INEPTXSA_SET(addr / 4);

	DCC_LOG2(LOG_INFO, "   RX: addr=%04x size=%d", 
			 0, otg_fs->grxfsiz / 4);

	DCC_LOG2(LOG_INFO, "TX[0]: addr=%04x size=%d", 
			 OTG_FS_TX0FSA_GET(otg_fs->dieptxf0) * 4, 
			 OTG_FS_TX0FD_GET(otg_fs->dieptxf0) * 4);

	DCC_LOG2(LOG_INFO, "TX[1]: addr=%04x size=%d", 
			 OTG_FS_INEPTXSA_GET(otg_fs->dieptxf1) * 4, 
			 OTG_FS_INEPTXFD_GET(otg_fs->dieptxf1) * 4);

	DCC_LOG2(LOG_INFO, "TX[2]: addr=%04x size=%d", 
			 OTG_FS_INEPTXSA_GET(otg_fs->dieptxf2) * 4, 
			 OTG_FS_INEPTXFD_GET(otg_fs->dieptxf2) * 4);

	DCC_LOG2(LOG_INFO, "TX[3]: addr=%04x size=%d", 
			 OTG_FS_INEPTXSA_GET(otg_fs->dieptxf3) * 4, 
			 OTG_FS_INEPTXFD_GET(otg_fs->dieptxf3) * 4);

}

void usb_device_init(struct usb_cdc_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	struct stm32f_rcc * rcc = STM32F_RCC;

	dev->rx_ev = thinkos_ev_alloc(); 
	dev->tx_ev = thinkos_ev_alloc();
	dev->ctrl_ev = thinkos_ev_alloc();
	dev->ctrl_rcv = 0;
	dev->ctrl_ack = 0;

	dev->tx_lock_ev = thinkos_ev_alloc(); 

	otg_fs_io_init();

	DCC_LOG(LOG_MSG, "Enabling USB FS clock...");
	rcc->ahb2enr |= RCC_OTGFSEN;

#if 0
	{
		int i;

		for (i = 0; i < 320; i++) {
			otg_fs->ram[i] = 0;
		}
	}
#endif

	/* Initialize as a device */
	stm32f_otg_fs_device_init(otg_fs);

	/* Set IRQ priority */
	cm3_irq_pri_set(STM32F_IRQ_OTG_FS, OTG_FS_IRQ_LVL);
	/* Enable Cortex interrupts */
	cm3_irq_enable(STM32F_IRQ_OTG_FS);

	dev->state = USB_STATE_ATTACHED;
	DCC_LOG(LOG_TRACE, "[ATTACHED]");
}

void otg_fs_device_reset(struct stm32f_otg_fs * otg_fs)
{
	int i;

	/* Clear the Remote Wake-up Signaling */
	otg_fs->dctl &= ~OTG_FS_RWUSIG;

	/* Flush the Tx FIFO */
	stm32f_otg_fs_txfifo_flush(otg_fs, 0x10);
	/* Flush the Rx FIFO */
	stm32f_otg_fs_rxfifo_flush(otg_fs);

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
	for (i = 0; i < 4; i++) {
		/* FIXME: we are setting all end points, but not all
		   of them are OUT?? */
		otg_fs->outep[i].doepctl = OTG_FS_SNAK;
	}
	
	/* 2. Unmask the following interrupt bits
	   – INEP0 = 1 in OTG_FS_DAINTMSK (control 0 IN endpoint)
	   – OUTEP0 = 1 in OTG_FS_DAINTMSK (control 0 OUT endpoint)
	   – STUP = 1 in DOEPMSK
	   – XFRC = 1 in DOEPMSK
	   – XFRC = 1 in DIEPMSK
	   – TOC = 1 in DIEPMSK */
	otg_fs->daintmsk = OTG_FS_IEPM0 | OTG_FS_OEPM0;
	otg_fs->doepmsk = OTG_FS_STUPM | OTG_FS_XFRCM | OTG_FS_EPDM;
//	otg_fs->diepmsk = OTG_FS_XFRCM | OTG_FS_TOM | OTG_FS_EPDM;
//	otg_fs->diepmsk = OTG_FS_INEPNEM | OTG_FS_INEPNMM | OTG_FS_ITTXFEMSK |
	otg_fs->diepmsk = OTG_FS_TOM | OTG_FS_EPDM | OTG_FS_XFRCM;

	/* 3. Set up the Data FIFO RAM for each of the FIFOs
	   – Program the OTG_FS_GRXFSIZ register, to be able to receive 
	   control OUT data and setup data. If thresholding is not enabled, 
	   at a minimum, this must be equal to 1 max packet size of 
	   control endpoint 0 + 2 Words (for the status of the control OUT 
	   data packet) + 10 Words (for setup packets).
	   - Program the OTG_FS_TX0FSIZ register (depending on the FIFO number 
	   chosen) to be able to transmit control IN data. At a minimum, this 
	   must be equal to 1 max packet size of control endpoint 0. */

	otg_fs_fifo_config(otg_fs);

	/*  4. Program the following fields in the endpoint-specific registers 
	   for control OUT endpoint 0 to receive a SETUP packet
	   – STUPCNT = 3 in OTG_FS_DOEPTSIZ0 (to receive up to 3 back-to-back 
	   SETUP packets)
	   At this point, all initialization required to receive SETUP packets 
	   is done. */
	otg_fs->outep[0].doeptsiz = OTG_FS_STUPCNT_SET(3) | 
		OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(3 * (8 + 2));

	otg_fs->gintmsk |=  OTG_FS_IEPINTM | OTG_FS_OEPINTM | 
		OTG_FS_IISOIXFRM | OTG_FS_IISOOXFRM | OTG_FS_RXFLVLM;

	/* Initialize EP0 */
	otg_fs->inep[0].diepctl = OTG_FS_TXFNUM_SET(0);
}

void otg_fs_on_enum_done(struct stm32f_otg_fs * otg_fs)
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
		DCC_LOG(LOG_INFO, "USB full speed");
	} else {
		diepctl |= OTG_FS_MPSIZ_8;
		DCC_LOG(LOG_INFO, "USB low speed");
	}
	otg_fs->inep[0].diepctl = diepctl;

	/* At this point, the device is ready to receive SOF packets and is 
	   configured to perform control transfers on control endpoint 0. */

	/*  Clear global IN NAK */
	otg_fs->dctl |= OTG_FS_CGINAK;

}

#if USB_CDC_ENABLE_STATE
bool usb_cdc_state_push(struct stm32f_otg_fs * otg_fs, unsigned int state)
{
	uint32_t buf[4];
	struct cdc_notification * pkt = (struct cdc_notification *)buf;
	uint32_t deptsiz;
	uint32_t xfrsiz;
	uint32_t free;
	int i;

	deptsiz = otg_fs->inep[EP_INT].dieptsiz;
	xfrsiz = OTG_FS_XFRSIZ_GET(deptsiz);
	if (xfrsiz == 0) {
		DCC_LOG(LOG_TRACE, "end of transfer.");
		return true;
	}

	free = otg_fs->inep[EP_INT].dtxfsts * 4;
	if (free < xfrsiz) {
		DCC_LOG(LOG_PANIC, "free < xfrsiz !!!");
		return false;
	}

	/* bmRequestType */
	pkt->bmRequestType = USB_CDC_NOTIFICATION;
	/* bNotification */
	pkt->bNotification = CDC_NOTIFICATION_SERIAL_STATE;
	/* wValue */
	pkt->wValue = 0;
	/* wIndex */
	pkt->wIndex = 1;
	/* wLength */
	pkt->wLength = 2;
	/* data */
	pkt->bData[0] = state;
	pkt->bData[1] = 0;

	/* push into fifo */
	for (i = 0; i < (xfrsiz + 3) / 4; i++)
		otg_fs->dfifo[EP_INT].push = buf[i];

	DCC_LOG1(LOG_TRACE, "Tx: (%d)", xfrsiz);

	return true;
}

bool usb_cdc_state_notify(struct stm32f_otg_fs * otg_fs)
{
	if (stm32f_otg_fs_txf_setup(otg_fs, EP_INT, 
								sizeof(struct cdc_notification) + 2)) {
	/* umask FIFO empty interrupt */
		DCC_LOG(LOG_TRACE, "int unmask....");
		otg_fs->diepempmsk |= (1 << EP_INT);
		return true;
	}

	return false;
}
#endif


void usb_on_recv(struct usb_cdc_dev * dev, int ep, int len)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	uint32_t data;
	int i;

	DCC_LOG1(LOG_TRACE, "Rx: (%d)", len);
	for (i = 0; i < len; i += 4) {
		data = otg_fs->dfifo[ep].pop;
		(void)data;
		DCC_LOG1(LOG_TRACE, " %08x", data);
	}
	DCC_LOG(LOG_TRACE, "");

	/* Prepare to receive more */
	otg_fs->outep[ep].doeptsiz = OTG_FS_PKTCNT_SET(1) | 
		OTG_FS_XFRSIZ_SET(64);
	/* EP enable */
	otg_fs->outep[ep].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
}

static bool otg_fs_ep_tx_start(struct stm32f_otg_fs * otg_fs, 
							   struct ep_tx_ctrl * tx, 
							   const void * buf, int len)
{
	bool ret;

	/* setup the tx info */
	tx->len = len;
	tx->ptr = (uint8_t *)buf;
	/* prepare fifo to transmit */
	if ((ret = stm32f_otg_fs_txf_setup(otg_fs, tx->ep, len))) {
		/* umask FIFO empty interrupt */
		otg_fs->diepempmsk |= (1 << tx->ep);
	}

	return ret;
}

/* Configure EP0 to receive Setup packets */
static void otg_fs_ep0_out_start(struct stm32f_otg_fs * otg_fs)
{
	/* Prepare to receive */
	otg_fs->outep[0].doeptsiz = OTG_FS_STUPCNT_SET(3) | 
		OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(8 * 3);

	/* EP enable */
	otg_fs->outep[0].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
}

/* Send zlp on the control pipe */
static void otg_fs_ep0_zlp_send(struct stm32f_otg_fs * otg_fs)
{
	DCC_LOG(LOG_TRACE, "Send: ZLP");
	otg_fs->inep[0].dieptsiz = OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(0); 
	otg_fs->inep[0].diepctl |= OTG_FS_EPENA | OTG_FS_CNAK;

	otg_fs_ep0_out_start(otg_fs);
}

static void usb_ep0_send_word(struct usb_cdc_dev * dev, unsigned int val)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;

	dev->pkt_buf[0] = val;
	otg_fs_ep_tx_start(otg_fs, &dev->ep0_tx, dev->pkt_buf, 2);
}

static void otg_fs_glb_out_nak(struct stm32f_otg_fs * otg_fs)
{
	DCC_LOG(LOG_TRACE, "Setting global OUT NACK");
/* 1. To stop receiving any kind of data in the receive FIFO, the application 
   must set the Global OUT NAK bit by programming the following field:
   – SGONAK = 1 in OTG_FS_DCTL
   2. Wait for the assertion of the GONAKEFF interrupt in OTG_FS_GINTSTS. 
   When asserted, this interrupt indicates that the core has stopped 
   receiving any type of data except SETUP packets.
   3. The application can receive valid OUT packets after it has set SGONAK 
   in OTG_FS_DCTL and before the core asserts the GONAKEFF interrupt
   (OTG_FS_GINTSTS).
   4. The application can temporarily mask this interrupt by writing to the 
   GINAKEFFM bit in the OTG_FS_GINTMSK register.
   – GINAKEFFM = 0 in the OTG_FS_GINTMSK register
   5. Whenever the application is ready to exit the Global OUT NAK mode, it 
   must clear the SGONAK bit in OTG_FS_DCTL. This also clears the GONAKEFF 
   interrupt (OTG_FS_GINTSTS).
   – OTG_FS_DCTL = 1 in CGONAK
   6. If the application has masked this interrupt earlier, it must be 
   unmasked as follows:
   – GINAKEFFM = 1 in GINTMSK */
	otg_fs->gintmsk |= OTG_FS_GONAKEFFM;
	otg_fs->dctl = OTG_FS_SGONAK;
}

static void otg_fs_ep0_stall(struct stm32f_otg_fs * otg_fs)
{
	DCC_LOG(LOG_WARNING, "EP0 STALL (not implemented)");
	otg_fs_glb_out_nak(otg_fs);
}

/* End point 0 Out */
void usb_on_oepint0(struct usb_cdc_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	uint32_t doepint;
	int req_type;
	int request;
	int value;
	int	index;
	int desc;
	int len;

	doepint = otg_fs->outep[0].doepint & otg_fs->doepmsk;

	/* clear interrupts */
	otg_fs->outep[0].doepint = doepint;

	req_type = (dev->setup_buf[0] >> 0) & 0xff;
	request = (dev->setup_buf[0] >> 8) & 0xff;
	value = (dev->setup_buf[0] >> 16) & 0xffff;
	index = (dev->setup_buf[1] >> 0) & 0xffff;
	len = (dev->setup_buf[1] >> 16) & 0xffff;

	if (doepint & OTG_FS_XFRC) {
		switch ((request << 8) | req_type) {
		case SET_LINE_CODING:
			memcpy(&dev->cdc.lc, dev->pkt_buf, 
				   sizeof(struct cdc_line_coding));
			DCC_LOG1(LOG_TRACE, "dsDTERate=%d", dev->cdc.lc.dwDTERate);
			DCC_LOG1(LOG_TRACE, "bCharFormat=%d", dev->cdc.lc.bCharFormat);
			DCC_LOG1(LOG_TRACE, "bParityType=%d", dev->cdc.lc.bParityType);
			DCC_LOG1(LOG_TRACE, "bDataBits=%d", dev->cdc.lc.bDataBits);
			otg_fs_ep0_zlp_send(otg_fs);
			break;
		}
//		otg_fs_ep0_out_start(otg_fs);
	}

	if (doepint & OTG_FS_EPDISD) {
		DCC_LOG(LOG_TRACE, "[0] <OEPINT> <EPDISD>"); 
	}

	if (doepint & OTG_FS_STUP) {
		struct usb_cdc * cdc = &dev->cdc;

		if (req_type & 0x80) {
			DCC_LOG1(LOG_TRACE, "[0] <OEPINT> <STUP> bmRequestType=%02x "
					"Dev->Host", req_type); 
		} else {
			DCC_LOG1(LOG_TRACE, "[0] <OEPINT> <STUP> bmRequestType=%02x "
					"Host->Dev", req_type); 
			otg_fs_ep0_out_start(otg_fs);
		}

		/* Handle supported standard device request Cf 
		   Table 9-3 in USB specification Rev 1.1 */

		switch ((request << 8) | req_type) {

		case STD_GET_DESCRIPTOR:
			desc = value >> 8;

			if (desc == USB_DESCRIPTOR_DEVICE) {	
				/* Return Device Descriptor */
				len = MIN(sizeof(struct usb_descriptor_device), len);
				DCC_LOG1(LOG_TRACE, "GetDesc: Device: len=%d", len);
				otg_fs_ep_tx_start(otg_fs, &dev->ep0_tx, 
									(void *)&cdc_acm_desc, len);
				break;
			} 

			if (desc == USB_DESCRIPTOR_CONFIGURATION)	{
				/* Return Configuration Descriptor */
				len = MIN(sizeof(struct usb_descriptor_set_cdc), len);
				DCC_LOG1(LOG_TRACE, "GetDesc: Config: len=%d", len);
				otg_fs_ep_tx_start(otg_fs, &dev->ep0_tx, 
									(void *)&cdc_acm_desc.conf, len);
				break;
			} 

			DCC_LOG1(LOG_TRACE, "GetDesc: %d ?", desc);
			otg_fs_ep0_stall(otg_fs);
			break;

		case STD_SET_ADDRESS:
			DCC_LOG1(LOG_TRACE, "SetAddr: %d -------- [ADDRESS]", value);
			stm32f_otg_fs_addr_set(otg_fs, value);
			dev->state = USB_STATE_ADDRESS;
			/* signalize any pending threads */
			__thinkos_ev_raise(dev->rx_ev);
			otg_fs_ep0_zlp_send(otg_fs);
			break;

		case STD_SET_CONFIGURATION: {
			const struct usb_descriptor_endpoint * ep;
			int i;

			DCC_LOG1(LOG_TRACE, "SetCfg: %d", value);

			for (i = 1; i < 4; i++) {
				ep = cdc_acm_ep[i];
				if (value)
					stm32f_otg_fs_ep_enable(otg_fs, ep->endpointaddress,
											ep->attributes,
											ep->maxpacketsize);
				else
					stm32f_otg_fs_ep_disable(otg_fs, ep->endpointaddress);
			}

			dev->state = USB_STATE_CONFIGURED;
			DCC_LOG(LOG_TRACE, "[CONFIGURED]");
			/* signalize any pending threads */
			__thinkos_ev_raise(dev->rx_ev);
			otg_fs_ep0_zlp_send(otg_fs);
			break;
		}

		case STD_GET_CONFIGURATION:
			DCC_LOG(LOG_TRACE, "GetCfg");
			//		data = (udp->glb_stat & UDP_CONFG) ? 1 : 0;
			usb_ep0_send_word(dev, 0);
			break;

		case STD_GET_STATUS_INTERFACE:
			DCC_LOG(LOG_TRACE, "GetStIf");
			usb_ep0_send_word(dev, 0);
			break;

		case STD_GET_STATUS_ZERO:
			DCC_LOG(LOG_TRACE, "GetStZr");
			usb_ep0_send_word(dev, 0);
			break;

		case STD_GET_STATUS_ENDPOINT:
			index &= 0x0f;
			DCC_LOG1(LOG_TRACE, "GetStEpt:%d", index);
#if 0
			if ((udp->glb_stat & UDP_CONFG) && (index <= 3)) {
				data = (udp->csr[index] & UDP_EPEDS) ? 0 : 1;
				usb_ep0_send_word(dev, data);
				break;
			} 

			if ((udp->glb_stat & UDP_FADDEN) && (index == 0)) {
				data = (udp->csr[index] & UDP_EPEDS) ? 0 : 1;
				usb_ep0_send_word(dev, data);
				break;
			}
#endif
			otg_fs_ep0_stall(otg_fs);
			break;

		case STD_SET_FEATURE_INTERFACE:
			DCC_LOG(LOG_TRACE, "SetIf");
			otg_fs_ep0_zlp_send(otg_fs);
			break;

		case STD_SET_FEATURE_ENDPOINT:
			DCC_LOG(LOG_TRACE, "SetEpt");
			index &= 0x0f;
#if 0
			if ((value == 0) && index && (index <= 3)) {
				udp->csr[index] = 0;
				goto zlp_send;
			}
#endif
			otg_fs_ep0_stall(otg_fs);
			break;

		case STD_CLEAR_FEATURE_ZERO:
			DCC_LOG(LOG_TRACE, "ClrZr");
			otg_fs_ep0_stall(otg_fs);
			break;

		case STD_CLEAR_FEATURE_INTERFACE:
			DCC_LOG(LOG_TRACE, "ClrIf");
			otg_fs_ep0_zlp_send(otg_fs);
			break;

		case STD_CLEAR_FEATURE_ENDPOINT:
			index &= 0x0f;
			DCC_LOG1(LOG_TRACE, "ClrEpt: %d", index);
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

			otg_fs_ep0_zlp_send(otg_fs);
				goto zlp_send;
			}
#endif
			otg_fs_ep0_stall(otg_fs);
			break;

		case SET_LINE_CODING:
			/* this will be handled after the data phase */
			DCC_LOG3(LOG_TRACE, "CDC SetLn: idx=%d val=%d len=%d", 
					 index, value, len);
			break;

		case GET_LINE_CODING:
			DCC_LOG(LOG_TRACE, "CDC GetLn");
			len = MIN(sizeof(struct cdc_line_coding), len);
			otg_fs_ep_tx_start(otg_fs, &dev->ep0_tx, 
							   (void *)&cdc->lc, len);
			break;

		case SET_CONTROL_LINE_STATE:
			DCC_LOG3(LOG_TRACE, "CDC SetCtrl: idx=%d val=%d len=%d", 
					 index, value, len);
			cdc->control = value;

			DCC_LOG1(LOG_TRACE, "DTR=%d", (value & CDC_DTE_PRESENT));

#if USB_CDC_ENABLE_STATE
			/* update the local serial state */
			cdc->lsst = (value & CDC_DTE_PRESENT) ? CDC_SERIAL_STATE_RX_CARRIER |
				CDC_SERIAL_STATE_TX_CARRIER : 0;

			/* trigger a local state notification */
			usb_cdc_state_notify(otg_fs);
#endif

			/* there might have threads waiting for
			   modem control line changes (DTR, RTS)
			   wake them up */
			__thinkos_ev_raise(dev->rx_ev);
			__thinkos_ev_raise(dev->tx_ev);
			__thinkos_ev_raise(dev->ctrl_ev);

			otg_fs_ep0_zlp_send(otg_fs);
			break;

		default:
			DCC_LOG5(LOG_TRACE, "CDC t=%x r=%x v=%x i=%d l=%d", 
					 req_type, request, value, index, len); 
			otg_fs_ep0_stall(otg_fs);
			break;
		}
	}
}

struct usb_cdc_dev usb_cdc_dev;

static int otg_fs_isr_cnt = 0;

void stm32f_otg_fs_isr(void)
{
	struct usb_cdc_dev * dev = &usb_cdc_dev;
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	uint32_t gintsts;
	uint32_t ep_intr;

	otg_fs_isr_cnt++;

	gintsts = otg_fs->gintsts & otg_fs->gintmsk;
	
	DCC_LOG1(LOG_MSG, "GINTSTS=0x%08x", gintsts); 

	if (gintsts & OTG_FS_SRQINT) {
		/* Session request/new session detected interrupt */
		DCC_LOG(LOG_TRACE, "<SRQINT>  [POWERED]");
		otg_fs->gintmsk |= OTG_FS_WUIM | OTG_FS_USBRSTM | OTG_FS_ENUMDNEM | 
			OTG_FS_ESUSPM | OTG_FS_USBSUSPM;
	}

	if (gintsts & OTG_FS_PTXFE) {
		DCC_LOG(LOG_TRACE, "<PTXFE>"); 
	}

	if (gintsts & OTG_FS_OTGINT) {
		uint32_t gotgint = otg_fs->gotgint;
		DCC_LOG(LOG_INFO, "<OTGINT>"); 
		if (gotgint & OTG_FS_OTGINT) {
			DCC_LOG(LOG_INFO, "<SEDET>  [ATTACHED]"); 
			otg_fs->gintmsk = OTG_FS_SRQIM | OTG_FS_OTGINT;
		}
		otg_fs->gotgint = gotgint;
	}
	
	if (gintsts & OTG_FS_GONAKEFF) {
		DCC_LOG(LOG_TRACE, "<GONAKEFF>"); 
		otg_fs->gintmsk &= ~OTG_FS_GONAKEFFM;
	}

	if (gintsts & OTG_FS_RXFLVL) {
		uint32_t grxsts;
		int ep_id;
		int len;
		int stat;

		/* 1. On catching an RXFLVL interrupt (OTG_FS_GINTSTS register), 
		   the application must read the Receive status pop 
		   register (OTG_FS_GRXSTSP). */
		/* pop the rx fifo status */
		grxsts = otg_fs->grxstsp;

		ep_id = OTG_FS_EPNUM_GET(grxsts);
		(void)ep_id;
		len = OTG_FS_BCNT_GET(grxsts);
		(void)len;
		stat = OTG_FS_PKTSTS_GET(grxsts); 
		(void)stat;

		DCC_LOG3(LOG_INFO, "[%d] <RXFLVL> len=%d status=%d", ep_id, len, stat);

		if (ep_id == 0) {
			/* 3. If the received packet’s byte count is not 0, the byte count 
			   amount of data is popped from the receive Data FIFO and stored in 
			   memory. If the received packet byte count is 0, no data is popped 
			   from the receive data FIFO. */

			switch (grxsts & OTG_FS_PKTSTS_MSK) {
			case OTG_FS_PKTSTS_GOUT_NACK:
				/* Global OUT NAK (triggers an interrupt) */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <GOUT_NACK>", ep_id);
				break;
			case OTG_FS_PKTSTS_OUT_DATA_UPDT: {
				/* OUT data packet received */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <OUT_DATA_UPDT>", ep_id);
				int i;
				for (i = 0; i < len; i += 4)
					dev->pkt_buf[i] = otg_fs->dfifo[0].pop;
				break;
			}
			case OTG_FS_PKTSTS_OUT_XFER_COMP:
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <OUT_XFER_COMP>", ep_id);
				break;
			case OTG_FS_PKTSTS_SETUP_COMP:
				/* SETUP transaction completed (triggers an interrupt) */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <SETUP_COMP>", ep_id);
				break;
			case OTG_FS_PKTSTS_SETUP_UPDT:
				/* SETUP data packet received */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <SETUP_UPDT>", ep_id);
				if (len != 8) {
					DCC_LOG(LOG_ERROR, "setup data len != 8!");
				}
				/* Copy the received setup packet into the setup buffer 
				   in RAM */
				dev->setup_buf[0] = otg_fs->dfifo[0].pop;
				dev->setup_buf[1] = otg_fs->dfifo[0].pop;
				DCC_LOG2(LOG_TRACE, "SETUP: 0x%08x 0x%08x", 
						 dev->setup_buf[0], dev->setup_buf[1]);
				break;
			}	
		} else if (ep_id == EP_OUT) {
			switch (grxsts & OTG_FS_PKTSTS_MSK) {
			case OTG_FS_PKTSTS_OUT_DATA_UPDT:
				/* OUT data packet received */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <OUT_DATA_UPDT>", ep_id);
				/* 2. The application can mask the RXFLVL interrupt (in 
				   OTG_FS_GINTSTS) by writing to RXFLVL = 0 (in 
				   OTG_FS_GINTMSK), until it has read the packet from 
				   the receive FIFO. */
				otg_fs->gintmsk &= ~OTG_FS_RXFLVLM;
				dev->ep1_rx.len = len;


				/* Enable SOF interrupts */
				otg_fs->gintmsk |=  OTG_FS_SOFM;
				dev->ep1_rx.tmr = 500;

				/* signal any pending threads */
				__thinkos_ev_raise(dev->rx_ev);
				DCC_LOG(LOG_INFO, "__thinkos_ev_raise(RX)");
				break;
			case OTG_FS_PKTSTS_OUT_XFER_COMP:
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <OUT_XFER_COMP>", ep_id);
				/* FIXME: generic code */
				/* Prepare to receive more */
				otg_fs->outep[ep_id].doeptsiz = 
					OTG_FS_PKTCNT_SET(EP_OUT_FIFO_SIZE / EP_OUT_MAX_PKT_SIZE) | 
					OTG_FS_XFRSIZ_SET(EP_OUT_FIFO_SIZE);
				/* EP enable */
				otg_fs->outep[ep_id].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;

				/* Disable SOF interrupts */
				otg_fs->gintmsk &= ~OTG_FS_SOFM;

				break;
			}
		} 

		/* 5. After the data payload is popped from the receive FIFO, the 
		   RXFLVL interrupt (OTG_FS_GINTSTS) must be unmasked. */
		//	otg_fs->gintmsk |= OTG_FS_RXFLVLM;
	}

	if (gintsts & OTG_FS_SOF) {
		DCC_LOG(LOG_MSG, "<SOF>"); 
		if (--dev->ep1_rx.tmr == 0) {
			int i;
			/* Disable SOF interrupts */
			otg_fs->gintmsk &= ~OTG_FS_SOFM;

			DCC_LOG(LOG_TRACE, "RX timeout"); 
			/* pop data from fifo */
			for (i = 0; i < dev->ep1_rx.len; i++) {
				(void)otg_fs->dfifo[EP_OUT].pop;
			}
			dev->ep1_rx.rem = 0;
			dev->ep1_rx.len = 0;
			/* Reenable RX fifo interrupts */
			otg_fs->gintmsk |= OTG_FS_RXFLVLM;
		}
	}

	if (gintsts & OTG_FS_WKUPINT) {
		DCC_LOG(LOG_TRACE, "<WKUPINT>"); 
		/* disable resume/wakeup interrupts */
	}

	if (gintsts & OTG_FS_USBRST ) {
		/* end of bus reset */
		//		DCC_LOG(LOG_TRACE, "<USBRST>"); 
		/* initialize tx buffer */
		ep_tx_ctrl_init(&dev->ep0_tx, EP0, EP0_FIFO_SIZE);
		ep_tx_ctrl_init(&dev->ep2_tx, EP_IN, EP_IN_FIFO_SIZE);
		ep_rx_ctrl_init(&dev->ep1_rx, EP_OUT);
		cdc_init(&dev->cdc);

		otg_fs_device_reset(otg_fs);

		dev->state = USB_STATE_DEFAULT;

		/* signalize any pending threads */
		__thinkos_ev_raise(dev->rx_ev);

		DCC_LOG(LOG_TRACE, "<USBRST> --------------- [DEFAULT]");
	}

	if (gintsts & OTG_FS_ENUMDNE) {
		DCC_LOG(LOG_TRACE, "<ENUMDNE>");
		otg_fs_on_enum_done(otg_fs);
	}

	if (gintsts & OTG_FS_ESUSP) {
		DCC_LOG(LOG_INFO, "<ESUSP>"); 
	}

	if (gintsts & OTG_FS_USBSUSP) {
		DCC_LOG(LOG_INFO, "<USBSUSP>"); 
	}

	if (gintsts & OTG_FS_IEPINT) {
		uint32_t diepmsk;
		uint32_t diepint;
		uint32_t diepempmsk;
		uint32_t msk;

		ep_intr = (otg_fs->daint & otg_fs->daintmsk);
		diepmsk = otg_fs->diepmsk;
		diepempmsk = otg_fs->diepempmsk;

		if (ep_intr & OTG_FS_IEPINT0) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 0) & 0x1) << 7;
			diepint = otg_fs->inep[0].diepint & msk;
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG1(LOG_TRACE, "%d [0] <IEPINT> <XFRC>", otg_fs_isr_cnt);
				if (dev->ep0_tx.len == 0) {
					/* mask FIFO empty interrupt */
					otg_fs->diepempmsk &= ~(1 << dev->ep0_tx.ep);
					/* Prepare to receive */
					otg_fs_ep0_out_start(otg_fs);
				} else {
					stm32f_otg_fs_txf_setup(otg_fs, dev->ep0_tx.ep, 
											dev->ep0_tx.len);
				}
			}
			if (diepint & OTG_FS_TXFE) {
				int n;
				DCC_LOG1(LOG_TRACE, "%d [0] <IEPINT> <TXFE>", otg_fs_isr_cnt);
				n = stm32f_otg_fs_txf_push(otg_fs, dev->ep0_tx.ep, 
										   dev->ep0_tx.ptr);
				if (n >= 0) {
					dev->ep0_tx.ptr += n;
					dev->ep0_tx.len -= n;
				} else {
					DCC_LOG(LOG_WARNING, "stm32f_otg_fs_txf_push() failed!");
				}
			}
			/* clear interrupts */
			otg_fs->inep[0].diepint = diepint;
		}

		if (ep_intr & OTG_FS_IEPINT1) {
			DCC_LOG(LOG_WARNING, "[1] <IEPINT> this should never happen!");
			diepint = otg_fs->inep[1].diepint;
			otg_fs->inep[1].diepint = diepint;
		}

		if (ep_intr & OTG_FS_IEPINT2) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 2) & 0x1) << 7;
			diepint = otg_fs->inep[2].diepint & msk;
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG1(LOG_TRACE, "%d [2] <IEPINT> <XFRC>", otg_fs_isr_cnt);
				if (dev->ep2_tx.len == 0) {
					/* mask FIFO empty interrupt */
//					otg_fs->diepempmsk &= ~(1 << dev->ep2_tx.ep);
					DCC_LOG(LOG_TRACE, "int mask....");
					/* signal blocked thread */
					__thinkos_ev_raise(dev->tx_ev);
				} else {
					stm32f_otg_fs_txf_setup(otg_fs, dev->ep2_tx.ep, 
											dev->ep2_tx.len);
				}
			}
			if (diepint & OTG_FS_TXFE) {
				int n;
				DCC_LOG1(LOG_TRACE, "%d [2] <IEPINT> <TXFE>", otg_fs_isr_cnt);
				n = stm32f_otg_fs_txf_push(otg_fs, dev->ep2_tx.ep, 
										   dev->ep2_tx.ptr);
				if (n >= 0) {
					dev->ep2_tx.ptr += n;
					dev->ep2_tx.len -= n;
				} else {
					/* mask FIFO empty interrupt */
					otg_fs->diepempmsk &= ~(1 << dev->ep2_tx.ep);
					DCC_LOG(LOG_WARNING, "stm32f_otg_fs_txf_push() failed!");
				}
			}
			otg_fs->inep[2].diepint = diepint;
		}

		if (ep_intr & OTG_FS_IEPINT3) {
			struct usb_cdc * cdc = &dev->cdc;
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 3) & 0x1) << 7;
			diepint = otg_fs->inep[3].diepint & msk;
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG1(LOG_TRACE, "%d [3] <IEPINT> <XFRC>", otg_fs_isr_cnt);
				/* mask FIFO empty interrupt */
				otg_fs->diepempmsk &= ~(1 << EP_INT);
				/* update modem signals according to
				   the remote state */
				cdc->status = cdc->rsst;
				/* signal any pending threads */
				__thinkos_ev_raise(dev->tx_ev);
				__thinkos_ev_raise(dev->rx_ev);
				__thinkos_ev_raise(dev->ctrl_ev);
#if USB_CDC_ENABLE_STATE
				if (cdc->rsst != cdc->lsst) {
					/* local state changed, transmit state again */
					DCC_LOG(LOG_WARNING, "local state changed!!");
					/* trigger a local state notification */
					usb_cdc_state_notify(otg_fs);
				}
#endif
			}
			if (diepint & OTG_FS_TXFE) {
				DCC_LOG1(LOG_TRACE, "%d [3] <IEPINT> <TXFE>", otg_fs_isr_cnt);
				if (usb_cdc_state_push(otg_fs, dev->cdc.lsst)) {
					/* We managed to put the state into the transmission 
					   fifo. Update the remote state cache. */
					dev->cdc.rsst = dev->cdc.lsst;
				} else {
					DCC_LOG(LOG_WARNING, "usb_cdc_state_push() failed!");
				}
			}
			otg_fs->inep[3].diepint = diepint;
		}
	}

	if (gintsts & OTG_FS_OEPINT) {

		//		DCC_LOG(LOG_TRACE, "<OEPINT>"); 

		ep_intr = (otg_fs->daint & otg_fs->daintmsk);

		if (ep_intr & OTG_FS_OEPINT0) {
			DCC_LOG(LOG_INFO, "[0] <OEPINT>"); 
			usb_on_oepint0(dev);
		}

		if (ep_intr & OTG_FS_OEPINT1) {
			uint32_t doepint;

			doepint = otg_fs->outep[1].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[1] <OEPINT> <OUT XFRC>"); 
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_TRACE, "[1] <OEPINT> <EPDISD>"); 
			}
			if (doepint & OTG_FS_STUP) {
				DCC_LOG(LOG_TRACE, "[1] <OEPINT> <STUP>"); 
			}
			/* clear interrupts */
			otg_fs->outep[1].doepint = doepint;
		}

		if (ep_intr & OTG_FS_OEPINT2) {
			uint32_t doepint;

			doepint = otg_fs->outep[2].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[2] <OEPINT> <OUT XFRC>"); 
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_TRACE, "[2] <OEPINT> <EPDISD>"); 
			}
			if (doepint & OTG_FS_STUP) {
				DCC_LOG(LOG_TRACE, "[2] <OEPINT> <STUP>"); 
			}
			/* clear interrupts */
			otg_fs->outep[1].doepint = doepint;
		}

		if (ep_intr & OTG_FS_OEPINT3) {
			uint32_t doepint;

			doepint = otg_fs->outep[3].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[3] <OEPINT> <OUT XFRC>"); 
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_TRACE, "[3] <OEPINT> <EPDISD>"); 
			}
			if (doepint & OTG_FS_STUP) {
				DCC_LOG(LOG_TRACE, "[3] <OEPINT> <STUP>"); 
			}

			/* clear interrupts */
			otg_fs->outep[1].doepint = doepint;
		}
	}

	if (gintsts & OTG_FS_IISOIXFR) {
		DCC_LOG(LOG_TRACE, "<IISOIXFR>"); 
	}

	if (gintsts & OTG_FS_INCOMPISOOUT) {
		DCC_LOG(LOG_TRACE, "<INCOMPISOOUT>"); 
	}

	if (gintsts & OTG_FS_MMIS) {
		DCC_LOG(LOG_TRACE, "<MMIS>"); 
	}

	/* clear pending interrupts */
	otg_fs->gintsts = gintsts;
}

void usb_enumaration_wait(struct usb_cdc_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;

	__thinkos_critical_enter();
	while (!(otg_fs->gintsts & OTG_FS_ENUMDNE)) {
		__thinkos_ev_wait(dev->rx_ev);
	}
	__thinkos_critical_exit();

	DCC_LOG1(LOG_TRACE, "Enumaration speed: %d.", 
			 OTG_FS_ENUMSPD_GET(otg_fs->dsts));
	DCC_LOG(LOG_TRACE, "USB Enumeration done.");
}

void usb_reset_wait(struct usb_cdc_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;

	__thinkos_critical_enter();
	while (!(otg_fs->gintsts & OTG_FS_USBRST)) {
		__thinkos_ev_wait(dev->rx_ev);
	}
	__thinkos_critical_exit();
	DCC_LOG(LOG_TRACE, "USB reset done.");
}

void usb_connect(struct usb_cdc_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;

	/* Connect device */
	otg_fs->dctl &= ~OTG_FS_SDIS;
	udelay(3000);
}

void usb_disconnect(struct usb_cdc_dev * dev)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	otg_fs->dctl |= OTG_FS_SDIS;
	udelay(3000);
}


int usb_cdc_write(struct usb_cdc_dev * dev, 
				  const void * buf, unsigned int len)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	int rem;

	/* lock the write call... */
	__thinkos_critical_enter();
	while (dev->tx_lock && (dev->cdc.control & CDC_DTE_PRESENT)) {
		DCC_LOG(LOG_TRACE, "lock wait .....................");
		__thinkos_ev_wait(dev->tx_lock_ev);
		DCC_LOG(LOG_TRACE, "..................... wakeup");
	}
	/* lock */
	dev->tx_lock = 1;
	__thinkos_critical_exit();

	if (!(dev->cdc.control & CDC_DTE_PRESENT)) {
		/* if there is no DTE connected we discard the data */
		DCC_LOG(LOG_WARNING, "no DTE!");
		/* unlock ... */
		dev->tx_lock = 0;
		__thinkos_ev_raise(dev->tx_lock_ev);
		return len;
	}

	/* Start the transmission */
	otg_fs_ep_tx_start(otg_fs, &dev->ep2_tx, buf, len);

	/* Wait until all the data is transfered to the TX queue */
	__thinkos_critical_enter_level(OTG_FS_IRQ_LVL);
	while (((rem = dev->ep2_tx.len) > 0) && 
		   (dev->cdc.control & CDC_DTE_PRESENT)) {
		DCC_LOG(LOG_TRACE, "wait .....................");
		__thinkos_critical_ev_wait(dev->tx_ev, OTG_FS_IRQ_LVL);
		DCC_LOG(LOG_TRACE, "..................... wakeup");
	}
	__thinkos_critical_exit();

	/* unlock */
	dev->tx_lock = 0;
	__thinkos_ev_raise(dev->tx_lock_ev);

	return len - rem;
}

int usb_cdc_read(struct usb_cdc_dev * dev, void * buf, 
				 unsigned int len, unsigned int msec)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	uint8_t * cp = (uint8_t *)buf;
	uint32_t data;
	int rx_len;
	int cnt;
	int rem;
	int i;

	DCC_LOG2(LOG_TRACE, "len=%d msec=%d", len, msec);

	__thinkos_critical_enter();
	while ((dev->state != USB_STATE_CONFIGURED) ||
		   ((rx_len = dev->ep1_rx.len) == 0)) {
		DCC_LOG(LOG_INFO, "wait .....................");
		__thinkos_ev_wait(dev->rx_ev);
		DCC_LOG(LOG_INFO, "..................... wakeup");
	}
	__thinkos_critical_exit();

	DCC_LOG2(LOG_INFO, "rx_len=%d len=%d", rx_len, len);

	/* transfer residual data from buffer */
	if ((rem = dev->ep1_rx.rem) > 0) {
		/* transfer no more than 'len' bytes */
		cnt = MIN(rem, len);
		/* get data from buffer */
		data = dev->ep1_rx.data;
		for (i = 0; i < cnt; i++) {
			cp[i] = data;
			data >>= 8;
		}
		dev->ep1_rx.data = data;
		dev->ep1_rx.rem = rem - cnt;
		rx_len -= cnt;
		len -= cnt;
		cp += cnt;
	} 

	/* transfer data from fifo */
	rem = MIN(rx_len, len);
	while (rem >= 4) {
		/* word trasfer */
		data = otg_fs->dfifo[EP_OUT].pop;
		cp[0] = data;
		cp[1] = data >> 8;
		cp[2] = data >> 16;
		cp[3] = data >> 24;
		cp += 4;
		rem -= 4;
		rx_len -= 4;
	}

	if (rem) {
		int popd;

		data = otg_fs->dfifo[EP_OUT].pop;
		popd = MIN(4, rx_len);
		cnt = MIN(popd, rem);
		for (i = 0; i < cnt; i++) {
			cp[i] = data;
			data >>= 8;
		}
		dev->ep1_rx.data = data;
		dev->ep1_rx.rem = popd - cnt;
		rx_len -= cnt;
		cp += cnt;
	}

	dev->ep1_rx.len = rx_len;

	cnt = cp - (uint8_t *)buf;

	/* 5. After the data payload is popped from the receive FIFO, the 
	   RXFLVL interrupt (OTG_FS_GINTSTS) must be unmasked. */
	if (rx_len == 0) {
		DCC_LOG1(LOG_TRACE, "cnt=%d enabling RXFLVL interrupt", cnt);
		/* Reenable RX fifo interrupts */
		otg_fs->gintmsk |= OTG_FS_RXFLVLM;
	} else {
		DCC_LOG1(LOG_TRACE, "cnt=%d", cnt);
	}


	return cnt;
}

int usb_cdc_flush(struct usb_cdc_dev * dev, 
				  const void * buf, unsigned int len)
{
	return 0;
}

struct usb_cdc_dev * usb_cdc_init(void)
{
	struct usb_cdc_dev * dev = (struct usb_cdc_dev *)&usb_cdc_dev;

	usb_device_init(dev);

	return dev;

}

#if 0
int usb_cdc_state_get(struct usb_cdc_dev * dev, struct usb_cdc_state * state)
{
	state->cfg.baud_rate = dev->cdc.lc.dwDTERate;
	state->cfg.data_bits = dev->cdc.lc.bDataBits;
	state->cfg.parity = dev->cdc.lc.bParityType;
	state->cfg.stop_bits = dev->cdc.lc.bCharFormat;

	state->ctrl.dtr = (dev->cdc.control & CDC_DTE_PRESENT);
	state->ctrl.rts = (dev->cdc.control & CDC_ACTIVATE_CARRIER);

	state->stat.dsr = (dev->cdc.status & CDC_SERIAL_STATE_TX_CARRIER);
	state->stat.ri = (dev->cdc.status & CDC_SERIAL_STATE_RING);
	state->stat.dcd = (dev->cdc.status & CDC_SERIAL_STATE_RX_CARRIER);
	state->stat.cts = 0;
	state->stat.brk = (dev->cdc.status & CDC_SERIAL_STATE_BREAK);

	state->err.ovr = (dev->cdc.status & CDC_SERIAL_STATE_OVERRUN);
	state->err.par = (dev->cdc.status & CDC_SERIAL_STATE_PARITY);
	state->err.frm = (dev->cdc.status & CDC_SERIAL_STATE_FRAMING);

	return 0;
}
#endif

int usb_cdc_ctrl_event_wait(struct usb_cdc_dev * dev, unsigned int msec)
{
	if (msec > 0)
		__thinkos_timer_set(msec);

	__thinkos_critical_enter();
	while ((dev->ctrl_rcv == dev->ctrl_ack) && (!__thinkos_timer_timedout())) {
		DCC_LOG(LOG_INFO, "wait .....................");
		__thinkos_ev_wait(dev->ctrl_ev);
		DCC_LOG(LOG_INFO, "..................... wakeup");
	}
	__thinkos_critical_exit();
	
	if (dev->ctrl_rcv == dev->ctrl_ack)
		return THINKOS_ETIMEDOUT;

	__thinkos_timer_cancel();

	dev->ctrl_ack = dev->ctrl_rcv;

	return 0;
}

