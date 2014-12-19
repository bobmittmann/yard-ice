#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stm32f.h>
#include <sys/delay.h>
#include <sys/serial.h>

#include "io.h"

#define USART1_TX STM32F_GPIOB, 6
#define USART1_RX STM32F_GPIOB, 7

#define TIM4_CH3 STM32F_GPIOB, 8
#define TIM4_CH4 STM32F_GPIOB, 9


struct file stm32f_uart1_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};

void freq_gen_init(uint32_t freq)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM4;
	uint32_t div;
	uint32_t pre;
	uint32_t n;


	/* USART1_TX */
	stm32f_gpio_mode(TIM4_CH3, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(TIM4_CH4, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_af(TIM4_CH3, GPIO_AF2);
	stm32f_gpio_af(TIM4_CH4, GPIO_AF2);


	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM4EN;
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = 0; 
	tim->ccmr1 = TIM_OC1M_PWM_MODE1 | TIM_OC2M_PWM_MODE1;
	tim->ccmr2 = TIM_OC3M_PWM_MODE1 | TIM_OC4M_PWM_MODE1;

	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;

	tim->ccr1 = tim->arr / 2;
	tim->ccr2 = tim->arr / 2;
	tim->ccr3 = tim->arr / 2;
	tim->ccr4 = tim->arr / 2;

	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}


void stdio_init(void)
{
	struct stm32f_usart * uart = STM32F_USART1;

	/* USART1_TX */
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);

	stm32f_gpio_mode(USART1_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(USART1_RX, GPIO_AF7);
	stm32f_gpio_af(USART1_TX, GPIO_AF7);

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stderr = &stm32f_uart1_file;
	stdin = stderr;
	stdout = stdin;
}


int main(int argc, char ** argv)
{
	int i;

	cm3_udelay_calibrate();
	cm3_cpsie_i();

	stdio_init();
	io_init();
	freq_gen_init(32768);

	for (i = 0; ;i++) {
		led_flash(2, 200);
//		printf("[%d] hello world...\n", i);
		udelay(500000);
		led_flash(3, 200);
//		leds_all_off();
		udelay(500000);
	}


	return 0;
}

