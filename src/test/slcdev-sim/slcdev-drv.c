/* 
 * File:	 slcdev-drv.c
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

#include "board.h"
#include "isink.h"
#include "slcdev.h"
#include <arch/cortex-m3.h>
#include <sys/dcclog.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

struct slcdev_drv slcdev_drv;
struct ss_device ss_dev_tab[SS_DEVICES_MAX];

static const uint8_t parity_lut[16] = {
/*	0000 0001 0010 0011 0100 0101 0110 0111 */
	   0,   1,   1,   0,   1,   0,   0,   1,
/*	1000 1001 1010 1011 1100 1101 1110 1111 */
	   1,   0,   0,   1,   0,   1,   1,   0,   
};

/* -------------------------------------------------------------------------
 * Address encoding decoding tables
 * ------------------------------------------------------------------------- */

/*	0000  0 
	0100  1 
	0110  2
	0111  3 
	1000  4
	0001  5 
	0101  6
	0010  7
	0011  8
	1001  9 */

/* System sensor rotary switch decoding table */
static const uint8_t addr_dec_lut[16] = {
/*	0000 0001 0010 0011 0100 0101 0110 0111 */
	 0x0, 0x4, 0x1, 0xc, 0x7, 0xf, 0x2, 0xa,
/*	1000 1001 1010 1011 1100 1101 1110 1111 */
	 0x5, 0x9, 0x6, 0xe, 0x8, 0xd, 0x3, 0xb   
};

/* System sensor rotary switch reverse decoding (encoding) table */
static const uint8_t addr_enc_lut[16] = {
/*	  0    1    2    3    4    5    6    7 */
	0x0, 0x2, 0x6, 0xe, 0x1, 0x8, 0xa, 0x4,
/*	  8    9    a    b    c    d    e    f */
	0xc, 0x9, 0x7, 0xf, 0x3, 0xd, 0xb, 0x5
};

static inline const char * __ms(bool module)
{
	return module ? "module" : "sensor";
}

/* -------------------------------------------------------------------------
 * Trigger module
 * ------------------------------------------------------------------------- */

bool trig_addr_set(unsigned int addr)
{
	unsigned int lo;
	unsigned int hi;

	if (addr >= 160)
		return false;

	hi = addr_enc_lut[(addr / 10) & 0xf];
	lo = addr_enc_lut[(addr % 10) & 0xf];

	if (addr >= 100) {
		/* disable CLIP trigger for invalid addresses */
		slcdev_drv.trig.msk = 0x0000;
		slcdev_drv.trig.cmp = 0xffff; 
	} else {
		/* Adjust the trigger mask for CLIP mode */
		slcdev_drv.trig.msk |= 0x01fe; /* don't mess with the 
										  module bit */
		/* Adjust the compare values for CLIP mode.*/
		slcdev_drv.trig.cmp &= 1; /* clear all except the module bit */
		slcdev_drv.trig.cmp |= hi << 1; 
		slcdev_drv.trig.cmp |= lo << 5; 
	}

	/* Adjust the trigger mask and compare values for AP mode */
	/* AP direct poll trigger : MTTT T111 1UUU U11S SCI 
	      M - Module/Sensor
	   TTTT - Tens address
	   UUUU - Units address 
	     SS - Sequence checksum
	      C - Checksum error 
	      I - Interrupt */
	slcdev_drv.trig.ap_msk |= 0x7ffe;
	slcdev_drv.trig.ap_cmp &= 1; /* clear all except the module bit */
	slcdev_drv.trig.ap_cmp |= 0x61e0;
	slcdev_drv.trig.ap_cmp |= hi << 1; 
	slcdev_drv.trig.ap_cmp |= lo << 9; 

	return true;
}

void trig_module_set(bool en)
{
	unsigned int addr;

	addr = trig_addr_get();

	if (addr < 100) {
		/* Adjust the trigger mask for CLIP mode */
		if (en) {
			slcdev_drv.trig.msk |= 1;
			slcdev_drv.trig.cmp |= 1;
		} else {
			if (slcdev_drv.trig.cmp & 1)
				slcdev_drv.trig.msk &= ~1;
		}
	}

	if (en) {
		slcdev_drv.trig.ap_msk |= 1;
		slcdev_drv.trig.ap_cmp |= 1;
	} else {
		if (slcdev_drv.trig.cmp & 1)
			slcdev_drv.trig.msk &= ~1;
	}

}

void trig_sensor_set(bool en)
{
	unsigned int addr;

	addr = trig_addr_get();

	if (addr < 100) {
		/* Adjust the trigger mask for CLIP mode */
		if (en) {
			slcdev_drv.trig.msk |= 1;
			slcdev_drv.trig.cmp &= ~1;
		} else {
			if ((slcdev_drv.trig.cmp & 1) == 0)
				slcdev_drv.trig.msk &= ~1;
		}
	}

	if (en) {
		slcdev_drv.trig.ap_msk |= 1;
		slcdev_drv.trig.ap_cmp &= ~1;
	} else {
		if ((slcdev_drv.trig.cmp & 1) ==0)
			slcdev_drv.trig.msk &= ~1;
	}
}

