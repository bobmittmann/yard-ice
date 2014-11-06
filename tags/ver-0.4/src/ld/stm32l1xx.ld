/* 
 * stm32f3xx.ld
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
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


_stack = ORIGIN(sram) + LENGTH(sram);

EXTERN(_stack)
EXTERN(_reset)
EXTERN(cm3_nmi_isr)
EXTERN(cm3_hard_fault_isr)
EXTERN(cm3_mem_manage_isr)
EXTERN(cm3_bus_fault_isr)
EXTERN(cm3_usage_fault_isr)
EXTERN(cm3_svc_isr)
EXTERN(cm3_debug_mon_isr)
EXTERN(cm3_pendsv_isr)
EXTERN(cm3_systick_isr)
EXTERN(ice_comm_blk)

EXTERN(stm32_wwdg_isr)
EXTERN(stm32_pvd_isr)
EXTERN(stm32_tamp_stamp_isr)
EXTERN(stm32_rtc_wkup_isr)
EXTERN(stm32_flash_isr)
EXTERN(stm32_rcc_isr)
EXTERN(stm32_exti0_isr)
EXTERN(stm32_exti1_isr)

EXTERN(stm32_exti2_isr)
EXTERN(stm32_exti3_isr)
EXTERN(stm32_exti4_isr)
EXTERN(stm32_dma1_channel1_isr)
EXTERN(stm32_dma1_channel2_isr)
EXTERN(stm32_dma1_channel3_isr)
EXTERN(stm32_dma1_channel4_isr)
EXTERN(stm32_dma1_channel5_isr)

EXTERN(stm32_dma1_channel6_isr)
EXTERN(stm32_dma1_channel7_isr)
EXTERN(stm32_adc_isr)
EXTERN(stm32_usb_hp_isr)
EXTERN(stm32_usb_lp_isr)
EXTERN(stm32_dac_isr)
EXTERN(stm32_comp_tsc_isr)
EXTERN(stm32f_exti9_5_isr)

EXTERN(stm32_lcd)
EXTERN(stm32_tim9_isr)
EXTERN(stm32_tim10_isr)
EXTERN(stm32_tim11_isr)
EXTERN(stm32_tim2_isr)
EXTERN(stm32_tim3_isr)
EXTERN(stm32_tim4_isr)
EXTERN(stm32_i2c1_ev_isr)

EXTERN(stm32_i2c1_er_isr)
EXTERN(stm32_i2c2_ev_isr)
EXTERN(stm32_i2c2_er_isr)
EXTERN(stm32_spi1_isr)
EXTERN(stm32_spi2_isr)
EXTERN(stm32_usart1_isr)
EXTERN(stm32_usart2_isr)
EXTERN(stm32_usart3_isr)
 
EXTERN(stm32_exti15_10_isr)
EXTERN(stm32_rtc_alarm_isr)
EXTERN(stm32_tim6_isr)
EXTERN(stm32_tim7_isr)
	
SECTIONS

{
	. = 0;

	.dcclog :
	{
		. = ALIGN(4);
		KEEP(*(.dcclog))
	} >dcclog

	.dccdata :
	{
		. = ALIGN(4);
		KEEP(*(.rodata.__FUNCTION__*))
		KEEP(*(.dccdata))
	} >dcclog

	.init :
	{
		KEEP (*(.rom_vectors))
		KEEP (*(.boot))
		KEEP (*(.init))
	} >flash = 0xffffffff

	.text :
	{
		/* all .text sections (code)  */
		*(.text .text.* .gnu.linkonce.t.*)
		/* all .glue_7t sections (interworking thumb helpers) */
		*(.glue_7) *(.glue_7t) *(.vfp11_veneer) *(.v4_bx)
		/* all .rodata sections (constants, strings, etc.)  */
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		. = ALIGN(4);
		/* constructors */
		PROVIDE(__ctors_start = .);
		KEEP (*(SORT(.init_array.*))) /* gcc 4.6 */
		KEEP (*(.init_array))
		KEEP(*(SORT(.ctors.*)))
		KEEP(*(.ctors))
		PROVIDE(__ctors_end = .);
		/* define a global symbol marking the end of read only data sections */
		. = ALIGN(8); 
		PROVIDE(__text_end = .);
	} >flash = 0xffffffff

	.data :
	{
		. = ALIGN(4);
		/* create a global symbol marking the start of the .data section  */
		PROVIDE (__data_start = . );
		/* all .data sections  */
		*(.data .data*)
		/* define a global symbol marking the end of the .data section  */
		. = ALIGN(4);
		PROVIDE (__data_end = . );
	} >sram AT >flash

	PROVIDE(__image_size = SIZEOF(.init) + SIZEOF(.text) + SIZEOF(.data));

	.bss :
	{
		/* define a global symbol marking the start of the .bss section */
		PROVIDE (__bss_start = . );
		/* all .bss sections  */
		*(.bss)
	} >sram

	/* advance location counter to the next 32-bit boundary */
	. = ALIGN(4);
	PROVIDE (__bss_end = . );
	PROVIDE (__heap_start = . );
}

