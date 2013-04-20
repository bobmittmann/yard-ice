#ifndef __STM32F_EXTI_H__
#define __STM32F_EXTI_H__

/*-------------------------------------------------------------------------
 * External interrupt/event controller (EXTI)
 *-------------------------------------------------------------------------*/

/* Interrupt mask register */
#define STM32F_EXTI_IMR 0x00

/* [31..23] Reserved, must be kept at reset value (0). */

/* Bits [22..0] - Interrupt mask on line x */
#define EXTI_MRX ((22 - 0) << 0)
/* 0: Interrupt request from line x is masked
   1: Interrupt request from line x is not masked */

/* Event mask register */
#define STM32F_EXTI_EMR 0x04

/* [31..23] Reserved, must be kept at reset value (0). */

/* Bits [22..0] - Event mask on line x */
#define EXTI_MRX ((22 - 0) << 0)
/* 0: Event request from line x is masked
   1: Event request from line x is not masked */

/* Rising trigger selection register */
#define STM32F_EXTI_RTSR 0x08

/* [31..23] Reserved, must be kept at reset value (0). */

/* Bits [22..0] - Rising trigger event configuration bit of line x */
#define EXTI_TRX ((22 - 0) << 0)
/* 0: Rising trigger disabled (for Event and Interrupt) for input line
   1: Rising trigger enabled (for Event and Interrupt) for input line

   Note: The external wakeup lines are edge triggered, no glitch must be 
   generated on these lines.
   If a rising edge occurs on the external interrupt line while writing to 
   the EXTI_RTSR register, the pending bit is be set.
   Rising and falling edge triggers can be set for the same interrupt line. 
   In this configuration, both generate a trigger condition. */

/* Falling trigger selection register */
#define STM32F_EXTI_FTSR 0x0C

/* Bits [22..0] - Falling trigger event configuration bit of line x */
#define EXTI_TRX ((22 - 0) << 0)
/* 0: Falling trigger disabled (for Event and Interrupt) for input line
   1: Falling trigger enabled (for Event and Interrupt) for input line.
   Note: The external wakeup lines are edge triggered, no glitch must be 
   generated on these lines.
   If a falling edge occurs on the external interrupt line while writing to 
   the EXTI_FTSR register, the pending bit is not set.
   Rising and falling edge triggers can be set for the same interrupt line. 
   In this configuration, both generate a trigger condition. */

/* Software interrupt event register */
#define STM32F_EXTI_SWIER 0x10


/* Bits [22..0] - Software Interrupt on line x */
#define EXTI_SWIERX ((22 - 0) << 0)
/* Writing a 1 to this bit when it is at 0 sets the corresponding pending 
   bit in EXTI_PR. If the interrupt is enabled on this line on the 
   EXTI_IMR and EXTI_EMR, an interrupt request is generated.
   This bit is cleared by clearing the corresponding bit in 
   EXTI_PR (by writing a 1 to the bit). */

/* Pending register */
#define STM32F_EXTI_PR 0x14

/* Bits [22..0] - Pending bit */
#define EXTI_PRX ((22 - 0) << 0)
/* 0: No trigger request occurred
   1: selected trigger request occurred
   This bit is set when the selected edge event arrives on the external 
   interrupt line. This bit is cleared by writing a 1 to the bit or by 
   changing the sensitivity of the edge detector. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_exti {
	volatile uint32_t imr;
	volatile uint32_t emr;
	volatile uint32_t rtsr;
	volatile uint32_t ftsr;
	volatile uint32_t swier;
	volatile uint32_t pr;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_EXTI_H__ */

