/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file hello.c
 * @brief application test
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <sys/serial.h>
#include <sys/dcclog.h>
#include <thinkos.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>

#define IRQ_PRIORITY_HIGHEST   (1 << 5)
#define IRQ_PRIORITY_VERY_HIGH (2 << 5)
#define IRQ_PRIORITY_HIGH      (3 << 5)
#define IRQ_PRIORITY_REGULAR   (4 << 5)
#define IRQ_PRIORITY_LOW       (5 << 5)
#define IRQ_PRIORITY_VERY_LOW  (6 << 5)

#define UART5_TX       STM32_GPIOC, 12
#define UART5_RX       STM32_GPIOD, 2

void print(const char * s)
{
	struct stm32_usart * uart = STM32_UART5;
	char * cp = (char *)s;
	int len;

	while (*cp != '\0')
		cp++;
	len = cp - s;

	stm32_usart_write(uart, s, len);
};

void io_init(void)
{
	struct stm32_usart * uart = STM32_UART5;

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	/* USART5 TX */
	stm32_gpio_mode(UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART5_TX, GPIO_AF8);
	/* USART5 RX */
	stm32_gpio_mode(UART5_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART5_RX, GPIO_AF8);

	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, 115200);
	stm32_usart_mode_set(uart, SERIAL_8N1);
	stm32_usart_enable(uart);
}

/*--------------------------------------------------------------------------
  Fault generators
  --------------------------------------------------------------------------*/

void read_fault(void)
{
	volatile uint32_t * ptr = (uint32_t *)(0x0);
	uint32_t x;
	int i;

	for (i = 0; i < (16 << 4); ++i) {
		x = *ptr;
		(void)x;
		ptr += 0x10000000 / (2 << 4);
	}
}

void write_fault(void)
{
	volatile uint32_t * ptr = (uint32_t *)(0x0);
	uint32_t x = 0;
	int i;

	for (i = 0; i < (16 << 4); ++i) {
		*ptr = x;
		ptr += 0x10000000 / (2 << 4);
	}
}

/*--------------------------------------------------------------------------
  Interrupt handlers  
  --------------------------------------------------------------------------*/

unsigned int irq_count = 0 ;

void stm32f_tim2_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;
	/* Clear timer interrupt flags */
	tim->sr = 0;

	irq_count++;

	print("2");

	if (irq_count == 30)
		read_fault();
}

void stm32f_tim3_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;
	int i;

	/* Clear timer interrupt flags */
	tim->sr = 0;

	for (;;) {
		print("3");
		for (i = 0; i < 10000000; ++i)
			__NOP();
	}
}

void stm32f_tim4_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM4;
	int i;

	/* Clear timer interrupt flags */
	tim->sr = 0;

	for (;;) {
		print("4");
		for (i = 0; i < 4000000; ++i)
			__NOP();
	}
}

void stm32f_tim5_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM5;
	int i;

	/* Clear timer interrupt flags */
	tim->sr = 0;

	for (;;) {
		print("5");
		for (i = 0; i < 4000000; ++i)
			__NOP();
	}
}

/*--------------------------------------------------------------------------
  Timers initialization
  --------------------------------------------------------------------------*/
void stm32_tim_init(struct stm32f_tim * tim, uint32_t period_ms)
{
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = (stm32f_tim1_hz / 1000) * period_ms;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = n - 1;
	tim->cr1 = TIM_CEN; /* Enable counter */
	tim->egr = TIM_UG;
	while (tim->sr == 0);
	tim->sr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
}

void timer2_init(uint32_t period_ms)
{
	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
	/* Initialize timer */
	stm32_tim_init(STM32F_TIM2, period_ms);
	/* configure interrupts */
	cm3_irq_pri_set(STM32F_IRQ_TIM2, IRQ_PRIORITY_VERY_HIGH);
	/* enable interrupts */
	cm3_irq_enable(STM32F_IRQ_TIM2);
}

void timer3_init(uint32_t period_ms)
{
	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM3);
	/* Initialize timer */
	stm32_tim_init(STM32F_TIM3, period_ms);
	/* configure interrupts */
	cm3_irq_pri_set(STM32F_IRQ_TIM3, IRQ_PRIORITY_HIGH);
	/* enable interrupts */
	cm3_irq_enable(STM32F_IRQ_TIM3);
}

void timer4_init(uint32_t period_ms)
{
	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM4);
	/* Initialize timer */
	stm32_tim_init(STM32F_TIM4, period_ms);
	/* configure interrupts */
	cm3_irq_pri_set(STM32F_IRQ_TIM4, IRQ_PRIORITY_REGULAR);
	/* enable interrupts */
	cm3_irq_enable(STM32F_IRQ_TIM4);
}

void timer5_init(uint32_t period_ms)
{
	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM5);
	/* Initialize timer */
	stm32_tim_init(STM32F_TIM5, period_ms);
	/* configure interrupts */
	cm3_irq_pri_set(STM32F_IRQ_TIM5, IRQ_PRIORITY_LOW);
	/* enable interrupts */
	cm3_irq_enable(STM32F_IRQ_TIM5);
}

void monitor_init(void)
{
#if 0
	struct dmon_comm * comm;

	DCC_LOG(LOG_TRACE, "1. usb_comm_init()");
	comm = usb_comm_init(&stm32f_otg_fs_dev);

	DCC_LOG(LOG_TRACE, "2. thinkos_dmon_init()");
	thinkos_dmon_init(comm, monitor_task);
#endif
}

int main(int argc, char ** argv)
{
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	print("\r\n");
	print("--------------------------------------------\r\n");
	print("- Thread Fault Test\r\n");
	print("--------------------------------------------\r\n");

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "3. monitor_init().");
	monitor_init();

	thinkos_sleep(500);

	timer5_init(200);
	timer4_init(400);
	timer3_init(600);
	timer2_init(100);


	for (i = 10; i > 0; --i) {
		print(".");
		thinkos_sleep(500);
	}

	print(" ^^^\r\n");

	read_fault();

	return 0;
}


