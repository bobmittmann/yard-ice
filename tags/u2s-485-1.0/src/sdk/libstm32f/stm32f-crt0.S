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
	.word	stm32f_wwdg_isr         /*  0 Window WatchDog */
	.word	stm32f_pvd_isr          /*  1 PVD through EXTI Line detection */
	.word	stm32f_tamp_stamp_isr   /*  2 Tamper and TimeStamps through 
									        the EXTI line */
	.word	stm32f_rtc_wkup_isr     /*  3 RTC Wakeup through the EXTI line */
	.word	stm32f_flash_isr        /*  4 FLASH */
	.word	stm32f_rcc_isr          /*  5 RCC */
	.word	stm32f_exti0_isr        /*  6 EXTI Line0 */
	.word	stm32f_exti1_isr        /*  7 EXTI Line1 */
#if defined(STM32F3X)
	.word	stm32f_exti2_tsc_isr    /*  8 EXTI Line2 */
#else
	.word	stm32f_exti2_isr        /*  8 EXTI Line2 */
#endif
	.word	stm32f_exti3_isr        /*  9 EXTI Line3 */
	.word	stm32f_exti4_isr        /* 10 EXTI Line4 */
	.word	stm32f_dma1_stream0_isr /* 11 DMA1 Stream 0 */
	.word	stm32f_dma1_stream1_isr /* 12 DMA1 Stream 1 */
	.word	stm32f_dma1_stream2_isr /* 13 DMA1 Stream 2 */
	.word	stm32f_dma1_stream3_isr /* 14 DMA1 Stream 3 */
	.word	stm32f_dma1_stream4_isr /* 15 DMA1 Stream 4 */
	.word	stm32f_dma1_stream5_isr /* 16 DMA1 Stream 5 */
	.word	stm32f_dma1_stream6_isr /* 17 DMA1 Stream 6 */
	.word	stm32f_adc_isr          /* 18 ADC1, ADC2 and ADC3s */

#if defined(STM32F2X) || defined(STM32F4X)
	.word	stm32f_can1_tx_isr         /* 19 CAN1 TX */
	.word	stm32f_can1_rx0_isr        /* 20 CAN1 RX0 */
	.word	stm32f_can1_rx1_isr        /* 21 CAN1 RX1 */
	.word	stm32f_can1_sce_isr        /* 22 CAN1 SCE */
#elif defined(STM32F10X) || defined(STM32F3X)
	.word   stm32f_can1_tx_usb_hp_isr  /* 19 CAN1 TX and USB High Priority */
	.word   stm32f_can1_rx0_usb_lp_isr /* 20 CAN1 RX0 and and USB 
										       Low Priority */
	.word	stm32f_can1_rx1_isr        /* 21 CAN1 RX1 */
	.word	stm32f_can1_sce_isr        /* 22 CAN1 SCE */
#elif defined(STM32F100)
	.word	_halt                      /* 19 Reserved */
	.word	_halt                      /* 20 Reserved */
	.word	_halt                      /* 21 Reserved */
	.word	_halt                      /* 22 Reserved */
#endif

	.word	stm32f_exti9_5_isr         /* 23 External Lines [9:5] */

#if defined(STM32F2X) || defined(STM32F4X)
	.word	stm32f_tim1_brk_tim9_isr      /* 24 TIM1 Break and TIM9 */
	.word	stm32f_tim1_up_tim10_isr      /* 25 TIM1 Update and TIM10 */
	.word	stm32f_tim1_trg_com_tim11_isr /* 26 TIM1 Trigger and Commutation 
	                                            and TIM11 */
#elif defined(STM32F10X)
	.word	stm32f_tim1_brk_isr           /* 24 TIM1 Break */
	.word	stm32f_tim1_up_isr            /* 25 TIM1 Update */
	.word	stm32f_tim1_trg_com_isr       /* 26 TIM1 Trigger and Commutation */
#elif defined(STM32F100) || defined(STM32F3X)
	.word	stm32f_tim1_brk_tim15_isr     /* 24 TIM1 Break and TIM15 */
	.word	stm32f_tim1_up_tim16_isr      /* 25 TIM1 Update and TIM16 */
	.word	stm32f_tim1_trg_com_tim17_isr /* 26 TIM1 Trigger and Commutation 
											 and TIM17 */
#endif

	.word	stm32f_tim1_cc_isr        /* 27 TIM1 Capture Compare */
	.word	stm32f_tim2_isr           /* 28 TIM2 */
	.word	stm32f_tim3_isr           /* 29 TIM3 */
	.word	stm32f_tim4_isr           /* 30 TIM4 */