unsigned int trig_addr_get(void)
{
	unsigned int lo;
	unsigned int hi;
	unsigned int addr;

	/* Get the address from the AP Direct poll trigger 
	   configuration (mask and compare values) */
	if ((slcdev_drv.trig.ap_msk == 0x7fff) &&
		((slcdev_drv.trig.ap_cmp & 0x61e0) == 0x61e0)) {
		/* Upper nibble address bits */
		hi = (slcdev_drv.trig.ap_cmp >> 1) & 0xf; 
		/* Lower nibble address bits. */
		lo = (slcdev_drv.trig.ap_cmp >> 9) & 0xf; 
		addr = 10 * addr_dec_lut[hi] + addr_dec_lut[lo];
	
		return addr;
	}	
	return 0;
}

static void trig_init(void)
{
	slcdev_drv.trig.msk = 0x0000;
	slcdev_drv.trig.cmp = 0xffff;

	slcdev_drv.trig.ap_msk = 0x0000;
	slcdev_drv.trig.ap_cmp = 0xffff;

	stm32_gpio_clr(TRIG_OUT);
	stm32_gpio_mode(TRIG_OUT, OUTPUT, PUSH_PULL | SPEED_MED);
}

/* -------------------------------------------------------------------------
 * Default device
 * ------------------------------------------------------------------------- */

const struct ss_device null_dev = {
	.addr = 0, /* reverse lookup address */
	.model = 0, /* reference to a device model */
	.apen = 0, /* advanced protocol */
	.module = 0, /* sensor/module */
	.enabled = 0, /* enable device simulation */
	.cfg = 0, 
	.led = 0, /* LED status */
	.tst = 0, /* Remote test mode */
	.tbias = 128,
	.icfg = 0,
	.ipre = 0,
	.ilat = ILAT_DEFAULT,
	.pw1 = 0,
	.pw2 = 0,
	.pw3 = 0,
	.pw4 = 0,
	.pw5 = 0
};

void dev_sim_reset(void)
{
	struct ss_device * dev;
	int i;
	int addr;

	for (i = 0; i < SS_DEVICES_MAX; ++i) {
		dev = &ss_dev_tab[i];
		if (i >= SS_MODULES_IDX) {
			addr = i - SS_MODULES_IDX; /* reverse lookup address */
			dev->module = 1; /* is a module */
		} else {
			addr = i;
			dev->module = 0; /* is a sensor */
		}
		dev->addr = addr; /* reverse lookup address */
		if (addr >= 100)
			dev->apen = 1; /* force AP for higher addresses */
		else
			dev->apen = 0;
		dev->model = 0; /* reference to a device model */
		dev->enabled = 0; /* enable device simulation */
		dev->cfg = 0, /* Unconfigured */
		dev->led = 0; /* LED status */
		dev->tst = 0; /* Remote test mode */

		dev->out1 = 0; /* Module output 1 */
		dev->out2 = 0; /* Module output 2 */
		dev->out3 = 0; /* Module output 3 */
		dev->out5 = 0; /* Module output 5 */

		dev->alm = 0; /* Alarm level */
		dev->tbl = 0; /* Trouble level */

		/* current sink configuration */
		dev->tbias = 128; /* time accuracy multiplication factor */
		dev->icfg = ISINK_CURRENT_NOM | ISINK_RATE_NORMAL;
		dev->ipre = IPRE_DEFAULT; /* current sink preenphasis time */
		dev->ilat = ILAT_DEFAULT; /* current sink latency (PW reponse time) */

		dev->grp[0] = 0;
		dev->grp[1] = 0;
		dev->grp[2] = 0;
		dev->grp[3] = 0;

		dev->pw1 = 300;
		dev->pw2 = 300;
		dev->pw3 = 900;
		dev->pw4 = 2200;
		dev->pw5 = 300;

		dev->lvl[0] = 0;
		dev->lvl[1] = 0;
		dev->lvl[2] = 0;
		dev->lvl[3] = 0;
	}
}

void dev_sim_uncofigure_all(void)
{
	dev_sim_reset();
}

#define COMP1_EXTI (1 << 21)
#define COMP2_EXTI (1 << 22)

static unsigned int ap_chksum(unsigned int msg)
{
	unsigned int x;
	unsigned int y;

	x = (msg << 1) & 0xfffe;
	y = (x >> 2) & 0x3333;
	x &= 0x3333;
	x += y;
	x &= 0x3333;
	y = x >> 8;
	x += y;
	x &= 0x0033;
	y = x >> 4;
	x += y;

	return x & 0x0003;
}

