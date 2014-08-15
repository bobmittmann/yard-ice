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
#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

struct ss_device ss_dev_tab[SS_DEVICES_MAX];

/* -------------------------------------------------------------------------
 * Trigger module
 * ------------------------------------------------------------------------- */

struct {
	uint8_t enabled;
	uint8_t addr;
} trig;

void trig_addr_set(unsigned int addr)
{
	trig.addr = addr;
}

unsigned int trig_addr_get(void)
{
	return trig.addr;
}

static void trig_init(void)
{
	trig.addr = 0xff;
	trig.enabled = true;

	stm32_gpio_clr(TRIG_OUT);
	stm32_gpio_mode(TRIG_OUT, OUTPUT, PUSH_PULL | SPEED_MED);
}

/* -------------------------------------------------------------------------
 * SLC Device Simulator
 * ------------------------------------------------------------------------- */

struct {
	volatile unsigned int addr;
	struct ss_device * dev;
} scan;

#define PW_RESPONSE_TIME (100 - 20)

const struct ss_device null_dev = {
	.enabled = 0,
	.poll_flash = 0,
	.ap = 0,
	.addr = 0,
	.model = 0,
	.tbias = 128,
	.icfg = 0,
	.ipre = 0,
	.ilat = PW_RESPONSE_TIME,
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

	for (i = 0; i < SS_DEVICES_MAX; ++i) {
		dev = &ss_dev_tab[i];
		dev->enabled = 0;
		dev->poll_flash = 1,
		dev->ap = 0,
		dev->addr = i,
		dev->model = 0,
		dev->tbias = 128,
		dev->icfg = ISINK_CURRENT_NOM | ISINK_RATE_NORMAL;
		dev->ipre = 35; /* preenphasis time */
		dev->ilat = PW_RESPONSE_TIME,
		dev->pw1 = 300;
		dev->pw2 = 300;
		dev->pw3 = 900;
		dev->pw4 = 2200;
		dev->pw5 = 300;
	}

	scan.dev = (struct ss_device *)&null_dev;
	scan.addr = 0;
}

void dev_sim_enable(unsigned int addr)
{
	if (addr > 199) 
		return;

	ss_dev_tab[addr].enabled = true;
}

void dev_sim_disable(unsigned int addr)
{
	if (addr > 199) 
		return;

	ss_dev_tab[addr].enabled = false;
}


/* -------------------------------------------------------------------------
 * SLC Device 
 * ------------------------------------------------------------------------- */

enum {
	DEV_IDLE = 0,
	DEV_MSG,

	DEV_PW1_START_WAIT,
	DEV_PW1_RESPONSE_TIME,
	DEV_PW1_PULSE,
	DEV_PW1_END_WAIT,

	DEV_PW2_START_WAIT,
	DEV_PW2_RESPONSE_TIME,
	DEV_PW2_PULSE,
	DEV_PW2_END_WAIT,

	DEV_PW3_START_WAIT,
	DEV_PW3_RESPONSE_TIME,
	DEV_PW3_PULSE,
	DEV_PW3_END_WAIT,

	DEV_PW4_START_WAIT,
	DEV_PW4_RESPONSE_TIME,
	DEV_PW4_PULSE,
	DEV_PW4_END_WAIT,

	DEV_PW5_START_WAIT,
	DEV_PW5_RESPONSE_TIME,
	DEV_PW5_PULSE,
	DEV_PW5_END_WAIT,
	DEV_RECOVER_TIME,

	DEV_INACTIVE_START_WAIT,
	DEV_INACTIVE_STOP_WAIT,

	DEV_PARITY_ERROR,
	DEV_PW_ABORT

};

struct {
	int8_t flag;
	bool trig_event;
	bool sim_event;
	uint8_t state;
	uint8_t bit_cnt;
	uint16_t msg;
} slcdev;

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

