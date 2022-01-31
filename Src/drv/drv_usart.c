/*
 * usart.c
 *
 *  Created on: Oct 19, 2021
 *      Author: jeremywolfe
 *
 *  USART1 on AF8
 */


#include "board.h"

char read;
uint8_t readFlag = 0;

void uart5Init(void){

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN |
			RCC_AHB1ENR_GPIODEN; 						// enable the clock for port C, D
	RCC->APB1ENR |= RCC_APB1ENR_UART5EN; 						// enable the clock for UART5

	GPIOC->AFR[1] |= (GPIO_AF8_UART5 << (4 * 4U));							// set pin A2 as alternate function
	GPIOD->AFR[0] |= (GPIO_AF8_UART5 << (4 * 2U));							// set pin A3 as alternate function

	GPIOC->MODER &= ~(GPIO_MODER_MODER12);
	GPIOD->MODER &= ~(GPIO_MODER_MODER2);
	GPIOC->MODER |= GPIO_MODER_MODER12_1;						// set PC12 as alternate function
	GPIOD->MODER |= GPIO_MODER_MODER2_1;						// set PD2 as alternate function
	UART5->BRR = 0x1D5; 										// set baud rate to 115200
	UART5->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; 	// enable the receiver, transmitter, and USART								// enable UART5 interrupts on the NVIC (nested vector interrupt controller)


}

void usart3Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; 						// enable the clock for port D
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN; 						// enable the clock for UART3

	GPIOD->AFR[1] |= (GPIO_AF7_USART3 << (4 * 0U));				// set pin A2 as alternate function
	GPIOD->AFR[1] |= (GPIO_AF7_USART3 << (4 * 1U));				// set pin A3 as alternate function

	GPIOD->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
	GPIOD->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;	// set PD8,9 as alternate function
	USART3->BRR = 0x1D5; 										// set baud rate to 115200
	USART3->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; 	// enable the receiver, transmitter, and USART

}

int usart3Write(int ch){
	while (!(USART3->ISR & USART_ISR_TXE)){}	// waits for TX buffer to become empty
	USART3->TDR = ch;								// transfers the value of the data register into ch
	return 0;
}

int uart5Write(int ch){
	while (!(UART5->ISR & USART_ISR_TXE)){}	// waits for TX buffer to become empty
	UART5->TDR = ch;								// transfers the value of the data register into ch
	return 0;
}

/*	This is required to use printf											*/
/*	This basically tells the compiler what to do when it encounters printf	*/
/*	I honestly can't fully explain what is going on but it works			*/
#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//	#define GETCHAR_PROTOTYPE int __io_getchar (void)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//	#define GETCHAR_PROTOTYPE int fgetc(FILE * f)
#endif

PUTCHAR_PROTOTYPE{
	usart3Write(ch);
//	uart5Write(ch);
	return ch;
}



