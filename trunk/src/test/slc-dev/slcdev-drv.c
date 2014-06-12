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

struct clip_device {
	uint8_t addr;
	uint8_t enabled;
	uint16_t pw1;
	uint16_t pw2;
	uint16_t pw3;
	uint16_t pw4;
	uint16_t pw5;
};

#define CLIP_DEV_MAX 200

struct clip_device clip_dev_tab[CLIP_DEV_MAX];

/* -------------------------------------------------------------------------
 * Trigger module
 * ------------------------------------------------------------------------- */

enum {
	TRIG_IDLE = 0,
	TRIG_VSLC,
	TRIG_BIT,
	TRIG_ADDR_MATCH,
	TRIG_PW_WAIT,
	TRIG_ADDR_WAIT
};

struct {
	uint8_t mode;
	uint8_t state;
	uint8_t addr;
} trig;

void trig_addr_set(unsigned int addr)
{
	trig.addr = addr;
}

void trig_mode_set(unsigned int mode)
{
	trig.mode = mode;
}

static void trig_init(void)
{
	trig.mode = TRIG_MODE_MATCH;
	trig.addr = 0xff;
	trig.state = TRIG_IDLE;

	stm32_gpio_clr(TRIG_OUT);
	stm32_gpio_mode(TRIG_OUT, OUTPUT, PUSH_PULL | SPEED_MED);
}

/* -------------------------------------------------------------------------
 * SLC Device Simulator
 * ------------------------------------------------------------------------- */

struct {
	volatile unsigned int addr;
	struct clip_device * dev;
} scan;

const struct clip_device null_dev = {
	.enabled = false,
	.pw1 = 0,
	.pw2 = 0,
	.pw3 = 0,
	.pw4 = 0,
	.pw5 = 0
};

void dev_sim_init(void)
{
	struct clip_device * dev;
	int i;

	for (i = 0; i < CLIP_DEV_MAX; ++i) {
		dev = &clip_dev_tab[i];
		dev->enabled = false;
		dev->pw1 = 300;
		dev->pw2 = 300;
		dev->pw3 = 900;
		dev->pw4 = 2200;
		dev->pw5 = 300;
	}

	scan.dev = (struct clip_device *)&null_dev;
	scan.addr = 0;

	clip_dev_tab[102].enabled = true;
//	clip_dev_tab[103].enabled = true;
//	clip_dev_tab[104].enabled = true;
//	clip_dev_tab[105].enabled = true;
//	clip_dev_tab[106].enabled = true;
}


/* -------------------------------------------------------------------------
 * SLC Device 
 * ------------------------------------------------------------------------- */

enum {
	DEV_IDLE = 0,
	DEV_MSG,

	DEV_WAIT_FOR_PW1,
	DEV_PW1_GUARD,
	DEV_PW1_PULSE,
	DEV_PW1_END_WAIT,

	DEV_WAIT_FOR_PW2,
	DEV_PW2_GUARD,
	DEV_PW2_PULSE,
	DEV_PW2_END_WAIT,

	DEV_WAIT_FOR_PW3,
	DEV_PW3_GUARD,
	DEV_PW3_PULSE,
	DEV_PW3_END_WAIT,

	DEV_WAIT_FOR_PW4,
	DEV_PW4_GUARD,
	DEV_PW4_PULSE,
	DEV_PW4_END_WAIT,

	DEV_WAIT_FOR_PW5,
	DEV_PW5_GUARD,
	DEV_PW5_PULSE,
	DEV_PW5_END_WAIT,

	DEV_WAIT_START,
	DEV_WAIT_STOP

};

