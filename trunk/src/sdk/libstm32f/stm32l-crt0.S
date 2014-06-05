/* 
 * stm32f-crt0.S
 *
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

#include <sys/stm32f.h>

	.syntax unified
	.cpu cortex-m3

	.section .vectors
__vectors:
	.global __vectors

	.section .rom_vectors, "ax", %progbits
__rom_vectors:
	.global __rom_vectors
	.type __rom_vectors, %object
	.word	_stack
	.word   _reset
	.word   cm3_nmi_isr
	.word   cm3_hard_fault_isr
	.word   cm3_mem_manage_isr
	.word   cm3_bus_fault_isr
	.word   cm3_usage_fault_isr
	.word   0
	.word   ice_comm_blk
	.word   0 
	.word   0
	.word   cm3_svc_isr
	.word   cm3_debug_mon_isr
	.word   0
	.word   cm3_pendsv_isr
	.word   cm3_systick_isr
	.word	stm32_wwdg_isr            /*  0 Window WatchDog */
	.word	stm32_pvd_isr             /*  1 PVD through EXTI Line detection */
	.word	stm32_tamp_stamp_isr      /*  2 Tamper and TimeStamps through 
									        the EXTI line */
	.word	stm32_rtc_wkup_isr        /*  3 RTC Wakeup through the EXTI line */
	.word	stm32_flash_isr           /*  4 FLASH */
	.word	stm32_rcc_isr             /*  5 RCC */
	.word	stm32_exti0_isr           /*  6 EXTI Line0 */
	.word	stm32_exti1_isr           /*  7 EXTI Line1 */

	.word	stm32_exti2_isr           /*  8 EXTI Line2 */
	.word	stm32_exti3_isr           /*  9 EXTI Line3 */
	.word	stm32_exti4_isr           /* 10 EXTI Line4 */
	.word	stm32_dma1_channel1_isr   /* 11 DMA1 Channel 1 */
	.word	stm32_dma1_channel2_isr   /* 12 DMA1 Channel 2 */
	.word	stm32_dma1_channel3_isr   /* 13 DMA1 Channel 3 */
	.word	stm32_dma1_channel4_isr   /* 14 DMA1 Channel 4 */
	.word	stm32_dma1_channel5_isr   /* 15 DMA1 Channel 5 */

	.word	stm32_dma1_channel6_isr   /* 16 DMA1 Channel 6 */
	.word	stm32_dma1_channel7_isr   /* 17 DMA1 Channel 7 */
	.word	stm32_adc_isr             /* 18 ADC1 */
	.word   stm32_usb_hp_isr          /* 19 USB High Priority */
	.word   stm32_usb_lp_isr          /* 20 USB Low Priority */
	.word	stm32_dac_isr             /* 21 DAC */
	.word	stm32_comp_tsc_isr        /* 22 Comparator, Touch sense */
	.word	stm32_exti9_5_isr         /* 23 External Lines [9:5] */

	.word	stm32_lcd                 /* 24 LCD */
	.word	stm32_tim9_isr            /* 25 TIM9 */
	.word	stm32_tim10_isr           /* 26 TIM10 */
	.word	stm32_tim11_isr           /* 27 TIM11 */
	.word	stm32_tim2_isr            /* 28 TIM2 */
	.word	stm32_tim3_isr            /* 29 TIM3 */
	.word	stm32_tim4_isr            /* 30 TIM4 */
	.word	stm32_i2c1_ev_isr         /* 31 I2C1 Event */

	.word	stm32_i2c1_er_isr         /* 32 I2C1 Error */
	.word	stm32_i2c2_ev_isr         /* 33 I2C2 Event */
	.word	stm32_i2c2_er_isr         /* 34 I2C2 Error */
	.word	stm32_spi1_isr            /* 35 SPI1 */
	.word	stm32_spi2_isr            /* 36 SPI2 */
	.word	stm32_usart1_isr          /* 37 USART1 */
	.word	stm32_usart2_isr          /* 38 USART2 */
	.word	stm32_usart3_isr          /* 39 USART3 */
 
	.word	stm32_exti15_10_isr       /* 40 External Line[15:10]s */
	.word	stm32_rtc_alarm_isr       /* 41 RTC Alarm (A and B) 
								 		 through EXTI Line */
	.word	stm32_tim6_isr            /* 42 TIM6 */
	.word	stm32_tim7_isr            /* 43 TIM7 */
	
	.size   __rom_vectors, . - __rom_vectors

	.align  0
	.thumb
	.section .text.init

	.thumb_func
