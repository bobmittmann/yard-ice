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
 * @file stm32f-usb_fs.c
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
#include <sys/dcclog.h>

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
		stm32f_gpio_mode(USB_FS_VBUS, INPUT, SPEED_LOW);
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

	/* disabling IO pins */
	stm32f_gpio_mode(USB_FS_DP, INPUT, 0);
	stm32f_gpio_mode(USB_FS_DM, INPUT, 0);
}

/* configure a RX descriptor */
int pktbuf_rx_cfg(struct stm32f_usb_rx_pktbuf * rx,
						 unsigned int addr, unsigned int mxpktsz)
{
//	int sz = mxpktsz + 2; /* alloc 2 extra bytes for CRC */
	int sz = mxpktsz;

	if (sz < 63) {
		sz = (sz + 1) & ~0x01;
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
int pktbuf_tx_cfg(struct stm32f_usb_tx_pktbuf * tx,
						 unsigned int addr, unsigned int mxpktsz)
{
	tx->addr = addr;
	tx->count = 0;

	return mxpktsz;
}

int pktbuf_rx_mxpktsz(struct stm32f_usb_rx_pktbuf * rx)
{
	int sz;

	if (rx->blsize)
		sz = (rx->num_block + 1) * 32;
	else
		sz = rx->num_block * 2;

	return sz;
}

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

/* EP_KIND EndPoint KIND */
#define EPKIND_MASK    (~USB_EP_KIND & USB_EPREG_MASK)

#define _SetEPType(bEpNum,wType) (_SetENDPOINT(bEpNum,\ ((_GetENDPOINT(bEpNum) & EP_T_MASK) | wType)))

/* STAT_TX[1:0] STATus for TX transfer */
#define EP_TX_DIS      (0x0000) /* EndPoint TX DISabled */
#define EP_TX_STALL    (0x0010) /* EndPoint TX STALLed */
#define EP_TX_NAK      (0x0020) /* EndPoint TX NAKed */
#define EP_TX_VALID    (0x0030) /* EndPoint TX VALID */
#define EPTX_DTOG1     (0x0010) /* EndPoint TX Data TOGgle bit1 */
#define EPTX_DTOG2     (0x0020) /* EndPoint TX Data TOGgle bit2 */

/* STAT_RX[1:0] STATus for RX transfer */
#define EP_RX_DIS      (0x0000) /* EndPoint RX DISabled */
#define EP_RX_STALL    (0x1000) /* EndPoint RX STALLed */
#define EP_RX_NAK      (0x2000) /* EndPoint RX NAKed */
#define EP_RX_VALID    (0x3000) /* EndPoint RX VALID */
#define EPRX_DTOG1     (0x1000) /* EndPoint RX Data TOGgle bit1 */
#define EPRX_DTOG2     (0x2000) /* EndPoint RX Data TOGgle bit1 */

#define _SetEPAddress(bEpNum,bAddr) _SetENDPOINT(bEpNum,\
_GetENDPOINT(bEpNum) & EPREG_MASK | bAddr)

/*******************************************************************************
  * Macro Name     : GetEPAddress.
  * Description    : Gets address in an endpoint register.
  * Input          : bEpNum: Endpoint Number.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
#define _GetEPAddress(bEpNum) ((uint8_t)(_GetENDPOINT(bEpNum) & EPADDR_FIELD))

#define _GetBTABLE() 0
#define PMAAddr ((uint32_t)STM32F_USB_PKTBUF_ADDR)

#define _pEPTxAddr(bEpNum) ((uint32_t *)((_GetBTABLE()+bEpNum*8  )*2 + PMAAddr))
#define _pEPTxCount(bEpNum) ((uint32_t *)((_GetBTABLE()+bEpNum*8+2)*2 + PMAAddr))

#define _pEPRxAddr(bEpNum) ((uint32_t *)((_GetBTABLE()+bEpNum*8+4)*2 + PMAAddr))
#define _pEPRxCount(bEpNum) ((uint32_t *)((_GetBTABLE()+bEpNum*8+6)*2 + PMAAddr))

/*******************************************************************************
  * Macro Name     : SetEPTxAddr / SetEPRxAddr.
  * Description    : sets address of the tx/rx buffer.
  * Input          : bEpNum: Endpoint Number.
  *                  wAddr: address to be set (must be word aligned).
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
#define _SetEPTxAddr(bEpNum,wAddr) (*_pEPTxAddr(bEpNum) = ((wAddr >> 1) << 1))
#define _SetEPRxAddr(bEpNum,wAddr) (*_pEPRxAddr(bEpNum) = ((wAddr >> 1) << 1))

/*******************************************************************************
  * Macro Name     : GetEPTxAddr / GetEPRxAddr.
  * Description    : Gets address of the tx/rx buffer.
  * Input          : bEpNum: Endpoint Number.
  * Output         : None.
  * Return         : address of the buffer.
  *******************************************************************************/
#define _GetEPTxAddr(bEpNum) ((uint16_t)*_pEPTxAddr(bEpNum))
#define _GetEPRxAddr(bEpNum) ((uint16_t)*_pEPRxAddr(bEpNum))

/*******************************************************************************
  * Macro Name     : SetEPCountRxReg.
  * Description    : Sets counter of rx buffer with no. of blocks.
  * Input          : pdwReg: pointer to counter.
  *                  wCount: Counter.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
#define _BlocksOf32(dwReg,wCount,wNBlocks) {\
	    wNBlocks = wCount >> 5;\
	    if((wCount & 0x1f) == 0)\
	      wNBlocks--;\
	    *pdwReg = (uint32_t)((wNBlocks << 10) | 0x8000);\
	  }/* _BlocksOf32 */

#define _BlocksOf2(dwReg,wCount,wNBlocks) {\
	    wNBlocks = wCount >> 1;\
	    if((wCount & 0x1) != 0)\
	      wNBlocks++;\
	    *pdwReg = (uint32_t)(wNBlocks << 10);\
	  }/* _BlocksOf2 */

