#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/dcclog.h>
#include <sys/shell.h>
#include <sys/tty.h>

#include "config.h"
#include "board.h"

#define VERSION_NUM "0.2"
#define VERSION_DATE "Jul, 2014"

extern const struct shell_cmd cmd_tab[];

const char * shell_greeting(void) 
{
	return "\n"
	"MicroJS " VERSION_NUM " - " VERSION_DATE "\n"
	"(c) Copyright 2014 - Mircom Group (bmittmann@mircomgroup.com)\n\n";
}

const char * shell_prompt(void)
{
	return "[JS]$ ";
}

const struct file stm32_uart_file = {
	.data = STM32_USART2, 
	.op = &stm32_usart_fops_raw 
};

FILE * uart_tty_fopen(void)
{
	struct tty_dev * tty;

	tty = tty_attach(&stm32_uart_file);
	return tty_fopen(tty);
}

void vm_reset(void); 

int main(int argc, char ** argv)
{
	struct stm32_usart * us = STM32_USART2;
	FILE * f;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate the delay loop fo udelay() and friends. */
	cm3_udelay_calibrate();

	io_init();

	DCC_LOG(LOG_TRACE, "open serial");
	stm32_usart_init(us);
	stm32_usart_baudrate_set(us, 115200);
	stm32_usart_mode_set(us, SERIAL_8N1);
	stm32_usart_enable(us);

//	f = (struct file *)&stm32_uart_file;
	f = uart_tty_fopen();
	stderr = f;
	stdout = f;

	vm_reset(); 

	/* start a shell on the serial TTY */
	shell(f, shell_prompt, shell_greeting, cmd_tab);

	return 0;

}

