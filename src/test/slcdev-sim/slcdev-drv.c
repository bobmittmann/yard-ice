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

/* -------------------------------------------------------------------------
 * Trigger module
 * ------------------------------------------------------------------------- */

void trig_addr_set(bool module, unsigned int addr)
{
	slcdev_drv.trig_msk = 0x1ff;
	if (module)
		slcdev_drv.trig_cmp = addr + 160;
	else
		slcdev_drv.trig_cmp = addr;
}

unsigned int trig_addr_get(void)
{
	return slcdev_drv.trig_cmp;
}

static void trig_init(void)
{
	slcdev_drv.trig_msk = 0xff;
	slcdev_drv.trig_cmp = 0x00;

	stm32_gpio_clr(TRIG_OUT);
	stm32_gpio_mode(TRIG_OUT, OUTPUT, PUSH_PULL | SPEED_MED);
}

const struct ss_device null_dev = {
	.addr = 0, /* reverse lookup address */
	.model = 0, /* reference to a device model */
	.ap = 0, /* advanced protocol */
	.module = 0, /* sensor/module */
	.enabled = 0, /* enable device simulation */
	.led = 0, /* LED status */
	.pw5en = 0, /* PW5 (Type ID) enabled */
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

void dev_sim_init(void)
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
			dev->ap = 1; /* force AP for higher addresses */
		else
			dev->ap = 0;
		dev->model = 0; /* reference to a device model */
		dev->enabled = 0; /* enable device simulation */
		dev->led = 0; /* LED status */
		dev->pw5en = 0; /* PW5 (Type ID) enabled */
		dev->tst = 0; /* Remote test mode */
		dev->tbias = 128;
		dev->icfg = ISINK_CURRENT_NOM | ISINK_RATE_NORMAL;
		dev->ipre = IPRE_DEFAULT; /* preenphasis time */
		dev->ilat = ILAT_DEFAULT;
		dev->pw1 = 300;
		dev->pw2 = 300;
		dev->pw3 = 900;
		dev->pw4 = 2200;
		dev->pw5 = 300;
	}

}

#define COMP1_EXTI (1 << 21)
#define COMP2_EXTI (1 << 22)

static const uint8_t parity_lut[16] = {
/*	0000 0001 0010 0011 0100 0101 0110 0111 */
	   0,   1,   1,   0,   1,   0,   0,   1,
/*	1000 1001 1010 1011 1100 1101 1110 1111 */
	   1,   0,   0,   1,   0,   1,   1,   0,   
};

static const uint8_t addr_lut[16] = {
/*	0000 0001 0010 0011 0100 0101 0110 0111 */
	 0x0, 0x4, 0x1, 0x0, 0x7, 0x0, 0x2, 0x0,
/*	1000 1001 1010 1011 1100 1101 1110 1111 */
	 0x5, 0x9, 0x6, 0x0, 0x8, 0x0, 0x3, 0x0,   
};

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
#define DEV_AP_IPULSE_DEFAULT 200 
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
	DEV_IDLE = 0,
	DEV_RST,
	DEV_MSG,

	DEV_CLIP, /* clip mode active device */
	DEV_PW1_ISINK,
	DEV_PW2_ISINK,
	DEV_PW3_ISINK,
	DEV_PW4_ISINK,
	DEV_PW5_ISINK,

	DEV_AP_HDR,
	DEV_AP_HDR_OK,
	DEV_AP_HDR_ERR,
	DEV_AP_ERR_BIT,
	DEV_AP_INT_BIT,
	DEV_AP_CMD,
};

/* -------------------------------------------------------------------------
   Advanced Protocol (200 Series)
   ------------------------------------------------------------------------- */

#define AP_POLL_MODE_MASK   0x01e0
#define AP_POLL_MODE_DIRCET 0x0060
#define AP_POLL_MODE_GROUP  0x01a0

/* mttt t111 1uuu u110 */
#define AP_DIRECT_POLL_MASK 0x61e0 /* 0000 0111 1000 0110 */
#define AP_DIRECT_POLL      0x01e0 /* 0000 0111 1000 0000 */

#define AP_RD_UNITS_MASK    0x7fe0 /* 0000 0111 1111 1110 */
#define AP_RD_PRESENCE      0x5ec0 /* 0000 0110 0111 1010 */
#define AP_RD_ALM_LATCH_U   0x2ec0 /* 0000 0110 0111 0100 */
#define AP_RD_TBL_LATCH_U   0x6ec0 /* 0000 0110 0111 0110 */

#define AP_GROUP_POLL_MASK  0x03fe /* 0111 1111 1100 0000 */
#define AP_GROUP_POLL       0x01be /* 0111 1101 1000 0000 */

#define AP_RD_TENS_MASK     0x7ffe /* 0111 1111 1111 1110 */
#define AP_RD_ALM_LATCH_T   0x7fbe /* 0111 1101 1111 1110 */   
#define AP_RD_TBL_LATCH_T   0x3fbe /* 0111 1101 1111 1100 */
#define AP_NULL             0x5fbe /* 0111 1101 1111 1010 */

