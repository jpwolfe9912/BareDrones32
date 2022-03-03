/** @file 		drv_spi.c
 *  @brief
 *  	This file initializes the SPI peripheral as well as writes
 *  	and reads to another device over SPI
 *
 *  Still needs to implement SPI2 for OSD chip
 *  Find a way to reuse write and read functions for SPI1 and SPI2
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 *  @bug
 */

/* Includes */
#include "board.h"

/* Functions */

/** @brief Initializes SPI1.
 *
 *  @return Void.
 */
void
spi1Init(void)
{
	printf("\nInitializing SPI 1\n");

	/* USER CODE BEGIN SPI1_Init 0 */
	/////////////////GPIO INIT///////////////////
	// enable clock for GPIOA
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	// set mode, speed, type, pull, AF
	GPIOA->MODER 	&= ~GPIO_MODER_MODER4;
	GPIOA->MODER 	|= GPIO_MODER_MODER4_0;			// output mode
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR4;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT4;
	GPIOA->PUPDR	|= GPIO_PUPDR_PUPDR4_0;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL4;

	GPIOA->MODER 	&= ~GPIO_MODER_MODER5;
	GPIOA->MODER 	|= GPIO_MODER_MODER5_1;			// AF Mode
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR5;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT5;
	GPIOA->PUPDR	|= GPIO_PUPDR_PUPDR5_0;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL5;
	GPIOA->AFR[0] 	|= (GPIO_AF5_SPI1 << (4U * 5U));// AF 5

	GPIOA->MODER 	&= ~GPIO_MODER_MODER6;
	GPIOA->MODER 	|= GPIO_MODER_MODER6_1;			// AF mode
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR6;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT6;
	GPIOA->PUPDR	|= GPIO_PUPDR_PUPDR6_0;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL6;
	GPIOA->AFR[0] 	|= (GPIO_AF5_SPI1 << (4U * 6U));// AF 5

	GPIOA->MODER 	&= ~GPIO_MODER_MODER7;
	GPIOA->MODER 	|= GPIO_MODER_MODER7_1;			// AF mode
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR7;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT7;
	GPIOA->PUPDR	|= GPIO_PUPDR_PUPDR7_0;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL7;
	GPIOA->AFR[0] 	|= (GPIO_AF5_SPI1 << (4U * 7U));// AF 5

	/////////////////SPI INIT///////////////////
	// enable clock for SPI1
	RCC->APB2ENR 	|= RCC_APB2ENR_SPI1EN;

	SPI1->CR1		&= ~SPI_CR1_BIDIMODE;	// rx and tx
	SPI1->CR1		|= SPI_CR1_MSTR;		// SPI master
	SPI1->CR2		|= (0x7 << 8U);			// byte transfer size
	SPI1->CR1		|= SPI_CR1_CPHA;		// high polarity
	SPI1->CR1		|= SPI_CR1_CPOL;		// 2nd edge
	SPI1->CR1		|= SPI_CR1_SSM;			// software slave management
	SPI1->CR1		|= SPI_CR1_SSI;
	SPI1->CR1		|= (0x6 << 3U);			// 128 divider
	SPI1->CR1		&= ~SPI_CR1_LSBFIRST;	// MSB first
	SPI1->CR1		&= ~SPI_CR1_CRCEN;		// disable CRC
	SPI1->CR2		&= ~(SPI_CR2_RXDMAEN |
						 SPI_CR2_TXDMAEN);
	SPI1->CR1		|= SPI_CR1_SPE;			// enable SPI

	/////////////////DMA INIT///////////////////
	/*
	 * SPI1_RX on DMA2_Stream0_CH3
	 * SPI1_TX on DMA2_Stream3_CH3
	 */

	// disable DMA stream 1
	DMA2_Stream0->CR 	&= ~DMA_SxCR_EN;
	while(DMA2_Stream0->CR & DMA_SxCR_EN){}
	DMA2_Stream0->CR	= 0;
	DMA2_Stream0->NDTR	= 0;
	DMA2_Stream0->PAR	= 0;
	DMA2_Stream0->M0AR	= 0;
	DMA2_Stream0->M1AR	= 0;
	DMA2_Stream0->FCR	= 0x00000021U;
	DMA2_Stream0->CR	&= ~DMA_SxCR_CHSEL;
	DMA2->LIFCR			|= 0x0000003FU;
	// disable DMA stream 7
	DMA2_Stream3->CR 	&= ~DMA_SxCR_EN;
	while(DMA2_Stream3->CR & DMA_SxCR_EN){}
	DMA2_Stream3->CR	= 0;
	DMA2_Stream3->NDTR	= 0;
	DMA2_Stream3->PAR	= 0;
	DMA2_Stream3->M0AR	= 0;
	DMA2_Stream3->M1AR	= 0;
	DMA2_Stream3->FCR	= 0x00000021U;
	DMA2_Stream3->CR	&= ~DMA_SxCR_CHSEL;
	DMA2->LIFCR			|= 0x0F400000U;

	// RX DMA settings
	DMA2_Stream0->CR	|= (0x3 << 25U);
	DMA2_Stream0->M0AR 	= 0;
	DMA2_Stream0->CR 	&= ~DMA_SxCR_DIR;			// per to mem
	DMA2_Stream0->FCR	&= ~DMA_SxFCR_DMDIS;		// fifo dis
	DMA2_Stream0->FCR	&= ~DMA_SxFCR_FTH;			//1/4 full
	DMA2_Stream0->CR 	&= ~DMA_SxCR_MBURST;
	DMA2_Stream0->CR 	&= ~DMA_SxCR_PBURST;
	DMA2_Stream0->PAR 	= (uint32_t)(&(SPI1->DR));
	DMA2_Stream0->NDTR	= 0;
	DMA2_Stream0->CR 	&= ~DMA_SxCR_PINC;
	DMA2_Stream0->CR 	|= DMA_SxCR_MINC;
	DMA2_Stream0->CR 	&= ~DMA_SxCR_MSIZE;
	DMA2_Stream0->CR 	&= ~DMA_SxCR_PSIZE;
	DMA2_Stream0->CR 	&= ~DMA_SxCR_CIRC;
	DMA2_Stream0->CR 	|= DMA_SxCR_PL_0;
	// DMA transfer complete interrupt enable
	DMA2_Stream0->CR 	|= DMA_SxCR_TCIE;
	// TX DMA settings
	DMA2_Stream3->CR	|= (0x3 << 25U);
	DMA2_Stream3->M0AR 	= 0;
	DMA2_Stream3->CR 	|= DMA_SxCR_DIR_0;			// mem to per
	DMA2_Stream3->FCR	&= ~DMA_SxFCR_DMDIS;		// fifo dis
	DMA2_Stream3->FCR	&= ~DMA_SxFCR_FTH;			//1/4 full
	DMA2_Stream3->CR 	&= ~DMA_SxCR_MBURST;
	DMA2_Stream3->CR 	&= ~DMA_SxCR_PBURST;
	DMA2_Stream3->PAR 	= (uint32_t)(&(SPI1->DR));
	DMA2_Stream3->NDTR	= 0;
	DMA2_Stream3->CR 	&= ~DMA_SxCR_PINC;
	DMA2_Stream3->CR 	|= DMA_SxCR_MINC;
	DMA2_Stream3->CR 	&= ~DMA_SxCR_MSIZE;
	DMA2_Stream3->CR 	&= ~DMA_SxCR_PSIZE;
	DMA2_Stream3->CR 	&= ~DMA_SxCR_CIRC;
	DMA2_Stream3->CR 	|= DMA_SxCR_PL_0;
	// DMA transfer complete interrupt enable
	DMA2_Stream3->CR 	|= DMA_SxCR_TCIE;
	/* USER CODE END SPI1_Init 0 */
	delay(500);
}

