/** @file 		drv_rcc.c
 *  @brief
 *  	This file initializes the RCC clock.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

/* Includes */
#include "board.h"

/** @brief Changes clock speed to 216 MHz
 *
 *  @return Void.
 */
void
rcc216MHzInit(void)
{
	RCC->APB1ENR	|= RCC_APB1ENR_PWREN;
	PWR->CR1		|= PWR_CR1_VOS;			// turn on voltage scaling
	PWR->CR1 		|= PWR_CR1_ODEN;		// enable overdrive

	RCC->APB2ENR 	|= RCC_APB2ENR_SYSCFGEN;
	FLASH->ACR		|= FLASH_ACR_LATENCY_7WS;		// set flash latency to 7 wait states
	while(!(FLASH->ACR & FLASH_ACR_LATENCY_7WS)){}	// wait for enabled


	RCC->CR 		|= RCC_CR_HSEON;		// turn on HSI
	while(!(RCC->CR & RCC_CR_HSERDY)){}		// wait until HSI ready

	/*		PLL Division Configuration	*/
	/*		16 / 8 = 2 * 216 = 432 / 2 = 216	*/
	RCC->CR			&= ~RCC_CR_PLLON;
	RCC->PLLCFGR	|= RCC_PLLCFGR_PLLSRC_HSE;
	RCC->PLLCFGR	&= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR	|= 4U;					// PLLM /8 div
	RCC->PLLCFGR	|= (216U << 6U);		// PLLN *216 multi
	RCC->PLLCFGR	&= ~RCC_PLLCFGR_PLLP;	// PLLP /2 div
	RCC->CR			|= RCC_CR_PLLON;			// turn on PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)){}		// wait for PLL to be ready

	/*		CPU, AHB, APB Bus Configuration	*/
	RCC->CFGR		|= RCC_CFGR_HPRE_DIV1;	// AHB = 216MHz
	RCC->CFGR		|= RCC_CFGR_PPRE1_DIV4;	// APB1 = 216 / 4 = 54MHz
	RCC->CFGR		|= RCC_CFGR_PPRE2_DIV2;	// APB2 = 216 / 2 = 108MHz
	RCC->CFGR		|= RCC_CFGR_SW_PLL;		// PLL selected as system clock
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)){}// wait until selected

	SystemCoreClock = 216000000;
}