#define _SetEPCountRxReg(dwReg,wCount)  {\
	    uint16_t wNBlocks;\
	    if(wCount > 62){_BlocksOf32(dwReg,wCount,wNBlocks);}\
	    else {_BlocksOf2(dwReg,wCount,wNBlocks);}\
	  }/* _SetEPCountRxReg */



#define _SetEPRxDblBuf0Count(bEpNum,wCount) {\
	    uint32_t *pdwReg = _pEPTxCount(bEpNum); \
	    _SetEPCountRxReg(pdwReg, wCount);\
	  }
/*******************************************************************************
  * Macro Name     : SetEPTxCount / SetEPRxCount.
  * Description    : sets counter for the tx/rx buffer.
  * Input          : bEpNum: endpoint number.
  *                  wCount: Counter value.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
#define _SetEPTxCount(bEpNum,wCount) (*_pEPTxCount(bEpNum) = wCount)
#define _SetEPRxCount(bEpNum,wCount) {\
	    uint32_t *pdwReg = _pEPRxCount(bEpNum); \
	    _SetEPCountRxReg(pdwReg, wCount);\
	  }
/*******************************************************************************
  * Macro Name     : GetEPTxCount / GetEPRxCount.
  * Description    : gets counter of the tx buffer.
  * Input          : bEpNum: endpoint number.
  * Output         : None.
  * Return         : Counter value.
  *******************************************************************************/
#define _GetEPTxCount(bEpNum)((uint16_t)(*_pEPTxCount(bEpNum)) & 0x3ff)
#define _GetEPRxCount(bEpNum)((uint16_t)(*_pEPRxCount(bEpNum)) & 0x3ff)

#define _ClearEP_CTR_RX(bEpNum)   (_SetENDPOINT(bEpNum,\
								_GetENDPOINT(bEpNum) & 0x7FFF & EPREG_MASK))

#define _ClearEP_CTR_TX(bEpNum)   (_SetENDPOINT(bEpNum,\
				_GetENDPOINT(bEpNum) & 0xFF7F & EPREG_MASK))


