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
 * @file stm32f-gpio.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __STM32F_GPIO_H__
#define __STM32F_GPIO_H__

/*-------------------------------------------------------------------------
 GPIO
 -------------------------------------------------------------------------*/

/* GPIO port mode register */
#define GPIO_MODER 0x00
#define GPIO_MODE_INPUT(P) (0 << (2 * (P)))
#define GPIO_MODE_OUTPUT(P) (1 << (2 * (P)))
#define GPIO_MODE_ALT_FUNC(P) (2 << (2 * (P)))
#define GPIO_MODE_ANALOG(P) (3 << (2 * (P)))
#define GPIO_MODE_MASK(P) (3 << (2 * (P)))
#define GPIO_MODE_GET(R, P) (((R) >> (2 * (P))) & 3)

/* GPIO port output type register */
#define GPIO_OTYPER 0x04
#define GPIO_PUSH_PULL(Y) (0 << (Y))
#define GPIO_OPEN_DRAIN(Y) (1 << (Y))
#define GPIO_OTYPE_MASK(Y) (1 << (Y))

/* GPIO port output speed register */
#define GPIO_OSPEEDR 0x08
#define GPIO_OSPEED_LOW(P) (0 << (2 * (P)))
#define GPIO_OSPEED_MED(P) (1 << (2 * (P)))
#define GPIO_OSPEED_FAST(P) (2 << (2 * (P)))
#define GPIO_OSPEED_HIGH(P) (3 << (2 * (P)))
#define GPIO_OSPEED_MASK(Y) (3 << (2 * (Y)))

/* GPIO port pull-up/pull-down register */
#define GPIO_PUPDR 0x0c
#define GPIO_PULL_NONE(P) (0 << (2 * (P)))
#define GPIO_PULL_UP(P) (1 << (2 * (P)))
#define GPIO_PULL_DOWN(P) (2 << (2 * (P)))
#define GPIO_PULL_MASK(Y) (3 << (2 * (Y)))

/* GPIO port input data register */
#define GPIO_IDR 0x10

/* GPIO port output data register */
#define GPIO_ODR 0x14

/* GPIO port bit set/reset register */
#define GPIO_BSRR 0x18
#define GPIO_SET(Y) (1 << (Y))
#define GPIO_RESET(Y) (1 << (Y))

/* GPIO port configuration lock register */
#define GPIO_LCKR 0x1c
#define GPIO_LCKK (1 << 16)

/* GPIO alternate function low register */
#define GPIO_AFRL 0x20
#define GPIO_AFRL_SET(P, F) ((F) << (4 * (P)))
#define GPIO_AFRL_MASK(P) ((0xf) << (4 * (P)))

/* GPIO alternate function high register */
#define GPIO_AFRH 0x24
#define GPIO_AFRH_SET(P, F) ((F) << (4 * ((P) - 8)))
#define GPIO_AFRH_MASK(P) ((0xf) << (4 * ((P) - 8)))

/* GPIO bit reset register */
#define GPIO_BRR 0x28

#define GPIO_AF0 0
#define GPIO_AF1 1
#define GPIO_AF2 2
#define GPIO_AF3 3
#define GPIO_AF4 4
#define GPIO_AF5 5
#define GPIO_AF6 6
#define GPIO_AF7 7
#define GPIO_AF8 8
#define GPIO_AF9 9
#define GPIO_AF10 10
#define GPIO_AF11 11
#define GPIO_AF12 12
#define GPIO_AF13 13
#define GPIO_AF14 14
#define GPIO_AF15 15

#if defined(STM32F1X) 

/* ------------------------------------------------------------------------- */
/* AF remap and debug I/O configuration register () */
#define STM32F_AFIO_MAPR 0x04