#if defined(STM32F3X)
	.word	stm32f_i2c1_ev_exti23_isr /* 31 I2C1 Event */
	.word	stm32f_i2c1_er_isr        /* 32 I2C1 Error */
	.word	stm32f_i2c2_ev_exti24_isr /* 33 I2C2 Event */
	.word	stm32f_i2c2_er_isr        /* 34 I2C2 Error */
#else
	.word	stm32f_i2c1_ev_isr        /* 31 I2C1 Event */
	.word	stm32f_i2c1_er_isr        /* 32 I2C1 Error */
	.word	stm32f_i2c2_ev_isr        /* 33 I2C2 Event */
	.word	stm32f_i2c2_er_isr        /* 34 I2C2 Error */
#endif

	.word	stm32f_spi1_isr           /* 35 SPI1 */
	.word	stm32f_spi2_isr           /* 36 SPI2 */

#if defined(STM32F3X)
	.word	stm32f_usart1_exti25_isr  /* 37 USART1 */
	.word	stm32f_usart2_exti26_isr  /* 38 USART2 */
	.word	stm32f_usart3_exti28_isr  /* 39 USART3 */
#else
	.word	stm32f_usart1_isr         /* 37 USART1 */
	.word	stm32f_usart2_isr         /* 38 USART2 */
	.word	stm32f_usart3_isr         /* 39 USART3 */
#endif

	.word	stm32f_exti15_10_isr      /* 40 External Line[15:10]s */
	.word	stm32f_rtc_alarm_isr      /* 41 RTC Alarm (A and B) 
										 through EXTI Line */
#if defined(STM32F2X) || defined(STM32F4X)
	.word	stm32f_otg_fs_wkup_isr    /* 42 USB OTG FS Wakeup 
	                                     through EXTI line */
#elif defined(STM32F10X) || defined(STM32F3X)
	.word	stm32f_usb_wkup_isr       /* 42 USB FS Wakeup  */
#else
	.word	stm32f_cec_isr            /* 42 CEC Global Interrupt */
#endif

#if defined(STM32F100)
	.word	stm32f_tim12_isr          /* 43 TIM12 */
	.word	stm32f_tim13_isr          /* 44 TIM13 */
	.word	stm32f_tim14_isr          /* 45 TIM14 */
#else
	.word	stm32f_tim8_brk_tim12_isr     /* 43 TIM8 Break and TIM12 */
	.word	stm32f_tim8_up_tim13_isr      /* 44 TIM8 Update and TIM13 */
	.word	stm32f_tim8_trg_com_tim14_isr /* 45 TIM8 Trigger and Commutation 
	                                            and TIM14 */
#endif

#if defined(STM32F100)
	.word	_halt                     /* 46 Reserved */
	.word	_halt                     /* 47 Reserved */
#else
	.word	stm32f_tim8_cc_isr        /* 46 TIM8 Capture Compare */
	.word	stm32f_dma1_stream7_isr   /* 47 DMA1 Stream7 */
#endif

	.word	stm32f_fsmc_isr           /* 48 FSMC */
	
#if defined(STM32F100)
	.word	_halt                     /* 49 Reserved */
#else
	.word	stm32f_sdio_isr           /* 49 SDIO */
#endif

	.word	stm32f_tim5_isr           /* 50 TIM5 */
	.word	stm32f_spi3_isr           /* 51 SPI3 */
	.word	stm32f_uart4_isr          /* 52 UART4 */
	.word	stm32f_uart5_isr          /* 53 UART5 */
	.word	stm32f_tim6_dac_isr       /* 54 TIM6 and DAC1&2 underrun errors */
	.word	stm32f_tim7_isr           /* 55 TIM7 */
	.word	stm32f_dma2_stream0_isr   /* 56 DMA2 Stream 0 */
	.word	stm32f_dma2_stream1_isr   /* 57 DMA2 Stream 1 */
	.word	stm32f_dma2_stream2_isr   /* 58 DMA2 Stream 2 */
	.word	stm32f_dma2_stream3_isr   /* 59 DMA2 Stream 3 */

#if defined(STM32F100)
	.word	stm32f_dma2_stream4_isr   /* 60 DMA2 Stream 4 */
