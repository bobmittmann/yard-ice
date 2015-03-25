/* 
 * File:	 sem_test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/stm32f.h>
#include <sys/delay.h>
#include <sys/param.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

void stdio_init(void);

int main(int argc, char ** argv)
{
	int16_t mutex[128];
	int16_t flag[128];
	int16_t cond[128];
	int16_t sem[128];
	int16_t ev[128];
	int i;
	int n;

	/* Run the test */
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* Calibrate the the uDelay loop */
	cm3_udelay_calibrate();

	/* Initialize the stdin, stdout and stderr */
	stdio_init();

	/* Print a useful information message */
	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS - Alloc demo\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	/* Initialize the ThinkOS kernel */
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	/* -------------------------------------------------------------------
	   Mutexes
	   ------------------------------------------------------------------- */
	for (n = 0; (mutex[n] = thinkos_mutex_alloc()) > 0; ++n);

	if (n == 0) {
		printf("#ERROR: thinkos_mutex_alloc() failed!\n");
		abort();
	}

	printf(" - %d mutexs: %d..%d \n", n, mutex[0], mutex[n - 1]);

	for (i = 0; i < n; ++i) {
		if (thinkos_mutex_lock(mutex[i]) < 0) {
			printf("#ERROR: mutex_lock(%d) failed!\n", mutex[i]);
			abort();
		}
	}

	for (i = 0; i < n; ++i) {
		if (thinkos_mutex_unlock(mutex[i]) < 0) {
			printf("#ERROR: mutex_unlock(%d) failed!\n", mutex[i]);
			abort();
		}
	}

	for (i = 0; i < n; ++i) {
		if (thinkos_mutex_free(mutex[i]) < 0) {
			printf("#ERROR: mutex_free(%d) failed!\n", mutex[i]);
			abort();
		}
	}

	/* -------------------------------------------------------------------
	   Conditional Variables 
	   ------------------------------------------------------------------- */
	for (n = 0; (cond[n] = thinkos_cond_alloc()) > 0; ++n);

	if (n == 0) {
		printf("#ERROR: thinkos_cond_alloc() failed!\n");
		abort();
	}

	printf(" - %d conditional variables: %d..%d \n", n, cond[0], cond[n - 1]);

	for (i = 0; i < n; ++i) {
		if (thinkos_cond_signal(cond[i]) < 0) {
			printf("#ERROR: cond_lock(%d) failed!\n", cond[i]);
			abort();
		}
	}

	for (i = 0; i < n; ++i) {
		if (thinkos_cond_free(cond[i]) < 0) {
			printf("#ERROR: cond_free(%d) failed!\n", cond[i]);
			abort();
		}
	}

	/* -------------------------------------------------------------------
	   Semaphores
	   ------------------------------------------------------------------- */
	for (n = 0; (sem[n] = thinkos_sem_alloc(0)) > 0; ++n);

	if (n == 0) {
		printf("#ERROR: thinkos_sem_alloc() failed!\n");
		abort();
	}

	printf(" - %d semaphores: %d..%d \n", n, sem[0], sem[n - 1]);

	for (i = 0; i < n; ++i) {
		if (thinkos_sem_post(sem[i]) < 0) {
			printf("#ERROR: thinkos_sem_post(%d) failed!\n", sem[i]);
			abort();
		}
	}

	for (i = 0; i < n; ++i) {
		if (thinkos_sem_wait(sem[i]) < 0) {
			printf("#ERROR: thinkos_sem_wait(%d) failed!\n", sem[i]);
			abort();
		}
	}

	for (i = 0; i < n; ++i) {
		if (thinkos_sem_free(sem[i]) < 0) {
			printf("#ERROR: thinkos_sem_free(%d) failed!\n", sem[i]);
			abort();
		}
	}

	/* -------------------------------------------------------------------
	   Semaphores
	   ------------------------------------------------------------------- */
	for (n = 0; (ev[n] = thinkos_ev_alloc()) > 0; ++n);

	if (n == 0) {
		printf("#ERROR: thinkos_ev_alloc() failed!\n");
		abort();
	}

	printf(" - %d event sets: %d..%d \n", n, ev[0], ev[n - 1]);

	for (i = 0; i < n; ++i) {
		if (thinkos_ev_raise(ev[i], 1) < 0) {
			printf("#ERROR: thinkos_ev_raise(%d) failed!\n", ev[i]);
			abort();
		}
	}

	for (i = 0; i < n; ++i) {
		if (thinkos_ev_wait(ev[i]) < 0) {
			printf("#ERROR: thinkos_ev_wait(%d) failed!\n", ev[i]);
			abort();
		}
	}

	for (i = 0; i < n; ++i) {
		if (thinkos_ev_free(ev[i]) < 0) {
			printf("#ERROR: thinkos_ev_free(%d) failed!\n", ev[i]);
			abort();
		}
	}


	/* -------------------------------------------------------------------
	   Flags
	   -------------------------------------------------------------------- */
	for (n = 0; (flag[n] = thinkos_flag_alloc()) > 0; ++n);

	if (n == 0) {
		printf("#ERROR: thinkos_flag_alloc() failed!\n");
		abort();
	}

	printf(" - %d flags: %d..%d \n", n, flag[0], flag[n - 1]);

	for (i = 0; i < n; ++i) {
		if (thinkos_flag_signal(flag[i]) < 0) {
			printf("#ERROR: flag_signal(%d) failed!\n", flag[i]);
			abort();
		}
	}

	for (i = 0; i < n; ++i) {
		if (thinkos_flag_wait(flag[i]) < 0) {
			printf("#ERROR: flag_wait(%d) failed!\n", flag[i]);
			abort();
		}
	}

	for (i = 0; i < n; ++i) {
		if (thinkos_flag_free(flag[i]) < 0) {
			printf("#ERROR: flag_free(%d) failed!\n", flag[i]);
			abort();
		}
	}


	printf("\n");

	thinkos_sleep(10000);

	return 0;
}