/* Bits [26..24] - Serial wire JTAG configuration */
#define AFIO_SWJ_CFG_MSK (((1 << (2 + 1)) - 1) << 24)
#define AFIO_SWJ_CFG_SET(VAL) (((VAL) << 24) & SWJ_CFG_MSK)
#define AFIO_SWJ_CFG_GET(REG) (((REG) & SWJ_CFG_MSK) >> 24)
/* These bits are write-only (when read, the value is undefined). They are used 
   to configure the SWJ and trace alternate function I/Os. The SWJ (Serial Wire 
   JTAG) supports JTAG or SWD access to the Cortex debug port. The default
   state after reset is SWJ ON without trace. This allows JTAG or SW mode to be 
   enabled by sending a specific sequence on the JTMS / JTCK pin. 000: Full SWJ 
   (JTAG-DP + SW-DP): Reset State 001: Full SWJ (JTAG-DP + SW-DP) but without
   NJTRST 010: JTAG-DP Disabled and SW-DP Enabled 100: JTAG-DP Disabled and
   SW-DP Disabled Other combinations: no effect Bits 23:21 Reserved. */

/* Bit 20 - ADC 2 external trigger regular conversion remapping */
#define AFIO_ADC2_ETRGREG_REMAP (1 << 20)
/* Set and cleared by software. This bit controls the trigger input connected
   to ADC2 external trigger regular conversion. When this bit is reset, the
   ADC2 external trigger regular conversion is connected to EXTI11. When this
   bit is set, the ADC2 external event regular conversion is connected to
   TIM8_TRGO. */

/* Bit 19 - ADC 2 external trigger injected conversion remapping */
#define AFIO_ADC2_ETRGINJ_REMAP (1 << 19)
/* Set and cleared by software. This bit controls the trigger input connected
   to ADC2 external trigger injected conversion. When this bit is reset, the
   ADC2 external trigger injected conversion is connected to EXTI15. When this
   bit is set, the ADC2 external event injected conversion is connected to
   TIM8_Channel4. */

/* Bit 18 - ADC 1 external trigger regular conversion remapping */
#define AFIO_ADC1_ETRGREG_REMAP (1 << 18)
/* Set and cleared by software. This bit controls the trigger input connected
   to ADC1 External trigger regular conversion. When reset the ADC1 External
   trigger regular conversion is connected to EXTI11. W */

/* Bit 17 - ADC 1 External trigger injected conversion remapping */
#define AFIO_ADC1_ETRGINJ_REMAP (1 << 17)
/* Set and cleared by software. This bit controls the trigger input connected
   to ADC1 External trigger injected conversion. When reset the ADC1 External
   trigger injected conversion is connected to EXTI15. When set the ADC1
   External Event injected conversion is connected to TIM8 Channel4. */

/* Bit 16 - TIM5 channel4 internal remap */
#define AFIO_TIM5CH4_IREMAP (1 << 16)
/* Set and cleared by software. This bit controls the TIM5_CH4 internal
   mapping. When reset the timer TIM5_CH4 is connected to PA3. When set the LSI 
   internal clock is connected to TIM5_CH4 input for calibration purpose. Note: 
   This bit is available only in high density value line devices. */

/* Bit 15 - Port D0/Port D1 mapping on OSC_IN/OSC_OUT */
#define AFIO_PD01_REMAP (1 << 15)
/* This bit is set and cleared by software. It controls the mapping of PD0 and
   PD1 GPIO functionality. When the HSE oscillator is not used (application
   running on internal 8 MHz RC) PD0 and PD1 can be mapped on OSC_IN and
   OSC_OUT. This is available only on 36-, 48- and 64-pin packages (PD0 and PD1 
   are available on 100-pin and 144-pin packages, no need for remapping). 0: No 
   remapping of PD0 and PD1 1: PD0 remapped on OSC_IN, PD1 remapped on OSC_OUT, 
 */

/* Bits [14..13] - CAN alternate function remapping */
#define AFIO_CAN_REMAP_MSK (((1 << (1 + 1)) - 1) << 13)
#define AFIO_CAN_REMAP_SET(VAL) (((VAL) << 13) & CAN_REMAP_MSK)
#define AFIO_CAN_REMAP_GET(REG) (((REG) & CAN_REMAP_MSK) >> 13)
/* These bits are set and cleared by software. They control the mapping of
   alternate functions CAN_RX and CAN_TX in devices with a single CAN
   interface. 00: CAN_RX mapped to PA11, CAN_TX mapped to PA12 01: Not used 10: 
   CAN_RX mapped to PB8, CAN_TX mapped to PB9 (not available on 36-pin package)
   11: CAN_RX mapped to PD0, CAN_TX mapped to PD1 */