struct {
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

/* Current sink pulse */
void stm32_tim4_isr(void)
{
	struct stm32f_tim * tim4 = STM32_TIM4;
	struct stm32f_tim * tim10 = STM32_TIM10;

	/* Clear timer interrupt flags */
	tim4->sr = 0;

	switch (slcdev.state) {
	case DEV_PW1_PULSE:
		/* trigger the gaurd timer */
		tim10->arr = 300;
		tim10->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
		slcdev.state = DEV_PW1_END_WAIT;
		DCC_LOG(LOG_TRACE, "[PW1 END WAIT]");
		break;

	case DEV_PW2_PULSE:
		tim10->arr = 300;
		tim10->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
		slcdev.state = DEV_PW2_END_WAIT;
		DCC_LOG(LOG_TRACE, "[PW2 END WAIT]");
		break;

	case DEV_PW3_PULSE:
		tim10->arr = 300;
		tim10->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
		slcdev.state = DEV_PW3_END_WAIT;
		DCC_LOG(LOG_TRACE, "[PW3 END WAIT]");
		break;

	case DEV_PW4_PULSE:
		tim10->arr = 300;
		tim10->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
		slcdev.state = DEV_PW4_END_WAIT;
		DCC_LOG(LOG_TRACE, "[PW4 END WAIT]");
		break;

	case DEV_PW5_PULSE:
		/* trigger the gaurd timer */
		tim10->arr = 300;
		tim10->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
		slcdev.state++;
		DCC_LOG(LOG_TRACE, "...");
		break;
	default:
		DCC_LOG1(LOG_WARNING, "Invalid state [%d]!!!", slcdev.state);
		break;
	}

}

void stm32_tim10_isr(void)
{
	struct stm32_comp * comp = STM32_COMP;
	struct stm32f_tim * tim = STM32_TIM10;
	struct clip_device * dev = scan.dev;
	uint32_t csr;
	int bit;

	/* Clear timer interrupt flags */
	tim->sr = 0;

	if (trig.state == TRIG_BIT)
		trig_out_set();

	csr = comp->csr;
	if (csr & COMP_CMP1OUT) {
		/* Power */
//		DCC_LOG(LOG_TRACE, "PWR [IDLE]");
		goto idle;
	} 
	
	bit = (csr & COMP_CMP2OUT) ? 1 : 0;

	switch (slcdev.state) {
	case DEV_IDLE:
#if 0
		if (bit) {
			DCC_LOG(LOG_TRACE, "HIGH");
		} else {
			DCC_LOG(LOG_TRACE, "LOW");
		}
#endif
		/* reset the device state */
		slcdev.state = DEV_MSG;
		slcdev.msg = bit;
		slcdev.bit_cnt = 1;
		/* reset the simultaor module */
		scan.dev = (struct clip_device *)&null_dev;

		break;
	case DEV_MSG:
#if 0
		if (bit) {
			DCC_LOG(LOG_TRACE, "HIGH");
		} else {
			DCC_LOG(LOG_TRACE, "LOW");
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
				scan.dev = &clip_dev_tab[addr];

				if ((addr == trig.addr) && (trig.state = TRIG_ADDR_WAIT)) {
					trig.state = TRIG_ADDR_MATCH;
					/* */
					DCC_LOG2(LOG_TRACE, "Match %s=%d", 
							 mod ? "MODULE" : "SENSOR", addr);
				}

				if (scan.dev->enabled) {
					DCC_LOG2(LOG_TRACE, "Simulating %s=%d", 
							 mod ? "MODULE" : "SENSOR", addr);
					slcdev.state = DEV_WAIT_FOR_PW1;
					DCC_LOG(LOG_TRACE, "[DEV WAIT FOR PW1]");
				} else {
					slcdev.state = DEV_WAIT_START;
					DCC_LOG(LOG_TRACE, "[WAIT START]");
				}
			} else {
				DCC_LOG1(LOG_WARNING, "MSG=%04x parity error!", msg);
				slcdev.state = DEV_WAIT_START;
				DCC_LOG(LOG_TRACE, "[WAIT START]");
			}
			/* adjust timeout */
			tim->arr = 500;
		}
		break;

	case DEV_PW1_GUARD:
		/* Reference Pulse Width */
		isink_pulse(35, dev->pw1); 
		slcdev.state = DEV_PW1_PULSE;
		DCC_LOG1(LOG_TRACE, "[PW1 PULSE %d us]", dev->pw1);
		break;

	case DEV_PW2_GUARD:
		/* Remote Test Status */
		isink_pulse(35, dev->pw2);
		slcdev.state = DEV_PW2_PULSE;
		DCC_LOG1(LOG_TRACE, "[PW2 PULSE %d us]", dev->pw2);
		break;

	case DEV_PW3_GUARD:
		/* Manufacturer Code */
		isink_pulse(35, dev->pw3); 
		slcdev.state = DEV_PW3_PULSE;
		DCC_LOG1(LOG_TRACE, "[PW3 PULSE %d us]", dev->pw3);
		break;

	case DEV_PW4_GUARD:
		/* Analog */
		isink_pulse(35, dev->pw4); 
		DCC_LOG1(LOG_TRACE, "[PW4 PULSE %d us]", dev->pw4);
		slcdev.state = DEV_PW4_PULSE;
		break;

	case DEV_PW5_GUARD:
		/* Type Id */
		DCC_LOG1(LOG_TRACE, "PW5: %d", dev->pw5);
		isink_pulse(35, dev->pw5); 
		DCC_LOG(LOG_TRACE, "PW5 END");
		slcdev.state = DEV_PW5_PULSE;
		break;;

	case DEV_PW1_END_WAIT:
		DCC_LOG(LOG_TRACE, "PW1 TMO");
		goto idle;
	case DEV_PW2_END_WAIT:
		DCC_LOG(LOG_TRACE, "PW2 TMO");
		goto idle;
	case DEV_PW3_END_WAIT:
		DCC_LOG(LOG_TRACE, "PW3 TMO");
		goto idle;
	case DEV_PW4_END_WAIT:
		DCC_LOG(LOG_TRACE, "PW4 TMO");
		goto idle;
	case DEV_PW5_END_WAIT:
		DCC_LOG(LOG_TRACE, "PW5 TMO");
		goto idle;
	}

