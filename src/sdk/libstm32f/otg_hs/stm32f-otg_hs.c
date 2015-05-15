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

#include <stdio.h>
#include <sys/param.h>

#include <sys/dcclog.h>
#include <sys/delay.h>


#ifdef STM32F_OTG_HS

void stm32f_otg_hs_core_reset(struct stm32f_otg_hs * otg_hs)
{
	DCC_LOG(LOG_INFO, "...");

	/* Wait for AHB master IDLE state. */
	while (!(otg_hs->grstctl & OTG_HS_AHBIDL)) {
		udelay(3);
	}

	/* Core Soft Reset */
	otg_hs->grstctl = OTG_HS_CSRST;
	do {
		udelay(3);
	} while (otg_hs->grstctl & OTG_HS_CSRST);

	/* Wait for 3 PHY Clocks*/
	udelay(3);
}

void stm32f_otg_hs_txfifo_flush(struct stm32f_otg_hs * otg_hs, 
								unsigned int num)
{
	DCC_LOG1(LOG_INFO, "%02x", num);

	otg_hs->grstctl = OTG_HS_TXFFLSH | OTG_HS_TXFIFO_SET(num);
	do {
	} while (otg_hs->grstctl & OTG_HS_TXFFLSH);
	/* Wait for 3 PHY Clocks ?? */
	udelay(3);
}

void stm32f_otg_hs_rxfifo_flush(struct stm32f_otg_hs * otg_hs)
{
	DCC_LOG(LOG_INFO, "...");

	otg_hs->grstctl = OTG_HS_RXFFLSH;
	do {
	} while (otg_hs->grstctl & OTG_HS_RXFFLSH);
	/* Wait for 3 PHY Clocks ?? */
	udelay(3);
}

void stm32f_otg_hs_addr_set(struct stm32f_otg_hs * otg_hs, unsigned int addr)
{
	DCC_LOG1(LOG_INFO, "addr=0x%02x", addr);

	uint32_t dcfg;
	/*  Endpoint initialization on SetAddress command */

	/* This section describes what the application must do when it 
	   receives a SetAddress command in a SETUP packet.
	   1. Program the OTG_HS_DCFG register with the device address 
	   received in the SetAddress command */
	dcfg = otg_hs->dcfg; 
	dcfg &= ~OTG_HS_DAD_MSK;
	dcfg |= OTG_HS_DAD_SET(addr);
	otg_hs->dcfg = dcfg;

	/* 2. Program the core to send out a status IN packet */
}

void stm32f_otg_hs_ep_disable(struct stm32f_otg_hs * otg_hs, unsigned int addr)
{
	int ep_id = addr & 0x7f;
	int input = addr & 0x80;

	DCC_LOG2(LOG_INFO, "ep_id=%d %s", ep_id, input ? "IN" : "OUT");

	/* Diable endpoint interrupt */
	otg_hs->daintmsk &= ~OTG_HS_IEPM(ep_id);

	/* Disable endpoint */
	if (input) {
		otg_hs->inep[ep_id].diepctl &= ~OTG_HS_USBAEP;
	} else {
		otg_hs->outep[ep_id].doepctl &= ~OTG_HS_USBAEP;
	}
}

void stm32f_otg_hs_ep_out_start(struct stm32f_otg_hs * otg_hs,
								unsigned int addr, unsigned int mpsiz)
{
	int ep_id = addr & 0x7f;
	uint32_t rxfsiz;
	uint32_t pktcnt;

	rxfsiz = otg_hs->grxfsiz * 4;
	pktcnt = rxfsiz / mpsiz;

	/* Prepare EP_OUT to receive */
	otg_hs->outep[ep_id].doeptsiz = OTG_HS_PKTCNT_SET(pktcnt) |
		OTG_HS_XFRSIZ_SET(pktcnt * mpsiz);
	/* EP enable */
	otg_hs->outep[ep_id].doepctl |= OTG_HS_EPENA | OTG_HS_CNAK;
}

const uint8_t stm32f_otg_hs_ep0_mpsiz_lut[] = {
	64, 32, 16, 8
};