/* default latency for current sink in AP mode */
#define DEV_AP_ILAT_DEFAULT 50
/* default current sink preenphasys time in AP mode */
#define DEV_AP_IPRE_DEFAULT 35
/* default current sink pulse time in AP mode */
#define DEV_AP_IPW_DEFAULT 200 
/* default current sink mode */
#define DEV_AP_ICFG_DEFAULT (ISINK_CURRENT_NOM | ISINK_RATE_NORMAL)


/* -------------------------------------------------------------------------
   Current sink state machine
   ------------------------------------------------------------------------- */

enum {
	ISINK_IDLE = 0,
	ISINK_START_WAIT,
	ISINK_LATENCY,
	ISINK_PULSE,
	ISINK_END_WAIT
};

/* -------------------------------------------------------------------------
   Device driver state machine
   ------------------------------------------------------------------------- */
enum {
	DEV_IDLE       = 0,
	DEV_RST        = 1,
	DEV_MSG        = 2,

	DEV_CLIP       = 3, /* clip mode active device */
	DEV_PW1_ISINK  = 4,
	DEV_PW2_ISINK  = 5,
	DEV_PW3_ISINK  = 6,
	DEV_PW4_ISINK  = 7,
	DEV_PW5_ISINK  = 8,

	DEV_AP_HDR     = 10,
	DEV_AP_HDR_OK  = 11,
	DEV_AP_HDR_ERR = 12,
	DEV_AP_ERR_BIT = 13,
	DEV_AP_INT_BIT = 14,
	DEV_AP_CMD     = 15,
	DEV_AP_PA_BIT  = 16,
	DEV_AP_CMD_ERR = 17,
	DEV_AP_ACK_BIT = 18,
	DEV_AP_DATA    = 19
};

/* -------------------------------------------------------------------------
   Advanced Protocol (200 Series)
   ------------------------------------------------------------------------- */

#define AP_POLL_MODE_MASK   0x01e0
#define AP_POLL_MODE_DIRCET 0x0060
#define AP_POLL_MODE_GROUP  0x01a0

/* mttt t111 1uuu u110 */
#define AP_OPC_DIRECT_POLL_MASK 0x61e0 /* 0000 0111 1000 0110 */
#define AP_OPC_DIRECT_POLL      0x01e0 /* 0000 0111 1000 0000 */

#define AP_OPC_RD_UNITS_MASK    0x7fe0 /* 0000 0111 1111 1110 */
#define AP_OPC_RD_PRESENCE      0x5ec0 /* 0000 0110 0111 1010 */
#define AP_OPC_RD_ALM_LATCH_U   0x2ec0 /* 0000 0110 0111 0100 */
#define AP_OPC_RD_TBL_LATCH_U   0x6ec0 /* 0000 0110 0111 0110 */

#define AP_OPC_GROUP_POLL_MASK  0x03fe /* 0111 1111 1100 0000 */
#define AP_OPC_GROUP_POLL       0x01be /* 0111 1101 1000 0000 */

#define AP_OPC_RD_TENS_MASK     0x7ffe /* 0111 1111 1111 1110 */
#define AP_OPC_RD_ALM_LATCH_T   0x7fbe /* 0111 1101 1111 1110 */   
#define AP_OPC_RD_TBL_LATCH_T   0x3fbe /* 0111 1101 1111 1100 */
#define AP_OPC_NULL             0x5fbe /* 0111 1101 1111 1010 */

enum {
	AP_DIRECT_POLL,
	AP_RD_PRESENCE,
	AP_RD_ALM_LATCH_U,
	AP_RD_TBL_LATCH_U,
	AP_GROUP_POLL,
	AP_RD_ALM_LATCH_T,
	AP_RD_TBL_LATCH_T,
	AP_NULL             
};

