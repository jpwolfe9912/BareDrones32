/** @file 		drv_adc.c
 *  @brief
 *  	This file initializes and enables the adc for reading the battery voltage.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
uint16_t *val = (uint16_t*)0x20010000;
float battVoltage;
battCells_e cells;

/* Static Function Prototypes */
static battCells_e batt_cells(void);
static void batt_warning(void);

/** @brief Initializes ADC1 for Channel 8.
 *
 *  @return Void.
 */
void
adc1Ch8Init(void)
{
	/* RCC Init */
	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOBEN;
	RCC->APB2ENR	|= RCC_APB2ENR_ADC1EN;

	/* GPIO Init */
	GPIOB->MODER 	&= ~GPIO_MODER_MODER0;
	GPIOB->MODER 	|= GPIO_MODER_MODER0;
	GPIOB->OTYPER	&= ~GPIO_OTYPER_OT0;
	GPIOB->PUPDR	&= ~GPIO_PUPDR_PUPDR0;

	GPIOB->MODER 	&= ~GPIO_MODER_MODER1;
	GPIOB->MODER 	|= GPIO_MODER_MODER1;
	GPIOB->OTYPER	&= ~GPIO_OTYPER_OT1;
	GPIOB->PUPDR	&= ~GPIO_PUPDR_PUPDR1;

	/* DMA Init */
	DMA2_Stream4->CR	&= ~(DMA_SxCR_EN);
	while(DMA2_Stream4->CR & DMA_SxCR_EN);

	DMA2_Stream4->CR	&= ~(0x0 << 25U);
	DMA2_Stream4->CR	&= ~DMA_SxCR_DIR;
	DMA2_Stream4->CR	&= ~DMA_SxCR_PL;
	DMA2_Stream4->CR	|= DMA_SxCR_CIRC;
	DMA2_Stream4->CR	&= ~DMA_SxCR_PINC;
	DMA2_Stream4->CR	|= DMA_SxCR_MINC;
	DMA2_Stream4->CR	|= DMA_SxCR_PSIZE_0;
	DMA2_Stream4->CR	|= DMA_SxCR_MSIZE_0;
	DMA2_Stream4->FCR	&= ~DMA_SxFCR_DMDIS;

	/* ADC1 interrupt Init */
	NVIC_SetPriority(ADC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(ADC_IRQn);

	ADC1->CR1	&= ~ADC_CR2_ADON;	// turn off the ADC

	ADC1->CR1	&= ~ADC_CR1_RES;	// 12 bits of resolution
	ADC1->CR1	&= ~ADC_CR1_SCAN;	// scan mode enable
	ADC1->CR2	&= ~ADC_CR2_ALIGN;	// data alignment right

	ADC1->CR1	&= ~ADC_CR1_DISCEN;	// disable discontinuous mode

	ADC1->CR2	&= ~ADC_CR2_EXTEN;	// software trigger
	ADC1->CR2	&= ~ADC_CR2_EXTSEL;
	ADC1->CR2	&= ~ADC_CR2_CONT;	// continuous mode
	ADC1->CR2	|= ADC_CR2_DMA;		// dma mode
	ADC1->CR2	|= ADC_CR2_DDS;

	ADC1->SQR1	&= ~ADC_SQR1_L;	// 1 conversions

	ADC1->CR2	&= ~ADC_CR2_EOCS;

	ADC123_COMMON->CCR	|= ADC_CCR_ADCPRE_0;	// clock div 4
	ADC123_COMMON->CCR	&= ~ADC_CCR_MULTI;
	ADC123_COMMON->CCR	&= ~ADC_CCR_DMA;
	ADC123_COMMON->CCR	&= ~ADC_CCR_DDS;
	ADC123_COMMON->CCR	&= ~ADC_CCR_DELAY;


	ADC1->SQR3	|= (8U << 0U);
	ADC1->SMPR2	|= ADC_SMPR2_SMP8;

	DMA2_Stream4->CR	|= DMA_SxCR_TCIE |
						   DMA_SxCR_TEIE;
	DMA2_Stream4->PAR = (uint32_t)(&(ADC1->DR));		// setting the ADC data register as the peripheral address
	DMA2_Stream4->M0AR = (uint32_t)val;				// setting the "val" array as the memory location
	DMA2_Stream4->NDTR = 1;

	DMA2_Stream4->CR	|= DMA_SxCR_EN;

	ADC1->CR1	|= ADC_CR1_EOCIE |
				   ADC_CR1_OVRIE;
	ADC1->CR2	|= ADC_CR2_ADON;

	cells = batt_cells();
	printf("\n%d cells detected\n", cells);
}

/** @brief Starts the ADC conversion.
 *
 *  @return Void.
 */
void
batMonRead(void)
{
	ADC1->CR2	|= ADC_CR2_SWSTART;
	battVoltage = *val * 0.0088623f;
	batt_warning();
}

/** @brief Determines the amount of cells in the battery based on the voltage.
 *
 *  @return battCell_e Cell count of the battery.
 */
static battCells_e
batt_cells(void)
{
	uint16_t summedVoltage = 0;
	uint16_t avgVoltage = 0;
	for(uint8_t i = 0; i < 10; i++)
	{
		batMonRead();
		summedVoltage += *val;
		delay(1);
	}
	avgVoltage = summedVoltage / 10;

	if((avgVoltage >= 1128) && (avgVoltage < 1501))
		return THREE;
	else if((avgVoltage >= 1501) && (avgVoltage < 1935))
		return FOUR;
	else if((avgVoltage >= 1935) && (avgVoltage < 2607))
		return FIVE;
	else if(avgVoltage >= 2607)
		return SIX;
	else
		return NO_BATT;

}

/** @brief Warns the user if voltage falls below 3.7V and 3.5V per cell.
 *
 *  @return Void.
 */
static void
batt_warning(void)
{
	if(battVoltage < (cells * 3.5)){
		color(RED, YES);
		printf("\nBATTERY EMPTY\n");
		colorDefault();
	}
	else if(battVoltage < (cells * 3.7)){
		color(YELLOW, YES);
		printf("\nBATTERY LOW\n");
		colorDefault();
	}
}

/** @brief	ADC global interrupt handler
 *
 * 	@return Void.
 */
void
ADC_IRQHandler(void){
	if(ADC1->SR & ADC_SR_OVR)	// data overrun
	{
		DMA2_Stream4->PAR = (uint32_t)(&(ADC1->DR));		// setting the ADC data register as the peripheral address
		DMA2_Stream4->M0AR = (uint32_t)val;				// setting the "val" array as the memory location
		DMA2_Stream4->NDTR = 1;

		ADC1->SR 	|= ADC_SR_OVR;
		ADC1->CR2	|= ADC_CR2_SWSTART;
	}
	if(ADC1->SR & ADC_SR_EOC)			// end of conversion
		ADC1->SR |= ADC_SR_EOC;

}

/** @brief	DMA2_Stream4 global interrupt handler
 *
 * 	@return Void.
 */
void
DMA2_Stream4_IRQHandler(void){
	if(DMA2->HISR & DMA_HISR_TEIF4)		// transmission error
		DMA2->HIFCR |= DMA_HIFCR_CTEIF4;

	if(DMA2->HISR & DMA_HISR_TCIF4)		// transmission complete
		DMA2->HIFCR |= DMA_HIFCR_CTCIF4;
}