#elif defined(STM32F2X) || defined(STM32F4X)
	.word	stm32f_dma2_stream4_isr   /* 60 DMA2 Stream 4 */
	.word	stm32f_eth_isr            /* 61 Ethernet */
	.word	stm32f_eth_wkup_isr       /* 62 Ethernet Wakeup through EXTI line */
	.word	stm32f_can2_tx_isr        /* 63 CAN2 TX */
	.word	stm32f_can2_rx0_isr       /* 64 CAN2 RX0 */
	.word	stm32f_can2_rx1_isr       /* 65 CAN2 RX1 */
	.word	stm32f_can2_sce_isr       /* 66 CAN2 SCE */
	.word	stm32f_otg_fs_isr         /* 67 USB OTG FS */
	.word	stm32f_dma2_stream5_isr   /* 68 DMA2 Stream 5 */
	.word	stm32f_dma2_stream6_isr   /* 69 DMA2 Stream 6 */
	.word	stm32f_dma2_stream7_isr   /* 70 DMA2 Stream 7 */
	.word	stm32f_usart6_isr         /* 71 USART6 */
	.word	stm32f_i2c3_ev_isr        /* 72 I2C3 event */
	.word	stm32f_i2c3_er_isr        /* 73 I2C3 error */
	.word	stm32f_otg_hs_ep1_out_isr /* 74 USB OTG HS End Point 1 Out */
	.word	stm32f_otg_hs_ep1_in_isr  /* 75 USB OTG HS End Point 1 In */
	.word	stm32f_otg_hs_wkup_isr    /* 76 USB OTG HS Wakeup through EXTI */
	.word	stm32f_otg_hs_isr         /* 77 USB OTG HS */
	.word	stm32f_dcmi_isr           /* 78 DCMI */
	.word	stm32f_cryp_isr           /* 79 CRYP crypto */
	.word	stm32f_hash_rng_isr       /* 80 Hash and Rng */
#endif
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

	.weak       stm32f_wwdg_isr
	.thumb_set  stm32f_wwdg_isr,_isr

	.weak       stm32f_pvd_isr
	.thumb_set  stm32f_pvd_isr,_isr

	.weak       stm32f_tamp_stamp_isr
	.thumb_set  stm32f_tamp_stamp_isr,_isr

	.weak       stm32f_rtc_wkup_isr
	.thumb_set  stm32f_rtc_wkup_isr,_isr

	.weak       stm32f_flash_isr
	.thumb_set  stm32f_flash_isr,_isr

	.weak       stm32f_rcc_isr
	.thumb_set  stm32f_rcc_isr,_isr

	.weak       stm32f_exti0_isr
	.thumb_set  stm32f_exti0_isr,_isr

	.weak       stm32f_exti1_isr
	.thumb_set  stm32f_exti1_isr,_isr

	.weak       stm32f_exti2_isr
	.thumb_set  stm32f_exti2_isr,_isr

	.weak       stm32f_exti2_tsc_isr
	.thumb_set  stm32f_exti2_tsc_isr,_isr

	.weak       stm32f_exti3_isr
	.thumb_set  stm32f_exti3_isr,_isr

	.weak       stm32f_exti4_isr
	.thumb_set  stm32f_exti4_isr,_isr

	.weak       stm32f_dma1_stream0_isr
	.thumb_set  stm32f_dma1_stream0_isr,_isr

	.weak       stm32f_dma1_stream1_isr
	.thumb_set  stm32f_dma1_stream1_isr,_isr

	.weak       stm32f_dma1_stream2_isr
	.thumb_set  stm32f_dma1_stream2_isr,_isr

	.weak       stm32f_dma1_stream3_isr
	.thumb_set  stm32f_dma1_stream3_isr,_isr

	.weak       stm32f_dma1_stream4_isr
	.thumb_set  stm32f_dma1_stream4_isr,_isr

	.weak       stm32f_dma1_stream5_isr
	.thumb_set  stm32f_dma1_stream5_isr,_isr

	.weak       stm32f_dma1_stream6_isr
	.thumb_set  stm32f_dma1_stream6_isr,_isr

	.weak       stm32f_adc_isr
	.thumb_set  stm32f_adc_isr,_isr

#if defined(STM32F2X) || defined(STM32F4X)
	.weak       stm32f_can1_tx_isr
	.thumb_set  stm32f_can1_tx_isr,_isr

	.weak       stm32f_can1_rx0_isr
	.thumb_set  stm32f_can1_rx0_isr,_isr
#endif

#ifdef STM32F10X
	.weak       stm32f_can1_tx_usb_hp_isr
	.thumb_set  stm32f_can1_tx_usb_hp_isr,_isr

	.weak       stm32f_can1_rx0_usb_lp_isr
	.thumb_set  stm32f_can1_rx0_usb_lp_isr,_isr