_reset:
	.global _reset
	.type   _reset, %function

	/* jump to hardware initialization code */
	/* disable interrupts */
	cpsid   i
	bl		_init

0:
	/* copy initialized variables .data section  (Copy from ROM to RAM) */
	ldr     r2, .L_data_start
	ldr     r3, .L_data_end
	ldr     r4, .L_idata_start
1:
	/* while the .data pointer is less then the .bss
	   copy from .idata to .data */
	cmp     r2, r3
	ittt	lo
	ldrlo   r7, [r4], #4
	strlo   r7, [r2], #4
	blo     1b

	/* clear .bss  */
	ldr     r2, .L_bss_start
	ldr     r3, .L_bss_end
	mov     r7, #0
2:
	cmp     r2, r3
	itt		lo
	strlo   r7, [r2], #4
	blo     2b

	/* call constructors */
	ldr     r5, .L_ctors_start
	ldr     r6, .L_ctors_end
	.L_ctor_loop:
	cmp     r5, r6
	beq     .L_ctor_end
	ldr     r4, [r5], #4
	blx     r4
	b       .L_ctor_loop
	.L_ctor_end:

	/* argc = 0 */
	mov     r0, #0
	/* argv = NULL */
	mov     r1, r0
	/* go to C main function */
	bl      main
	b       _reset

/* section of initialized data at ROM */
.L_idata_start:
	.word	__text_end
/* section of initialized data at RAM */
.L_data_start:
	.word	__data_start
.L_data_end:
/* end of initialized data */
	.word	__data_end	
.L_bss_start:
	.word	__bss_start
.L_bss_end:
	.word	__bss_end
.L_heap_start:
	.word	__heap_start
.L_ctors_start:
	.word   __ctors_start
.L_ctors_end:
	.word   __ctors_end

	.size   _reset, . - _reset

	.text
	.align  2
	.global _halt
	.thumb
	.thumb_func
	.type   _halt, %function
_halt:
	b		_halt
	.size   _halt, . - _halt

	.text
	.align  2
	.global _isr
	.thumb
	.thumb_func
	.type   _isr, %function
_isr:
	mrs     r0, IPSR
	subs    r0, #16
	b       cm3_default_isr
	.size   _isr, . - _isr

	.align  2
	.global	_void
	.thumb
	.thumb_func
	.type   _void, %function
