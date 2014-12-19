/* 
 * stm32f100.ld
 *
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
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

OUTPUT_FORMAT ("elf32-littlearm")
ENTRY(_reset)
SEARCH_DIR(.)

MEMORY
{
	vectors(r) : ORIGIN = 0x00000000, LENGTH = 1K
	flash (rx) : ORIGIN = 0x08000000, LENGTH = 32K
	sram (rw) : ORIGIN = 0x20000000, LENGTH = 4K
	dcclog (r) : ORIGIN = 0x40000000, LENGTH = 1024K
}

_stack = ORIGIN(sram) + LENGTH(sram);

EXTERN(_stack)
EXTERN(_reset)
EXTERN(_except)
EXTERN(_isr)

/* Cortex-Mx exception vectors */
EXTERN(_rom_vectors)

_nmi_isr = DEFINED(cm_nmi_isr) ? cm_nmi_isr : _except ;
_hard_fault_isr = DEFINED(cm_hard_fault_isr) ? cm_hard_fault_isr : _except ;
_mem_manage_isr = DEFINED(cm_mem_manage_isr) ? cm_mem_manage_isr : _except ;
_bus_fault_isr = DEFINED(cm_bus_fault_isr) ? cm_bus_fault_isr : _except ;
_usage_fault_isr = DEFINED(cm_usage_fault_isr) ? cm_usage_fault_isr : _except ;
_svc_isr = DEFINED(cm_svc_isr) ? cm_svc_isr : _except ;
_debug_mon_isr = DEFINED(cm_debug_mon_isr) ? cm_debug_mon_isr : _except ;
_pendsv_isr = DEFINED(cm_pendsv_isr) ? cm_pendsv_isr : _except ;
_systick_isr = DEFINED(cm_systick_isr) ? cm_systick_isr : _isr ;

/* Common STM32F IRQ vectors: 0 to 7 */
EXTERN(_irq0_vec)
EXTERN(_irq1_vec)
EXTERN(_irq2_vec)
EXTERN(_irq3_vec)
EXTERN(_irq4_vec)
EXTERN(_irq5_vec)
EXTERN(_irq6_vec)
EXTERN(_irq7_vec)

_irq0_isr = DEFINED(stm32f_wwdg_isr) ? stm32f_wwdg_isr : _isr ;
_irq1_isr = DEFINED(stm32f_pvd_isr) ? stm32f_pvd_isr : _isr ;
_irq2_isr = DEFINED(stm32f_tamp_stamp_isr) ? stm32f_tamp_stamp_isr : _isr ;
_irq3_isr = DEFINED(stm32f_rtc_wkup_isr) ? stm32f_rtc_wkup_isr : _isr ;
_irq4_isr = DEFINED(stm32f_flash_isr) ? stm32f_flash_isr : _isr ;
_irq5_isr = DEFINED(stm32f_rcc_isr) ? stm32f_rcc_isr : _isr ;
_irq6_isr = DEFINED(stm32f_exti0_isr) ? stm32f_exti0_isr : _isr ;
_irq7_isr = DEFINED(stm32f_exti1_isr) ? stm32f_exti1_isr : _isr ;

/* Common STM32F3xx IRQ vectors: 0 to 7 */
EXTERN(_irq8_vec)
EXTERN(_irq9_vec)
EXTERN(_irq10_vec)
EXTERN(_irq11_vec)
EXTERN(_irq12_vec)
EXTERN(_irq13_vec)
EXTERN(_irq14_vec)
EXTERN(_irq15_vec)

_irq8_isr = DEFINED(stm32f_exti2_isr) ? stm32f_exti2_isr : 
			DEFINED(stm32f_tsc_isr) ? stm32f_tsc_isr : _isr ;
_irq9_isr = DEFINED(stm32f_exti3_isr) ? stm32f_exti3_isr : _isr ;
_irq10_isr = DEFINED(stm32f_exti4_isr) ? stm32f_exti4_isr : _isr ;
_irq11_isr = DEFINED(stm32f_dma1_chan1_isr) ? stm32f_dma1_chan1_isr : _isr ;
_irq12_isr = DEFINED(stm32f_dma1_chan2_isr) ? stm32f_dma1_chan2_isr : _isr ;
_irq13_isr = DEFINED(stm32f_dma1_chan3_isr) ? stm32f_dma1_chan3_isr : _isr ;
_irq14_isr = DEFINED(stm32f_dma1_chan4_isr) ? stm32f_dma1_chan4_isr : _isr ;
_irq15_isr = DEFINED(stm32f_dma1_chan5_isr) ? stm32f_dma1_chan5_isr : _isr ;

EXTERN(_irq16_vec)
EXTERN(_irq17_vec)
EXTERN(_irq18_vec)
EXTERN(_irq19_vec)
EXTERN(_irq20_vec)
EXTERN(_irq21_vec)
EXTERN(_irq22_vec)
EXTERN(_irq23_vec)

_irq16_isr = DEFINED(stm32f_dma1_chan6_isr) ? stm32f_dma1_chan6_isr : _isr ;
_irq17_isr = DEFINED(stm32f_dma1_chan7_isr) ? stm32f_dma1_chan7_isr : _isr ;
_irq18_isr = DEFINED(stm32f_adc1_isr) ? stm32f_adc1_isr :
			 DEFINED(stm32f_adc2_isr) ? stm32f_adc2_isr : _isr ;
_irq19_isr = DEFINED(stm32f_can1_tx_isr) ? stm32f_can1_tx_isr :
			 DEFINED(stm32f_usb_hp_isr) ? stm32f_usb_hp_isr : _isr ;
_irq20_isr = DEFINED(stm32f_can1_rx0_isr) ? stm32f_can1_rx0_isr :
			 DEFINED(stm32f_usb_lp_isr) ? stm32f_usb_lp_isr : _isr ;
_irq21_isr = DEFINED(stm32f_can1_rx1_isr) ? stm32f_can1_rx1_isr : _isr ;
_irq22_isr = DEFINED(stm32f_can1_sce_isr) ? stm32f_can1_sce_isr : _isr ;
_irq23_isr = DEFINED(stm32f_exti9_5_isr) ? stm32f_exti9_5_isr :
			 DEFINED(stm32f_exti9_isr) ? stm32f_exti9_isr :
			 DEFINED(stm32f_exti8_isr) ? stm32f_exti8_isr :
			 DEFINED(stm32f_exti7_isr) ? stm32f_exti7_isr :
			 DEFINED(stm32f_exti6_isr) ? stm32f_exti6_isr :
			 DEFINED(stm32f_exti5_isr) ? stm32f_exti5_isr : _isr ;


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
		KEEP(*(.rodata.__FUNCTION__.*))
		KEEP(*(.dccdata))
	} >dcclog

	.init :
	{
		KEEP (*(.boot))
		KEEP (*(.init))
	} >flash = 0xffffffff

	.text :
	{
		KEEP (*(.rom_vectors))
		KEEP (*(SORT(.rom_vectors.*)))

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
