/** @file 		drv_worm.c
 *  @brief
 *  	This file initializes the timer and GPIO interrupt for the arm rotation.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		17 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
bool arm1Hit = false;
bool arm2Hit = false;

/** @brief Initializes the low level registers to set up timer 1 and GPIO interrupts.
 *
 *  @return Void.
 */
void
wormInit(void)
{
	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR	|= RCC_APB2ENR_SYSCFGEN;
	RCC->APB2ENR	|= RCC_APB2ENR_TIM1EN;

	/* GPIOA Initialization */
	// PA8 as TIM1 CH1
	GPIOA->MODER 	&= ~GPIO_MODER_MODER8;
	GPIOA->MODER 	|= GPIO_MODER_MODER8_1;
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR8;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT8;
	GPIOA->AFR[1]	&= ~GPIO_AFRH_AFRH0;
	GPIOA->AFR[1]	|= (0x1 << 0U);
	// PA9 as TIM1 CH2
	GPIOA->MODER 	&= ~GPIO_MODER_MODER9;
	GPIOA->MODER 	|= GPIO_MODER_MODER9_1;
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR9;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT9;
	GPIOA->AFR[1]	&= ~GPIO_AFRH_AFRH1;
	GPIOA->AFR[1]	|= (0x1 << 4U);
	// PC8 as output and pulled down
	GPIOC->MODER 	|= GPIO_MODER_MODER8_0;;
	GPIOC->PUPDR	|= GPIO_PUPDR_PUPDR8_1;
	// PC9 as output and pulled down
	GPIOC->MODER 	|= GPIO_MODER_MODER9_0;
	GPIOC->PUPDR	|= GPIO_PUPDR_PUPDR9_1;
	// PC13 as input and pulled down
	GPIOC->MODER 	&= ~GPIO_MODER_MODER13;
	GPIOC->PUPDR	|= GPIO_PUPDR_PUPDR13_1;
	// PC14 as input and pulled down
	GPIOC->MODER 	&= ~GPIO_MODER_MODER14;
	GPIOC->PUPDR	|= GPIO_PUPDR_PUPDR14_1;

	/* TIM1 Initialization */
	TIM1->CCMR1 	&= ~TIM_CCMR1_OC1M;
	TIM1->CCMR1 	|= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; 	// enable CH1 as PWM mode on CCMR1
	TIM1->BDTR 		|= TIM_BDTR_MOE;
	TIM1->CCER 		&= ~TIM_CCER_CC1E;							// enables channel 1 as output

	TIM1->CCMR1 	&= ~TIM_CCMR1_OC2M;
	TIM1->CCMR1 	|= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; 	// enable CH1 as PWM mode on CCMR1
	TIM1->CCER 		&= ~TIM_CCER_CC2E;							// enables channel 1 as output

	TIM1->PSC		= 0;
	TIM1->ARR		= 5082 - 1;			// 42.5kHz
	TIM1->BDTR 		|= TIM_BDTR_MOE;
	TIM1->CNT		= 0;				// start the counter at 0
	TIM1->CR1		|= TIM_CR1_CEN;

	/* GPIO IRQ Initialization */
	SYSCFG->EXTICR[3]	|= SYSCFG_EXTICR4_EXTI13_PC |
						   SYSCFG_EXTICR4_EXTI14_PC;
	EXTI->RTSR			|= EXTI_RTSR_TR13 |
						   EXTI_RTSR_TR14;

	NVIC_SetPriority(EXTI15_10_IRQn, 0);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/** @brief Sets the PWM going to the motor driver circuit.
 *
 *  @return Void.
 */
void
wormDrive(void)
{
	float speed = 0.5;

	EXTI->IMR		&= ~(EXTI_IMR_IM13 |	// turn off GPIO interrupts
					  	 EXTI_IMR_IM14);
	TIM1->CCER 		|= TIM_CCER_CC1E |		// enables channel 1 as output
					   TIM_CCER_CC2E;

	if(mode == TRANS_FLIGHT)
	{
		GPIOC->BSRR	|= GPIO_BSRR_BR8;
		TIM1->CCR1 = (uint32_t)(5082 * speed) - 1;

		GPIOC->BSRR	|= GPIO_BSRR_BR9;
		TIM1->CCR2 = (uint32_t)(5082 * speed) - 1;
	}
	else if(mode == TRANS_ROVER)
	{
		GPIOC->BSRR	|= GPIO_BSRR_BS8;
		TIM1->CCR1 = (uint32_t)(5082 * speed) - 1;

		GPIOC->BSRR	|= GPIO_BSRR_BS9;
		TIM1->CCR2 = (uint32_t)(5082 * speed) - 1;
	}
	else
	{
		TIM1->CCR1 = 0;
		TIM1->CCR2 = 0;
	}

	delay(100);
	EXTI->IMR		|= EXTI_IMR_IM13 |		// wait for arm to be clear of limit switch
					   EXTI_IMR_IM14;		// and then turn interrupts back on
}

void
EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR13){
		EXTI->PR |= EXTI_PR_PR13;
		TIM1->CCER &= ~TIM_CCER_CC1E;
		arm1Hit = true;
	}
	if(EXTI->PR & EXTI_PR_PR14){
		EXTI->PR |= EXTI_PR_PR14;
		TIM1->CCER &= ~TIM_CCER_CC2E;
		arm2Hit = true;
	}
}