static void ap_hdr_decode(unsigned int msg)
{
	struct ss_device * dev = slcdev_drv.dev;
	unsigned int sum;
	unsigned int addr;
	unsigned int icfg;
	unsigned int ipre;
	unsigned int ipw;
	unsigned int ilat;
	unsigned int irq;

	icfg = DEV_AP_ICFG_DEFAULT;
	ipre = DEV_AP_IPRE_DEFAULT;
	ipw = DEV_AP_IPW_DEFAULT;
	ilat = DEV_AP_ILAT_DEFAULT;
	irq = 0;

	DCC_LOG(LOG_INFO, "[AP]");
	sum = ap_chksum(msg);

	if (sum != (msg >> 15)) {
		DCC_LOG1(LOG_WARNING, "MSG=%05x checksum error!", msg);

		/* prepare for sinking current */
		/* FIXME: configurable current sink parameters for AP */ 
		isink_mode_set(icfg);
		slcdev_drv.isink.pre = ipre;
		slcdev_drv.isink.pw = ipw;
		slcdev_drv.isink.lat = ilat;
		slcdev_drv.state = DEV_AP_HDR_ERR;
		DCC_LOG(LOG_INFO, "[AP HDR ERR]");
		return;
	}

	/* trigger module */
	if ((msg & slcdev_drv.trig.ap_msk) == slcdev_drv.trig.ap_cmp) {
		slcdev_drv.ev_bmp |= SLC_EV_TRIG;
		trig_out_set();
		__thinkos_flag_signal(SLCDEV_DRV_EV_FLAG);
		trig_out_clr();
	}

	if ((msg & AP_OPC_DIRECT_POLL_MASK) == AP_OPC_DIRECT_POLL) {
		unsigned int lo;
		unsigned int hi;
		unsigned int mod;

		mod = msg & 1; /* Module or sensor */

		hi = (msg >> 1) & 0xf; /* Upper nibble address bits */
		lo = (msg >> 9) & 0xf; /* Lower nibble address bits. */
		addr = 10 * addr_dec_lut[hi] + addr_dec_lut[lo];
		addr = addr + (mod * 160);
		DCC_LOG1(LOG_TRACE, "[AP Direct %d]", addr);

		/* */
		if (addr != slcdev_drv.addr) {
			dev = &ss_dev_tab[addr];
			slcdev_drv.addr = addr;
			slcdev_drv.dev = dev;
		} else {
			DCC_LOG(LOG_INFO, "Consecutive pooling");
		}

		if ((dev->enabled) && (dev->apen)) {
			DCC_LOG2(LOG_INFO, "Simulating %s=%d", 
					 mod ? "MODULE" : "SENSOR", addr);

			/* prepare for sinking current */
			/* program the current sink */ 
			icfg = dev->icfg;
			ilat = dev->ilat;
			ipre = dev->ipre;
			ipw = dev->ap.pw;
			irq = (dev->alm || dev->tbl) ? 1 : 0;

			/* signal the simulator */
			__bit_mem_wr(&slcdev_drv.ev_bmp, SLC_EV_DEV_POLL, 1);  
			__thinkos_flag_signal(SLCDEV_DRV_EV_FLAG);

			/* AP opcode */
			slcdev_drv.ap.insn = AP_DIRECT_POLL;
		} else {
			slcdev_drv.state = DEV_IDLE;
			DCC_LOG1(LOG_INFO, "[%d IDLE]", addr);
			return;
		}
	} else if ((msg & AP_OPC_GROUP_POLL_MASK) == AP_OPC_GROUP_POLL) {
		DCC_LOG(LOG_TRACE, "[AP Group]");
		slcdev_drv.ap.insn = AP_GROUP_POLL;
	} else {
		switch (msg & AP_OPC_RD_TENS_MASK) {
		case AP_OPC_NULL:
			DCC_LOG(LOG_TRACE, "AP Null");
			slcdev_drv.ap.insn = AP_NULL;
			break;
		case AP_OPC_RD_ALM_LATCH_T:
			DCC_LOG(LOG_TRACE, "AP Read Alarm Latch Tens");
			slcdev_drv.ap.insn = AP_RD_ALM_LATCH_T;
			break;
		case AP_OPC_RD_TBL_LATCH_T:
			DCC_LOG(LOG_TRACE, "AP Read Trouble Latch Tens");
			slcdev_drv.ap.insn = AP_RD_TBL_LATCH_T;
			break;
		default:
			switch (msg & AP_OPC_RD_UNITS_MASK) {
			case AP_OPC_RD_PRESENCE:
				DCC_LOG(LOG_TRACE, "AP Read Presence");
				slcdev_drv.ap.insn = AP_RD_PRESENCE;
				break;
			case AP_OPC_RD_ALM_LATCH_U:
				DCC_LOG(LOG_TRACE, "AP Read Alarm Latch Units");
				slcdev_drv.ap.insn = AP_RD_ALM_LATCH_U;
				break;
			case AP_OPC_RD_TBL_LATCH_U:
				DCC_LOG(LOG_TRACE, "AP Read Trouble Latch Units");
				slcdev_drv.ap.insn = AP_RD_TBL_LATCH_U;
				break;
			default:
				DCC_LOG(LOG_WARNING, "AP command invalid");
				slcdev_drv.state = DEV_IDLE;
				DCC_LOG(LOG_INFO, "[IDLE]");
				return;
			}
		}
	}

	isink_mode_set(icfg);
	slcdev_drv.isink.pre = ipre;
	slcdev_drv.isink.pw = ipw;
	slcdev_drv.isink.lat = ilat;
	slcdev_drv.ap.irq = irq; 
	/* skip error report bit */
	slcdev_drv.state = DEV_AP_HDR_OK;
}

static void ap_2nd_phase(void)
{
	switch (slcdev_drv.ap.insn) {
	case AP_DIRECT_POLL:
		slcdev_drv.state = DEV_AP_CMD;
		DCC_LOG(LOG_INFO, "[AP CMD]");
		break;
	case AP_GROUP_POLL:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_NULL:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_RD_ALM_LATCH_T:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_RD_TBL_LATCH_T:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_RD_ALM_LATCH_U:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_RD_TBL_LATCH_U:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	}
}