int stm32f_otg_hs_txf_setup(struct stm32f_otg_hs * otg_hs, 
							unsigned int ep_id, unsigned int len)
{
	uint32_t deptsiz;
	uint32_t depctl;
	uint32_t mpsiz;
	uint32_t xfrsiz;
	uint32_t pktcnt;

	deptsiz = otg_hs->inep[ep_id].dieptsiz;
	xfrsiz = OTG_HS_XFRSIZ_GET(deptsiz);
	pktcnt = OTG_HS_PKTCNT_GET(deptsiz);
	if ((xfrsiz == 0) && (pktcnt)) {
		DCC_LOG3(LOG_WARNING, "ep_id=%d len=%d %d outstanding packets in FIFO", 
				 ep_id, len, pktcnt);
		return -1;
	}

	depctl = otg_hs->inep[ep_id].diepctl;
	if (ep_id == 0)
		mpsiz = OTGFS_EP0_MPSIZ_GET(depctl);
	else
		mpsiz = OTG_HS_MPSIZ_GET(depctl);

	if (len > 0) {
		/* XXX: check whether to get rid of this division or not,
		 if the CM3 div is used it is not necessary.... */
		pktcnt = (len + (mpsiz - 1)) / mpsiz;
		if (pktcnt > 7) {
			pktcnt = 7;
			xfrsiz = 7 * mpsiz;
		} else {
			xfrsiz = len;
		}
	} else {
		/* zero lenght packet */
		pktcnt = 1;
		xfrsiz = 0;
	}

	DCC_LOG3(LOG_INFO, "ep_id=%d pktcnt=%d xfrsiz=%d", ep_id, pktcnt, xfrsiz);

	otg_hs->inep[ep_id].dieptsiz = OTG_HS_PKTCNT_SET(pktcnt) | 
		OTG_HS_XFRSIZ_SET(xfrsiz); 
	/* enable end point, clear NACK */
	otg_hs->inep[ep_id].diepctl = depctl | OTG_HS_EPENA | OTG_HS_CNAK; 

	deptsiz = otg_hs->inep[ep_id].dieptsiz;
	(void)deptsiz;

	DCC_LOG2(LOG_INFO, "PKTCNT=%d XFRSIZ=%d", OTG_HS_PKTCNT_GET(deptsiz), 
			 OTG_HS_XFRSIZ_GET(deptsiz));

	return xfrsiz;
}

int stm32f_otg_hs_txf_push(struct stm32f_otg_hs * otg_hs, unsigned int ep_id,
						   void * buf)
{
	uint32_t depctl;
	uint32_t deptsiz;
	uint32_t mpsiz;
	uint32_t xfrsiz;
	uint32_t pktcnt;
	uint32_t free;
	uint32_t data;
	uint8_t * cp;
	int cnt;
	int i;

	free = otg_hs->inep[ep_id].dtxfsts * 4;
	depctl = otg_hs->inep[ep_id].diepctl;
	deptsiz = otg_hs->inep[ep_id].dieptsiz;

	if (ep_id == 0)
		mpsiz = OTGFS_EP0_MPSIZ_GET(depctl);
	else
		mpsiz = OTG_HS_MPSIZ_GET(depctl);

	xfrsiz = OTG_HS_XFRSIZ_GET(deptsiz);
	pktcnt = OTG_HS_PKTCNT_GET(deptsiz);
	(void)pktcnt;

	DCC_LOG5(LOG_MSG, "ep_id=%d mpsiz=%d pktcnt=%d xfrsiz=%d free=%d", 
			 ep_id, mpsiz, pktcnt, xfrsiz, free);

	if (xfrsiz < mpsiz) {
		if (free < xfrsiz) {
			DCC_LOG(LOG_PANIC, "free < xfrsiz !!!");
			return -1;
		}
		/* Transfer the last partial packet */
		cnt = xfrsiz;
	} else {
		if (free < mpsiz) {
			DCC_LOG(LOG_PANIC, "free < mpsiz !!!");
			return -1;
		}
		if (free < xfrsiz) {
			/* Transfer only full packets */
			cnt = (free / mpsiz) * mpsiz;
		} else {
			/* Transfer all */
			cnt = xfrsiz;
		}
		// XXX: debug
//		cnt = mpsiz;
	}

	/* push into fifo */
	cp = (uint8_t *)buf;
	for (i = 0; i < cnt; i += 4) {
		data = cp[0] + (cp[1] << 8) + (cp[2] << 16) + (cp[3] << 24);
		otg_hs->dfifo[ep_id].push = data;
		cp += 4;
	}	

	DCC_LOG2(LOG_MSG, "Tx: (%d) free=%d", cnt, 
			 otg_hs->inep[ep_id].dtxfsts * 4 );

	return cnt;
}