#endif

	.weak       stm32f_can1_rx1_isr
	.thumb_set  stm32f_can1_rx1_isr,_isr

	.weak       stm32f_can1_sce_isr
	.thumb_set  stm32f_can1_sce_isr,_isr

	.weak       stm32f_exti9_5_isr
	.thumb_set  stm32f_exti9_5_isr,_isr

	.weak       stm32f_tim1_brk_tim9_isr
	.thumb_set  stm32f_tim1_brk_tim9_isr,_isr

	.weak       stm32f_tim1_up_tim10_isr
	.thumb_set  stm32f_tim1_up_tim10_isr,_isr

	.weak       stm32f_tim1_trg_com_tim11_isr
	.thumb_set  stm32f_tim1_trg_com_tim11_isr,_isr

	.weak       stm32f_tim1_brk_isr
	.thumb_set  stm32f_tim1_brk_isr,_isr

	.weak       stm32f_tim1_up_isr
	.thumb_set  stm32f_tim1_up_isr,_isr

	.weak       stm32f_tim1_trg_com_isr
	.thumb_set  stm32f_tim1_trg_com_isr,_isr

	.weak       stm32f_tim1_brk_tim15_isr
	.thumb_set  stm32f_tim1_brk_tim15_isr,_isr

	.weak       stm32f_tim1_up_tim16_isr
	.thumb_set  stm32f_tim1_up_tim16_isr,_isr

	.weak       stm32f_tim1_trg_com_tim17_isr
	.thumb_set  stm32f_tim1_trg_com_tim17_isr,_isr

	.weak       stm32f_tim1_cc_isr
	.thumb_set  stm32f_tim1_cc_isr,_isr

	.weak       stm32f_tim2_isr
	.thumb_set  stm32f_tim2_isr,_isr

	.weak       stm32f_tim3_isr
	.thumb_set  stm32f_tim3_isr,_isr

	.weak       stm32f_tim4_isr
	.thumb_set  stm32f_tim4_isr,_isr

	.weak       stm32f_i2c1_ev_isr
	.thumb_set  stm32f_i2c1_ev_isr,_isr

	.weak       stm32f_i2c1_ev_exti23_isr
	.thumb_set  stm32f_i2c1_ev_exti23_isr,_isr

	.weak       stm32f_i2c1_er_isr
	.thumb_set  stm32f_i2c1_er_isr,_isr

	.weak       stm32f_i2c2_ev_isr
	.thumb_set  stm32f_i2c2_ev_isr,_isr

	.weak       stm32f_i2c2_ev_exti24_isr
	.thumb_set  stm32f_i2c2_ev_exti24_isr,_isr

	.weak       stm32f_i2c2_er_isr
	.thumb_set  stm32f_i2c2_er_isr,_isr

	.weak       stm32f_spi1_isr
	.thumb_set  stm32f_spi1_isr,_isr

	.weak       stm32f_spi2_isr
	.thumb_set  stm32f_spi2_isr,_isr

	.weak       stm32f_usart1_isr
	.thumb_set  stm32f_usart1_isr,_isr

	.weak       stm32f_usart1_exti25_isr
	.thumb_set  stm32f_usart1_exti25_isr,_isr

	.weak       stm32f_usart2_isr
	.thumb_set  stm32f_usart2_isr,_isr

	.weak       stm32f_usart2_exti26_isr
	.thumb_set  stm32f_usart2_exti26_isr,_isr

	.weak       stm32f_usart3_isr
	.thumb_set  stm32f_usart3_isr,_isr

	.weak       stm32f_usart3_exti28_isr
	.thumb_set  stm32f_usart3_exti28_isr,_isr

	.weak       stm32f_exti15_10_isr
	.thumb_set  stm32f_exti15_10_isr,_isr

	.weak       stm32f_rtc_alarm_isr
	.thumb_set  stm32f_rtc_alarm_isr,_isr

	.weak       stm32f_otg_fs_wkup_isr
	.thumb_set  stm32f_otg_fs_wkup_isr,_isr

	.weak       stm32f_usb_wkup_isr
	.thumb_set  stm32f_usb_wkup_isr,_isr

	.weak       stm32f_cec_isr
	.thumb_set  stm32f_cec_isr,_isr

	.weak       stm32f_tim12_isr
	.thumb_set  stm32f_tim12_isr,_isr

	.weak       stm32f_tim13_isr
	.thumb_set  stm32f_tim13_isr,_isr

	.weak       stm32f_tim14_isr
	.thumb_set  stm32f_tim14_isr,_isr

	.weak       stm32f_tim8_brk_tim12_isr
	.thumb_set  stm32f_tim8_brk_tim12_isr,_isr

	.weak       stm32f_tim8_up_tim13_isr
	.thumb_set  stm32f_tim8_up_tim13_isr,_isr

	.weak       stm32f_tim8_trg_com_tim14_isr
	.thumb_set  stm32f_tim8_trg_com_tim14_isr,_isr

	.weak       stm32f_tim8_cc_isr
	.thumb_set  stm32f_tim8_cc_isr,_isr

	.weak       stm32f_dma1_stream7_isr
	.thumb_set  stm32f_dma1_stream7_isr,_isr

	.weak       stm32f_fsmc_isr
	.thumb_set  stm32f_fsmc_isr,_isr

	.weak       stm32f_sdio_isr
	.thumb_set  stm32f_sdio_isr,_isr

	.weak       stm32f_tim5_isr
	.thumb_set  stm32f_tim5_isr,_isr

	.weak       stm32f_spi3_isr
	.thumb_set  stm32f_spi3_isr,_isr

	.weak       stm32f_uart4_isr
	.thumb_set  stm32f_uart4_isr,_isr

	.weak       stm32f_uart5_isr
	.thumb_set  stm32f_uart5_isr,_isr

	.weak       stm32f_tim6_dac_isr
	.thumb_set  stm32f_tim6_dac_isr,_isr

	.weak       stm32f_tim7_isr
	.thumb_set  stm32f_tim7_isr,_isr

	.weak       stm32f_dma2_stream0_isr
	.thumb_set  stm32f_dma2_stream0_isr,_isr

	.weak       stm32f_dma2_stream1_isr
	.thumb_set  stm32f_dma2_stream1_isr,_isr

	.weak       stm32f_dma2_stream2_isr
	.thumb_set  stm32f_dma2_stream2_isr,_isr

	.weak       stm32f_dma2_stream3_isr
	.thumb_set  stm32f_dma2_stream3_isr,_isr

	.weak       stm32f_dma2_stream4_isr
	.thumb_set  stm32f_dma2_stream4_isr,_isr