/* Bit 12 - TIM4 remapping */
#define AFIO_TIM4_REMAP (1 << 12)
/* This bit is set and cleared by software. It controls the mapping of TIM4
   channels 1 to 4 onto the GPIO ports. 0: No remap (TIM4_CH1/PB6,
   TIM4_CH2/PB7, TIM4_CH3/PB8, TIM4_CH4/PB9) 1: Full remap (TIM4_CH1/PD12,
   TIM4_CH2/PD13, TIM4_CH3/PD14, TIM4_CH4/PD15) Note: TIM4_ETR on PE0 is not
   re-mapped. */

/* Bits [11..10] - TIM3 remapping */
#define AFIO_TIM3_REMAP_MSK (((1 << (1 + 1)) - 1) << 10)
#define AFIO_TIM3_REMAP_SET(VAL) (((VAL) << 10) & TIM3_REMAP_MSK)
#define AFIO_TIM3_REMAP_GET(REG) (((REG) & TIM3_REMAP_MSK) >> 10)
/* These bits are set and cleared by software. They control the mapping of TIM3 
   channels 1 to 4 on the GPIO ports. 00: No remap (CH1/PA6, CH2/PA7, CH3/PB0,
   CH4/PB1) 01: Not used 10: Partial remap (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1)
   11: Full remap (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9) Note: TIM3_ETR on PE0 is
   not re-mapped. */

/* Bits [9..8] - TIM2 remapping */
#define AFIO_TIM2_REMAP_MSK (((1 << (1 + 1)) - 1) << 8)
#define AFIO_TIM2_REMAP_SET(VAL) (((VAL) << 8) & TIM2_REMAP_MSK)
#define AFIO_TIM2_REMAP_GET(REG) (((REG) & TIM2_REMAP_MSK) >> 8)
/* These bits are set and cleared by software. They control the mapping of TIM2 
   channels 1 to 4 and external trigger (ETR) on the GPIO ports. 00: No remap
   (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3) 01: Partial remap (CH1/ETR/PA15,
   CH2/PB3, CH3/PA2, CH4/PA3) 10: Partial remap (CH1/ETR/PA0, CH2/PA1,
   CH3/PB10, CH4/PB11) 11: Full remap (CH1/ETR/PA15, CH2/PB3, CH3/PB10,
   CH4/PB11) */

/* Bits [7..6] - TIM1 remapping */
#define AFIO_TIM1_REMAP_MSK (((1 << (1 + 1)) - 1) << 6)
#define AFIO_TIM1_REMAP_SET(VAL) (((VAL) << 6) & TIM1_REMAP_MSK)
#define AFIO_TIM1_REMAP_GET(REG) (((REG) & TIM1_REMAP_MSK) >> 6)
/* These bits are set and cleared by software. They control the mapping of TIM1 
   channels 1 to 4, 1N to 3N, external trigger (ETR) and Break input (BKIN) on
   the GPIO ports. 00: No remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10,
   CH4/PA11, BKIN/PB12, CH1N/PB13, CH2N/PB14, CH3N/PB15) 01: Partial remap
   (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7,
   CH2N/PB0, CH3N/PB1) 10: not used 11: Full remap (ETR/PE7, CH1/PE9, CH2/PE11, 
   CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12) */

/* Bits [5..4] - USART3 remapping */
#define AFIO_USART3_REMAP_MSK (((1 << (1 + 1)) - 1) << 4)
#define AFIO_USART3_REMAP_SET(VAL) (((VAL) << 4) & USART3_REMAP_MSK)
#define AFIO_USART3_REMAP_GET(REG) (((REG) & USART3_REMAP_MSK) >> 4)
/* These bits are set and cleared by software. They control the mapping of
   USART3 CTS, RTS,CK,TX and RX alternate functions on the GPIO ports. 00: No
   remap (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14) 01: Partial remap
   (TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14) 10: not used 11: Full remap
   (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12) */

/* Bit 3 - USART2 remapping */
#define AFIO_USART2_REMAP (1 << 3)
/* This bit is set and cleared by software. It controls the mapping of USART2
   CTS, RTS,CK,TX and RX alternate functions on the GPIO ports. 0: No remap
   (CTS/PA0, RTS/PA1, TX/PA2, RX/PA3, CK/PA4) 1: Remap (CTS/PD3, RTS/PD4,
   TX/PD5, RX/PD6, CK/PD7) */