void stm32f_otg_hs_device_init(struct stm32f_otg_hs * otg_hs)
{
//	uint32_t depctl;
//	int i;

	DCC_LOG(LOG_INFO, "1.");

	/* Disable global interrupts */
	otg_hs->gahbcfg &= ~OTG_HS_GINTMSK; 

	/* - Force device mode 
	   - Full Speed serial transceiver select */
	otg_hs->gusbcfg = OTG_HS_FDMOD | OTG_HS_TRDT_SET(9) | 
		 OTG_HS_PHYSEL | OTG_HS_SRPCAP | OTG_HS_TOCAL_SET(4);
//		OTG_HS_SRPCAP | OTG_HS_TOCAL_SET(1) | OTG_HS_PHSEL;

	/* Reset after a PHY select and set Device mode */
	stm32f_otg_hs_core_reset(otg_hs);

	DCC_LOG(LOG_INFO, "2.");

	/* Restart the Phy Clock */
	otg_hs->pcgcctl = 0;
//	otg_hs->pcgcctl = OTG_HS_GATEHCLK; 

	/* AHB configuration */
	/* Enable global interrupts */
	otg_hs->gahbcfg = OTG_HS_PTXFELVL | OTG_HS_TXFELVL | OTG_HS_GINTMSK; 
#if 0
	/* Flush the FIFOs */
	stm32f_otg_hs_txfifo_flush(otg_hs, 0x10);
	stm32f_otg_hs_rxfifo_flush(otg_hs);

	/* Clear all pending Device Interrupts */
	otg_hs->diepmsk = 0;
	otg_hs->doepmsk = 0;
	otg_hs->daint = 0xffffffff;
	otg_hs->daintmsk = 0;
	for (i = 0; i < 4; i++) {
		depctl = otg_hs->inep[i].diepctl;
		depctl = (depctl & OTG_HS_EPENA) ? OTG_HS_EPDIS | OTG_HS_SNAK : 0;
		otg_hs->inep[i].diepctl = depctl;
		otg_hs->inep[i].dieptsiz = 0;
		otg_hs->inep[i].diepint = 0xff;
		otg_hs->inep[i].diepdma = 0;
	}
	for (i = 0; i < 4; i++) {
		depctl = otg_hs->outep[i].doepctl;
		depctl = (depctl & OTG_HS_EPENA) ? OTG_HS_EPDIS | OTG_HS_SNAK : 0;
		otg_hs->outep[i].doepctl = depctl;
		otg_hs->outep[i].doeptsiz = 0;
		otg_hs->outep[i].doepint = 0xff;
		otg_hs->outep[i].doepdma = 0;
	}
#endif
	/* The application must perform the following steps to initialize the 
	   core as a device on power-up or after a mode change from host 
	   to device. */

	/* 1. Program the following fields in the OTG_HS_DCFG register: 
	   - Device speed
	   - Non-zero-length status OUT handshake */
	otg_hs->dcfg = OTG_HS_PFIVL_80 | OTG_HS_DSPD_FULL;

	/* 2. Program the OTG_HS_GINTMSK register to unmask the 
	   following interrupts:
	   - Wakeup 
	   - USB reset
	   - Enumeration done
	   - Early suspend
	   - USB suspend
	   - SOF */
	otg_hs->gintmsk = OTG_HS_SRQIM | OTG_HS_OTGINT;

	/* 3. Program the VBUSBSEN bit in the OTG_HS_GCCFG register to enable VBUS 
	   sensing in "B" device mode and supply the 5 volts across the pull-up 
	   resistor on the DP line. */
	otg_hs->gccfg = OTG_HS_VBUSBSEN | OTG_HS_PWRDWN;

	/* 4. Wait for the USBRST interrupt in OTG_HS_GINTSTS. It indicates that 
	   a reset has been detected on the USB that lasts for about 10 ms on 
	   receiving this interrupt. Wait for the ENUMDNE interrupt in 
	   OTG_HS_GINTSTS.
	   This interrupt indicates the end of reset on the USB. On receiving this 
	   interrupt, the application must read the OTG_HS_DSTS register to 
	   determine the enumeration speed and perform the steps listed in 
	   Endpoint initialization on enumeration completion on page 1035.
	   At this point, the device is ready to accept SOF packets and perform 
	   control transfers on control endpoint 0. */


	DCC_LOG(LOG_INFO, "3.");
}

#ifdef DEBUG

static const char * const eptyp_nm[] = {
	"CTRL",
	"ISOC",
	"BULK",
	"INT"
};