_void:
	bx      lr
	.size   _void, . - _void

	/***********************************************************************
	 * Provide weak aliases for each Exception handler to _halt().
	 * Any function with the same name will override these definitions.
	 */

	.weak       ice_comm_blk
	.set		ice_comm_blk, __vectors

	.weak       halt
	.thumb_set  halt,_halt

	.weak       cm3_default_isr
	.thumb_set  cm3_default_isr,_halt

	.weak       cm3_nmi_isr
	.thumb_set  cm3_nmi_isr,_halt

	.weak       cm3_hard_fault_isr
	.thumb_set  cm3_hard_fault_isr,_halt

	.weak       cm3_mem_manage_isr
	.thumb_set  cm3_mem_manage_isr,_halt

	.weak       cm3_bus_fault_isr
	.thumb_set  cm3_bus_fault_isr,_halt

	.weak       cm3_usage_fault_isr
	.thumb_set  cm3_usage_fault_isr,_halt

	.weak       cm3_svc_isr
	.thumb_set  cm3_svc_isr,_halt

	.weak       cm3_debug_mon_isr
	.thumb_set  cm3_debug_mon_isr,_halt

	.weak     	cm3_pendsv_isr
	.thumb_set  cm3_pendsv_isr,_halt

	.weak       cm3_systick_isr
	.thumb_set  cm3_systick_isr,_halt

	.weak       stm32_wwdg_isr
	.thumb_set  stm32_wwdg_isr,_isr

	.weak       stm32_pvd_isr
	.thumb_set  stm32_pvd_isr,_isr

	.weak       stm32_tamp_stamp_isr
	.thumb_set  stm32_tamp_stamp_isr,_isr

	.weak       stm32_rtc_wkup_isr
	.thumb_set  stm32_rtc_wkup_isr,_isr

	.weak       stm32_flash_isr
	.thumb_set  stm32_flash_isr,_isr

	.weak       stm32_rcc_isr
	.thumb_set  stm32_rcc_isr,_isr

	.weak       stm32_exti0_isr
	.thumb_set  stm32_exti0_isr,_isr

	.weak       stm32_exti1_isr
	.thumb_set  stm32_exti1_isr,_isr

	.weak       stm32_exti2_isr
	.thumb_set  stm32_exti2_isr,_isr

	.weak       stm32_exti2_isr
	.thumb_set  stm32_exti2_isr,_isr

	.weak       stm32_exti3_isr
	.thumb_set  stm32_exti3_isr,_isr

	.weak       stm32_exti4_isr
	.thumb_set  stm32_exti4_isr,_isr

	.weak       stm32_dma1_channel1_isr
	.thumb_set  stm32_dma1_channel1_isr,_isr

	.weak       stm32_dma1_channel2_isr
	.thumb_set  stm32_dma1_channel2_isr,_isr

	.weak       stm32_dma1_channel3_isr
	.thumb_set  stm32_dma1_channel3_isr,_isr

	.weak       stm32_dma1_channel4_isr
	.thumb_set  stm32_dma1_channel4_isr,_isr

	.weak       stm32_dma1_channel5_isr
	.thumb_set  stm32_dma1_channel5_isr,_isr

	.weak       stm32_dma1_channel6_isr
	.thumb_set  stm32_dma1_channel6_isr,_isr

	.weak       stm32_dma1_channel7_isr
	.thumb_set  stm32_dma1_channel7_isr,_isr

	.weak       stm32_adc_isr
	.thumb_set  stm32_adc_isr,_isr

	.weak       stm32_usb_hp_isr
	.thumb_set  stm32_usb_hp_isr,_isr

	.weak       stm32_usb_lp_isr
	.thumb_set  stm32_usb_lp_isr,_isr

	.weak       stm32_dac_isr 
	.thumb_set  stm32_dac_isr,_isr

	.weak       stm32_comp_tsc_isr
	.thumb_set  stm32_comp_tsc_isr,_isr

	.weak       stm32_exti9_5_isr
	.thumb_set  stm32_exti9_5_isr,_isr

	.weak       stm32_lcd
	.thumb_set  stm32_lcd,_isr

	.weak       stm32_tim9_isr
	.thumb_set  stm32_tim9_isr,_isr

	.weak       stm32_tim10_isr
	.thumb_set  stm32_tim10_isr,_isr

	.weak       stm32_tim11_isr
	.thumb_set  stm32_tim11_isr,_isr

	.weak       stm32_tim2_isr
	.thumb_set  stm32_tim2_isr,_isr

	.weak       stm32_tim3_isr
	.thumb_set  stm32_tim3_isr,_isr

	.weak       stm32_tim4_isr
	.thumb_set  stm32_tim4_isr,_isr

	.weak       stm32_i2c1_ev_isr
	.thumb_set  stm32_i2c1_ev_isr,_isr

	.weak       stm32_i2c1_er_isr
	.thumb_set  stm32_i2c1_er_isr,_isr

	.weak       stm32_i2c2_ev_isr
	.thumb_set  stm32_i2c2_ev_isr,_isr

	.weak       stm32_i2c2_er_isr
	.thumb_set  stm32_i2c2_er_isr,_isr

	.weak       stm32_spi1_isr
	.thumb_set  stm32_spi1_isr,_isr

	.weak       stm32_spi2_isr
	.thumb_set  stm32_spi2_isr,_isr

	.weak       stm32_usart1_isr
	.thumb_set  stm32_usart1_isr,_isr

	.weak       stm32_usart2_isr
	.thumb_set  stm32_usart2_isr,_isr

	.weak       stm32_usart3_isr
	.thumb_set  stm32_usart3_isr,_isr

	.weak       stm32_exti15_10_isr
	.thumb_set  stm32_exti15_10_isr,_isr

	.weak       stm32_rtc_alarm_isr
	.thumb_set  stm32_rtc_alarm_isr,_isr

	.weak       stm32_tim6_isr
	.thumb_set  stm32_tim6_isr,_isr

	.weak       stm32_tim7_isr
	.thumb_set  stm32_tim7_isr,_isr


	.end
