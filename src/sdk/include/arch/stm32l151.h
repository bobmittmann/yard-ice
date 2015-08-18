/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the libstm32f.
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

/** 
 * @file stm32l151.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

/* 
 
 */

#ifndef __STM32L151_H__
#define __STM32L151_H__

#ifndef STM32L
#define STM32L
#endif

#ifndef STM32L1X
#define STM32L1X
#endif

#ifndef STM32L15X
#define STM32L15X
#endif

#ifndef STM32L151
#define STM32L151
#endif

#include <stm32f/stm32l-pwr.h>
#include <stm32f/stm32l-rcc.h>
#include <stm32f/stm32l-flash.h>
#include <stm32f/stm32f-syscfg.h>
#include <stm32f/stm32f-usart.h>
#include <stm32f/stm32f-gpio.h>
#include <stm32f/stm32f-dac.h>
#include <stm32f/stm32f-adc.h>
#include <stm32f/stm32f-rtc.h>
#include <stm32f/stm32f-spi.h>
#include <stm32f/stm32f-i2c.h>
#include <stm32f/stm32f-tim.h>
#include <stm32f/stm32f-dma.h>
#include <stm32f/stm32f-exti.h>
#include <stm32f/stm32-comp.h>
#include <stm32f/stm32-crc.h>

#define STM32_BASE_UID		0x1ff80050
#define STM32_BASE_FSMC    0xa0000000
#define STM32_BASE_AES     0x50060000
#define STM32_BASE_DMA2    0x40026400
#define STM32_BASE_DMA1    0x40026000
#define STM32_BASE_FLASH   0x40023c00
#define STM32_BASE_RCC     0x40023800
#define STM32_BASE_CRC     0x40023000
#define STM32_BASE_GPIOG   0x40021c00
#define STM32_BASE_GPIOF   0x40021800
#define STM32_BASE_GPIOH   0x40021400
#define STM32_BASE_GPIOE   0x40021000
#define STM32_BASE_GPIOD   0x40020c00
#define STM32_BASE_GPIOC   0x40020800
#define STM32_BASE_GPIOB   0x40020400
#define STM32_BASE_GPIOA   0x40020000
#define STM32_BASE_USART1  0x40013800
#define STM32_BASE_SPI1    0x40013000
#define STM32_BASE_SDIO    0x40012c00
#define STM32_BASE_ADC     0x40012400
#define STM32_BASE_TIM11   0x40011000
#define STM32_BASE_TIM10   0x40010c00
#define STM32_BASE_TIM9    0x40010800
#define STM32_BASE_EXTI    0x40010400
#define STM32_BASE_SYSCFG  0x40010000
#define STM32_BASE_OPAMP   0x40007c5c
#define STM32_BASE_RI      0x40007c04
#define STM32_BASE_COMP    0x40007c00
#define STM32_BASE_DAC     0x40007400
#define STM32_BASE_PWR     0x40007000
#define STM32_BASE_USB_FS  0x40005c00
#define STM32_BASE_I2C2    0x40005800
#define STM32_BASE_I2C1    0x40005400
#define STM32_BASE_UART5   0x40005000
#define STM32_BASE_UART4   0x40004c00
#define STM32_BASE_USART3  0x40004800
#define STM32_BASE_USART2  0x40004400
#define STM32_BASE_SPI3    0x40003c00
#define STM32_BASE_SPI2    0x40003800
#define STM32_BASE_IWDG    0x40003000
#define STM32_BASE_WWDG    0x40002c00
#define STM32_BASE_RTC     0x40002800
#define STM32_BASE_LCD     0x40002400
#define STM32_BASE_TIM7    0x40001400
#define STM32_BASE_TIM6    0x40001000
#define STM32_BASE_TIM5    0x40000c00
#define STM32_BASE_TIM4    0x40000800
#define STM32_BASE_TIM3    0x40000400
#define STM32_BASE_TIM2    0x40000000