static void ap_hdr_decode(unsigned int msg)
{
	struct ss_device * dev = slcdev_drv.dev;
	unsigned int sum;
	unsigned int lo;
	unsigned int hi;
	unsigned int addr;
	unsigned int mod;

	DCC_LOG(LOG_INFO, "[AP]");
	sum = ap_chksum(msg);

	if (sum != (msg >> 15)) {
		DCC_LOG1(LOG_WARNING, "MSG=%05x checksum error!", msg);

		/* prepare for sinking current */
		/* FIXME: configurable current sink parameters for AP */ 
		isink_mode_set(DEV_AP_ICFG_DEFAULT);
		slcdev_drv.isink.pre = DEV_AP_IPRE_DEFAULT;
		slcdev_drv.isink.pulse = DEV_AP_IPULSE_DEFAULT;

		slcdev_drv.state = DEV_AP_HDR_ERR;
		DCC_LOG(LOG_INFO, "[AP HDR ERR]");
		return;
	}

	/* trigger module */
	if ((msg & slcdev_drv.trig_msk) == slcdev_drv.trig_cmp) {
		slcdev_drv.ev_bmp |= SLC_EV_TRIG;
//		trig_out_set();
		__thinkos_flag_signal(slcdev_drv.ev_flag);
//		trig_out_clr();
		/* */
		DCC_LOG2(LOG_INFO, "Trigger %s %d", 
				 mod ? "MODULE" : "SENSOR", addr);
	}

	mod = msg & 1; /* Module or sensor */

	if ((msg & AP_DIRECT_POLL_MASK) == AP_DIRECT_POLL) {
		hi = (msg >> 1) & 0xf; /* Upper nibble address bits */
		lo = (msg >> 9) & 0xf; /* Lower nibble address bits. */
		addr = 10 * addr_lut[hi] + addr_lut[lo];
		addr = addr + (mod * 160);
		DCC_LOG1(LOG_TRACE, "[AP Direct %d]", addr);
	} else if ((msg & AP_GROUP_POLL_MASK) == AP_GROUP_POLL) {
		DCC_LOG(LOG_TRACE, "[AP Group]");
	} else {
		do {
			switch (msg & AP_RD_TENS_MASK) {
			case AP_NULL:
				DCC_LOG(LOG_TRACE, "[AP Null]");
				continue;
			case AP_RD_ALM_LATCH_T:
				DCC_LOG(LOG_TRACE, "[AP Read Alarm Latch Tens]");
				continue;
			case AP_RD_TBL_LATCH_T:
				DCC_LOG(LOG_TRACE, "[AP Read Trouble Latch Tens]");
				continue;
			}
			switch (msg & AP_RD_UNITS_MASK) {
			case AP_RD_PRESENCE:
				DCC_LOG(LOG_TRACE, "[AP Read Presence]");
				continue;
			case AP_RD_ALM_LATCH_U:
				DCC_LOG(LOG_TRACE, "[AP Read Alarm Latch Units]");
				continue;
			case AP_RD_TBL_LATCH_U:
				DCC_LOG(LOG_TRACE, "[AP Read Trouble Latch Units]");
				continue;
			}
			DCC_LOG(LOG_WARNING, "[AP command invalid]");
			slcdev_drv.state = DEV_AP_HDR_ERR;
			DCC_LOG(LOG_INFO, "[AP HDR ERR]");
			return;
		} while (false);
	}

	hi = (msg >> 1) & 0xf; /* Upper nibble address bits */
	lo = (msg >> 9) & 0xf; /* Lower nibble address bits. */

	addr = 10 * addr_lut[hi] + addr_lut[lo];
	addr = addr + (mod * 160);

	DCC_LOG(LOG_INFO, "[AP CHECKSUM OK]");


	/* */
	if (addr != slcdev_drv.addr) {
		dev = &ss_dev_tab[addr];
		slcdev_drv.addr = addr;
		slcdev_drv.dev = dev;
	} else {
		DCC_LOG(LOG_INFO, "Consecutive pooling");
	}

	if ((dev->enabled) && (dev->ap)) {
		DCC_LOG2(LOG_INFO, "Simulating %s=%d", 
				 mod ? "MODULE" : "SENSOR", addr);

		/* prepare for sinking current */
		/* program the current sink */ 
		isink_mode_set(dev->icfg);
		slcdev_drv.isink.pre = dev->ipre;

		/* skip error report bit */
		slcdev_drv.state = DEV_AP_HDR_OK;

		/* signal the simulator */
		slcdev_drv.ev_bmp |= SLC_EV_SIM;
		__thinkos_flag_signal(slcdev_drv.ev_flag);

	} else {
		slcdev_drv.state = DEV_IDLE;
		DCC_LOG1(LOG_INFO, "[%d IDLE]", addr);
	}
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
	addr = 10 * addr_lut[hi] + addr_lut[lo];
	addr = addr + (mod * 160);

	/* trigger module */
	if ((msg & slcdev_drv.trig_msk) == slcdev_drv.trig_cmp) {
		slcdev_drv.ev_bmp |= SLC_EV_TRIG;
//		trig_out_set();
		__thinkos_flag_signal(slcdev_drv.ev_flag);
//		trig_out_clr();
		/* */
		DCC_LOG2(LOG_INFO, "Trigger %s %d", 
				 mod ? "MODULE" : "SENSOR", addr);
	}

	/* */
	if (addr != slcdev_drv.addr) {
		dev = &ss_dev_tab[addr];
		slcdev_drv.addr = addr;
		slcdev_drv.dev = dev;
		/* clear the control sequence */
		dev->ctls = 0;
	} else {
		DCC_LOG(LOG_INFO, "Consecutive pooling");
	}

	if (dev->enabled) {
		unsigned int ctl = (msg >> 9) & 0x7;

		/* shift the control bits into the sequence register */
		dev->ctls <<= 3;
		dev->ctls |= ctl;

		/* update the PW5 request bit */
		/* This is a hardwired protocol rule,
		   the other control bit sequences are treated in
		   the simulation loop */
		dev->pw5en = (ctl & 2) ? 0 : 1;

		DCC_LOG2(LOG_INFO, "Simulating %s=%d", 
				 mod ? "MODULE" : "SENSOR", addr);

		/* prepare for sinking current */
		/* program the current sink */ 
		isink_mode_set(dev->icfg);
		slcdev_drv.isink.pre = dev->ipre;

		/* start clip mode response */
		slcdev_drv.state = DEV_CLIP;
		DCC_LOG(LOG_INFO, "[CLIP]");

		/* signal the simulator */
		slcdev_drv.ev_bmp |= SLC_EV_SIM;
		__thinkos_flag_signal(slcdev_drv.ev_flag);

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
		if (slcdev_drv.state == DEV_IDLE) {
			DCC_LOG1(LOG_WARNING, "unexpected state: %d", slcdev_drv.state);
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
		isink_pulse(slcdev_drv.isink.pre, slcdev_drv.isink.pulse); 
//		trig_out_set();
		slcdev_drv.isink.state = ISINK_PULSE;
		DCC_LOG1(LOG_INFO, "<ISINK PULSE %d us>", slcdev_drv.isink.pulse);
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
			tim->arr = dev->ilat;
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
				slcdev_drv.isink.pulse = dev->pw2;
				slcdev_drv.isink.state = ISINK_START_WAIT;
				DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				slcdev_drv.state = DEV_PW2_ISINK;
				DCC_LOG(LOG_INFO, "[PW2 ISINK]");
				break;
			case DEV_PW2_ISINK:
				slcdev_drv.isink.pulse = dev->pw3;
				slcdev_drv.isink.state = ISINK_START_WAIT;
				DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				slcdev_drv.state = DEV_PW3_ISINK;
				DCC_LOG(LOG_INFO, "[PW3 ISINK]");
				break;
			case DEV_PW3_ISINK:
				slcdev_drv.isink.pulse = dev->pw4;
				slcdev_drv.isink.state = ISINK_START_WAIT;
				DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				slcdev_drv.state = DEV_PW4_ISINK;
				DCC_LOG(LOG_INFO, "[PW4 ISINK]");
				break;
			case DEV_PW4_ISINK:
				if (dev->pw5en) {
					slcdev_drv.isink.pulse = dev->pw5;
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
				slcdev_drv.state = DEV_AP_CMD;
				DCC_LOG(LOG_INFO, "[AP CMD]");
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
				slcdev_drv.isink.pulse = dev->pw1;
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
				if (dev->irq) {
					/* send the interrupt bit */
					slcdev_drv.isink.state = ISINK_START_WAIT;
					DCC_LOG(LOG_INFO, "<ISINK START WAIT>");
				}
				slcdev_drv.state = DEV_AP_INT_BIT;
				DCC_LOG(LOG_INFO, "[AP INT BIT]");
				break;
			case DEV_AP_INT_BIT:
				slcdev_drv.state = DEV_AP_CMD;
				DCC_LOG(LOG_INFO, "[AP CMD]");
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
}

void slcdev_init(void)
{
	slcdev_drv.ev_flag = thinkos_flag_alloc();

	/* reset the driver state */
	slcdev_reset();

	trig_init();

	slc_sense_init();

	dev_sim_init();

	DCC_LOG1(LOG_TRACE, "sizeof(ss_dev_tab) = %d bytes.", sizeof(ss_dev_tab));
}

void slcdev_stop(void)
{
	/* Enable interrupt */
	cm3_irq_disable(STM32_IRQ_COMP);
	/* Enable interrupt */
	cm3_irq_disable(STM32_IRQ_TIM10);

	thinkos_flag_clr(slcdev_drv.ev_flag);

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

