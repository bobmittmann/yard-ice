/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
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
 * @file stm32f100.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

/* 
   High-density performance line ARM-based 32-bit MCU with 256 to
   512KB Flash, USB, CAN, 11 timers, 3 ADCs, 13 communication interfaces
*/

#ifndef __STM32F100_H__
#define __STM32F100_H__

#define STM32F
#define STM32F1X

#include <stm32f/stm32f-pwr.h>
#include <stm32f/stm32f-syscfg.h>
#include <stm32f/stm32f-rcc.h>
#include <stm32f/stm32f-usart.h>
#include <stm32f/stm32f-gpio.h>
#include <stm32f/stm32f-flash.h>
#include <stm32f/stm32f-dac.h>
#include <stm32f/stm32f-adc.h>
#include <stm32f/stm32f-rtc.h>
#include <stm32f/stm32f-spi.h>
#include <stm32f/stm32f-i2c.h>
#include <stm32f/stm32f-fsmc.h>
#include <stm32f/stm32f-tim.h>
#include <stm32f/stm32f-dma.h>
#include <stm32f/stm32f-exti.h>
#include <stm32f/stm32f-iwdg.h>

#define STM32F_BASE_FSMC    0xa0000000

#define STM32F_BASE_CRC     0x40023000
#define STM32F_BASE_FLASH   0x40022000
#define STM32F_BASE_RCC     0x40021000
#define STM32F_BASE_DMA2    0x40020400
#define STM32F_BASE_DMA1    0x40020000

#define STM32F_BASE_TIM17   0x40014800
#define STM32F_BASE_TIM16   0x40014400
#define STM32F_BASE_TIM15   0x40014000

#define STM32F_BASE_USART1  0x40013800

#define STM32F_BASE_SPI1    0x40013000
#define STM32F_BASE_TIM1    0x40012c00

#define STM32F_BASE_ADC1    0x40012400
#define STM32_BASE_GPIOG   0x40012000
#define STM32_BASE_GPIOF   0x40011c00
#define STM32_BASE_GPIOE   0x40011800
#define STM32_BASE_GPIOD   0x40011400
#define STM32_BASE_GPIOC   0x40011000
#define STM32_BASE_GPIOB   0x40010c00
#define STM32_BASE_GPIOA   0x40010800
#define STM32F_BASE_EXTI    0x40010400
#define STM32F_BASE_AFIO    0x40010000

#define STM32F_BASE_CEC     0x40007800
#define STM32F_BASE_DAC     0x40007400

#define STM32F_BASE_PWR     0x40007000
#define STM32F_BASE_BKP     0x40006c00

#define STM32F_BASE_I2C2    0x40005800
#define STM32F_BASE_I2C1    0x40005400

#define STM32F_BASE_UART5   0x40005000
#define STM32F_BASE_UART4   0x40004c00
#define STM32F_BASE_USART3  0x40004800
#define STM32F_BASE_USART2  0x40004400
#define STM32F_BASE_SPI3    0x40003c00
#define STM32F_BASE_SPI2    0x40003800
#define STM32F_BASE_IWDG    0x40003000
#define STM32F_BASE_WWDG    0x40002c00
#define STM32F_BASE_RTC     0x40002800

#define STM32F_BASE_TIM14   0x40002000
#define STM32F_BASE_TIM13   0x40001c00
#define STM32F_BASE_TIM12   0x40001800
#define STM32F_BASE_TIM7    0x40001400
#define STM32F_BASE_TIM6    0x40001000
#define STM32F_BASE_TIM5    0x40000c00
#define STM32F_BASE_TIM4    0x40000800
#define STM32F_BASE_TIM3    0x40000400
#define STM32F_BASE_TIM2    0x40000000


#define STM32F_IRQ_WWDG 0 /* Window WatchDog Interrupt */
#define STM32F_IRQ_PVD 1 /* PVD through EXTI Line detection Interrupt */
#define STM32F_IRQ_TAMPER 2 /* Tamper interrupts through the EXTI line */
#define STM32F_IRQ_RTC_WKUP 3 /* RTC Wakeup interrupt through the EXTI line */
#define STM32F_IRQ_FLASH 4 /* FLASH global Interrupt */
#define STM32F_IRQ_RCC 5 /* RCC global Interrupt */
#define STM32F_IRQ_EXTI0 6 /* EXTI Line0 Interrupt */
#define STM32F_IRQ_EXTI1 7 /* EXTI Line1 Interrupt */