static void ap_3rd_phase(void)
{
	switch (slcdev_drv.ap.insn) {
	case AP_DIRECT_POLL:
		slcdev_drv.state = DEV_AP_DATA;
		DCC_LOG(LOG_INFO, "[AP DATA]");
		break;
	case AP_GROUP_POLL:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_NULL:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_RD_ALM_LATCH_T:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_RD_TBL_LATCH_T:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_RD_ALM_LATCH_U:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	case AP_RD_TBL_LATCH_U:
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		break;
	}
}


static void ap_cmd_decode(unsigned int msg)
{
//	struct ss_device * dev = slcdev_drv.dev;
	unsigned int cmd;
	unsigned int parity;

	cmd = (msg >> 17) & 0xff;

	parity = parity_lut[msg & 0xf] ^ parity_lut[(cmd >> 4) & 0xf];

	if (parity != (msg >> 26)) {
		DCC_LOG1(LOG_WARNING, "MSG=%04x parity error!", msg);
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		return;
	}

	slcdev_drv.state = DEV_AP_PA_BIT;
	DCC_LOG(LOG_INFO, "[AP PA BIT]");
}

/* -------------------------------------------------------------------------
   Clip Protocol (500 Series)
   ------------------------------------------------------------------------- */

static void clip_msg_decode(unsigned int msg)
{
	struct ss_device * dev = slcdev_drv.dev;
	unsigned int parity;
	unsigned int addr;
	unsigned int mod;
	unsigned int lo;
	unsigned int hi;

	lo = (msg >> 5) & 0xf;
	hi = (msg >> 1) & 0xf;

	if ((lo == 0x03) || (lo == 0x0d)) {
		/* AP protocol */
		slcdev_drv.state = DEV_AP_HDR;
		DCC_LOG(LOG_INFO, "[AP HDR]");
		return;
	}

	parity = parity_lut[msg & 0xf] ^ 
		parity_lut[(msg >> 4) & 0xf] ^ 
		parity_lut[(msg >> 8) & 0xf];

	if (parity != (msg >> 12)) {
		DCC_LOG1(LOG_WARNING, "MSG=%04x parity error!", msg);
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG(LOG_INFO, "[IDLE]");
		return;
	}

	mod = msg & 1;
	addr = 10 * addr_dec_lut[hi] + addr_dec_lut[lo];
	addr = addr + (mod * 160);

	/* */
	dev = &ss_dev_tab[addr];
	if (addr != slcdev_drv.addr) {
		slcdev_drv.addr = addr;
		/* clear the control sequence */
		slcdev_drv.ctls = 0;
	} else {
		DCC_LOG1(LOG_INFO, "ctls=0x%04x", slcdev_drv.ctls);
	}
	slcdev_drv.dev = dev;

	/* trigger module */
	if ((msg & slcdev_drv.trig.msk) == slcdev_drv.trig.cmp) {
		__bit_mem_wr(&slcdev_drv.ev_bmp, SLC_EV_TRIG, 1);  
		trig_out_set();
		__thinkos_flag_signal(SLCDEV_DRV_EV_FLAG);
		trig_out_clr();
		/* */
		DCC_LOG2(LOG_INFO, "Trigger %s %d", 
				 mod ? "MODULE" : "SENSOR", addr);
	}

	if (dev->enabled) {
		unsigned int ctl = (msg >> 9) & 0x7;

		/* shift the control bits into the sequence register */
		slcdev_drv.ctls <<= 3;
		slcdev_drv.ctls |= ctl;

		DCC_LOG2(LOG_INFO, "Simulating %s=%d", 
				 mod ? "MODULE" : "SENSOR", addr);

		/* prepare for sinking current */
		/* program the current sink */ 
		isink_mode_set(dev->icfg);
		slcdev_drv.isink.pre = dev->ipre;
		slcdev_drv.isink.lat = dev->ilat;

		/* start clip mode response */
		slcdev_drv.state = DEV_CLIP;
		DCC_LOG(LOG_INFO, "[CLIP]");

		/* signal the simulator */
		__bit_mem_wr(&slcdev_drv.ev_bmp, SLC_EV_DEV_POLL, 1);  
		__thinkos_flag_signal(SLCDEV_DRV_EV_FLAG);

		/* update poll counter */
		dev->pcnt++;

		if (dev->event != 0) {
			/* if an simulation event is correlated to the device,
			 signal the simulator. */
			__bit_mem_wr(&slcdev_drv.ev_bmp, dev->event, 1);  
			__thinkos_flag_signal(SLCDEV_DRV_EV_FLAG);
		}
	} else {
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG1(LOG_INFO, "[%d IDLE]", addr);
	}
}

/* -------------------------------------------------------------------------
   Device timer interrupt handler
   This is the 1us resolution timer ISR 
   ------------------------------------------------------------------------- */

