/** @file 		drv_adc.c
 *  @brief
 *  	This file initializes and enables the adc for reading the battery voltage.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 MAR 2022
 */

/* Includes */
#include "drv_adc.h"

#include "stm32f7xx.h"

/* Global Variables */
// uint16_t rawADC;//[2];
uint16_t rawADC[2]; // = (uint16_t*)0x20010000;

/** @brief Initializes ADC1 for Channel 8.
 *  @note PC0 on ADC1 Ch10 : PC1 on ADC1 Ch11
 *
 *  @return Void.
 */
void adc1Init(void)
{
	/* RCC Init */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	/* GPIO Init */
	GPIOC->MODER |= (GPIO_MODER_MODER0 |
					 GPIO_MODER_MODER1);
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0 &
					   GPIO_OTYPER_OT1);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR0 &
					  GPIO_PUPDR_PUPDR1);

	// DMA2 Ch0 Stream4

	/* DMA Init */
	DMA2_Stream4->CR &= ~(DMA_SxCR_EN);
	while (DMA2_Stream4->CR & DMA_SxCR_EN)
		;

	DMA2_Stream4->CR &= ~(0x0 << 25U);
	DMA2_Stream4->CR &= ~DMA_SxCR_DIR;
	DMA2_Stream4->CR &= ~DMA_SxCR_PL;
	DMA2_Stream4->CR |= DMA_SxCR_CIRC;
	DMA2_Stream4->CR &= ~DMA_SxCR_PINC;
	DMA2_Stream4->CR |= DMA_SxCR_MINC;
	DMA2_Stream4->CR |= DMA_SxCR_PSIZE_0;
	DMA2_Stream4->CR |= DMA_SxCR_MSIZE_0;
	DMA2_Stream4->FCR &= ~DMA_SxFCR_DMDIS;

	/* ADC1 interrupt Init */
	NVIC_SetPriority(ADC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(ADC_IRQn);

	ADC1->CR2 &= ~ADC_CR2_ADON; // turn off the ADC

	ADC1->CR1 &= ~ADC_CR1_RES;	 // 12 bits of resolution
	ADC1->CR1 &= ~ADC_CR1_SCAN;	 // scan mode enable
	ADC1->CR2 &= ~ADC_CR2_ALIGN; // data alignment right

	ADC1->CR1 &= ~ADC_CR1_DISCEN; // disable discontinuous mode

	ADC1->CR2 &= ~ADC_CR2_EXTEN; // software trigger
	ADC1->CR2 &= ~ADC_CR2_EXTSEL;
	ADC1->CR2 &= ~ADC_CR2_CONT; // single conversion mode
	ADC1->CR2 |= ADC_CR2_DMA;	// dma mode
	ADC1->CR2 |= ADC_CR2_DDS;

	ADC1->SQR1 &= ~ADC_SQR1_L;	// 1 conversion
	ADC1->SQR1 |= ADC_SQR1_L_0; // 2 conversions

	// ADC1->CR2 |= ADC_CR2_EOCS;

	ADC123_COMMON->CCR |= ADC_CCR_ADCPRE; // clock div 4
	ADC123_COMMON->CCR &= ~ADC_CCR_MULTI;
	ADC123_COMMON->CCR &= ~ADC_CCR_DMA;
	ADC123_COMMON->CCR &= ~ADC_CCR_DDS;
	ADC123_COMMON->CCR &= ~ADC_CCR_DELAY;

	ADC1->SQR3 |= ((11U << 0x0) |
				   (10U << 0x5));
	ADC1->SMPR1 |= (ADC_SMPR1_SMP10 |
					ADC_SMPR1_SMP11);

	DMA2_Stream4->CR |= DMA_SxCR_TCIE |
						DMA_SxCR_HTIE |
						DMA_SxCR_TEIE;
	DMA2_Stream4->PAR = (uint32_t)(&(ADC1->DR)); // setting the ADC data register as the peripheral address
	DMA2_Stream4->M0AR = (uint32_t)rawADC;		 // setting the "rawADC" array as the memory location
	DMA2_Stream4->NDTR = 2;

	DMA2_Stream4->CR |= DMA_SxCR_EN;

	ADC1->CR1 |= ADC_CR1_EOCIE |
				 ADC_CR1_OVRIE;
	ADC1->CR2 |= ADC_CR2_ADON;
}

/** @brief	Starts the ADC conversion.
 *
 * 	@return Void.
 */
void adc1Start(void)
{
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

/** @brief	ADC global interrupt handler
 *
 * 	@return Void.
 */
void ADC_IRQHandler(void)
{
	if (ADC1->SR & ADC_SR_OVR) // data overrun
	{
		DMA2_Stream4->PAR = (uint32_t)(&(ADC1->DR)); // setting the ADC data register as the peripheral address
		DMA2_Stream4->M0AR = (uint32_t)rawADC;		 // setting the "rawADC" array as the memory location
		DMA2_Stream4->NDTR = 2;

		ADC1->SR |= ADC_SR_OVR;
		ADC1->CR2 |= ADC_CR2_SWSTART;
	}
	if ((ADC1->SR & ADC_SR_EOC) & (ADC1->CR1 & ADC_CR1_EOCIE)) // end of conversion
		ADC1->SR |= ADC_SR_EOC;
}

/** @brief	DMA2_Stream4 global interrupt handler
 *
 * 	@return Void.
 */
void DMA2_Stream4_IRQHandler(void)
{
	if (DMA2->HISR & DMA_HISR_TEIF4) // transmission error
		DMA2->HIFCR |= DMA_HIFCR_CTEIF4;

	if (DMA2->HISR & DMA_HISR_TCIF4) // transmission complete
		DMA2->HIFCR |= DMA_HIFCR_CTCIF4;

	if (DMA2->HISR & DMA_HISR_HTIF4)
		DMA2->HIFCR |= DMA_HIFCR_CHTIF4;
}