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

#include <sys/dcclog.h>
#include <sys/delay.h>

void stm32f_otg_fs_core_reset(struct stm32f_otg_fs * otg_fs)
{
	DCC_LOG(LOG_INFO, "...");

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
}

void stm32f_otg_fs_txfifo_flush(struct stm32f_otg_fs * otg_fs, 
								unsigned int num)
{
	DCC_LOG1(LOG_INFO, "%02x", num);

	otg_fs->grstctl = OTG_FS_TXFFLSH | OTG_FS_TXFIFO_SET(num);
	do {
	} while (otg_fs->grstctl & OTG_FS_TXFFLSH);
	/* Wait for 3 PHY Clocks ?? */
	udelay(3);
}

void stm32f_otg_fs_rxfifo_flush(struct stm32f_otg_fs * otg_fs)
{
	DCC_LOG(LOG_INFO, "...");

	otg_fs->grstctl = OTG_FS_RXFFLSH;
	do {
	} while (otg_fs->grstctl & OTG_FS_RXFFLSH);
	/* Wait for 3 PHY Clocks ?? */
	udelay(3);
}

void stm32f_otg_fs_addr_set(struct stm32f_otg_fs * otg_fs, unsigned int addr)
{
	DCC_LOG1(LOG_INFO, "addr=0x%02x", addr);

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

void stm32f_otg_fs_ep_disable(struct stm32f_otg_fs * otg_fs, unsigned int addr)
{
	int ep = addr & 0x7f;
	int input = addr & 0x80;

	DCC_LOG2(LOG_TRACE, "ep=%d %s", ep, input ? "IN" : "OUT");

	/* Diable endpoint interrupt */
	otg_fs->daintmsk &= ~OTG_FS_IEPM(ep);

	/* Disable endpoint */
	if (input) {
		otg_fs->inep[ep].diepctl &= ~OTG_FS_USBAEP;
	} else {
		otg_fs->outep[ep].doepctl &= ~OTG_FS_USBAEP;
	}
}

static const char * const eptyp_nm[] = {
	"CTRL",
	"ISOC",
	"BULK",
	"INT"
};

static const uint8_t ep0_mpsiz_lut[] = {
	64, 32, 16, 8
};

void stm32f_otg_fs_ep_dump(struct stm32f_otg_fs * otg_fs, unsigned int addr) 
{
	int ep = addr & 0x7f;
	uint32_t depctl;
	uint32_t eptsiz;
	uint32_t eptfsav;
	uint32_t mpsiz;

	depctl = otg_fs->inep[ep].diepctl;
	if (depctl & OTG_FS_USBAEP) {
		eptfsav = otg_fs->inep[ep].dtxfsts;
		eptsiz = otg_fs->inep[ep].dieptsiz;

		mpsiz = (ep == 0) ? ep0_mpsiz_lut[OTG_FS_MPSIZ_GET(depctl)] : 
			OTG_FS_MPSIZ_GET(depctl);

		eptsiz = eptsiz;
		eptfsav = eptfsav * 4;
		mpsiz = mpsiz;

		DCC_LOG5(LOG_TRACE, "EP%d IN %s TXFNUM=%d STALL=%d NAKSTS=%d",
				 ep, eptyp_nm[OTG_FS_EPTYP_GET(depctl)],
				 OTG_FS_TXFNUM_GET(depctl),
				 (depctl & OTG_FS_STALL) ? 1 : 0,
				 (depctl & OTG_FS_NAKSTS) ? 1 : 0);

		DCC_LOG4(LOG_TRACE, "EONUM=%s DPID=%d USBAEP=%d MPSIZ=%d", 
				 (depctl & OTG_FS_EONUM) ? "EVEN" : "ODD",
				 (depctl & OTG_FS_DPID) ? 1 : 0,
				 (depctl & OTG_FS_USBAEP) ? 1 : 0,
				 mpsiz);


		DCC_LOG3(LOG_TRACE, "PKTCNT=%d XFRSIZ=%d FSAVAIL=%d",
				 OTG_FS_PKTCNT_GET(eptsiz), OTG_FS_XFRSIZ_GET(eptsiz), 
				 eptfsav);
	} else {
		depctl = otg_fs->outep[ep].doepctl;
		if (depctl & OTG_FS_USBAEP) {
			eptsiz = otg_fs->outep[ep].doeptsiz;
			eptsiz = eptsiz;
		} else {
			DCC_LOG1(LOG_WARNING, "EP%d not active!", ep);
			return;
		}
		
		mpsiz = (ep == 0) ? ep0_mpsiz_lut[OTG_FS_MPSIZ_GET(depctl)] : 
			OTG_FS_MPSIZ_GET(depctl);

		mpsiz = mpsiz;

		DCC_LOG5(LOG_TRACE, "EP%d OUT %s SNPM=%d STALL=%d NAKSTS=%d",
				 ep, eptyp_nm[OTG_FS_EPTYP_GET(depctl)],
				 (depctl & OTG_FS_SNPM) ? 1 : 0,
				 (depctl & OTG_FS_STALL) ? 1 : 0,
				 (depctl & OTG_FS_NAKSTS) ? 1 : 0);

		DCC_LOG4(LOG_TRACE, "EONUM=%s DPID=%d USBAEP=%d MPSIZ=%d", 
				 (depctl & OTG_FS_EONUM) ? "EVEN" : "ODD",
				 (depctl & OTG_FS_DPID) ? 1 : 0,
				 (depctl & OTG_FS_USBAEP) ? 1 : 0,
				 mpsiz);

		DCC_LOG2(LOG_TRACE, "PKTCNT=%d XFRSIZ=%d",
				 OTG_FS_PKTCNT_GET(eptsiz), OTG_FS_XFRSIZ_GET(eptsiz));
	}


}

void otg_fs_fifo(struct stm32f_otg_fs * otg_fs, 
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
		DCC_LOG5(LOG_TRACE, "%04x: %08x %08x %08x %08x", i * 4, 
				 otg_fs->ram[i + 0], otg_fs->ram[i + 1],
				 otg_fs->ram[i + 2], otg_fs->ram[i + 3]);
	}

	switch (r) {
	case 1:
		DCC_LOG2(LOG_TRACE, "%04x: %08x ", i * 4, 
				 otg_fs->ram[i + 0]);
		break;
	case 2:
		DCC_LOG3(LOG_TRACE, "%04x: %08x %08x", i * 4, 
				 otg_fs->ram[i + 0], otg_fs->ram[i + 1]);
		break;
	case 3:
		DCC_LOG4(LOG_TRACE, "%04x: %08x %08x %08x", i * 4, 
				 otg_fs->ram[i + 0], otg_fs->ram[i + 1],
				 otg_fs->ram[i + 2]);
		break;
	};
}

