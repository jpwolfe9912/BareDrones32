/** @file 		drv_lager.c
 *  @brief
 *  	This file initializes the UART and provides functions for writing
 *  	to the OpenLager.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#include "board.h"

/* Static Functions */
static void lagerWriteByte(uint8_t ch);

/** @brief Initializes the low level uart registers in order to use the lager.
 *
 *  @return Void.
 */
void
lagerInit(void)
{
	/*
	 * PC6 - TX6
	 * PC7 - RX6
	 */
	RCC->DCKCFGR2	&= ~RCC_DCKCFGR2_USART6SEL;
	RCC->DCKCFGR2	|= RCC_DCKCFGR2_USART6SEL_0;		// choose SysClk (216MHz) as clock source

	RCC->AHB1ENR 	|= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR 	|= RCC_APB2ENR_USART6EN; 						// enable the clock for UART5

	GPIOC->AFR[0] |= (0x8 << (4 * 6U));							// set pin A2 as alternate function
	GPIOC->AFR[0] |= (0x8 << (4 * 7U));							// set pin A2 as alternate function

	GPIOC->MODER &= ~(GPIO_MODER_MODER6);
	GPIOC->MODER &= ~(GPIO_MODER_MODER7);
	GPIOC->MODER |= GPIO_MODER_MODER6_1;						// set PC12 as alternate function
	GPIOC->MODER |= GPIO_MODER_MODER7_1;						// set PD2 as alternate function

	USART6->BRR = 0x6C; 										// set baud rate to 2000000
	USART6->CR1 |= USART_CR1_TE | USART_CR1_UE; 	// enable the receiver, transmitter, and USART
}

/** @brief Loops through the log.
 *
 *  @param ch The character to send.
 *  @return Void.
 */
void
lagerWriteLog(char *pLog)
{
	uint8_t i = 0;
	do
	{
		lagerWriteByte(pLog[i]);
	}
	while(pLog[i++] != '\r');
}

/** @brief Uses polling to write data to the transmit buffer.
 *
 *  @param ch The character to send.
 *  @return Void.
 */
static void
lagerWriteByte(uint8_t ch)
{
	while (!(USART6->ISR & USART_ISR_TXE)){}	// waits for TX buffer to become empty
	USART6->TDR = ch;							// transfers the value of the data register into ch
}
