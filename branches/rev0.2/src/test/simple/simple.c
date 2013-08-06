/* 
 * File:	 simple.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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

#include <sys/stm32f.h>
#include <sys/param.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/dcclog.h>

void _init(void)
{
}

void stm32f_can1_sce_isr(void)
{
	__NOP();
}

void stm32f_exti5_isr(void)
{
	__NOP();
}

void stm32f_wwdg_isr(void)
{
	__NOP();
}

void stm32f_pvd_isr(void)
{
	__NOP();
}

void stm32f_rtc_wkup_isr(void)
{
	__NOP();
}

int main(int argc, char ** argv)
{
	int i;
	for (;;) {
		i++;
	}

	return 0;
}

