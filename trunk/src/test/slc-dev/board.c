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

#include "board.h"
#include "isink.h"
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
	clip_dev_tab[103].enabled = true;
	clip_dev_tab[104].enabled = true;
	clip_dev_tab[105].enabled = true;
	clip_dev_tab[106].enabled = true;
}

volatile unsigned int dev_addr;
volatile unsigned int dev_sw;
volatile bool addr_match;
volatile unsigned int trig_mode = TRIG_ADDR;
volatile uint32_t dev_event;

const uint8_t addr_sw_lut[] = {
/*  00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0a, 0b, 0c, 0d,  e, 0f */
	 0, 10, 40, 50, 20, 30, 60, 70, 80, 90,  0,  0,  0,  0,  0, 0,
/*	10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 1a, 1b, 1c, 1d, 1e, 1f */
	 1, 11, 41, 51, 21, 31, 61, 71, 81, 91,  0,  0,  0,  0,  0, 0,
/*	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 2a, 2b, 2c, 2d, 2e, 2f */
	 4, 14, 44, 54, 24, 34, 64, 74, 84, 94,  0,  0,  0,  0,  0, 0,
/*	30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 3a, 3b, 3c, 3d, 3e, 3f */
	 5, 15, 45, 55, 25, 35, 65, 75, 85, 95,  0,  0,  0,  0,  0, 0,
/*	40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 4a, 4b, 4c, 4d, 4e, 4f */
	 2, 12, 42, 52, 22, 32, 62, 72, 82, 92,  0,  0,  0,  0,  0, 0,
/*	50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 5a, 5b, 5c, 5d, 5e, 5f */
	 3, 13, 53, 53, 23, 33, 63, 73, 83, 93,  0,  0,  0,  0,  0, 0,
/*	60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 6a, 6b, 6c, 6d, 6e, 6f */
	 6, 16, 56, 56, 26, 36, 66, 76, 86, 96,  0,  0,  0,  0,  0, 0,
/*	70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 7a, 7b, 7c, 7d, 7e, 7f */
	 7, 17, 47, 57, 27, 37, 67, 77, 87, 97,  0,  0,  0,  0,  0, 0,
/*	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 8a, 8b, 8c, 8d, 8e, 8f */
	 8, 18, 48, 58, 28, 38, 68, 78, 88, 98,  0,  0,  0,  0,  0, 0,
/*	90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 9a, 9b, 9c, 9d, 9e, 9f */
	 9, 19, 92, 59, 29, 39, 69, 79, 89, 99,  0,  0,  0,  0,  0, 0
};


void stm32_tim9_isr(void)
{
	struct stm32f_tim * tim = STM32_TIM9;
	struct stm32_gpio * gpioa = STM32_GPIOA;
	struct stm32_gpio * gpiob = STM32_GPIOB;
	struct stm32_gpio * gpioc = STM32_GPIOC;
	static unsigned int addr_prev;
	static unsigned int sw_prev;
	unsigned int addr;
	unsigned int sw;
	unsigned int d;
	uint32_t pa;
	uint32_t pb;
	uint32_t pc;
	unsigned int mod;

	/* Clear interrupt flags */
	tim->sr = 0;

	pa = gpioa->idr; 
	pb = gpiob->idr; 
	pc = gpioc->idr; 

	/* Rotatory switches decoder */
	addr = addr_sw_lut[((~pa & (0x1f << 8)) | (~pc & (0x7 << 13))) >> 8];
	/* Sensor/Module Switch */
	mod = (pb >> 5) & 1;
	mod = 1;
	addr += mod * 100;

	if (addr != addr_prev) {
		/* Debouncing */
		addr_prev = addr;
	} else if (addr != dev_addr) {
		/* State change */
		dev_addr = addr;
		dev_event_set(EV_ADDR);  
		DCC_LOG1(LOG_INFO, "Addr=%d", addr);
	}

	/* Lever switches */
	sw = (~pb >> 12) & 0xf; 

	if (sw != sw_prev) {
		/* Debouncing */
		sw_prev = sw;
	} if ((d = sw ^ dev_sw) != 0) {
		/* State change */
		dev_sw = sw;

		DCC_LOG1(LOG_INFO, "SW=%d", sw);

		if ((d & SW1_MSK) != 0)
			dev_event_set(EV_SW1);  

		if ((d & SW2_MSK) != 0)
			dev_event_set(EV_SW2);  
	}

//	DCC_LOG(LOG_TRACE, "TMR");
}

#define IO_POLL_FREQ 100

static void io_timer_init(void)
{
	struct stm32f_tim * tim = STM32_TIM9;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = (stm32f_tim1_hz + (IO_POLL_FREQ / 2)) / IO_POLL_FREQ;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM9);
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	cm3_irq_pri_set(STM32_IRQ_TIM9, (4 << 5));
	/* Enable interrupt */
	cm3_irq_enable(STM32_IRQ_TIM9);

	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

#define PWM_FREQ 100000