void stm32f_otg_fs_ep_out_start(struct stm32f_otg_fs * otg_fs, 
								unsigned int addr, unsigned int mpsiz)
{
	int ep = addr & 0x7f;
	uint32_t rxfsiz;
	uint32_t pktcnt;

	rxfsiz = otg_fs->grxfsiz * 4;
	pktcnt = rxfsiz / mpsiz;

	/* Prepare EP_OUT to receive */
	otg_fs->outep[ep].doeptsiz = OTG_FS_PKTCNT_SET(pktcnt) | 
		OTG_FS_XFRSIZ_SET(pktcnt * mpsiz);
	/* EP enable */
	otg_fs->outep[ep].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
}

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
	   – Maximum packet size
	   – USB active endpoint = 1
	   – Endpoint start data toggle (for interrupt and bulk endpoints)
	   – Endpoint type
	   – TxFIFO number */
	
	depctl |= OTG_FS_MPSIZ_SET(mpsiz);
	depctl |= OTG_FS_EPTYP_SET(type);
	depctl |= OTG_FS_SD0PID | OTG_FS_USBAEP;

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

void stm32f_otg_fs_device_init(struct stm32f_otg_fs * otg_fs)
{
	uint32_t depctl;
	int i;

	/* Disable global interrupts */
	otg_fs->gahbcfg &= ~OTG_FS_GINTMSK; 

	/* - Force device mode 
	   - Full Speed serial transceiver select */
	otg_fs->gusbcfg = OTG_FS_FDMOD | OTG_FS_TRDT_SET(5) | 
		OTG_FS_PHYSEL | OTG_FS_SRPCAP | OTG_FS_TOCAL_SET(1);

	/* Reset after a PHY select and set Device mode */
	stm32f_otg_fs_core_reset(otg_fs);

	/* Restart the Phy Clock */
	otg_fs->pcgcctl = 0;

	/* Flush the FIFOs */
	stm32f_otg_fs_txfifo_flush(otg_fs, 0x10);
	stm32f_otg_fs_rxfifo_flush(otg_fs);

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

	/* The application must perform the following steps to initialize the 
	   core as a device on power-up or after a mode change from host to device.
	1. Program the following fields in the OTG_FS_DCFG register:
	– Device speed
	- Non-zero-length status OUT handshake */
	otg_fs->dcfg = OTG_FS_PFIVL_80 | OTG_FS_DSPD_FULL;

	/* 2. Program the OTG_FS_GINTMSK register to unmask the 
	   following interrupts:
	   – Wakeup 
	   – USB reset
	   – Enumeration done
	   – Early suspend
	   – USB suspend
	   – SOF */
	otg_fs->gintmsk = OTG_FS_SRQIM | OTG_FS_OTGINT;

//	otg_fs->gintmsk = OTG_FS_WUIM | OTG_FS_USBRSTM | OTG_FS_ENUMDNEM | 
//		OTG_FS_ESUSPM | OTG_FS_USBSUSPM;
//	otg_fs->gintmsk |=  OTG_FS_IEPINTM | OTG_FS_OEPINTM | 
//		OTG_FS_IISOIXFRM | OTG_FS_IISOOXFRM | OTG_FS_RXFLVLM | OTG_FS_SOFM;


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

	/* AHB configuration */
	/* Enable global interrupts */
	otg_fs->gahbcfg |= OTG_FS_PTXFELVL | OTG_FS_TXFELVL | OTG_FS_GINTMSK; 
}

