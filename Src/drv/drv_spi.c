/**
 ******************************************************************************
 * @file    spi.c
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* SPI1 init function */
void MX_SPI1_Init(void)
{

	/* USER CODE BEGIN SPI1_Init 0 */
	/////////////////GPIO INIT///////////////////
	// enable clock for GPIOA
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	// set mode, speed, type, pull, AF
	GPIOA->MODER 	&= ~GPIO_MODER_MODER4;
	GPIOA->MODER 	|= GPIO_MODER_MODER4_0;			// output mode
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR4;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT4;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR4;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL4;

	GPIOA->MODER 	&= ~GPIO_MODER_MODER5;
	GPIOA->MODER 	|= GPIO_MODER_MODER5_1;			// AF Mode
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR5;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT5;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR5;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL5;
	GPIOA->AFR[0] 	|= (GPIO_AF5_SPI1 << (4U * 5U));// AF 5

	GPIOA->MODER 	&= ~GPIO_MODER_MODER6;
	GPIOA->MODER 	|= GPIO_MODER_MODER6_1;			// AF mode
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR6;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT6;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR6;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL6;
	GPIOA->AFR[0] 	|= (GPIO_AF5_SPI1 << (4U * 6U));// AF 5

	GPIOA->MODER 	&= ~GPIO_MODER_MODER7;
	GPIOA->MODER 	|= GPIO_MODER_MODER7_1;			// AF mode
	GPIOA->OSPEEDR	|= GPIO_OSPEEDR_OSPEEDR7;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT7;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR7;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL7;
	GPIOA->AFR[0] 	|= (GPIO_AF5_SPI1 << (4U * 7U));// AF 5

	// enable clock
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	// enable interrupts
	NVIC_SetPriority(DMA2_Stream0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	NVIC_SetPriority(DMA2_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(DMA2_Stream3_IRQn);
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

	// reset DMA
	RCC->AHB1RSTR		|= RCC_AHB1RSTR_DMA2RST;
	RCC->AHB1RSTR		&= ~RCC_AHB1RSTR_DMA2RST;
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
	DMA2_Stream0->CR 	|= DMA_SxCR_PL;
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
	DMA2_Stream3->CR 	|= DMA_SxCR_PL;
	// DMA transfer complete interrupt enable
	DMA2_Stream3->CR 	|= DMA_SxCR_TCIE;
	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
//	hspi1.Instance = SPI1;
//	hspi1.Init.Mode = SPI_MODE_MASTER;
//	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
//	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
//	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
//	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
//	hspi1.Init.NSS = SPI_NSS_SOFT;
//	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
//	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
//	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
//	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//	hspi1.Init.CRCPolynomial = 7;
//	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
//	hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
//	if (HAL_SPI_Init(&hspi1) != HAL_OK)
//	{
//		Error_Handler();
//	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}
/* SPI2 init function */
//void MX_SPI2_Init(void)
//{
//
//	/* USER CODE BEGIN SPI2_Init 0 */
//
//	/* USER CODE END SPI2_Init 0 */
//
//	/* USER CODE BEGIN SPI2_Init 1 */
//
//	/* USER CODE END SPI2_Init 1 */
//	hspi2.Instance = SPI2;
//	hspi2.Init.Mode = SPI_MODE_MASTER;
//	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
//	hspi2.Init.DataSize = SPI_DATASIZE_4BIT;
//	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
//	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
//	hspi2.Init.NSS = SPI_NSS_SOFT;
//	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
//	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
//	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
//	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//	hspi2.Init.CRCPolynomial = 7;
//	hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
//	hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
//	if (HAL_SPI_Init(&hspi2) != HAL_OK)
//	{
//		Error_Handler();
//	}
//	/* USER CODE BEGIN SPI2_Init 2 */
//
//	/* USER CODE END SPI2_Init 2 */
//
//}

//void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
//{

//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	if(spiHandle->Instance==SPI1)
//	{
//		/* USER CODE BEGIN SPI1_MspInit 0 */
//
//		/* USER CODE END SPI1_MspInit 0 */
//		/* SPI1 clock enable */
//		__HAL_RCC_SPI1_CLK_ENABLE();
//
//		__HAL_RCC_GPIOA_CLK_ENABLE();
//		/**SPI1 GPIO Configuration
//    PA5     ------> SPI1_SCK
//    PA6     ------> SPI1_MISO
//    PA7     ------> SPI1_MOSI
//		 */
//		GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
//		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
//		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//		/* SPI1 DMA Init */
//		/* SPI1_RX Init */
//		hdma_spi1_rx.Instance = DMA2_Stream0;
//		hdma_spi1_rx.Init.Channel = DMA_CHANNEL_3;
//		hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//		hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
//		hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
//		hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//		hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//		hdma_spi1_rx.Init.Mode = DMA_NORMAL;
//		hdma_spi1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
//		hdma_spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//		if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
//		{
//			Error_Handler();
//		}
//
//		__HAL_LINKDMA(spiHandle,hdmarx,hdma_spi1_rx);
//
//		/* SPI1_TX Init */
//		hdma_spi1_tx.Instance = DMA2_Stream3;
//		hdma_spi1_tx.Init.Channel = DMA_CHANNEL_3;
//		hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//		hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//		hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
//		hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//		hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//		hdma_spi1_tx.Init.Mode = DMA_NORMAL;
//		hdma_spi1_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
//		hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//		if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
//		{
//			Error_Handler();
//		}
//
//		__HAL_LINKDMA(spiHandle,hdmatx,hdma_spi1_tx);

		/* USER CODE BEGIN SPI1_MspInit 1 */

		/* USER CODE END SPI1_MspInit 1 */
//	}
//	if(spiHandle->Instance==SPI2)
//	{
//		/* USER CODE BEGIN SPI2_MspInit 0 */
//
//		/* USER CODE END SPI2_MspInit 0 */
//		/* SPI2 clock enable */
//		__HAL_RCC_SPI2_CLK_ENABLE();
//
//		__HAL_RCC_GPIOB_CLK_ENABLE();
//		/**SPI2 GPIO Configuration
//    PB13     ------> SPI2_SCK
//    PB14     ------> SPI2_MISO
//    PB15     ------> SPI2_MOSI
//		 */
//		GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
//		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
//		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//		/* SPI2 DMA Init */
//		/* SPI2_RX Init */
//		hdma_spi2_rx.Instance = DMA1_Stream3;
//		hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
//		hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//		hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
//		hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
//		hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//		hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//		hdma_spi2_rx.Init.Mode = DMA_NORMAL;
//		hdma_spi2_rx.Init.Priority = DMA_PRIORITY_LOW;
//		hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//		if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
//		{
//			Error_Handler();
//		}
//
//		__HAL_LINKDMA(spiHandle,hdmarx,hdma_spi2_rx);
//
//		/* SPI2_TX Init */
//		hdma_spi2_tx.Instance = DMA1_Stream4;
//		hdma_spi2_tx.Init.Channel = DMA_CHANNEL_0;
//		hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//		hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//		hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
//		hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//		hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//		hdma_spi2_tx.Init.Mode = DMA_NORMAL;
//		hdma_spi2_tx.Init.Priority = DMA_PRIORITY_LOW;
//		hdma_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//		if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
//		{
//			Error_Handler();
//		}
//
//		__HAL_LINKDMA(spiHandle,hdmatx,hdma_spi2_tx);
//
//		/* USER CODE BEGIN SPI2_MspInit 1 */
//
//		/* USER CODE END SPI2_MspInit 1 */
//	}
//}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

	if(spiHandle->Instance==SPI1)
	{
		/* USER CODE BEGIN SPI1_MspDeInit 0 */

		/* USER CODE END SPI1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_SPI1_CLK_DISABLE();

		/**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
		 */
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

		/* USER CODE BEGIN SPI1_MspDeInit 1 */

		/* USER CODE END SPI1_MspDeInit 1 */
	}
	else if(spiHandle->Instance==SPI2)
	{
		/* USER CODE BEGIN SPI2_MspDeInit 0 */

		/* USER CODE END SPI2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_SPI2_CLK_DISABLE();

		/**SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI
		 */
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

		/* SPI2 DMA DeInit */
		HAL_DMA_DeInit(spiHandle->hdmarx);
		HAL_DMA_DeInit(spiHandle->hdmatx);
		/* USER CODE BEGIN SPI2_MspDeInit 1 */

		/* USER CODE END SPI2_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */
void spiReadBytes(uint8_t reg, uint8_t *pData, uint8_t size){
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

void spiWriteBytes(uint8_t *pData, uint8_t size){
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
void spiReadOneByte(uint8_t reg, uint8_t *pData){
	reg |= 0x80;		// read operation

	DMA2_Stream0->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream0->CR & DMA_SxCR_EN);
	DMA2_Stream0->CR	|= (0x3 << 25U);
	DMA2_Stream0->NDTR	= 2;
	DMA2_Stream0->M0AR	= (uint32_t)pData;

	DMA2_Stream3->CR	&= ~DMA_SxCR_EN;
	while(DMA2_Stream3->CR & DMA_SxCR_EN);
	DMA2_Stream3->CR	|= (0x3 << 25U);
	DMA2_Stream3->NDTR	= 2;
	DMA2_Stream3->M0AR	= (uint32_t)&reg;

	SPI1_ENABLE;

	DMA2_Stream0->CR	|= DMA_SxCR_EN;
	DMA2_Stream3->CR	|= DMA_SxCR_EN;

	SPI1->CR2			|= SPI_CR2_RXDMAEN;
	SPI1->CR2			|= SPI_CR2_TXDMAEN;

	SPI1->CR1			|= SPI_CR1_SPE;
}
void spiWriteOneByte(uint8_t reg, uint8_t data){
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
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