#define STM32_IRQ_WWDG       0 /* Window WatchDog Interrupt */
#define STM32_IRQ_PVD        1 /* PVD through EXTI Line detection Interrupt */
#define STM32_IRQ_TAMPER     2 /* Tamper interrupts through the EXTI line */
#define STM32_IRQ_RTC_WKUP   3 /* RTC Wakeup interrupt through the EXTI line */
#define STM32_IRQ_FLASH      4 /* FLASH global Interrupt */
#define STM32_IRQ_RCC        5 /* RCC global Interrupt */
#define STM32_IRQ_EXTI0      6 /* EXTI Line0 Interrupt */
#define STM32_IRQ_EXTI1      7 /* EXTI Line1 Interrupt */

#define STM32_IRQ_EXTI2      8 /* EXTI Line2 Interrupt */
#define STM32_IRQ_EXTI3      9 /* EXTI Line3 Interrupt */
#define STM32_IRQ_EXTI4     10 /* EXTI Line4 Interrupt */
#define STM32_IRQ_DMA1_CH1  11 /* DMA1 Channel1 global Interrupt */
#define STM32_IRQ_DMA1_CH2  12 /* DMA1 Channel2 global Interrupt */
#define STM32_IRQ_DMA1_CH3  13 /* DMA1 Channel3 global Interrupt */
#define STM32_IRQ_DMA1_CH4  14 /* DMA1 Channel4 global Interrupt */
#define STM32_IRQ_DMA1_CH5  15 /* DMA1 Channel5 global Interrupt */

#define STM32_IRQ_DMA1_CH6  16 /* DMA1 Channel6 global Interrupt */
#define STM32_IRQ_DMA1_CH7  17 /* DMA1 Channel7 global Interrupt */
#define STM32_IRQ_ADC1      18 /* ADC1 and ADC2 global Interrupts */
#define STM32_IRQ_USB_HP    19 /* USB High Priority */
#define STM32_IRQ_USB_LP    20 /* USB Low Priority */
#define STM32_IRQ_DAC       21 /* DAC Interrupt */
#define STM32_IRQ_COMP      22 /* Comparator wakeup through EXTI 
								  line (21 and 22) Interrupt */
#define STM32_IRQ_TSC       22 /* Touch Sense Interrupt */
#define STM32_IRQ_EXTI9_5   23 /* External Line[9:5] Interrupts */

#define STM32_IRQ_LCD       24 /* LCD global interrupt  */
#define STM32_IRQ_TIM9      25 /* TIM9 global interrupt */
#define STM32_IRQ_TIM10     26 /* TIM10 global interrupt */
#define STM32_IRQ_TIM11     27 /* TIM11 global interrupt */
#define STM32_IRQ_TIM2      28 /* TIM2 global Interrupt */
#define STM32_IRQ_TIM3      29 /* TIM3 global Interrupt */
#define STM32_IRQ_TIM4      30 /* TIM4 global Interrupt */
#define STM32_IRQ_I2C1_EV   31 /* I2C1 Event Interrupt */

#define STM32_IRQ_I2C1_ER   32 /* I2C1 Error Interrupt */
#define STM32_IRQ_I2C2_EV   33 /* I2C2 Event Interrupt */
#define STM32_IRQ_I2C2_ER   34 /* I2C2 Error Interrupt */
#define STM32_IRQ_SPI1      35 /* SPI1 global Interrupt */
#define STM32_IRQ_SPI2      36 /* SPI2 global Interrupt */
#define STM32_IRQ_USART1    37 /* USART1 global Interrupt */
#define STM32_IRQ_USART2    38 /* USART2 global Interrupt */
#define STM32_IRQ_USART3    39 /* USART3 global Interrupt */

#define STM32_IRQ_EXTI15_10 40 /* External Line[15:10] Interrupts */
#define STM32_IRQ_RTC_ALARM 41 /* RTC Alarm (A and B) through EXTI 
								  Line Interrupt */
#define STM32_IRQ_TIM6      42 /* TIM12 global interrupt */
#define STM32_IRQ_TIM7      43 /* TIM13 global interrupt */

#ifndef __ASSEMBLER__

#define STM32_PWR    ((struct stm32_pwr *)STM32_BASE_PWR)
#define STM32_RCC    ((struct stm32_rcc *)STM32_BASE_RCC)
#define STM32_RTC    ((struct stm32f_rtc *)STM32_BASE_RTC)
#define STM32_USB    ((struct stm32f_usb_fs *)STM32_BASE_USB_FS)
#define STM32_FLASH  ((struct stm32_flash *)STM32_BASE_FLASH)
#define STM32_FSMC   ((struct stm32f_fsmc *)STM32_BASE_FSMC)
#define STM32_DAC    ((struct stm32f_dac *)STM32_BASE_DAC)
#define STM32_ADC    ((struct stm32f_adc *)STM32_BASE_ADC)