/** @brief Reads multiple bytes of data in.
 *
 *  @param reg The hex value of the first register to read from.
 *  @param *pData A pointer to location where you want to read data to.
 *  @param size The amount of bytes to be read.
 *  @return Void.
 */
void
spiReadBytes(uint8_t reg, uint8_t *pData, uint8_t size){
	reg = reg | 0x80;			// read operation

	DMA2_Stream0->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream0->CR & DMA_SxCR_EN);
	DMA2_Stream0->CR	|= (0x3 << 25U);
	DMA2_Stream0->NDTR	= size;
	DMA2_Stream0->M0AR	= (uint32_t)pData;

	DMA2_Stream3->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream3->CR & DMA_SxCR_EN);
	DMA2_Stream3->CR	|= (0x3 << 25U);
	DMA2_Stream3->NDTR	= size;
	DMA2_Stream3->M0AR	= (uint32_t)&reg;

	SPI1_ENABLE;

	DMA2_Stream0->CR	|= DMA_SxCR_EN;
	DMA2_Stream3->CR	|= DMA_SxCR_EN;

	SPI1->CR2			|= SPI_CR2_RXDMAEN;
	SPI1->CR2			|= SPI_CR2_TXDMAEN;

	SPI1->CR1			|= SPI_CR1_SPE;
}

/** @brief Writes multiple bytes of data in.
 *
 *  @param reg The hex value of the register to write to.
 *  @param *pData A pointer to the data you want to write.
 *  @param size The amount of bytes to write.
 *  @return Void.
 */
