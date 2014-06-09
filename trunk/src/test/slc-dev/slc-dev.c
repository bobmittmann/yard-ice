#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/dcclog.h>

#include "board.h"

struct file stm32_uart2_file = {
	.data = STM32_USART2, 
	.op = &stm32_usart_fops 
};

void stdio_init(void)
{
	struct stm32_usart * uart = STM32_USART2;

	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, 9600);
	stm32_usart_mode_set(uart, SERIAL_8N1);
	stm32_usart_enable(uart);

	stderr = &stm32_uart2_file;
	stdin = stderr;
	stdout = stdin;
}

int main(int argc, char ** argv)
{
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "3. stdio_init()");
	stdio_init();

	DCC_LOG(LOG_TRACE, "4. enabling interrupts");
	cm3_cpsie_i();

	for (i = 0; ;i++) {
//		DCC_LOG1(LOG_TRACE, "%d...", i);
		led_on(LED1);
#if 0
//		isink_start(0, 100, 300);
//		udelay(400);
//		isink_start(1, 100, 300);
//		udelay(400);
//		isink_start(2, 100, 300);
//		udelay(400);
		isink_start(3, 100, 300);
		udelay(500);
//		isink_start(4, 100, 300);
//		udelay(400);
//		isink_start(5, 100, 300);
		isink_start(6, 100, 300);
		udelay(500);
//		isink_start(7, 100, 300);
//		udelay(400);
//		isink_start(8, 100, 300);
//		udelay(400);
		isink_start(9, 100, 300);
		udelay(500);
//		isink_start(10, 100, 300);
//		udelay(500);
		led_off(LED1);
#endif
		irate_set(0);
		isink_start(9, 35, 300);

		udelay(700);

		irate_set(8191);
		isink_start(9, 35, 300);

		udelay(10000);

//		led_on(LED2);
//		udelay(200000);
//		led_off(LED2);

//		udelay(200000);
//		udelay(200000);
//		led_flash(3, 200);
//		leds_all_off();
//		udelay(500000);
	}


	return 0;
}

