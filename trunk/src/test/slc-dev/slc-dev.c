#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/dcclog.h>

#include "board.h"
#include "isink.h"

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

void process_events(uint32_t event)
{
	unsigned int sw = dev_sw;

	if (event & (1 << EV_SW1)) {
		dev_event_clr(EV_SW1);  
		
		switch (sw & SW1_MSK) {
		case SW1_OFF:
			DCC_LOG(LOG_TRACE, "SW1 OFF");
			led_off(LED3);
			led_off(LED4);
			break;

		case SW1_A:
			DCC_LOG(LOG_TRACE, "SW1 A");
			led_on(LED3);
			break;

		case SW1_B:
			DCC_LOG(LOG_TRACE, "SW1 B");
			led_on(LED4);
		}
	}

	if (event & (1 << EV_SW2)) {
		DCC_LOG(LOG_TRACE, "SW2");
		dev_event_clr(EV_SW2);  

		switch (sw & SW2_MSK) {

		case SW2_OFF:
			DCC_LOG(LOG_TRACE, "SW2 OFF");
			trig_mode = TRIG_ADDR;
			led_off(LED5);
			led_off(LED6);
			break;

		case SW2_A:
			DCC_LOG(LOG_TRACE, "SW2 A");
			led_on(LED5);
			trig_mode = TRIG_BIT;
			break;

		case SW2_B:
			DCC_LOG(LOG_TRACE, "SW2 B");
			led_on(LED6);
			trig_mode = TRIG_VSLC;
		}
	}

	if (event & (1 << EV_ADDR)) {
		DCC_LOG(LOG_TRACE, "ADDR");
		dev_event_clr(EV_ADDR);  
	}

}

int main(int argc, char ** argv)
{
	uint32_t event;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	udelay(50000);

	DCC_LOG(LOG_TRACE, "2. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "3. stdio_init()");
	stdio_init();

	udelay(50000);

	isink_init();

	udelay(50000);

	DCC_LOG(LOG_TRACE, "4. enabling interrupts");
	cm3_cpsie_i();

	isink_slewrate_set(200);

	for (i = 0; ; ++i) {
		
		if ((event = dev_event) != 0) {
			process_events(event);
		}

//		isink_pulse(35, 300);
//		udelay(1000);

//		trig_out_set();
//		udelay(50000);
//		trig_out_clr();
//		udelay(500000);

//		for (j = 1; j <= 12; ++j) {
//			isink_start(j, 35, 300);
//			isink_slewrate_set(200 * j);
//			udelay(50);
//			isink_pulse(35, 35);
//			udelay(50);
//		isink_start(4, 35, 300);
//			udelay(500);
//		isink_start(6, 35, 300);
//		}

		isink_pulse_dual(35, 300);
		trig_out_set();
		udelay(300);
		trig_out_clr();
		udelay(700);

//		isink_pulse(35, 300);
//		udelay(10000);
//		DCC_LOG1(LOG_TRACE, "%d...", i);
//		led_on(LED1);
#if 0
//		isink_start(0, 100, 300);
//		udelay(400);
//		isink_start(1, 100, 300);
//		udelay(400);
//		isink_start(2, 100, 300);
//		udelay(400);
//		isink_start(3, 35, 300);
//		isink_start(4, 100, 300);
//		udelay(400);
//		isink_start(5, 100, 300);
//		isink_start(6, 100, 300);
//		udelay(500);
//		isink_start(7, 100, 300);
//		udelay(400);
//		isink_start(8, 100, 300);
//		udelay(400);
//		isink_start(9, 100, 300);
//		udelay(500);
//		isink_start(10, 100, 300);
//		udelay(500);
//		irate_set(0);
//		isink_start(9, 35, 300);

//		udelay(700);

//		irate_set(8191);
//		isink_start(9, 35, 300);

		udelay(10000);
#endif

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