/* Current sink pulse timer */
void stm32_tim4_isr(void)
{
	struct stm32f_tim * tim4 = STM32_TIM4;
	struct stm32f_tim * tim10 = STM32_TIM10;

	/* Clear timer interrupt flags */
	tim4->sr = 0;
	switch (slcdev.state) {
	case DEV_PW1_PULSE:
		slcdev.state = DEV_PW1_END_WAIT;
		DCC_LOG(LOG_INFO, "[PW1 END WAIT]");
		break;

	case DEV_PW2_PULSE:
		slcdev.state = DEV_PW2_END_WAIT;
		DCC_LOG(LOG_INFO, "[PW2 END WAIT]");
		break;

	case DEV_PW3_PULSE:
		slcdev.state = DEV_PW3_END_WAIT;
		DCC_LOG(LOG_INFO, "[PW3 END WAIT]");
		break;

	case DEV_PW4_PULSE:
		slcdev.state = DEV_PW4_END_WAIT;
		DCC_LOG(LOG_INFO, "[PW4 END WAIT]");
		break;

	case DEV_PW5_PULSE:
		slcdev.state = DEV_PW5_END_WAIT;
		DCC_LOG(LOG_INFO, "[PW5 END WAIT]");
		break;

	default:
		DCC_LOG1(LOG_MSG, "Invalid state [%d]!!!", slcdev.state);
	}

	tim10->arr = 4000; /* trigger the end wait timer */
	tim10->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
}

/* This is the 1us resolution timer... */
void stm32_tim10_isr(void)
{
	struct stm32_comp * comp = STM32_COMP;
	struct stm32f_tim * tim = STM32_TIM10;
	struct ss_device * dev = scan.dev;
	uint32_t csr;
	int bit;

	/* Clear timer interrupt flags */
	tim->sr = 0;

	csr = comp->csr;
	if (csr & COMP_CMP1OUT) {
		/* VSLC at 24V (Power Level) */
		/* Power */
		DCC_LOG(LOG_INFO, "[IDLE]");
		/* */
		slcdev.state = DEV_IDLE;
	} else { 

		bit = (csr & COMP_CMP2OUT) ? 1 : 0;

		switch (slcdev.state) {
		case DEV_IDLE:
#if 0
			if (bit) {
				DCC_LOG(LOG_INFO, "HIGH");
			} else {
				DCC_LOG(LOG_INFO, "LOW");
			}
#endif
			/* reset the device state */
			slcdev.state = DEV_MSG;
			slcdev.msg = bit;
			slcdev.bit_cnt = 1;
			/* reset the simultaor module */
			scan.dev = (struct ss_device *)&null_dev;

			break;

		case DEV_MSG:
#if 0
			if (bit) {
				DCC_LOG(LOG_INFO, "HIGH");
			} else {
				DCC_LOG(LOG_INFO, "LOW");
			}
#endif
			slcdev.msg |= bit << slcdev.bit_cnt;

			if (++slcdev.bit_cnt == 13) {
				unsigned int msg = slcdev.msg;
				unsigned int parity;

				parity = parity_lut[msg & 0xf] ^ 
					parity_lut[(msg >> 4) & 0xf] ^ 
					parity_lut[(msg >> 8) & 0xf];

				if (parity == (msg >> 12)) {
					unsigned int addr;
					unsigned int mod;
					mod = msg & 1;
					addr = 10 * addr_lut[(msg >> 1) & 0xf] + 
						addr_lut[(msg >> 5) & 0xf];
					addr = addr + (mod * 100);
					scan.addr = addr;
					scan.dev = &ss_dev_tab[addr];

					if ((addr == trig.addr) && (trig.enabled)) {
						slcdev.trig_event = true;
						trig_out_set();
						__thinkos_flag_signal(slcdev.flag);
						trig_out_clr();
						/* */
						DCC_LOG2(LOG_INFO, "Match %s=%d", 
								 mod ? "MODULE" : "SENSOR", addr);
					}

					if (scan.dev->enabled) {
						DCC_LOG2(LOG_INFO, "Simulating %s=%d", 
								 mod ? "MODULE" : "SENSOR", addr);
						slcdev.state = DEV_PW1_START_WAIT;
						slcdev.sim_event = true;
						__thinkos_flag_signal(slcdev.flag);
						DCC_LOG(LOG_INFO, "[PW1 START WAIT]");
						isink_mode_set(scan.dev->icfg);
					} else {
						slcdev.state = DEV_INACTIVE_START_WAIT;
						DCC_LOG(LOG_INFO, "[INACTIVE WAIT START]");
					}
				} else {
					DCC_LOG1(LOG_WARNING, "MSG=%04x parity error!", msg);
					slcdev.state = DEV_PARITY_ERROR;
					DCC_LOG(LOG_INFO, "[PARITY ERR]");
				}
			}
			break;

		case DEV_PW1_RESPONSE_TIME:
			/* Reference Pulse Width */
			isink_pulse(dev->ipre, dev->pw1); 
			slcdev.state = DEV_PW1_PULSE;
			DCC_LOG1(LOG_INFO, "[PW1 PULSE %d us]", dev->pw1);
			break;

		case DEV_PW2_RESPONSE_TIME:
			/* Remote Test Status */
			isink_pulse(dev->ipre, dev->pw2);
			slcdev.state = DEV_PW2_PULSE;
			DCC_LOG1(LOG_INFO, "[PW2 PULSE %d us]", dev->pw2);
			break;

		case DEV_PW3_RESPONSE_TIME:
			/* Manufacturer Code */
			isink_pulse(dev->ipre, dev->pw3); 
			slcdev.state = DEV_PW3_PULSE;
			DCC_LOG1(LOG_INFO, "[PW3 PULSE %d us]", dev->pw3);
			break;

		case DEV_PW4_RESPONSE_TIME:
			/* Analog */
			isink_pulse(dev->ipre, dev->pw4); 
			slcdev.state = DEV_PW4_PULSE;
			DCC_LOG1(LOG_INFO, "[PW4 PULSE %d us]", dev->pw4);
			break;

		case DEV_PW5_RESPONSE_TIME:
			/* Type Id */
			isink_pulse(dev->ipre, dev->pw5); 
			slcdev.state = DEV_PW5_PULSE;
			DCC_LOG1(LOG_INFO, "[PW5 PULSE %d us]", dev->pw5);
			break;
		}
	}

	trig_out_clr();
}

