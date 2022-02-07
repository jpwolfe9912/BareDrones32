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

/*		Global Variables	*/
uint8_t rxBuf[RXBUF_SIZE];			// dma data
lwrb_t rxRingBuf;					// ring buffer instance
uint8_t rxRingBufData[RXBUF_SIZE];	// ring buffer data

void usart1Init(void){
	/* ---PIN INFO---
	 * USART1_RX
	 * 		PA10
	 * 		AF7
	 * 		DMA 2
	 * 		Stream 2
	 * 		Channel 4
	 * USART1_TX
	 * 		PA9
	 * 		AF7
	 * 		DMA 2
	 * 		Stream 7
	 * 		Channel 4
	 * */

	/////////////////GPIO INIT///////////////////
	// enable clock for GPIOA
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	// set mode, speed, type, pull, AF
	GPIOA->MODER 	&= ~GPIO_MODER_MODER9;
	GPIOA->MODER 	|= GPIO_MODER_MODER9_1;				// AF mode
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR9;			// high speed
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT9;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR9;
	GPIOA->AFR[1]	&= ~GPIO_AFRH_AFRH1;
	GPIOA->AFR[1] 	|= (0x7 << (4U * 1U));				// AF 7

	GPIOA->MODER 	&= ~GPIO_MODER_MODER10;
	GPIOA->MODER 	|= GPIO_MODER_MODER10_1;
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR10;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT10;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR10;
	GPIOA->AFR[1]	&= ~GPIO_AFRH_AFRH2;
	GPIOA->AFR[1] 	|= (0x7 << (4U * 2U));

	// DMA IRQ Init
	NVIC_SetPriority(DMA2_Stream2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(USART1_IRQn);

	/////////////////USART INIT///////////////////
	RCC->APB2ENR	|= RCC_APB2ENR_USART1EN;

	USART1->CR1		&= ~USART_CR1_UE;		// disable usart
	USART1->BRR		= 0x3AA;				// 115200 BR
	USART1->CR1		&= ~USART_CR1_M;		// 8 bit transfer
	USART1->CR2		&= ~USART_CR2_STOP;
	USART1->CR1		&= ~USART_CR1_PCE;
	USART1->CR1		|= USART_CR1_RE	|		// enable rx, tx
			USART_CR1_TE;
	USART1->CR3		&= ~(USART_CR3_CTSE |
			USART_CR3_RTSE);
	USART1->CR1		&= ~USART_CR1_OVER8;

	/////////////////DMA INIT///////////////////
	RCC->AHB1RSTR	|= RCC_AHB1RSTR_DMA2RST;
	RCC->AHB1RSTR	&= ~RCC_AHB1RSTR_DMA2RST;

	// disable DMA 1 stream 1
	DMA2_Stream2->CR 	&= ~DMA_SxCR_EN;
	while(DMA2_Stream2->CR & DMA_SxCR_EN){}
	DMA2_Stream2->CR	= 0;
	DMA2_Stream2->NDTR	= 0;
	DMA2_Stream2->PAR	= 0;
	DMA2_Stream2->M0AR	= 0;
	DMA2_Stream2->M1AR	= 0;
	DMA2_Stream2->FCR	= 0x00000021U;
	DMA2_Stream2->CR	&= ~DMA_SxCR_CHSEL;
	DMA2->LIFCR			|= (0x3F << 16U); //0x00000F40U;

	RCC->AHB1ENR		|= RCC_AHB1ENR_DMA2EN;

	// stream 1 ch 4 DMA settings
	DMA2_Stream2->CR	|= (0x4 << 25U);			// channel 4
	DMA2_Stream2->M0AR 	= (uint32_t)rxBuf;			// set mem address
	DMA2_Stream2->CR 	&= ~DMA_SxCR_DIR;			// per to mem
	DMA2_Stream2->FCR	&= ~DMA_SxFCR_DMDIS;		// fifo dis
	DMA2_Stream2->CR 	&= ~DMA_SxCR_MBURST;
	DMA2_Stream2->CR 	&= ~DMA_SxCR_PBURST;
	DMA2_Stream2->PAR 	= (uint32_t)(&(USART1->RDR));// set per address
	DMA2_Stream2->NDTR	= RXBUF_SIZE;		// 64 bytes
	DMA2_Stream2->CR 	&= ~DMA_SxCR_PINC;			// don't inc per
	DMA2_Stream2->CR 	|= DMA_SxCR_MINC;			// increment mem
	DMA2_Stream2->CR 	&= ~DMA_SxCR_MSIZE;			// 8 bit size
	DMA2_Stream2->CR 	&= ~DMA_SxCR_PSIZE;			// 8 bit size
	DMA2_Stream2->CR 	|= DMA_SxCR_CIRC;			// circ mode en
	DMA2_Stream2->CR 	&= ~DMA_SxCR_PL_0;			// medium priority
}

void usart1Read(uint8_t *pData, uint8_t size)
{
	if(!(USART1->ISR & USART_ISR_BUSY)){		// wait for UART to be ready
		DMA2_Stream2->CR	&= ~DMA_SxCR_EN;	// disable DMA
		while(DMA2_Stream2->CR & DMA_SxCR_EN);
		DMA2_Stream2->CR	|= (0x4 << 25U);	// set DMA channel
		DMA2_Stream2->NDTR	= size;				// set transfer size
		DMA2_Stream2->M0AR	= (uint32_t)pData;	// set memory address

		DMA2->LIFCR			|= (0x3F << 16U);	// clear flags

		DMA2_Stream2->CR 	|= DMA_SxCR_TCIE;	// set transfer complete interrupts
		DMA2_Stream2->CR	|= DMA_SxCR_HTIE;

		DMA2_Stream2->CR	|= DMA_SxCR_EN;		// enable DMA

		USART1->CR3			|= USART_CR3_DMAR;	// enable DMA for UART

		USART1->ICR			|= USART_ICR_IDLECF;// clear idle interrupt flag
		USART1->CR1			|= USART_CR1_IDLEIE;// enable idle line interrupts
		USART1->CR1			|= USART_CR1_UE;			// enable usart
	}
}


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