#define STM32_CRC    ((struct stm32_crc *)STM32_BASE_CRC)

#define STM32_EXTI   ((struct stm32f_exti *)STM32_BASE_EXTI)
#define STM32_COMP   ((struct stm32_comp *)STM32_BASE_COMP)

#define STM32_USART1 ((struct stm32_usart *)STM32_BASE_USART1)
#define STM32_USART2 ((struct stm32_usart *)STM32_BASE_USART2)
#define STM32_USART3 ((struct stm32_usart *)STM32_BASE_USART3)
#define STM32_UART4  ((struct stm32_usart *)STM32_BASE_UART4)
#define STM32_UART5  ((struct stm32_usart *)STM32_BASE_UART5)

#define STM32_SPI1   ((struct stm32f_spi *)STM32_BASE_SPI1)
#define STM32_SPI2   ((struct stm32f_spi *)STM32_BASE_SPI2)
#define STM32_SPI3   ((struct stm32f_spi *)STM32_BASE_SPI3)

#define STM32_DMA1   ((struct stm32f_dma *)STM32_BASE_DMA1)
#define STM32_DMA2   ((struct stm32f_dma *)STM32_BASE_DMA2)

#define STM32_TIM2   ((struct stm32f_tim *)STM32_BASE_TIM2)
#define STM32_TIM3   ((struct stm32f_tim *)STM32_BASE_TIM3)
#define STM32_TIM4   ((struct stm32f_tim *)STM32_BASE_TIM4)
#define STM32_TIM5   ((struct stm32f_tim *)STM32_BASE_TIM5)
#define STM32_TIM6   ((struct stm32f_tim *)STM32_BASE_TIM6)
#define STM32_TIM7   ((struct stm32f_tim *)STM32_BASE_TIM7)
#define STM32_TIM9   ((struct stm32f_tim *)STM32_BASE_TIM9)
#define STM32_TIM10  ((struct stm32f_tim *)STM32_BASE_TIM10)
#define STM32_TIM11  ((struct stm32f_tim *)STM32_BASE_TIM11)

#define STM32_GPIOA  ((struct stm32_gpio *)STM32_BASE_GPIOA)
#define STM32_GPIOB  ((struct stm32_gpio *)STM32_BASE_GPIOB)
#define STM32_GPIOC  ((struct stm32_gpio *)STM32_BASE_GPIOC)
#define STM32_GPIOD  ((struct stm32_gpio *)STM32_BASE_GPIOD)
#define STM32_GPIOE  ((struct stm32_gpio *)STM32_BASE_GPIOE)
#define STM32_GPIOH  ((struct stm32_gpio *)STM32_BASE_GPIOH)
#define STM32_GPIOF  ((struct stm32_gpio *)STM32_BASE_GPIOF)
#define STM32_GPIOG  ((struct stm32_gpio *)STM32_BASE_GPIOG)

#define STM32_GPIO(N) ((struct stm32_gpio *)(STM32_BASE_GPIOA + (N)*0x400))

#define STM32_AFIO ((struct stm32f_afio *)STM32_BASE_AFIO)

#define STM32_UID ((uint32_t *)STM32_BASE_UID)

enum {
	STM32_GPIOA_ID = 0,
	STM32_GPIOB_ID,
	STM32_GPIOC_ID,
	STM32_GPIOD_ID,
	STM32_GPIOE_ID,
	STM32_GPIOH_ID,
	STM32_GPIOF_ID,
	STM32_GPIOG_ID
};

enum {
	STM32_UART1_ID = 0,
	STM32_UART2_ID, 
	STM32_UART3_ID,
	STM32_UART4_ID,
	STM32_UART5_ID
};

#define STM32_MEM_FLASH  (void *)(0x08000000)
#define STM32_MEM_EEPROM (void *)(0x08080000)
#define STM32_MEM_SRAM   (void *)(0x20000000)

#endif /* __ASSEMBLER__ */

#endif /* __STM32L151_H__ */