/* Bit 2 - USART1 remapping */
#define AFIO_USART1_REMAP (1 << 2)
/* This bit is set and cleared by software. It controls the mapping of USART1
   TX and RX alternate functions on the GPIO ports. 0: No remap (TX/PA9,
   RX/PA10) 1: Remap (TX/PB6, RX/PB7) */

/* Bit 1 - I2C1 remapping */
#define AFIO_I2C1_REMAP (1 << 1)
/* This bit is set and cleared by software. It controls the mapping of I2C1 SCL 
   and SDA alternate functions on the GPIO ports. 0: No remap (SCL/PB6,
   SDA/PB7) 1: Remap (SCL/PB8, SDA/PB9) */

/* Bit 0 - SPI1 remapping */
#define AFIO_SPI1_REMAP (1 << 0)
/* This bit is set and cleared by software. It controls the mapping of SPI1
   NSS, SCK, MISO, MOSI alternate functions on the GPIO ports. 0: No remap
   (NSS/PA4, SCK/PA5, MISO/PA6, MOSI/PA7) 1: Remap (NSS/PA15, SCK/PB3,
   MISO/PB4, MOSI/PB5) */

/* ------------------------------------------------------------------------- */
/* AF remap and debug I/O configuration register2 */
#define STM32F_AFIO_MAPR2 0x1C

/* Bit 10 - NADV connect/disconnect */
#define AFIO_FSMC_NADV (1 << 10)
/* This bit is set and cleared by software. It controls the use of the optional 
   FSMC_NADV signal. 0: The NADV signal is connected to the output (default) 1: 
   The NADV signal is not connected. The I/O pin can be used by another
   peripheral. */

/* Bit 9 - TIM14 remapping */
#define AFIO_TIM14_REMAP (1 << 9)
/* This bit is set and cleared by software. It controls the mapping of the
   TIM14_CH1 alternate function onto the GPIO ports. 0: No remap (PA7) 1: Remap 
   (PF9) */

/* Bit 8 - TIM13 remapping */
#define AFIO_TIM13_REMAP (1 << 8)
/* This bit is set and cleared by software. It controls the mapping of the
   TIM13_CH1 alternate function onto the GPIO ports. 0: No remap (PA6) 1: Remap 
   (PF8) */

/* Bit 7 - TIM11 remapping */
#define AFIO_TIM11_REMAP (1 << 7)
/* This bit is set and cleared by software. It controls the mapping of the
   TIM11_CH1 alternate function onto the GPIO ports. 0: No remap (PB9) 1: Remap 
   (PF7) */

/* Bit 6 - TIM10 remapping */
#define AFIO_TIM10_REMAP (1 << 6)
/* This bit is set and cleared by software. It controls the mapping of the
   TIM10_CH1 alternate function onto the GPIO ports. 0: No remap (PB8) 1: Remap 
   (PF6) */

/* Bit 5 - TIM9 remapping */
#define AFIO_TIM9_REMAP (1 << 5)
/* This bit is set and cleared by software. It controls the mapping of the
   TIM9_CH1 and TIM9_CH2 alternate functions onto the GPIO ports. 0: No remap
   (TIM9_CH1 on PA2 and TIM9_CH2 on PA3) 1: Remap (TIM9_CH1 on PE5 and TIM9_CH2 
   on PE6) Bits 4:0 Reserved. */

#endif /* defined(STM32F1X) */

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stdbool.h>

#if defined(STM32F2X) || defined(STM32F3X) || defined(STM32F4X) || \
	defined(STM32L1X)
struct stm32_gpio {
	volatile uint32_t moder;
	volatile uint32_t otyper;
	volatile uint32_t ospeedr;
	volatile uint32_t pupdr;
	volatile uint32_t idr;
	volatile uint32_t odr;
	volatile uint16_t bsr;
	volatile uint16_t brr;
	volatile uint32_t lckr;
	volatile uint32_t afrl;
	volatile uint32_t afrh;
};

#endif

#if defined(STM32F1X)
struct stm32_gpio {
	volatile uint32_t crl;
	volatile uint32_t crh;
	volatile uint32_t idr;
	volatile uint32_t odr;
	volatile uint32_t bsr;
	volatile uint32_t brr;
	volatile uint32_t lckr;
};

