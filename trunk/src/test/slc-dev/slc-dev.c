#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/dcclog.h>

#include "board.h"
#include "isink.h"
#include "slcdev.h"

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
			led_off(LED5);
			led_off(LED6);
			trig_mode_set(TRIG_MODE_MATCH);
			break;

		case SW2_A:
			DCC_LOG(LOG_TRACE, "SW2 A");
			led_on(LED5);
			trig_mode_set(TRIG_MODE_BIT);
			break;

		case SW2_B:
			DCC_LOG(LOG_TRACE, "SW2 B");
			led_on(LED6);
			trig_mode_set(TRIG_MODE_VSLC);
		}
	}

	if (event & (1 << EV_ADDR)) {
		DCC_LOG(LOG_TRACE, "ADDR");
		dev_event_clr(EV_ADDR);  
		trig_addr_set(dev_addr);
	}
}

void isink_pulse_test(void)
{
	int j;

	isink_slewrate_set(100);

	for (j = 1; j <= 17; ++j) {
		isink_start(j, 35, 300);
		udelay(500);
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

	udelay(100000);

	DCC_LOG(LOG_TRACE, "2. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "3. stdio_init()");
	stdio_init();

	udelay(50000);

	isink_init();

	udelay(50000);

	DCC_LOG(LOG_TRACE, "4. enabling interrupts");
	cm3_cpsie_i();

	for (i = 0; ; ++i) {
		
		if ((event = dev_event) != 0) {
			process_events(event);
		}

		isink_pulse_test();
	
		udelay(500000);
	}


	return 0;
}

