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
	.word	stm32f_wwdg_isr /* Window WatchDog */
	.word	stm32f_pvd_isr /* PVD through EXTI Line detection */
	.word	stm32f_tamp_stamp_isr /* Tamper and TimeStamps through 
									 the EXTI line */
	.word	stm32f_rtc_wkup_isr /* RTC Wakeup through the EXTI line */
	.word	stm32f_flash_isr /* FLASH */
	.word	stm32f_rcc_isr /* RCC */
	.word	stm32f_exti0_isr /* EXTI Line0 */
	.word	stm32f_exti1_isr /* EXTI Line1 */
	.word	stm32f_exti2_isr /* EXTI Line2 */
	.word	stm32f_exti3_isr /* EXTI Line3 */
	.word	stm32f_exti4_isr /* EXTI Line4 */
	.word	stm32f_dma1_stream0_isr /* DMA1 Stream 0 */
	.word	stm32f_dma1_stream1_isr /* DMA1 Stream 1 */
	.word	stm32f_dma1_stream2_isr /* DMA1 Stream 2 */
	.word	stm32f_dma1_stream3_isr /* DMA1 Stream 3 */
	.word	stm32f_dma1_stream4_isr /* DMA1 Stream 4 */
	.word	stm32f_dma1_stream5_isr /* DMA1 Stream 5 */
	.word	stm32f_dma1_stream6_isr /* DMA1 Stream 6 */
	.word	stm32f_adc_isr /* ADC1, ADC2 and ADC3s */
	.word	stm32f_can1_tx_isr /* CAN1 TX */
	.word	stm32f_can1_rx0_isr /* CAN1 RX0 */
	.word	stm32f_can1_rx1_isr /* CAN1 RX1 */
	.word	stm32f_can1_sce_isr /* CAN1 SCE */
	.word	stm32f_exti9_5_isr /* External Line[9:5]s */
	.word	stm32f_tim1_brk_tim9_isr /* TIM1 Break and TIM9 */
	.word	stm32f_tim1_up_tim10_isr /* TIM1 Update and TIM10 */
	.word	stm32f_tim1_trg_com_tim11_isr /* TIM1 Trigger and Commutation 
											 and TIM11 */
	.word	stm32f_tim1_cc_isr /* TIM1 Capture Compare */
	.word	stm32f_tim2_isr /* TIM2 */
	.word	stm32f_tim3_isr /* TIM3 */
	.word	stm32f_tim4_isr /* TIM4 */
	.word	stm32f_i2c1_ev_isr /* I2C1 Event */
	.word	stm32f_i2c1_er_isr /* I2C1 Error */
	.word	stm32f_i2c2_ev_isr /* I2C2 Event */
	.word	stm32f_i2c2_er_isr /* I2C2 Error */
	.word	stm32f_spi1_isr /* SPI1 */
	.word	stm32f_spi2_isr /* SPI2 */
	.word	stm32f_usart1_isr /* USART1 */
	.word	stm32f_usart2_isr /* USART2 */
	.word	stm32f_usart3_isr /* USART3 */
	.word	stm32f_exti15_10_isr /* External Line[15:10]s */
	.word	stm32f_rtc_alarm_isr /* RTC Alarm (A and B) through EXTI Line */
	.word	stm32f_otg_fs_wkup_isr /* USB OTG FS Wakeup through EXTI line */
	.word	stm32f_tim8_brk_tim12_isr /* TIM8 Break and TIM12 */
	.word	stm32f_tim8_up_tim13_isr /* TIM8 Update and TIM13 */
	.word	stm32f_tim8_trg_com_tim14_isr /* TIM8 Trigger and Commutation 
											 and TIM14 */
	.word	stm32f_tim8_cc_isr /* TIM8 Capture Compare */
	.word	stm32f_dma1_stream7_isr /* DMA1 Stream7 */
	.word	stm32f_fsmc_isr /* FSMC */
	.word	stm32f_sdio_isr /* SDIO */
	.word	stm32f_tim5_isr /* TIM5 */
	.word	stm32f_spi3_isr /* SPI3 */
	.word	stm32f_uart4_isr /* UART4 */
	.word	stm32f_uart5_isr /* UART5 */
	.word	stm32f_tim6_dac_isr /* TIM6 and DAC1&2 underrun errors */
	.word	stm32f_tim7_isr /* TIM7 */
	.word	stm32f_dma2_stream0_isr /* DMA2 Stream 0 */
	.word	stm32f_dma2_stream1_isr /* DMA2 Stream 1 */
	.word	stm32f_dma2_stream2_isr /* DMA2 Stream 2 */
	.word	stm32f_dma2_stream3_isr /* DMA2 Stream 3 */
	.word	stm32f_dma2_stream4_isr /* DMA2 Stream 4 */
	.word	stm32f_eth_isr /* Ethernet */
	.word	stm32f_eth_wkup_isr /* Ethernet Wakeup through EXTI line */
	.word	stm32f_can2_tx_isr /* CAN2 TX */
	.word	stm32f_can2_rx0_isr /* CAN2 RX0 */
	.word	stm32f_can2_rx1_isr /* CAN2 RX1 */
	.word	stm32f_can2_sce_isr /* CAN2 SCE */
	.word	stm32f_otg_fs_isr /* USB OTG FS */
	.word	stm32f_dma2_stream5_isr /* DMA2 Stream 5 */
	.word	stm32f_dma2_stream6_isr /* DMA2 Stream 6 */
	.word	stm32f_dma2_stream7_isr /* DMA2 Stream 7 */
	.word	stm32f_usart6_isr /* USART6 */
	.word	stm32f_i2c3_ev_isr /* I2C3 event */
	.word	stm32f_i2c3_er_isr /* I2C3 error */
	.word	stm32f_otg_hs_ep1_out_isr /* USB OTG HS End Point 1 Out */
	.word	stm32f_otg_hs_ep1_in_isr /* USB OTG HS End Point 1 In */
	.word	stm32f_otg_hs_wkup_isr /* USB OTG HS Wakeup through EXTI */
	.word	stm32f_otg_hs_isr /* USB OTG HS */
	.word	stm32f_dcmi_isr /* DCMI */
	.word	stm32f_cryp_isr /* CRYP crypto */
	.word	stm32f_hash_rng_isr /* Hash and Rng */
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

	.weak       stm32f_can1_tx_isr
	.thumb_set  stm32f_can1_tx_isr,_isr

	.weak       stm32f_can1_rx0_isr
	.thumb_set  stm32f_can1_rx0_isr,_isr

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

	.weak       stm32f_i2c1_er_isr
	.thumb_set  stm32f_i2c1_er_isr,_isr

	.weak       stm32f_i2c2_ev_isr
	.thumb_set  stm32f_i2c2_ev_isr,_isr

	.weak       stm32f_i2c2_er_isr
	.thumb_set  stm32f_i2c2_er_isr,_isr

	.weak       stm32f_spi1_isr
	.thumb_set  stm32f_spi1_isr,_isr

	.weak       stm32f_spi2_isr
	.thumb_set  stm32f_spi2_isr,_isr

	.weak       stm32f_usart1_isr
	.thumb_set  stm32f_usart1_isr,_isr

	.weak       stm32f_usart2_isr
	.thumb_set  stm32f_usart2_isr,_isr

	.weak       stm32f_usart3_isr
	.thumb_set  stm32f_usart3_isr,_isr

	.weak       stm32f_exti15_10_isr
	.thumb_set  stm32f_exti15_10_isr,_isr

	.weak       stm32f_rtc_alarm_isr
	.thumb_set  stm32f_rtc_alarm_isr,_isr

	.weak       stm32f_otg_fs_wkup_isr
	.thumb_set  stm32f_otg_fs_wkup_isr,_isr

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

	.end