void
spiWriteBytes(uint8_t *pData, uint8_t size){
	// RX Setup
	volatile uint8_t dummy __attribute__((unused));

	DMA2_Stream0->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream0->CR & DMA_SxCR_EN);
	DMA2_Stream0->CR	|= (0x3 << 25U);
	DMA2_Stream0->NDTR	= size;
	DMA2_Stream0->M0AR	= (uint32_t)&dummy;
	// TX Setup
	// perhaps include a circular buffer
	DMA2_Stream3->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream3->CR & DMA_SxCR_EN);
	DMA2_Stream3->CR	|= (0x3 << 25U);
	DMA2_Stream3->NDTR	= size;
	DMA2_Stream3->M0AR	= (uint32_t)pData;

	DMA2_Stream3->CR	|= DMA_SxCR_TCIE;		// RX interrupts

	SPI1_ENABLE;

	DMA2_Stream0->CR	|= DMA_SxCR_EN;
	DMA2_Stream3->CR	|= DMA_SxCR_EN;

	SPI1->CR2			|= SPI_CR2_RXDMAEN;
	SPI1->CR2			|= SPI_CR2_TXDMAEN;

	SPI1->CR1			|= SPI_CR1_SPE;
}

/** @brief Reads one byte of data in.
 *
 *  @param reg The hex value of the register to read from.
 *  @param *pData A pointer to location where you want to read data to.
 *  @return Void.
 */
void
spiReadOneByte(uint8_t reg, uint8_t *pData){
	reg |= 0x80;		// read operation

	DMA2_Stream0->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream0->CR & DMA_SxCR_EN);
	DMA2_Stream0->CR	|= (0x3 << 25U);
	DMA2_Stream0->NDTR	= 3;
	DMA2_Stream0->M0AR	= (uint32_t)pData;

	DMA2_Stream3->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream3->CR & DMA_SxCR_EN);
	DMA2_Stream3->CR	|= (0x3 << 25U);
	DMA2_Stream3->NDTR	= 3;
	DMA2_Stream3->M0AR	= (uint32_t)&reg;

	SPI1_ENABLE;

	DMA2_Stream0->CR	|= DMA_SxCR_EN;
	DMA2_Stream3->CR	|= DMA_SxCR_EN;

	SPI1->CR2			|= SPI_CR2_RXDMAEN;
	SPI1->CR2			|= SPI_CR2_TXDMAEN;

	SPI1->CR1			|= SPI_CR1_SPE;
}

/** @brief Writes one byte of data.
 *
 *  @param reg The hex value of the register to write to.
 *  @param data The data to write.
 *  @return Void.
 */
void
spiWriteOneByte(uint8_t reg, uint8_t data){
	// need to pass in array of values to be written
	// RX Setup
	volatile uint8_t dummy __attribute__((unused));
	uint8_t temp[2] = {reg, data};

	DMA2_Stream0->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream0->CR & DMA_SxCR_EN);
	DMA2_Stream0->CR	|= (0x3 << 25U);
	DMA2_Stream0->NDTR	= 2;
	DMA2_Stream0->M0AR	= (uint32_t)&dummy;
	// TX Setup
	// perhaps include a circular buffer
	DMA2_Stream3->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream3->CR & DMA_SxCR_EN);
	DMA2_Stream3->CR	|= (0x3 << 25U);
	DMA2_Stream3->NDTR	= 2;
	DMA2_Stream3->M0AR	= (uint32_t)temp;

	SPI1_ENABLE;

	DMA2_Stream0->CR	|= DMA_SxCR_EN;
	DMA2_Stream3->CR	|= DMA_SxCR_EN;

	SPI1->CR2			|= SPI_CR2_RXDMAEN;
	SPI1->CR2			|= SPI_CR2_TXDMAEN;

	SPI1->CR1			|= SPI_CR1_SPE;
}

/* Interrupt Handlers */

/**
 * @brief This function handles DMA2 stream1 global interrupt.
 */
void
DMA2_Stream0_IRQHandler(void)
{
	if(DMA2->LISR & DMA_LISR_TCIF0){
		DMA2->LIFCR		|= DMA_LIFCR_CTCIF0;

		SPI1_DISABLE;

		DMA2_Stream0->CR	&= ~DMA_SxCR_EN;
		DMA2_Stream3->CR	&= ~DMA_SxCR_EN;

		SPI1->CR2			&= ~SPI_CR2_RXDMAEN;
		SPI1->CR2			&= ~SPI_CR2_TXDMAEN;

		SPI1->CR1			&= ~SPI_CR1_SPE;

	}
}

/**
 * @brief This function handles DMA2 stream3 global interrupt.
 */
void
DMA2_Stream3_IRQHandler(void)
{
	if(DMA2->LISR & DMA_LISR_TCIF3){
		DMA2->LIFCR		|= DMA_LIFCR_CTCIF3;
	}
}