#define STM32F_IRQ_EXTI2 8 /* EXTI Line2 Interrupt */
#define STM32F_IRQ_EXTI3 9 /* EXTI Line3 Interrupt */
#define STM32F_IRQ_EXTI4 10 /* EXTI Line4 Interrupt */
#define STM32F_IRQ_DMA1_STREAM0 11 /* DMA1 Stream 0 global Interrupt */
#define STM32F_IRQ_DMA1_STREAM1 12 /* DMA1 Stream 1 global Interrupt */
#define STM32F_IRQ_DMA1_STREAM2 13 /* DMA1 Stream 2 global Interrupt */
#define STM32F_IRQ_DMA1_STREAM3 14 /* DMA1 Stream 3 global Interrupt */
#define STM32F_IRQ_DMA1_STREAM4 15 /* DMA1 Stream 4 global Interrupt */

#define STM32F_IRQ_DMA1_STREAM5 16 /* DMA1 Stream 5 global Interrupt */
#define STM32F_IRQ_DMA1_STREAM6 17 /* DMA1 Stream 6 global Interrupt */
#define STM32F_IRQ_ADC 18 /* ADC1 and ADC2 global Interrupts */

#define STM32F_IRQ_EXTI9_5 23 /* External Line[9:5] Interrupts */

/* TIM1 Break interrupt and TIM15 global interrupt */
#define STM32F_IRQ_TIM1_BRK 24 /* TIM1 Break interrupt  */
#define STM32F_IRQ_TIM15 24 /* TIM15 global interrupt */

/* TIM1 Update Interrupt and TIM16 global interrupt */
#define STM32F_IRQ_TIM1_UP 25 /* TIM1 Update Interrupt */
#define STM32F_IRQ_TIM16 25 /* TIM16 global interrupt */

/* TIM1 Trigger and Commutation Interrupt and TIM17 global interrupt */
#define STM32F_IRQ_TIM1_TRG_COM 26 /* TIM1 Trigger and Commutation Interrupt */
#define STM32F_IRQ_TIM17 26 /* TIM17 global interrupt */

#define STM32F_IRQ_TIM1_CC 27 /* TIM1 Capture Compare Interrupt */
#define STM32F_IRQ_TIM2 28 /* TIM2 global Interrupt */
#define STM32F_IRQ_TIM3 29 /* TIM3 global Interrupt */
#define STM32F_IRQ_TIM4 30 /* TIM4 global Interrupt */
#define STM32F_IRQ_I2C1_EV 31 /* I2C1 Event Interrupt */
#define STM32F_IRQ_I2C1_ER 32 /* I2C1 Error Interrupt */
#define STM32F_IRQ_I2C2_EV 33 /* I2C2 Event Interrupt */
#define STM32F_IRQ_I2C2_ER 34 /* I2C2 Error Interrupt */
#define STM32F_IRQ_SPI1 35 /* SPI1 global Interrupt */
#define STM32F_IRQ_SPI2 36 /* SPI2 global Interrupt */
#define STM32_IRQ_USART1 37 /* USART1 global Interrupt */
#define STM32_IRQ_USART2 38 /* USART2 global Interrupt */
#define STM32_IRQ_USART3 39 /* USART3 global Interrupt */
#define STM32F_IRQ_EXTI15_10 40 /* External Line[15:10] Interrupts */
#define STM32F_IRQ_RTC_ALARM 41 /* RTC Alarm (A and B) through EXTI Line Interrupt */

#define STM32F_IRQ_CEC 42 /* CEC Global interrupt */

#define STM32F_IRQ_TIM12 43 /* TIM12 global interrupt */
#define STM32F_IRQ_TIM13 44 /* TIM13 global interrupt */
#define STM32F_IRQ_TIM14 45 /* TIM14 global interrupt */

#define STM32F_IRQ_FSMC 48 /* FSMC global Interrupt */

#define STM32F_IRQ_TIM5 50 /* TIM5 global Interrupt */
#define STM32F_IRQ_SPI3 51 /* SPI3 global Interrupt */
#define STM32_IRQ_UART4 52 /* UART4 global Interrupt */
#define STM32_IRQ_UART5 53 /* UART5 global Interrupt */

/* TIM6 global and DAC1&2 underrun error */
#define STM32F_IRQ_TIM6 54 /* TIM6 global Interrupt */
#define STM32F_IRQ_DAC 54 /* DAC1&2 underrun error Interrupt */

#define STM32F_IRQ_TIM7 55 /* TIM7 global interrupt */
#define STM32F_IRQ_DMA2_STREAM0 56 /* DMA2 Stream 0 global Interrupt */
#define STM32F_IRQ_DMA2_STREAM1 57 /* DMA2 Stream 1 global Interrupt */
#define STM32F_IRQ_DMA2_STREAM2 58 /* DMA2 Stream 2 global Interrupt */
#define STM32F_IRQ_DMA2_STREAM3 59 /* DMA2 Stream 3 global Interrupt */

#ifndef __ASSEMBLER__