void stm32f_otg_hs_ep_dump(struct stm32f_otg_hs * otg_hs, unsigned int addr) 
{
	int ep_id = addr & 0x7f;
	uint32_t depctl;
	uint32_t eptsiz;
	uint32_t eptfsav;
	uint32_t mpsiz;

	depctl = otg_hs->inep[ep_id].diepctl;
	if (depctl & OTG_HS_USBAEP) {
		eptfsav = otg_hs->inep[ep_id].dtxfsts;
		eptsiz = otg_hs->inep[ep_id].dieptsiz;

		mpsiz = (ep_id == 0) ? OTGFS_EP0_MPSIZ_GET(depctl) : 
			OTG_HS_MPSIZ_GET(depctl);

		(void)eptsiz;
		eptfsav = eptfsav * 4;
		(void)mpsiz;

		DCC_LOG5(LOG_INFO, "EP%d IN %s TXFNUM=%d STALL=%d NAKSTS=%d",
				 ep_id, eptyp_nm[OTG_HS_EPTYP_GET(depctl)],
				 OTG_HS_TXFNUM_GET(depctl),
				 (depctl & OTG_HS_STALL) ? 1 : 0,
				 (depctl & OTG_HS_NAKSTS) ? 1 : 0);

		DCC_LOG4(LOG_INFO, "EONUM=%s DPID=%d USBAEP=%d MPSIZ=%d", 
				 (depctl & OTG_HS_EONUM) ? "EVEN" : "ODD",
				 OTG_HS_DPID_GET(depctl),
				 (depctl & OTG_HS_USBAEP) ? 1 : 0,
				 mpsiz);


		DCC_LOG3(LOG_INFO, "PKTCNT=%d XFRSIZ=%d FSAVAIL=%d",
				 OTG_HS_PKTCNT_GET(eptsiz), OTG_HS_XFRSIZ_GET(eptsiz), 
				 eptfsav);
	} else {
		depctl = otg_hs->outep[ep_id].doepctl;
		if (depctl & OTG_HS_USBAEP) {
			eptsiz = otg_hs->outep[ep_id].doeptsiz;
			(void)eptsiz;
		} else {
			DCC_LOG1(LOG_WARNING, "EP%d not active!", ep_id);
			return;
		}
		
		mpsiz = (ep_id == 0) ? OTGFS_EP0_MPSIZ_GET(depctl) : 
			OTG_HS_MPSIZ_GET(depctl);
		(void)mpsiz;

		DCC_LOG5(LOG_INFO, "EP%d OUT %s SNPM=%d STALL=%d NAKSTS=%d",
				 ep_id, eptyp_nm[OTG_HS_EPTYP_GET(depctl)],
				 (depctl & OTG_HS_SNPM) ? 1 : 0,
				 (depctl & OTG_HS_STALL) ? 1 : 0,
				 (depctl & OTG_HS_NAKSTS) ? 1 : 0);

		DCC_LOG4(LOG_INFO, "EONUM=%s DPID=%d USBAEP=%d MPSIZ=%d", 
				 (depctl & OTG_HS_EONUM) ? "EVEN" : "ODD",
				 OTG_HS_DPID_GET(depctl),
				 (depctl & OTG_HS_USBAEP) ? 1 : 0,
				 mpsiz);

		DCC_LOG2(LOG_INFO, "PKTCNT=%d XFRSIZ=%d",
				 OTG_HS_PKTCNT_GET(eptsiz), OTG_HS_XFRSIZ_GET(eptsiz));
	}


}

void otg_hs_fifo(struct stm32f_otg_hs * otg_hs, 
				 unsigned int addr, unsigned int len)
{
	unsigned int q;
	unsigned int r;
	unsigned int n;
	unsigned int i;

	i = ((addr >> 2) / 4) * 4;
	n = (addr + len + 3) >> 2;
	q = (n / 4) * 4;
	r = n - q;

	for (; i < q; i += 4) {
		DCC_LOG5(LOG_INFO, "%04x: %08x %08x %08x %08x", i * 4, 
				 otg_hs->ram[i + 0], otg_hs->ram[i + 1],
				 otg_hs->ram[i + 2], otg_hs->ram[i + 3]);
	}

	switch (r) {
	case 1:
		DCC_LOG2(LOG_INFO, "%04x: %08x ", i * 4, 
				 otg_hs->ram[i + 0]);
		break;
	case 2:
		DCC_LOG3(LOG_INFO, "%04x: %08x %08x", i * 4, 
				 otg_hs->ram[i + 0], otg_hs->ram[i + 1]);
		break;
	case 3:
		DCC_LOG4(LOG_INFO, "%04x: %08x %08x %08x", i * 4, 
				 otg_hs->ram[i + 0], otg_hs->ram[i + 1],
				 otg_hs->ram[i + 2]);
		break;
	};
}

#endif

#endif /* STM32F_OTG_HS */