void stm32_tim10_isr(void)
{
	struct stm32_comp * comp = STM32_COMP;
	struct stm32f_tim * tim = STM32_TIM10;
//	struct ss_device * dev = slcdev_drv.dev;
	uint32_t csr;
	int bit;

	/* Clear timer interrupt flags */
	tim->sr = 0;

	csr = comp->csr;
	if (csr & COMP_CMP1OUT) {
		/* VSLC at 24V (Power Level) */


		/* XXX: this is for debug only */
		if ((slcdev_drv.state != DEV_IDLE) &&
			(slcdev_drv.state != DEV_PW5_ISINK)) {
			DCC_LOG2(LOG_WARNING, "%d: unexpected state: %d", 
					 slcdev_drv.addr, slcdev_drv.state);
		}

		DCC_LOG(LOG_INFO, "[RST] !!!!!!!!");
		slcdev_drv.state = DEV_RST;

		/* XXX: sanity check. Reset the current sink state machine */
		if (slcdev_drv.isink.state != ISINK_IDLE) {
			slcdev_drv.isink.state = ISINK_IDLE;
			DCC_LOG(LOG_INFO, "<ISINK IDLE>");
		}

		return;
	}

	/* ----------------------------------------------------------------
	   Output:
	   ---------------------------------------------------------------- */

	/* Process the current sink state machine  */
	if (slcdev_drv.isink.state == ISINK_LATENCY) {
		isink_pulse(slcdev_drv.isink.pre, slcdev_drv.isink.pw); 
//		trig_out_set();
		slcdev_drv.isink.state = ISINK_PULSE;
		DCC_LOG1(LOG_INFO, "<ISINK PULSE %d us>", slcdev_drv.isink.pw);
	}
#if 0	
	else if (slcdev_drv.isink.state == ISINK_END_WAIT) {
		/* after a valid pulse the line does not return high !!!! */
		slcdev_drv.isink.state = ISINK_IDLE;
		DCC_LOG(LOG_INFO, "<ISINK IDLE>");
	}
#endif
	/* ----------------------------------------------------------------
	   Input
	   ---------------------------------------------------------------- */

	/* read the bit level comparator */
	bit = (csr & COMP_CMP2OUT) ? 1 : 0;

	switch (slcdev_drv.state) {
	case DEV_RST:
		/* reset the device state */
		slcdev_drv.state = DEV_MSG;
		DCC_LOG(LOG_INFO, "[MSG]");
		slcdev_drv.msg = bit;
		slcdev_drv.bit_cnt = 1;
		/* reset the simultaor module */
		slcdev_drv.dev = (struct ss_device *)&null_dev;
		break;

	case DEV_MSG:
		slcdev_drv.msg |= bit << slcdev_drv.bit_cnt;

		if (++slcdev_drv.bit_cnt == 13)
			clip_msg_decode(slcdev_drv.msg);
		break;

	case DEV_AP_HDR:
		/* AP header decoder */
		slcdev_drv.msg |= bit << slcdev_drv.bit_cnt;
		if (++slcdev_drv.bit_cnt == 17)
			ap_hdr_decode(slcdev_drv.msg);
		break;

	case DEV_AP_CMD:
		/* AP header decoder */
		slcdev_drv.msg |= bit << slcdev_drv.bit_cnt;
		if (++slcdev_drv.bit_cnt == 26)
			ap_cmd_decode(slcdev_drv.msg);
		break;
	}
}

/****************************************************************************
 * voltage comparator interrupt handler
 ****************************************************************************/

