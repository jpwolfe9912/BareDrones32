/** @file 		drv_usart.c
 *  @brief
 *  	This file enables reading of usart data for use with a serial receiver.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
uint8_t rx1Buf[RX1BUF_SIZE];

char read;
uint8_t readFlag = 0;

/** @brief Initializes the low level registers for usart1.
 *
 *  @return Void.
 */
void
usart1Init(void)
{
	printf("\nInitializing USART 1\n");
	/////////////////GPIO INIT///////////////////
	// enable clock for GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	// set mode, speed, type, pull, AF
	GPIOB->MODER 	&= ~GPIO_MODER_MODER7;
	GPIOB->MODER 	|= GPIO_MODER_MODER7_1;
	GPIOB->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR7;
	GPIOB->OTYPER	&= ~GPIO_OTYPER_OT7;
	GPIOB->PUPDR	&= ~GPIO_PUPDR_PUPDR7;
	GPIOB->AFR[0]	&= ~GPIO_AFRL_AFRL7;
	GPIOB->AFR[0] 	|= (0x7 << (4U * 7U));

	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(USART1_IRQn);

	/////////////////USART INIT///////////////////
	RCC->APB2ENR	|= RCC_APB2ENR_USART1EN;

	USART1->CR1		&= ~USART_CR1_UE;		// disable usart
	USART1->BRR		= 0x3AA;				// 115200 BR
	USART1->CR1		&= ~USART_CR1_M;		// 8 bit transfer
	USART1->CR2		&= ~USART_CR2_STOP;
	USART1->CR1		&= ~USART_CR1_PCE;
	USART1->CR1		|= USART_CR1_RE;
	USART1->CR3		&= ~(USART_CR3_CTSE |
			USART_CR3_RTSE);
	USART1->CR1		&= ~USART_CR1_OVER8;

	/////////////////DMA INIT///////////////////

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

	// stream 1 ch 4 DMA settings
	DMA2_Stream2->CR 	&= ~DMA_SxCR_DIR;			// per to mem
	DMA2_Stream2->FCR	&= ~DMA_SxFCR_DMDIS;		// fifo dis
	DMA2_Stream2->CR 	&= ~DMA_SxCR_MBURST;
	DMA2_Stream2->CR 	&= ~DMA_SxCR_PBURST;
	DMA2_Stream2->PAR 	= (uint32_t)(&(USART1->RDR));// set per address
	DMA2_Stream2->CR 	&= ~DMA_SxCR_PINC;			// don't inc per
	DMA2_Stream2->CR 	|= DMA_SxCR_MINC;			// increment mem
	DMA2_Stream2->CR 	&= ~DMA_SxCR_MSIZE;			// 8 bit size
	DMA2_Stream2->CR 	&= ~DMA_SxCR_PSIZE;			// 8 bit size
	DMA2_Stream2->CR 	|= DMA_SxCR_CIRC;			// circ mode en
	DMA2_Stream2->CR 	|= DMA_SxCR_PL;			// medium priority
}

/** @brief Reads in data form usart1 with DMA.
 *
 *  @param *pData A pointer to location where you want to read data to.
 *  @param size The amount of bytes to be read.
 *  @return Void.
 */
void
usart1Read(uint8_t *pData, uint8_t size)
{
	if(!(USART1->ISR & USART_ISR_BUSY)){		// wait for UART to be ready
		DMA2_Stream2->CR	&= ~DMA_SxCR_EN;	// disable DMA
		while(DMA2_Stream2->CR & DMA_SxCR_EN);
		DMA2_Stream2->CR	|= (0x4 << 25U);	// set DMA channel
		DMA2_Stream2->NDTR	= size;				// set transfer size
		DMA2_Stream2->M0AR	= (uint32_t)pData;	// set memory address

		DMA2->LIFCR			|= (0x3F << 16U);	// clear flags

		DMA2_Stream2->CR 	|= DMA_SxCR_TCIE;	// set transfer complete interrupts

		DMA2_Stream2->CR	|= DMA_SxCR_EN;		// enable DMA

		USART1->CR3			|= USART_CR3_DMAR;	// enable DMA for UART

		USART1->CR1			|= USART_CR1_UE;	// enable usart
	}
}

/** @brief Initializes the low level registers for usart1.
 *
 *  @return Void.
 */
