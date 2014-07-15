#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/dcclog.h>

#include <thinkos.h>

#include "board.h"
#include "isink.h"
#include "slcdev.h"
#include "serdrv.h"
#include "xmodem.h"

int device_db_init(void);

struct file stm32_uart2_file = {
	.data = STM32_USART2, 
	.op = &stm32_usart_fops 
};

void stdio_init(void)
{
	struct stm32_usart * uart = STM32_USART2;

	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, 115200);
	stm32_usart_mode_set(uart, SERIAL_8N1);
	stm32_usart_enable(uart);

	stderr = &stm32_uart2_file;
	stdin = stderr;
	stdout = stdin;
}

#if 0
union {
	struct xmodem_rcv rx;
	struct xmodem_snd sx;
} xmodem;

int flash_xmodem_recv(struct serdrv * pdrv, uint32_t offs)
{
	struct comm_dev comm;
	uint8_t buf[512];
	int ret;
	int cnt;

	comm.arg = pdrv;
	comm.op.send = (int (*)(void *, const void *, unsigned int))serdrv_send;
	comm.op.recv = (int (*)(void *, void *, unsigned int, 
							unsigned int))serdrv_recv;

	DCC_LOG(LOG_TRACE, ".................................");

	xmodem_rcv_init(&xmodem.rx, &comm, XMODEM_RCV_CRC);

	cnt = 0;
	do {
		if ((ret = xmodem_rcv_loop(&xmodem.rx, buf, 512)) < 0) {
			DCC_LOG1(LOG_ERROR, "ret=%d", ret);
			return ret;
		}
		stm32_flash_write(offs, buf, ret);
		cnt += ret;
		offs += ret;
	} while (ret > 0);

	return cnt;
}
#endif


void __attribute__((noreturn)) event_task(void)
{
	uint32_t event;
	unsigned int sw;

	for (;;) {
	
		event = io_event_wait();

		sw = io_drv.sw;

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
			DCC_LOG1(LOG_TRACE, "ADDR=%d", io_drv.addr);
			dev_event_clr(EV_ADDR);  
			trig_addr_set(io_drv.addr);
		}
	}
}

void isink_pulse_test(void)
{
	int j;

	isink_slewrate_set(2500);

	for (j = 1; j <= 17; ++j) {
		isink_start(j, 35, 300);
		udelay(500);
	}
}

void isink_delay_test(void)
{
	trig_out_set();
	isink_pulse(35, 300);
	udelay(100);
	trig_out_clr();
	udelay(900);
}

uint32_t __attribute__((aligned(8))) event_stack[32];

extern const uint8_t device_db_js[];
extern unsigned int sizeof_device_db_js;

const char str_loop_config_xfer[] = "\r\nLoop config Xfer...\r\n";
const char str_xmodem_err[] = "\r\nXmodem transfer failed.\r\n";
const char str_xmodem_done[] = "\r\nXmodem transfer done.\r\n";

int main(int argc, char ** argv)
{
	struct serdrv * sdrv;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	thinkos_sleep(100);

	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "4. stdio_init()");
	stdio_init();

	DCC_LOG(LOG_TRACE, "5. isink_init()");
	isink_init();
	thinkos_sleep(100);

	slcdev_init();
	isink_mode_set(ISINK_CURRENT_NOM | ISINK_RATE_FAST);
	isink_slewrate_set(1000);

	device_db_init();

	thinkos_thread_create((void *)event_task, (void *)NULL,
						  event_stack, sizeof(event_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	sdrv = serdrv_init(115200);
	serdrv_tty_fopen(sdrv);

	for (i = 0; ; ++i) {

		printf("U %4d\n", i);
		thinkos_sleep(500);

	//	ser_send(device_db_js, sizeof_device_db_js);
//		n = serdrv_recv(sdrv, buf, 2, 200);
//		DCC_LOG1(LOG_TRACE, "n=%d", n);

//		serdrv_send(sdrv, str_loop_config_xfer, sizeof(str_loop_config_xfer));
//		if (flash_xmodem_recv(sdrv, 0x10000) < 0) 
//			serdrv_send(sdrv, str_xmodem_err, sizeof(str_xmodem_err));
//		else
//			serdrv_send(sdrv, str_xmodem_done, sizeof(str_xmodem_done));

//		isink_pulse_test();
	
//		isink_delay_test();
//		udelay(500000);
	}


	return 0;
}