void stm32_comp_tsc_isr(void)
{
	struct stm32f_exti * exti = STM32_EXTI;
	struct stm32f_tim * tim = STM32_TIM10;
	struct ss_device * dev = slcdev_drv.dev;
	uint32_t ftsr;

	/* get the falling edge sensing configuration */
	ftsr = exti->ftsr;

	/* Invert edge sensing */
	exti->ftsr = ftsr ^ COMP1_EXTI;
	exti->rtsr ^= COMP1_EXTI;
	/* clear interrupt flag */
	exti->pr = COMP1_EXTI;

	tim->cnt = 0;

	if (ftsr & COMP1_EXTI) {
		/*----------------------------------------------------------------
		  VSLC Falling Edge 
		  ---------------------------------------------------------------- */

		/*-----------------------------------------------------------------
		  Output
		  -----------------------------------------------------------------*/

		/* Process the current sink state machine  */
		if (slcdev_drv.isink.state == ISINK_START_WAIT) {
			tim->arr = slcdev_drv.isink.lat;
			slcdev_drv.isink.state = ISINK_LATENCY;
			DCC_LOG(LOG_INFO, "<ISINK LATENCY>");
		} else {

			/*-------------------------------------------------------------
			  Input
			  -------------------------------------------------------------*/
			if (slcdev_drv.state == DEV_IDLE) {
				DCC_LOG(LOG_INFO, "......");
				return;
			}

			/* 150 us time for bit decoding. The bit time specified
			   by System Sensor is a minimum of 250us and a maximum of
			   2000us. The recomended bit value is 300us.
			   Reading the bit at 150us seems to be a good compromise.
			   Although reading a little further could help 
			   with slow or noisy lines...*/
			tim->arr = 150; 
		}

		/* trigger the timer */
		tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
	} else {

		/*----------------------------------------------------------------
		  VSLC Rising Edge 
		  ---------------------------------------------------------------- */

		switch (slcdev_drv.isink.state) {

		case ISINK_PULSE: /* pulse aborted */
			isink_stop();
			/* fall through */
		case ISINK_LATENCY: /* pulse aborted */
//			trig_out_clr();
			slcdev_drv.isink.state = ISINK_IDLE;
			DCC_LOG(LOG_WARNING, "<ISINK IDLE>");
			slcdev_drv.state = DEV_IDLE;
			DCC_LOG(LOG_WARNING, "Pulse aborted! [IDLE]");
			break;

		case ISINK_END_WAIT:
			switch (slcdev_drv.state) {
			case DEV_PW1_ISINK:
				slcdev_drv.isink.pw = (dev->pw2 * dev->tbias) / 128;
				slcdev_drv.isink.state = ISINK_START_WAIT;
				DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				slcdev_drv.state = DEV_PW2_ISINK;
				DCC_LOG(LOG_INFO, "[PW2 ISINK]");
				break;
			case DEV_PW2_ISINK:
				slcdev_drv.isink.pw = (dev->pw3 * dev->tbias) / 128;
				slcdev_drv.isink.state = ISINK_START_WAIT;
				DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				slcdev_drv.state = DEV_PW3_ISINK;
				DCC_LOG(LOG_INFO, "[PW3 ISINK]");
				break;
			case DEV_PW3_ISINK:
				slcdev_drv.isink.pw = (dev->pw4 * dev->tbias) / 128;
				slcdev_drv.isink.state = ISINK_START_WAIT;
				DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				slcdev_drv.state = DEV_PW4_ISINK;
				DCC_LOG(LOG_INFO, "[PW4 ISINK]");
				break;
			case DEV_PW4_ISINK:
				/* Get the PW5 request info from the control bits */
				if ((slcdev_drv.ctls & 2) == 0) {
					slcdev_drv.isink.pw = (dev->pw5 * dev->tbias) / 128;
					slcdev_drv.isink.state = ISINK_START_WAIT;
					DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
					slcdev_drv.state = DEV_PW5_ISINK;
					DCC_LOG(LOG_INFO, "[PW5 ISINK]");
				} else {
					slcdev_drv.isink.state = ISINK_IDLE;
					DCC_LOG(LOG_INFO, "<ISINK IDLE>");
					slcdev_drv.state = DEV_IDLE;
					DCC_LOG1(LOG_INFO, "[%d IDLE]", dev->addr);
				}
				break;
			case DEV_PW5_ISINK:
				slcdev_drv.isink.state = ISINK_IDLE;
				DCC_LOG(LOG_INFO, "<ISINK IDLE>");
				slcdev_drv.state = DEV_IDLE;
				DCC_LOG(LOG_INFO, "[IDLE]");
				break;
			case DEV_AP_ERR_BIT:
				slcdev_drv.isink.state = ISINK_IDLE;
				DCC_LOG(LOG_INFO, "<ISINK IDLE>");
				slcdev_drv.state = DEV_IDLE;
				DCC_LOG(LOG_INFO, "[IDLE]");
				break;
			case DEV_AP_INT_BIT:
				slcdev_drv.isink.state = ISINK_IDLE;
				DCC_LOG(LOG_INFO, "<ISINK IDLE>");
				ap_2nd_phase();
				break;
			case DEV_AP_ACK_BIT:
				slcdev_drv.isink.state = ISINK_IDLE;
				DCC_LOG(LOG_INFO, "<ISINK IDLE>");
				slcdev_drv.state = DEV_IDLE;
				DCC_LOG(LOG_INFO, "[IDLE]");
				break;
			}
			break;

		case ISINK_IDLE:
			switch (slcdev_drv.state) {
			case DEV_IDLE:
//				trig_out_clr();
				DCC_LOG(LOG_MSG, "wait reset 4.5ms");
				break;
			case DEV_CLIP:
				/* respond with PW1 */
				slcdev_drv.isink.pw = (dev->pw1 * dev->tbias) / 128;
				slcdev_drv.isink.state = ISINK_START_WAIT;
				DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				slcdev_drv.state = DEV_PW1_ISINK;
				DCC_LOG(LOG_INFO, "[PW1 ISINK]");
				break;
			case DEV_AP_HDR_ERR:
				/* AP header checksum error respnse bit */
				slcdev_drv.isink.state = ISINK_START_WAIT;
				DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				slcdev_drv.state = DEV_AP_ERR_BIT;
				DCC_LOG(LOG_INFO, "[AP ERR BIT]");
				break;
			case DEV_AP_HDR_OK:
				/* AP header ok. Skip the checksum error respnse bit */
				slcdev_drv.state = DEV_AP_ERR_BIT;
				DCC_LOG(LOG_INFO, "[AP ERR BIT]");
				break;
			case DEV_AP_ERR_BIT:
				/* AP interrupt bit */
				if (slcdev_drv.ap.irq) {
					/* send the interrupt bit */
					slcdev_drv.isink.state = ISINK_START_WAIT;
					DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				}
				slcdev_drv.state = DEV_AP_INT_BIT;
				DCC_LOG(LOG_INFO, "[AP INT BIT]");
				break;
			case DEV_AP_INT_BIT:
				ap_2nd_phase();
				break;
			case DEV_AP_PA_BIT:
				/* AP command parity error respnse bit */
				slcdev_drv.isink.state = ISINK_START_WAIT;
				DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				slcdev_drv.state = DEV_AP_ACK_BIT;
				DCC_LOG(LOG_INFO, "[AP PA BIT]");
				break;
			case DEV_AP_ACK_BIT:
				ap_3rd_phase();
				break;
			}

			break;
		}

		/* set a timer for 4.5ms which will trigger a 
		   software reset if no other event happen before 
		   it times out. */
		tim->arr = 4500; 
		tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
	}
}