/* **************************************************************************
 * voltage comparator interrupt handler
 ****************************************************************************/

void stm32_comp_tsc_isr(void)
{
	struct stm32f_exti * exti = STM32_EXTI;
	struct stm32f_tim * tim = STM32_TIM10;
	struct ss_device * dev = scan.dev;
	uint32_t ftsr;

	/* get the falling edge sensing configuration */
	ftsr = exti->ftsr;

	/* Invert edge sensing */
	exti->ftsr = ftsr ^ COMP1_EXTI;
	exti->rtsr ^= COMP1_EXTI;
	/* clear interrupt flag */
	exti->pr = COMP1_EXTI;

	if (ftsr & COMP1_EXTI) {
		/* ********************************************************
		 *  VSLC Falling Edge 
		 *********************************************************/

		tim->cnt = 0;

		switch (slcdev.state) {
		case DEV_PW1_START_WAIT:
			tim->arr = dev->ilat;
			slcdev.state = DEV_PW1_RESPONSE_TIME;
			DCC_LOG(LOG_INFO, "[PW1 RESPONSE_TIME]");
			break;

		case DEV_PW2_START_WAIT:
			tim->arr = dev->ilat;
			slcdev.state = DEV_PW2_RESPONSE_TIME;
			DCC_LOG(LOG_INFO, "[PW2 RESPONSE_TIME]");
			break;

		case DEV_PW3_START_WAIT:
			tim->arr = dev->ilat;
			slcdev.state = DEV_PW3_RESPONSE_TIME;
			DCC_LOG(LOG_INFO, "[PW3 RESPONSE_TIME]");
			break;

		case DEV_PW4_START_WAIT:
			tim->arr = dev->ilat;
			slcdev.state = DEV_PW4_RESPONSE_TIME;
			DCC_LOG(LOG_INFO, "[PW4 RESPONSE_TIME]");
			break;

		case DEV_PW5_START_WAIT:
			tim->arr = dev->ilat;
			slcdev.state = DEV_PW5_RESPONSE_TIME;
			DCC_LOG(LOG_INFO, "[PW5 RESPONSE_TIME]");
			break;

		case DEV_INACTIVE_START_WAIT:
			tim->arr = 4000; /* Inactive PW window */
			slcdev.state = DEV_INACTIVE_STOP_WAIT;
			DCC_LOG(LOG_INFO, "[INACTIVE STOP WAIT]");
			break;

		default:
			tim->arr = 150;
			break;
		}

		/* trigger the timer */
		tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
	} else {

		/* ********************************************************
		 *  VSLC Rising Edge 
		 *********************************************************/

		tim->cnt = 0;

		switch (slcdev.state) {

		case DEV_PW1_END_WAIT:
			slcdev.state = DEV_PW2_START_WAIT;
			DCC_LOG(LOG_INFO, "[PW2 START WAIT]");
			break;

		case DEV_PW2_END_WAIT:
			slcdev.state = DEV_PW3_START_WAIT;
			DCC_LOG(LOG_INFO, "[PW3 START WAIT]");
			break;

		case DEV_PW3_END_WAIT:
			slcdev.state = DEV_PW4_START_WAIT;
			DCC_LOG(LOG_INFO, "[PW4 START WAIT]");
			break;

		case DEV_PW4_END_WAIT:
			slcdev.state = DEV_PW5_START_WAIT;
			DCC_LOG(LOG_INFO, "[PW5 START WAIT]");
			break;

		case DEV_PW5_END_WAIT:
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_INFO, "[RECOVER TIME]");
			break;

		case DEV_PW1_PULSE:
			isink_stop();
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW1 abort! [RECOVER TIME]");
			break;

		case DEV_PW2_PULSE:
			isink_stop();
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW2 abort! [RECOVER TIME]");
			break;

		case DEV_PW3_PULSE:
			isink_stop();
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW3 abort! [RECOVER TIME]");
			break;

		case DEV_INACTIVE_STOP_WAIT:
			slcdev.state = DEV_INACTIVE_START_WAIT;
			DCC_LOG(LOG_INFO, "[INACTIVE WAIT START]");
			break;

		case DEV_PW4_PULSE:
			isink_stop();
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW4 abort! [RECOVER TIME]");
			break;

		case DEV_PW5_PULSE:
			isink_stop();
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW5 abort! [RECOVER TIME]");
			break;

		case DEV_PW1_RESPONSE_TIME:
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW1 guard abort! [RECOVER TIME]");
			break;

		case DEV_PW2_RESPONSE_TIME:
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW2 response abort! [RECOVER TIME]");
			break;

		case DEV_PW3_RESPONSE_TIME:
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW3 guard abort! [RECOVER TIME]");
			break;

		case DEV_PW4_RESPONSE_TIME:
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW4 guard abort! [RECOVER TIME]");
			break;

		case DEV_PW5_RESPONSE_TIME:
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_WARNING, "PW5 guard abort! [RECOVER TIME]");
			break;

		case DEV_PARITY_ERROR:
			tim->cnt = 0;
			tim->arr = 10;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_RECOVER_TIME;
			DCC_LOG(LOG_INFO, "[RECOVER TIME]");
			break;
		}

	}

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
	/* Enable interrupt */
	cm3_irq_enable(STM32_IRQ_COMP);

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
	/* Enable interrupt */
	cm3_irq_enable(STM32_IRQ_TIM10);
}

uint32_t slcdev_event_wait(void)
{
	uint32_t event = 0;

//	tracef("%s(): wait...", __func__);

	do {
		thinkos_flag_wait(slcdev.flag);
		/* FIXME: event queue */
		if (slcdev.trig_event) {
			event = SLC_EV_TRIG;
			slcdev.trig_event = false;
		} else {
			if (slcdev.sim_event) {
				event = SLC_EV_SIM;
				slcdev.sim_event = false;
			}
			thinkos_flag_clr(slcdev.flag);
		} 
	} while (event == 0);

	return event;
}

void slcdev_init(void)
{
	slcdev.state = DEV_IDLE;
	slcdev.sim_event = false;
	slcdev.trig_event = false;
	slcdev.flag = thinkos_flag_alloc();

	trig_init();

	slc_sense_init();

	dev_sim_init();

	DCC_LOG1(LOG_TRACE, "sizeof(ss_dev_tab) = %d bytes.", sizeof(ss_dev_tab));
}