void clr_ep_kind(struct stm32f_usb * usb, int ep_id)
{
	clr_ep_flag(usb, ep_id, USB_EP_KIND);
}

void set_ep_kind(struct stm32f_usb * usb, int ep_id)
{
	set_ep_flag(usb, ep_id, USB_EP_KIND);
}

void clr_status_out(struct stm32f_usb * usb, int ep_id)
{
	clr_ep_flag(usb, ep_id, USB_EP_KIND);
}

void set_ep_type(struct stm32f_usb * usb, int ep_id, int type)
{
	uint32_t epr;
	epr = usb->epr[ep_id] & EP_T_MASK;
	usb->epr[ep_id] = epr | type;
}

void set_ep_txcount(struct stm32f_usb * usb, int ep_id, int count)
{
	*_pEPTxCount(ep_id) = count;
}

void set_ep_rxcount(struct stm32f_usb * usb, int ep_id, int count)
{
	uint32_t * pdwReg = _pEPRxCount(ep_id);

	_SetEPCountRxReg(pdwReg, count);
}

void set_ep_txaddr(struct stm32f_usb * usb, int ep_id, int addr)
{
	*_pEPTxAddr(ep_id) = ((addr >> 1) << 1);
}

void set_ep_rxaddr(struct stm32f_usb * usb, int ep_id, int addr)
{
	*_pEPRxAddr(ep_id) = ((addr >> 1) << 1);
}

int get_ep_txcount(struct stm32f_usb * usb, int ep_id)
{
	return (uint16_t)(*_pEPTxCount(ep_id)) & 0x3ff;
}

int get_ep_rxcount(struct stm32f_usb * usb, int ep_id)
{
	return (uint16_t)(*_pEPRxCount(ep_id)) & 0x3ff;
}

int get_ep_txaddr(struct stm32f_usb * usb, int ep_id)
{
	return (uint16_t)*_pEPTxAddr(ep_id);
}

int get_ep_rxaddr(struct stm32f_usb * usb, int ep_id)
{
	return (uint16_t)*_pEPRxAddr(ep_id);
}

/* Set the ase of the packet buffers just above the buffer descriptors:
   We have up to 8 descriptors of 8 bytes each. */
#define PKTBUF_BUF_BASE (8 * 8)
#define STM32F_USB_PKTBUF ((struct stm32f_usb_pktbuf *)STM32F_USB_PKTBUF_ADDR)

void stm32f_usb_ep0_init(struct stm32f_usb * usb, int mxpktsz)
{
	/* packet buffer  */
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
//	uint32_t epr;
	unsigned int addr = PKTBUF_BUF_BASE;
	unsigned int sz;


	/* clear the correct transfer bits */
	usb->epr[0] &= ~(USB_CTR_RX | USB_CTR_TX);

	set_ep_type(usb, 0, EP_CONTROL);
	set_ep_txstat(usb, 0, EP_TX_NAK);

	/* allocate single buffers for TX and RX */
	sz = pktbuf_tx_cfg(&pktbuf[0].tx, addr, mxpktsz);
	addr += sz;
	sz = pktbuf_rx_cfg(&pktbuf[0].rx, addr, mxpktsz);
	addr += sz;

	clr_status_out(usb, 0);
	set_ep_rxvalid(usb, 0);

	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);
}



#if 0
int stm32f_usb_ep_xmit(struct stm32f_usb * usb, int ep_id)
{
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb_ep * ep;
	int len;

	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	stm32f_copy_to_pktbuf(tx, buf, len);
	set_ep_txstat(usb, ep_id, USB_TX_VALID);

	return 0;
}
#endif


/* This is a bitmask that when applied to the EPR register
 * will NOT change its value except possibly for the address
 */
#define EPR_INVARIANT (USB_CTR_RX | USB_CTR_TX)

void stm32f_usb_ep_init(struct stm32f_usb * usb, int ep_id, int mxpktsz)
{
	struct stm32f_usb_pktbuf * pktbuf;
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