#if defined(STM32F2X) || defined(STM32F4X)
	.weak       stm32f_eth_isr
	.thumb_set  stm32f_eth_isr,_isr

	.weak       stm32f_eth_wkup_isr
	.thumb_set  stm32f_eth_wkup_isr,_isr

	.weak       stm32f_can2_tx_isr
	.thumb_set  stm32f_can2_tx_isr,_isr

	.weak       stm32f_can2_rx0_isr
	.thumb_set  stm32f_can2_rx0_isr,_isr

	.weak       stm32f_can2_rx1_isr
	.thumb_set  stm32f_can2_rx1_isr,_isr

	.weak       stm32f_can2_sce_isr
	.thumb_set  stm32f_can2_sce_isr,_isr

	.weak       stm32f_otg_fs_isr
	.thumb_set  stm32f_otg_fs_isr,_isr

	.weak       stm32f_dma2_stream5_isr
	.thumb_set  stm32f_dma2_stream5_isr,_isr

	.weak       stm32f_dma2_stream6_isr
	.thumb_set  stm32f_dma2_stream6_isr,_isr

	.weak       stm32f_dma2_stream7_isr
	.thumb_set  stm32f_dma2_stream7_isr,_isr

	.weak       stm32f_usart6_isr
	.thumb_set  stm32f_usart6_isr,_isr

	.weak       stm32f_i2c3_ev_isr
	.thumb_set  stm32f_i2c3_ev_isr,_isr

	.weak       stm32f_i2c3_er_isr
	.thumb_set  stm32f_i2c3_er_isr,_isr

	.weak       stm32f_otg_hs_ep1_out_isr
	.thumb_set  stm32f_otg_hs_ep1_out_isr,_isr

	.weak       stm32f_otg_hs_ep1_in_isr
	.thumb_set  stm32f_otg_hs_ep1_in_isr,_isr

	.weak       stm32f_otg_hs_wkup_isr
	.thumb_set  stm32f_otg_hs_wkup_isr,_isr

	.weak       stm32f_otg_hs_isr
	.thumb_set  stm32f_otg_hs_isr,_isr

	.weak       stm32f_dcmi_isr
	.thumb_set  stm32f_dcmi_isr,_isr

	.weak       stm32f_cryp_isr
	.thumb_set  stm32f_cryp_isr,_isr

	.weak       stm32f_hash_rng_isr
	.thumb_set  stm32f_hash_rng_isr,_isr
#endif

	.end