	return;

idle:
	slcdev.state = DEV_IDLE;
	/* adjust timeout */
	tim->arr = 150;

	if (trig.state == TRIG_ADDR_MATCH)
		trig_out_clr();

	/* reset the trigger module */
	switch (trig.mode) {
	case TRIG_MODE_MATCH:
		trig.state = TRIG_ADDR_WAIT;
		break;
	case TRIG_MODE_VSLC:
		trig.state = TRIG_VSLC;
		break;
	case TRIG_MODE_BIT:
		trig.state = TRIG_BIT;
		break;
	case TRIG_MODE_PW:
		trig.state = TRIG_PW_WAIT;
		break;
	default: 
		trig.state = TRIG_IDLE;
		break;
	}
}

#define PW_RESPONSE_TIME 100

void stm32_comp_tsc_isr(void)
{
//	struct clip_device * dev = scan.dev;
	struct stm32f_exti * exti = STM32_EXTI;
	struct stm32f_tim * tim = STM32_TIM10;
	uint32_t ftsr;

	/* get the falling edge sensing configuration */
	ftsr = exti->ftsr;

	/* Invert edge sensing */
	exti->ftsr = ftsr ^ COMP1_EXTI;
	exti->rtsr ^= COMP1_EXTI;
	/* clear interrupt flag */
	exti->pr = COMP1_EXTI;

	if (ftsr & COMP1_EXTI) {
		/* Falling Edge */
		if (trig.state <= TRIG_BIT)
			trig_out_clr();

		/* disable the timer */
		tim->cnt = 0;

		switch (slcdev.state) {
		case DEV_WAIT_FOR_PW1:
			tim->arr = PW_RESPONSE_TIME;
			slcdev.state = DEV_PW1_GUARD;
			DCC_LOG(LOG_TRACE, "[PW1 GUARD]");
			if (trig.state == TRIG_ADDR_MATCH)
				trig_out_set();
			break;

		case DEV_WAIT_FOR_PW2:
			tim->arr = PW_RESPONSE_TIME;
			slcdev.state = DEV_PW2_GUARD;
			DCC_LOG(LOG_TRACE, "[PW2 GUARD]");
			if (trig.state == TRIG_ADDR_MATCH)
				trig_out_set();
			break;

		case DEV_WAIT_FOR_PW3:
			tim->arr = PW_RESPONSE_TIME;
			slcdev.state = DEV_PW3_GUARD;
			DCC_LOG(LOG_TRACE, "[PW3 GUARD]");
			if (trig.state == TRIG_ADDR_MATCH)
				trig_out_set();
			break;

		case DEV_WAIT_FOR_PW4:
			tim->arr = PW_RESPONSE_TIME;
			slcdev.state = DEV_PW4_GUARD;
			DCC_LOG(LOG_TRACE, "[PW4 GUARD]");
			if (trig.state == TRIG_ADDR_MATCH)
				trig_out_set();
			break;

		case DEV_WAIT_FOR_PW5:
			tim->arr = PW_RESPONSE_TIME;
			slcdev.state = DEV_PW5_GUARD;
			DCC_LOG(LOG_TRACE, "[PW5 GUARD]");
			if (trig.state == TRIG_ADDR_MATCH)
				trig_out_set();
			break;

		case DEV_WAIT_START:
			tim->arr = 1000;
			slcdev.state = DEV_WAIT_STOP;
			DCC_LOG(LOG_TRACE, "[WAIT STTOP]");
			if (trig.state == TRIG_ADDR_MATCH)
				trig_out_set();
			break;

		default:
			break;
		}

		/* trigger the timer */
		tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
	} else {

		/* disable the timer */
		tim->cnt = 0;

		/* Rising Edge */
		if (trig.state == TRIG_VSLC)
			trig_out_set();

		switch (slcdev.state) {

		case DEV_WAIT_STOP:
			tim->cr1 = 0;
			slcdev.state = DEV_IDLE;
			DCC_LOG(LOG_TRACE, "[IDLE]");
			break;

		case DEV_PW1_END_WAIT:
			tim->arr = 500;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_WAIT_FOR_PW2;
			DCC_LOG(LOG_TRACE, "[WAIT FOR PW2]");
			break;

		case DEV_PW2_END_WAIT:
			tim->arr = 500;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_WAIT_FOR_PW3;
			DCC_LOG(LOG_TRACE, "[WAIT FOR PW3]");
			break;

		case DEV_PW3_END_WAIT:
			tim->arr = 500;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_WAIT_FOR_PW4;
			DCC_LOG(LOG_TRACE, "[WAIT FOR PW4]");
			break;

		case DEV_PW4_END_WAIT:
			tim->arr = 500;
			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			slcdev.state = DEV_WAIT_FOR_PW5;
			DCC_LOG(LOG_TRACE, "[WAIT FOR PW5]");
			break;

		case DEV_PW5_END_WAIT:
			/* disable the timer */
			tim->cr1 = 0;
			slcdev.state = DEV_IDLE;
			DCC_LOG(LOG_TRACE, "[IDLE]");
//			tim->arr = 50;
			/* trigger the timer */
//			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
			break;

		case DEV_PW1_PULSE:
			DCC_LOG(LOG_WARNING, "PW1 pulse abort!");
			slcdev.state = DEV_IDLE;
			isink_stop();
			break;
		case DEV_PW2_PULSE:
			DCC_LOG(LOG_WARNING, "PW2 pulse abort!");
			slcdev.state = DEV_IDLE;
			isink_stop();
			break;
		case DEV_PW3_PULSE:
			DCC_LOG(LOG_WARNING, "PW3 pulse abort!");
			slcdev.state = DEV_IDLE;
			isink_stop();
			break;
		case DEV_PW4_PULSE:
			DCC_LOG(LOG_WARNING, "PW4 pulse abort!");
			slcdev.state = DEV_IDLE;
			isink_stop();
			break;
		case DEV_PW5_PULSE:
			DCC_LOG(LOG_WARNING, "PW5 pulse abort!");
			slcdev.state = DEV_IDLE;
			isink_stop();
			break;

		case DEV_PW1_GUARD:
		case DEV_PW2_GUARD:
		case DEV_PW3_GUARD:
		case DEV_PW4_GUARD:
		case DEV_PW5_GUARD:
			DCC_LOG(LOG_WARNING, "PW guard abort!");
			slcdev.state = DEV_IDLE;
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

	cm3_irq_pri_set(STM32_IRQ_COMP, (4 << 5));
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

	cm3_irq_pri_set(STM32_IRQ_TIM10, (4 << 5));
	/* Enable interrupt */
	cm3_irq_enable(STM32_IRQ_TIM10);
}

void slcdev_init(void)
{
	slcdev.state = DEV_IDLE;

	trig_init();

	slc_sense_init();

	dev_sim_init();
}