#define STM32F_PWR ((struct stm32f_pwr *)STM32F_BASE_PWR)
#define STM32F_RCC ((struct stm32f_rcc *)STM32F_BASE_RCC)
#define STM32F_RTC ((struct stm32f_rtc *)STM32F_BASE_RTC)
#define STM32F_FLASH ((struct stm32f_flash *)STM32F_BASE_FLASH)
#define STM32F_FSMC ((struct stm32f_fsmc *)STM32F_BASE_FSMC)
#define STM32F_DAC ((struct stm32f_dac *)STM32F_BASE_DAC)
#define STM32F_ADC1 ((struct stm32f_adc *)STM32F_BASE_ADC1)

#define STM32F_EXTI ((struct stm32f_exti *)STM32F_BASE_EXTI)
#define STM32F_FLASH ((struct stm32f_flash *)STM32F_BASE_FLASH)

#define STM32_USART1 ((struct stm32f_usart *)STM32F_BASE_USART1)
#define STM32_USART2 ((struct stm32f_usart *)STM32F_BASE_USART2)
#define STM32_USART3 ((struct stm32f_usart *)STM32F_BASE_USART3)
#define STM32_UART4 ((struct stm32f_usart *)STM32F_BASE_UART4)
#define STM32_UART5 ((struct stm32f_usart *)STM32F_BASE_UART5)

#define STM32F_SPI1 ((struct stm32f_spi *)STM32F_BASE_SPI1)
#define STM32F_SPI2 ((struct stm32f_spi *)STM32F_BASE_SPI2)
#define STM32F_SPI3 ((struct stm32f_spi *)STM32F_BASE_SPI3)

#define STM32F_I2C1 ((struct stm32f_i2c *)STM32F_BASE_I2C1)

#define STM32F_DMA1 ((struct stm32f_dma *)STM32F_BASE_DMA1)
#define STM32F_DMA2 ((struct stm32f_dma *)STM32F_BASE_DMA2)

#define STM32F_TIM1 ((struct stm32f_tim *)STM32F_BASE_TIM1)

#define STM32F_TIM2 ((struct stm32f_tim *)STM32F_BASE_TIM2)
#define STM32F_TIM3 ((struct stm32f_tim *)STM32F_BASE_TIM3)
#define STM32F_TIM4 ((struct stm32f_tim *)STM32F_BASE_TIM4)
#define STM32F_TIM5 ((struct stm32f_tim *)STM32F_BASE_TIM5)

#define STM32F_TIM6 ((struct stm32f_tim *)STM32F_BASE_TIM6)
#define STM32F_TIM7 ((struct stm32f_tim *)STM32F_BASE_TIM7)

#define STM32F_TIM12 ((struct stm32f_tim *)STM32F_BASE_TIM12)
#define STM32F_TIM13 ((struct stm32f_tim *)STM32F_BASE_TIM13)
#define STM32F_TIM14 ((struct stm32f_tim *)STM32F_BASE_TIM14)

#define STM32F_TIM15 ((struct stm32f_tim *)STM32F_BASE_TIM15)
#define STM32F_TIM16 ((struct stm32f_tim *)STM32F_BASE_TIM16)
#define STM32F_TIM17 ((struct stm32f_tim *)STM32F_BASE_TIM17)

#define STM32_GPIOA ((struct stm32_gpio *)STM32_BASE_GPIOA)
#define STM32_GPIOB ((struct stm32_gpio *)STM32_BASE_GPIOB)
#define STM32_GPIOC ((struct stm32_gpio *)STM32_BASE_GPIOC)
#define STM32_GPIOD ((struct stm32_gpio *)STM32_BASE_GPIOD)
#define STM32_GPIOE ((struct stm32_gpio *)STM32_BASE_GPIOE)
#define STM32_GPIOF ((struct stm32_gpio *)STM32_BASE_GPIOF)
#define STM32_GPIOG ((struct stm32_gpio *)STM32_BASE_GPIOG)

#define STM32_GPIO(N) ((struct stm32_gpio *)(STM32_BASE_GPIOA + (N)*0x400))

#define STM32_AFIO ((struct stm32_afio *)STM32F_BASE_AFIO)

#define STM32F_IWDG ((struct stm32f_iwdg *)STM32F_BASE_IWDG)

enum {
	STM32_GPIOA_ID = 0,
	STM32_GPIOB_ID,
	STM32_GPIOC_ID,
	STM32_GPIOD_ID,
	STM32_GPIOE_ID,
	STM32_GPIOF_ID,
	STM32_GPIOG_ID,
};

enum {
	STM32_UART1_ID = 0,
	STM32_UART2_ID, 
	STM32_UART3_ID,
	STM32_UART4_ID,
	STM32_UART5_ID
};

#define STM32F_FSMC_NE1 ((uint32_t *)0x60000000)

#endif /* __ASSEMBLER__ */

#endif /* __STM32F100_H__ */