/* Current sink pulse timer 
   This ISR is invoked at the end of the current pulse */
void stm32_tim4_isr(void)
{
	struct stm32f_tim * tim4 = STM32_TIM4;
//	struct stm32f_tim * tim10 = STM32_TIM10;

	/* Clear timer interrupt flags */
	tim4->sr = 0;

//	trig_out_clr();

	/* End of current pulse. wait for the SLC line to go up */
	slcdev_drv.isink.state = ISINK_END_WAIT;

	DCC_LOG(LOG_INFO, "<ISINK END WAIT>");

	/* Start a timeout timer */
//	tim10->arr = 4500; 
//	tim10->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
}

#define SLC_TMR_FREQ 1000000

static void slc_sense_init(void)
{
	struct stm32_comp * comp = STM32_COMP;
	struct stm32f_exti * exti = STM32_EXTI;
	struct stm32f_tim * tim = STM32_TIM10;
	uint32_t div;

	stm32_clk_enable(STM32_RCC, STM32_CLK_COMP);

	comp->csr = COMP_OUTSEL_TIM10_CAP1 | COMP_INSEL_VREF_1_4 |
		COMP_SPEED | COMP_CMP1EN | COMP_WNDWE | COMP_VREFOUTEN;

	stm32_gpio_mode(COMP1, ANALOG, 0);
	stm32_gpio_mode(COMP2, ANALOG, 0);

	stm32_clk_enable(STM32_RCC, STM32_CLK_SYSCFG);

	/* CoMP1 set falling edge trigger */
	exti->ftsr |= COMP1_EXTI;
	/* COMP1 clear rising edge trigger */
	exti->rtsr &= ~COMP1_EXTI;
	/* Clear pending flag */
	exti->pr = COMP1_EXTI;
	/* Unmask interrupt */
	exti->imr |= COMP1_EXTI;

	cm3_irq_pri_set(STM32_IRQ_COMP, IRQ_PRIORITY_HIGHEST);

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM10);
	/* get the total divisior */
	div = (stm32f_tim1_hz + (SLC_TMR_FREQ / 2)) / SLC_TMR_FREQ ;
	/* Timer configuration */
	tim->psc = div - 1;
	tim->arr = 2000;
	tim->cnt = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS; 

	cm3_irq_pri_set(STM32_IRQ_TIM10, IRQ_PRIORITY_VERY_HIGH);
}

static void slcdev_reset(void)
{
	slcdev_drv.state = DEV_IDLE;
	slcdev_drv.ev_bmp = 0;
	slcdev_drv.dev = (struct ss_device *)&null_dev;
	slcdev_drv.addr = 0;
	slcdev_drv.isink.state = ISINK_IDLE;

	/* AP configuration */
	slcdev_drv.ap.icfg = DEV_AP_ICFG_DEFAULT;
	slcdev_drv.ap.ilat = DEV_AP_ILAT_DEFAULT;
	slcdev_drv.ap.ipre = DEV_AP_IPRE_DEFAULT;
	slcdev_drv.ap.ipw = DEV_AP_IPW_DEFAULT;
}

void slcdev_init(void)
{
	/* reset the driver state */
	slcdev_reset();

	trig_init();

	slc_sense_init();

	dev_sim_reset();

	DCC_LOG1(LOG_TRACE, "sizeof(ss_dev_tab) = %d bytes.", sizeof(ss_dev_tab));
}

void slcdev_stop(void)
{
	/* Enable interrupt */
	cm3_irq_disable(STM32_IRQ_COMP);
	/* Enable interrupt */
	cm3_irq_disable(STM32_IRQ_TIM10);

	thinkos_flag_clr(SLCDEV_DRV_EV_FLAG);

	/* reset the driver state */
	slcdev_reset();
}

void slcdev_resume(void)
{
	/* Enable interrupt */
	cm3_irq_enable(STM32_IRQ_COMP);
	/* Enable interrupt */
	cm3_irq_enable(STM32_IRQ_TIM10);
}