void io_leds_init(void)
{
	struct stm32f_tim * tim;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = (stm32f_tim1_hz + (PWM_FREQ / 2)) / PWM_FREQ;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);

	stm32_gpio_mode(LED3, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED4, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED5, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED6, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_af(LED3, GPIO_AF2);
	stm32_gpio_af(LED4, GPIO_AF2);
	stm32_gpio_af(LED5, GPIO_AF2);
	stm32_gpio_af(LED6, GPIO_AF2);
	stm32_gpio_set(LED3);
	stm32_gpio_set(LED4);
	stm32_gpio_set(LED5);
	stm32_gpio_set(LED6);

	/* Timer clock enable */
	tim = STM32_TIM3;
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM3);
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->ccmr1 = TIM_OC1M_PWM_MODE1 | TIM_OC2M_PWM_MODE1;
	tim->ccmr2 = TIM_OC3M_PWM_MODE1 | TIM_OC4M_PWM_MODE1;
	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;
	tim->ccr1 = (tim->arr * 15) / 16;
	tim->ccr2 = (tim->arr * 15) / 16;
	tim->ccr3 = (tim->arr * 15) / 16;
	tim->ccr4 = (tim->arr * 15) / 16;
	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */

	stm32_gpio_mode(LED1, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED2, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_af(LED1, GPIO_AF1);
	stm32_gpio_af(LED2, GPIO_AF1);
	stm32_gpio_set(LED1);
	stm32_gpio_set(LED2);

	/* Timer clock enable */
	tim = STM32_TIM2;
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->ccmr1 = TIM_OC1M_PWM_MODE1 | TIM_OC2M_PWM_MODE1;
	tim->ccmr2 = TIM_OC3M_PWM_MODE1 | TIM_OC4M_PWM_MODE1;
	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;
	tim->ccr1 = (tim->arr * 4) / 8;
	tim->ccr2 = (tim->arr * 4) / 8;
	tim->ccr3 = (tim->arr * 14) / 16; /* White LED */
	tim->ccr4 = (tim->arr * 10) / 16; /* Blue LED */
	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */

	/* Negative voltage supply */
	stm32_gpio_mode(VNEG_SW, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(VNEG_SW, GPIO_AF1);
}


#define COMP1_EXTI (1 << 21)
#define COMP2_EXTI (1 << 22)

enum {
	DEV_IDLE = 0,
	DEV_WAIT,
	DEV_MSG,
	DEV_PW1,
	DEV_PW2,
	DEV_PW3,
	DEV_PW4,
	DEV_PW5
};

int dev_state = DEV_IDLE;
int bit_cnt;
int dev_msg;

uint8_t parity_lut[16] = {
/*	0000 0001 0010 0011 0100 0101 0110 0111 */
	   0,   1,   1,   0,   1,   0,   0,   1,
/*	1000 1001 1010 1011 1100 1101 1110 1111 */
	   1,   0,   0,   1,   0,   1,   1,   0,   
};

uint8_t addr_lut[16] = {
/*	0000 0001 0010 0011 0100 0101 0110 0111 */
	 0x0, 0x4, 0x1, 0x0, 0x7, 0x0, 0x2, 0x0,
/*	1000 1001 1010 1011 1100 1101 1110 1111 */
	 0x5, 0x9, 0x6, 0x0, 0x8, 0x0, 0x3, 0x0,   
};

void stm32_tim10_isr(void)
{
	struct stm32_comp * comp = STM32_COMP;
	struct stm32f_tim * tim = STM32_TIM10;
	uint32_t csr;
	int bit;

	/* Clear timer interrupt flags */
	tim->sr = 0;

	if (trig_mode == TRIG_BIT)
		trig_out_set();

	csr = comp->csr;
	if (csr & COMP_CMP1OUT) {
		/* Power */
//		DCC_LOG(LOG_TRACE, "PWR [IDLE]");
		goto idle;
	} 
	
	bit = (csr & COMP_CMP2OUT) ? 1 : 0;

	switch (dev_state) {
	case DEV_IDLE:
#if 0
		if (bit) {
			DCC_LOG(LOG_TRACE, "HIGH");
		} else {
			DCC_LOG(LOG_TRACE, "LOW");
		}
#endif
		dev_state = DEV_MSG;
		dev_msg = bit;
		bit_cnt = 1;
		addr_match = false;
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
		dev_msg |= bit << bit_cnt;

		if (++bit_cnt == 13) {
			unsigned int msg = dev_msg;
			int parity;
			parity = parity_lut[msg & 0xf] ^ 
				parity_lut[(msg >> 4) & 0xf] ^ 
				parity_lut[(msg >> 8) & 0xf];
			if (parity == (msg >> 12)) {
				int addr;
				int mod;
				mod = msg & 1;
				addr = 10 * addr_lut[(msg >> 1) & 0xf] + 
					addr_lut[(msg >> 5) & 0xf];
				addr = addr + (mod * 100);
				scan.addr = addr;
				scan.dev = &clip_dev_tab[addr];

				if (addr == dev_addr) {
					addr_match = true;
					/* */
					DCC_LOG2(LOG_TRACE, "Match %s=%d", 
							 mod ? "MODULE" : "SENSOR", addr);
				}
			} else {
				DCC_LOG1(LOG_WARNING, "MSG=%04x parity error!", msg);
			}
			dev_state = DEV_PW1;
			/* adjust timeout */
			tim->arr = 500;
		}
		break;
	case DEV_PW1:
		DCC_LOG(LOG_TRACE, "PW1 TMO");
		goto idle;
	case DEV_PW2:
		DCC_LOG(LOG_TRACE, "PW2 TMO");
		goto idle;
	case DEV_PW3:
		DCC_LOG(LOG_TRACE, "PW3 TMO");
		goto idle;
	case DEV_PW4:
		DCC_LOG(LOG_TRACE, "PW4 TMO");
		goto idle;
	case DEV_PW5:
		DCC_LOG(LOG_TRACE, "PW5 TMO");
		goto idle;
	}

	return;

idle:
	dev_state = DEV_IDLE;
	/* adjust timeout */
	tim->arr = 150;
	if (trig_mode == TRIG_ADDR)
		trig_out_clr();

}
void stm32_comp_tsc_isr(void)
{
	struct clip_device * dev = scan.dev;
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
		if (trig_mode <= TRIG_BIT)
			trig_out_clr();

		/* disable the timer */
		tim->cnt = 0;

		dev->enabled = true;
		dev->pw2 = 300;
		dev->pw3 = 900;
		dev->pw4 = 2200;
		dev->pw5 = 300;


		switch (dev_state) {
		case DEV_PW1:
			//		DCC_LOG1(LOG_TRACE, "PW1: %d", tim->arr);
			/* Reference Pulse Width */
			if ((addr_match) && (trig_mode == TRIG_ADDR))
				trig_out_set();

			if (dev->enabled) {
				isink_pulse(35, dev->pw1); /* reference pulse */
				tim->arr = 1000;
			} else {
				tim->arr = 1000;
			}
			dev_state = DEV_PW2;
			break;
		case DEV_PW2:
			//		DCC_LOG1(LOG_TRACE, "PW2: %d", tim->arr);
			/* Remote Test Status */
			if (dev->enabled) {
				isink_pulse(35, dev->pw2);
				tim->arr = 1000;
			} else {
				tim->arr = 1000;
			}
			dev_state = DEV_PW3;
			break;
		case DEV_PW3:
			//		DCC_LOG1(LOG_TRACE, "PW3: %d", tim->arr);
			/* Manufacturer Code */
			if (dev->enabled) {
				isink_pulse(35, dev->pw3);
				tim->arr = 4000;
			} else {
				tim->arr = 4000;
			}
			dev_state = DEV_PW4;
			break;
		case DEV_PW4:
			/* Analog */
			DCC_LOG1(LOG_TRACE, "%d: Got PW4", scan_addr & 0xff);
			if (dev->enabled) {
				isink_pulse(35, dev->pw4);
				tim->arr = dev->pw5 + 500;
			} else {
				tim->arr = 4000;
			}
			dev_state = DEV_PW5;
			break;
		case DEV_PW5:
			/* Type Id */
			DCC_LOG1(LOG_TRACE, "%d: Got PW5", scan_addr & 0xff);
			/* adjust timeout */
			if (dev->enabled) {
				isink_pulse(35, dev->pw5);
				tim->arr = dev->pw5 + 150;
			} else {
				tim->arr = 150;
			}
			break;
		default:
			break;
		}

		/* trigger the timer */
		tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
	} else {
		if (trig_mode == TRIG_VSLC)
			trig_out_set();

		if (dev_state >= DEV_PW2) {
			/* disable the timer */
//			tim->cnt = 0;
//			tim->arr = 50;
			/* trigger the timer */
//			tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
		}
	}
}

#define SLC_TMR_FREQ 1000000

void slc_sense_init(void)
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

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);

	stm32_gpio_mode(ADDR0, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR1, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR2, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR3, INPUT, PULL_UP | SPEED_LOW);

	stm32_gpio_mode(ADDR4, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR5, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR6, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR7, INPUT, PULL_UP | SPEED_LOW);

	stm32_gpio_mode(SW3A, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(SW3B, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(SW4A, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(SW4B, INPUT, PULL_UP | SPEED_LOW);

#if 0
	/* JTAG TRST Pin */
	stm32_gpio_mode(MODSW, INPUT, SPEED_LOW);
#endif

	/* USART2_TX */
	stm32_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(USART2_TX, GPIO_AF7);
	/* USART2_RX */
	stm32_gpio_mode(USART2_RX, INPUT, 0);
	stm32_gpio_af(USART2_RX, GPIO_AF7);

	stm32_gpio_clr(TRIG_OUT);
	stm32_gpio_mode(TRIG_OUT, OUTPUT, PUSH_PULL | SPEED_MED);

	io_timer_init();
	io_leds_init();
	slc_sense_init();
	dev_sim_init();
}

