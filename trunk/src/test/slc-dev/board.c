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
#include <sys/dcclog.h>


enum {
	TRIG_VSLC = 0,
	TRIG_BIT,
	TRIG_ADDR
};

volatile unsigned int dev_addr;
volatile unsigned int dev_sw;
volatile unsigned int trig_mode = TRIG_VSLC;

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
	uint32_t pa;
	uint32_t pb;
	uint32_t pc;
	unsigned int mod;

	/* Clear interrupt flags */
	tim->sr = 0;

	pa = gpioa->idr; 
	pb = gpiob->idr; 
	pc = gpioc->idr; 

	addr = addr_sw_lut[((~pa & (0x1f << 8)) | (~pc & (0x7 << 13))) >> 8];
	/* Sensor/Module Switch */
	mod = (pb >> 5) & 1;
//	mod = 1;
	addr |= mod << 8;

	if (addr != addr_prev) {
		dev_addr = addr;
		DCC_LOG1(LOG_TRACE, "Addr=%d", addr);
	}
	addr_prev = addr;

	sw = (~pb >> 12) & 0xf; 
	if (sw != sw_prev) {
		dev_sw = sw;
		DCC_LOG1(LOG_TRACE, "SW=%d", sw);

		if ((sw & (3 << 0)) == 0) {
			trig_mode = TRIG_VSLC;
			led_off(LED3);
			led_off(LED4);
		} else if (sw & (1 << 0)) {
			led_on(LED3);
			trig_mode = TRIG_BIT;
		} else if (sw & (1 << 1)) {
			led_on(LED4);
			trig_mode = TRIG_ADDR;
		}

		if (sw & (1 << 2))
			led_on(LED5);
		else
			led_off(LED5);

		if (sw & (1 << 3))
			led_on(LED6);
		else
			led_off(LED6);
	}
	sw_prev = sw;

//	DCC_LOG(LOG_TRACE, "TMR");
}

static void io_timer_init(uint32_t freq)
{
	struct stm32f_tim * tim = STM32_TIM9;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((16 * stm32f_apb2_hz) + (freq / 2)) / freq;
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


void io_leds_init(uint32_t freq)
{
	struct stm32f_tim * tim;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);

	stm32_gpio_mode(LED3, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED4, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED5, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED6, OUTPUT, PUSH_PULL | SPEED_LOW);
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

	stm32_gpio_mode(LED1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED2, OUTPUT, PUSH_PULL | SPEED_LOW);
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
	stm32_gpio_mode(VNEG_SW, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(VNEG_SW, GPIO_AF1);
}

void isink_init(void)
{
	stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(SINK4, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IRATE, ANALOG, 0);
	stm32_gpio_clr(SINK1);
	stm32_gpio_clr(SINK2);
	stm32_gpio_clr(SINK3);
	stm32_gpio_clr(SINK4);
	stm32_gpio_clr(IRATE);
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
		stm32_gpio_set(TRIG);

	csr = comp->csr;
	if (csr & COMP_CMP1OUT) {
		/* Power */
//		DCC_LOG(LOG_TRACE, "PWR [IDLE]");
		if (trig_mode == TRIG_ADDR)
			stm32_gpio_clr(TRIG);
		dev_state = DEV_IDLE;
		/* adjust timeout */
		tim->arr = 150;
		return;
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
				DCC_LOG2(LOG_TRACE, "%s=%d", mod ? "MODULE" : "SENSOR",
						 addr);
				addr = addr + (mod << 8);
				if (addr == dev_addr) {
					if (trig_mode == TRIG_ADDR)
						stm32_gpio_set(TRIG);
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
	case DEV_PW2:
	case DEV_PW3:
	case DEV_PW4:
	case DEV_PW5:
		DCC_LOG(LOG_TRACE, "TMO");
		dev_state = DEV_IDLE;
		/* adjust timeout */
		tim->arr = 150;
		break;
	}

}

void stm32_comp_tsc_isr(void)
{
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
			stm32_gpio_clr(TRIG);

		/* disable the timer */
		tim->cnt = 0;
		/* trigger the timer */
		tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 

		switch (dev_state) {
		case DEV_PW1:
			//		DCC_LOG1(LOG_TRACE, "PW1: %d", tim->arr);
			/* Reference Pulse Width */
			dev_state = DEV_PW2;
			tim->arr = 1000;
			break;
		case DEV_PW2:
			//		DCC_LOG1(LOG_TRACE, "PW2: %d", tim->arr);
			/* Remote Test Status */
			dev_state = DEV_PW3;
			tim->arr = 1000;
			break;
		case DEV_PW3:
			//		DCC_LOG1(LOG_TRACE, "PW3: %d", tim->arr);
			/* Manufacturer Code */
			dev_state = DEV_PW4;
			tim->arr = 4000;
			break;
		case DEV_PW4:
			/* Analog */
			DCC_LOG(LOG_TRACE, "Got PW4");
			dev_state = DEV_PW5;
			tim->arr = 4000;
			break;
		case DEV_PW5:
			/* Type Id */
			DCC_LOG(LOG_TRACE, "Got PW5");
			dev_state = DEV_IDLE;
			/* adjust timeout */
			tim->arr = 150;
			break;
		default:
			break;
		}
	} else {
		if (trig_mode == TRIG_VSLC)
			stm32_gpio_set(TRIG);
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

	stm32_gpio_clr(TRIG);
	stm32_gpio_mode(TRIG, OUTPUT, PUSH_PULL | SPEED_LOW);

	io_timer_init(100);
	io_leds_init(10000);
	isink_init();
	slc_sense_init();
}