void
usart6Init(void)
{
	printf("\nInitializing USART 6\n");
	/////////////////GPIO INIT///////////////////
	// enable clock for GPIOC
	RCC->DCKCFGR2	&= ~RCC_DCKCFGR2_USART6SEL;
	RCC->DCKCFGR2	|= RCC_DCKCFGR2_USART6SEL_0;		// choose SysClk (216MHz) as clock source

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	// set mode, speed, type, pull, AF
	GPIOC->MODER 	&= ~GPIO_MODER_MODER6;
	GPIOC->MODER 	|= GPIO_MODER_MODER6_1;
	GPIOC->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR6;
	GPIOC->OTYPER	&= ~GPIO_OTYPER_OT6;
	GPIOC->PUPDR	&= ~GPIO_PUPDR_PUPDR6;
	GPIOC->AFR[0]	&= ~GPIO_AFRL_AFRL6;
	GPIOC->AFR[0] 	|= (0x8 << (4U * 6U));

	NVIC_SetPriority(USART6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(USART6_IRQn);

	/////////////////USART INIT///////////////////
	RCC->APB2ENR	|= RCC_APB2ENR_USART6EN;

	USART6->CR1		&= ~USART_CR1_UE;		// disable usart
	USART6->BRR		= 0x6C;					// 2000000 BR
	USART6->CR1		&= ~USART_CR1_M;		// 8 bit transfer
	USART6->CR2		&= ~USART_CR2_STOP;
	USART6->CR1		&= ~USART_CR1_PCE;
	USART6->CR1		|= USART_CR1_TE;
	USART6->CR3		&= ~(USART_CR3_CTSE |
			USART_CR3_RTSE);
	USART6->CR1		&= ~USART_CR1_OVER8;

	/////////////////DMA INIT///////////////////

	// disable DMA 1 stream 1
	DMA2_Stream6->CR 	&= ~DMA_SxCR_EN;
	while(DMA2_Stream6->CR & DMA_SxCR_EN){}
	DMA2_Stream6->CR	= 0;
	DMA2_Stream6->NDTR	= 0;
	DMA2_Stream6->PAR	= 0;
	DMA2_Stream6->M0AR	= 0;
	DMA2_Stream6->M1AR	= 0;
	DMA2_Stream6->FCR	= 0x00000021U;
	DMA2_Stream6->CR	&= ~DMA_SxCR_CHSEL;
	DMA2->HIFCR			|= (0x3F << 16U); //0x00000F40U;

	// stream 1 ch 4 DMA settings
	DMA2_Stream6->CR 	|= DMA_SxCR_DIR_0;			// mem to per
	DMA2_Stream6->FCR	&= ~DMA_SxFCR_DMDIS;		// fifo dis
	DMA2_Stream6->CR 	&= ~DMA_SxCR_MBURST;
	DMA2_Stream6->CR 	&= ~DMA_SxCR_PBURST;
	DMA2_Stream6->PAR 	= (uint32_t)(&(USART6->TDR));// set per address
	DMA2_Stream6->CR 	&= ~DMA_SxCR_PINC;			// don't inc per
	DMA2_Stream6->CR 	|= DMA_SxCR_MINC;			// increment mem
	DMA2_Stream6->CR 	&= ~DMA_SxCR_MSIZE;			// 8 bit size
	DMA2_Stream6->CR 	&= ~DMA_SxCR_PSIZE;			// 8 bit size
	DMA2_Stream6->CR 	&= ~DMA_SxCR_CIRC;			// circ mode dis
	DMA2_Stream6->CR 	|= DMA_SxCR_PL;				// medium priority

}

/** @brief Reads in data form usart1 with DMA.
 *
 *  @param *pData A pointer to location where you want to read data to.
 *  @param size The amount of bytes to be read.
 *  @return Void.
 */
void
usart6Write(char *pData, uint8_t size)
{
	if(!(USART6->ISR & USART_ISR_BUSY)){		// wait for UART to be ready
		DMA2_Stream6->CR	&= ~DMA_SxCR_EN;	// disable DMA
		while(DMA2_Stream6->CR & DMA_SxCR_EN);
		DMA2_Stream6->M0AR	= (uint32_t)pData;
		DMA2_Stream6->CR	|= (0x5 << 25U);	// set DMA channel
		DMA2_Stream6->NDTR	= size;				// set transfer size

		DMA2->HIFCR			|= (0x3F << 16U);	// clear flags

		DMA2_Stream6->CR 	|= DMA_SxCR_TCIE;	// set transfer complete interrupts

		DMA2_Stream6->CR	|= DMA_SxCR_EN;		// enable DMA

		USART6->CR3			|= USART_CR3_DMAT;	// enable DMA for UART

//		USART6->CR1			|= USART_CR1_TCIE;
		USART6->ICR			|= USART_ICR_TCCF;
		USART6->CR1			|= USART_CR1_UE;	// enable usart
	}
}

/* Interrupt Handlers */

/** @brief	USART6 global interrupt handler
 *
 * 	@return Void.
 */
void
USART6_IRQHandler(void) {
	/* Check for IDLE line interrupt */
	if ((USART6->ISR & USART_ISR_TC) && (USART6->CR1 & USART_CR1_TCIE)) {
		USART6->ICR		|= USART_ICR_TCCF;	/* Clear IDLE line flag */

		USART6->CR1			&= ~USART_CR1_TCIE;
	}
}

/** @brief	DMA2_Stream2 global interrupt handler
 *
 * 	@return Void.
 */
void
DMA2_Stream2_IRQHandler(void) {
	/* Check half-transfer complete interrupt */
	if(DMA2->LISR & DMA_LISR_TCIF2){
		DMA2->LIFCR		|= DMA_LIFCR_CTCIF2;	/* Clear half-transfer complete flag */
	}
}

/** @brief	DMA2_Stream6 global interrupt handler
 *
 * 	@return Void.
 */
void
DMA2_Stream6_IRQHandler(void) {
	/* Check half-transfer complete interrupt */
	if(DMA2->HISR & DMA_HISR_TCIF6){
		DMA2->HIFCR		|= DMA_HIFCR_CTCIF6;	/* Clear half-transfer complete flag */

		DMA2_Stream6->CR	&= ~DMA_SxCR_EN;	// disable DMA

		USART6->CR3			&= ~USART_CR3_DMAT;	// disable USART DMA

		USART6->CR1			&= ~USART_CR1_TCIE;
		USART6->CR1			&= ~USART_CR1_UE;	// disable USART
	}
}