struct stm32_afio {
	volatile uint32_t evcr;
	volatile uint32_t mapr;
	volatile uint32_t exticr1;
	volatile uint32_t exticr2;

	volatile uint32_t exticr3;
	volatile uint32_t exticr4;
	volatile uint32_t mapr2;
};

#endif /* defined(STM32F1X) */

#define	SPEED_LOW (0 << 0)
#define	SPEED_MED (1 << 0)
#define	SPEED_FAST (2 << 0)
#define	SPEED_HIGH (3 << 0)

#define OPT_SPEED(OPT) (((OPT) >> 0) & 0x3)

#define PUSH_PULL (0 << 4)
#define OPEN_DRAIN (1 << 4)

#define PULL_UP (1 << 5)
#define PULL_DOWN (1 << 6)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F2X) || defined(STM32F3X) || defined(STM32F4X) || \
	defined(STM32L1X)
static inline void stm32_gpio_mode_out(struct stm32_gpio * __gpio, 
									   unsigned int __pin) {
	uint32_t tmp = __gpio->moder & ~GPIO_MODE_MASK(__pin);
	__gpio->moder = tmp | GPIO_MODE_OUTPUT(__pin);
}

static inline bool stm32_gpio_is_mode_out(struct stm32_gpio * __gpio, 
										 unsigned int __pin) {
	return (__gpio->moder & GPIO_MODE_MASK(__pin)) == GPIO_MODE_OUTPUT(__pin);
}

static inline void stm32_gpio_mode_in(struct stm32_gpio * __gpio, 
									  unsigned int __pin) {
	uint32_t tmp = __gpio->moder & ~GPIO_MODE_MASK(__pin);
	__gpio->moder = tmp | GPIO_MODE_INPUT(__pin);
}

static inline bool stm32_gpio_is_mode_in(struct stm32_gpio * __gpio, 
										 unsigned int __pin) {
	return (__gpio->moder & GPIO_MODE_MASK(__pin)) == GPIO_MODE_INPUT(__pin);
}

static inline void stm32_gpio_mode_af(struct stm32_gpio * __gpio, 
									  unsigned int __pin) {
	uint32_t tmp = __gpio->moder & ~GPIO_MODE_MASK(__pin);
	__gpio->moder = tmp | GPIO_MODE_ALT_FUNC(__pin);
}

static inline bool stm32_gpio_is_mode_af(struct stm32_gpio * __gpio, 
										 unsigned int __pin) {
	return (__gpio->moder & GPIO_MODE_MASK(__pin)) == GPIO_MODE_ALT_FUNC(__pin);
}

static inline uint32_t stm32_gpio_mode_get(struct stm32_gpio * __gpio, 
										   unsigned int __pin) {
	return GPIO_MODE_GET(__gpio->moder, __pin);
}

#endif

/* set pin */
static inline void stm32_gpio_set(struct stm32_gpio *__gpio, int __pin) {
	__gpio->bsr = GPIO_SET(__pin);
}

/* clear pin */
static inline void stm32_gpio_clr(struct stm32_gpio *__gpio, int __pin) {
	__gpio->brr = GPIO_RESET(__pin);
}

/* get pin status */
static inline int stm32_gpio_stat(struct stm32_gpio *__gpio, int __pin) {
	return __gpio->idr & (1 << __pin);
}

/* */
static inline void stm32_gpio_wr(struct stm32_gpio *__gpio, 
								uint32_t __val) {
	__gpio->odr = __val;
}

static inline uint32_t stm32_gpio_rd(struct stm32_gpio *__gpio) {
	return __gpio->idr;
}

static inline struct stm32_gpio * stm32_io_gpio(struct stm32_gpio * __gpio, 
												unsigned int __pin) {
	return __gpio;
}

static inline unsigned int stm32_io_pin(struct stm32_gpio * __gpio, 
										unsigned int __pin) {
	return __pin;
}

/* mode */
void stm32_gpio_mode(struct stm32_gpio * gpio, unsigned int pin,
					  unsigned int mode, unsigned int opt);

/* Alternate function selection */
void stm32_gpio_af(struct stm32_gpio * gpio, int port, int af);

#ifdef __cplusplus
}
#endif

#endif				/* __ASSEMBLER__ */

#endif				/* __STM32F_GPIO_H__ */
