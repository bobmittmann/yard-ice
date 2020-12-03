
/** 
 * @file timer.c
 * @brief application test
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <sys/stm32f.h>
#include <sys/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <thinkos.h>

#include "board.h"

static void tim6_init(uint32_t freq)
{
	struct stm32f_tim *tim = STM32F_TIM6;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((stm32f_tim1_hz + (freq / 2)) / freq);
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM6);

	tim->cr1 = 0;		/* Disable the counter */
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = TIM_UIE;	/* Update interrupt enable */
	tim->ccmr1 = 0;
	tim->ccr1 = 0;
	tim->cr2 = TIM_MMS_UPDATE;
	tim->sr = 0;
	tim->cr1 = TIM_URS | TIM_CEN;	/* Enable counter */

    thinkos_irq_priority_set(STM32F_IRQ_TIM6, IRQ_PRIORITY_HIGHEST);
//	thinkos_irq_priority_set(STM32_IRQ_TIM6, 0);
}

volatile uint32_t irq_count = 0;

int timer_task(uint32_t freq)
{
	struct stm32f_tim *tim = STM32F_TIM6;

	printf("<%d> Starting timer test...!\n", thinkos_thread_self());
	printf("Frequency: %d Hz!\n", freq);

	tim6_init(freq);

	thinkos_sleep(3000);

	for (;;) {
		thinkos_irq_wait(STM32F_IRQ_TIM6);
		//thinkos_irq_wait_cyccnt(STM32_IRQ_TIM6, &cyccnt);
		//thinkos_irq_timedwait(STM32_IRQ_TIM6, 10000);
		/* Clear timer interrupt flags */
		tim->sr = 0;
		irq_count += 1;
		printf("\ntick");
	}

	return 0;
}

uint32_t timer_stack[1024] __attribute__ ((aligned(8), section(".stack")));

const struct thinkos_thread_inf timer_thread_inf = {
	.stack_ptr = timer_stack,
	.stack_size = sizeof(timer_stack),
	.priority = 26,
	.thread_id = 26,
	.paused = false,
	.tag = "APP"
};


int timer_test_init(uint32_t freq)
{
//	thinkos_thread_create((void *)timer_task, (void *)freq,
//			      timer_stack, sizeof(timer_stack));
	/* Start the main application thread */
	thinkos_thread_create_inf(C_TASK(timer_task), (void *)freq,
							&timer_thread_inf);

	return 0;
}

