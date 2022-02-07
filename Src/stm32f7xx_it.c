/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f7xx_it.c
 * @brief   Interrupt Service Routines.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_i2c1_rx;
extern DMA_HandleTypeDef hdma_i2c1_tx;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi2_tx;
extern DMA_HandleTypeDef hdma_uart5_rx;
extern DMA_HandleTypeDef hdma_uart5_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M7 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1)
	{
	}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
	/* USER CODE BEGIN HardFault_IRQn 0 */

	/* USER CODE END HardFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
	/* USER CODE BEGIN MemoryManagement_IRQn 0 */

	/* USER CODE END MemoryManagement_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
		/* USER CODE END W1_MemoryManagement_IRQn 0 */
	}
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
	/* USER CODE BEGIN BusFault_IRQn 0 */

	/* USER CODE END BusFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_BusFault_IRQn 0 */
		/* USER CODE END W1_BusFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
	/* USER CODE BEGIN UsageFault_IRQn 0 */

	/* USER CODE END UsageFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_UsageFault_IRQn 0 */
		/* USER CODE END W1_UsageFault_IRQn 0 */
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
	/* USER CODE BEGIN SVCall_IRQn 0 */

	/* USER CODE END SVCall_IRQn 0 */
	/* USER CODE BEGIN SVCall_IRQn 1 */

	/* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
	/* USER CODE BEGIN DebugMonitor_IRQn 0 */

	/* USER CODE END DebugMonitor_IRQn 0 */
	/* USER CODE BEGIN DebugMonitor_IRQn 1 */

	/* USER CODE END DebugMonitor_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
	/* USER CODE BEGIN PendSV_IRQn 0 */

	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */

	/* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
//void SysTick_Handler(void)
//{
//  /* USER CODE BEGIN SysTick_IRQn 0 */
//
//  /* USER CODE END SysTick_IRQn 0 */
//  HAL_IncTick();
//  /* USER CODE BEGIN SysTick_IRQn 1 */
//
//  /* USER CODE END SysTick_IRQn 1 */
//}

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/

/**
 * \brief           USART1 global interrupt handler
 */
void USART1_IRQHandler(void) {
	/* Check for IDLE line interrupt */
	if (USART1->ISR & USART_ISR_IDLE) {
		USART1->ICR		|= USART_ICR_IDLECF;	/* Clear IDLE line flag */
		usart_rx_check(DMA2_Stream2->NDTR);                       /* Check for data to process */
	}

	/* Implement other events when needed */
}


/**
 * @brief This function handles DMA1 stream0 global interrupt.
 */
//void DMA1_Stream0_IRQHandler(void)
//{
//	/* USER CODE BEGIN DMA1_Stream0_IRQn 0 */
//
//	/* USER CODE END DMA1_Stream0_IRQn 0 */
//	HAL_DMA_IRQHandler(&hdma_uart5_rx);
//	/* USER CODE BEGIN DMA1_Stream0_IRQn 1 */
//
//	/* USER CODE END DMA1_Stream0_IRQn 1 */
//}

/**
 * @brief This function handles DMA1 stream3 global interrupt.
 */
//void DMA1_Stream3_IRQHandler(void)
//{
//	/* USER CODE BEGIN DMA1_Stream3_IRQn 0 */
//
//	/* USER CODE END DMA1_Stream3_IRQn 0 */
//	HAL_DMA_IRQHandler(&hdma_spi2_rx);
//	/* USER CODE BEGIN DMA1_Stream3_IRQn 1 */
//
//	/* USER CODE END DMA1_Stream3_IRQn 1 */
//}

/**
 * @brief This function handles DMA1 stream4 global interrupt.
 */
//void DMA1_Stream4_IRQHandler(void)
//{
//	/* USER CODE BEGIN DMA1_Stream4_IRQn 0 */
//
//	/* USER CODE END DMA1_Stream4_IRQn 0 */
//	HAL_DMA_IRQHandler(&hdma_spi2_tx);
//	/* USER CODE BEGIN DMA1_Stream4_IRQn 1 */
//
//	/* USER CODE END DMA1_Stream4_IRQn 1 */
//}

/**
 * @brief This function handles DMA1 stream5 global interrupt.
 */
void DMA1_Stream5_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

	/* USER CODE END DMA1_Stream5_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_i2c1_rx);
	/* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

	/* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream6 global interrupt.
 */
void DMA1_Stream6_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Stream6_IRQn 0 */

	/* USER CODE END DMA1_Stream6_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_i2c1_tx);
	/* USER CODE BEGIN DMA1_Stream6_IRQn 1 */

	/* USER CODE END DMA1_Stream6_IRQn 1 */
}

/**
 * @brief This function handles EXTI line[15:10] interrupts.
 */
void EXTI15_10_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI15_10_IRQn 0 */

	/* USER CODE END EXTI15_10_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	/* USER CODE BEGIN EXTI15_10_IRQn 1 */

	/* USER CODE END EXTI15_10_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream7 global interrupt.
 */
void DMA1_Stream7_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Stream7_IRQn 0 */

	/* USER CODE END DMA1_Stream7_IRQn 0 */

	/* USER CODE BEGIN DMA1_Stream7_IRQn 1 */

	/* USER CODE END DMA1_Stream7_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream1 global interrupt.
 */
void DMA2_Stream0_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream0_IRQn 0 */
	if(DMA2->LISR & DMA_LISR_TCIF0){
		DMA2->LIFCR		|= DMA_LIFCR_CTCIF0;

		SPI1_DISABLE;

		DMA2_Stream0->CR	&= ~DMA_SxCR_EN;
		DMA2_Stream3->CR	&= ~DMA_SxCR_EN;

		SPI1->CR2			&= ~SPI_CR2_RXDMAEN;
		SPI1->CR2			&= ~SPI_CR2_TXDMAEN;

		SPI1->CR1			&= ~SPI_CR1_SPE;

	}
	/* USER CODE END DMA2_Stream0_IRQn 0 */

	/* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

	/* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream1 global interrupt.
 */
void DMA2_Stream1_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream1_IRQn 0 */

	/* USER CODE END DMA2_Stream1_IRQn 0 */

	/* USER CODE BEGIN DMA2_Stream1_IRQn 1 */

	/* USER CODE END DMA2_Stream1_IRQn 1 */
}

void DMA2_Stream2_IRQHandler(void) {
	/* Check half-transfer complete interrupt */
	if(DMA2->LISR & DMA_LISR_TCIF2){
		DMA2->LIFCR		|= DMA_LIFCR_CTCIF2;	/* Clear half-transfer complete flag */
		usart_rx_check(DMA2_Stream2->NDTR);                       /* Check for data to process */
	}

	/* Check transfer-complete interrupt */
	if(DMA2->LISR & DMA_LISR_HTIF2){
		DMA2->LIFCR		|= DMA_LIFCR_CHTIF2;	/* Clear half-transfer complete flag */
		usart_rx_check(DMA2_Stream2->NDTR);                       /* Check for data to process */
	}
}

/**
 * @brief This function handles DMA2 stream5 global interrupt.
 */
void DMA2_Stream3_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream3_IRQn 0 */
	if(DMA2->LISR & DMA_LISR_TCIF3){
		DMA2->LIFCR		|= DMA_LIFCR_CTCIF3;
	}
	/* USER CODE END DMA2_Stream3_IRQn 0 */

	/* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

	/* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream5 global interrupt.
 */
void DMA2_Stream5_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream5_IRQn 0 */

	/* USER CODE END DMA2_Stream5_IRQn 0 */

	/* USER CODE BEGIN DMA2_Stream5_IRQn 1 */

	/* USER CODE END DMA2_Stream5_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream6 global interrupt.
 */
void DMA2_Stream6_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream6_IRQn 0 */

	/* USER CODE END DMA2_Stream6_IRQn 0 */

	/* USER CODE BEGIN DMA2_Stream6_IRQn 1 */

	/* USER CODE END DMA2_Stream6_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream7 global interrupt.
 */
void DMA2_Stream7_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream7_IRQn 0 */

	/* USER CODE END DMA2_Stream7_IRQn 0 */

	/* USER CODE BEGIN DMA2_Stream7_IRQn 1 */

	/* USER CODE END DMA2_Stream7_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void DMA1_Stream2_IRQHandler(void){
	if(DMA1->LISR & DMA_LISR_TCIF2){
		DMA1_Stream2->CR 	&= ~DMA_SxCR_EN;
		while(DMA1_Stream2->CR & DMA_SxCR_EN){}
		TIM5->DIER 			&= ~TIM_DIER_CC1DE;
		DMA1->LIFCR			|= DMA_LIFCR_CTCIF2;
		DMA1_Stream2->NDTR	= DSHOT_DMA_BUFFER_SIZE;
	}
}
void DMA1_Stream4_IRQHandler(void){
	if(DMA1->HISR & DMA_HISR_TCIF4){
		DMA1_Stream4->CR 	&= ~DMA_SxCR_EN;
		while(DMA1_Stream4->CR & DMA_SxCR_EN){}
		TIM5->DIER 			&= ~TIM_DIER_CC2DE;
		DMA1->HIFCR			|= DMA_HIFCR_CTCIF4;
		DMA1_Stream4->NDTR	= DSHOT_DMA_BUFFER_SIZE;
	}
}
void DMA1_Stream0_IRQHandler(void){
	if(DMA1->LISR & DMA_LISR_TCIF0){
		DMA1_Stream0->CR 	&= ~DMA_SxCR_EN;
		while(DMA1_Stream0->CR & DMA_SxCR_EN){}
		TIM5->DIER 			&= ~TIM_DIER_CC3DE;
		DMA1->LIFCR			|= DMA_LIFCR_CTCIF0;
		DMA1_Stream0->NDTR	= DSHOT_DMA_BUFFER_SIZE;
	}
}
void DMA1_Stream3_IRQHandler(void){
	if(DMA1->LISR & DMA_LISR_TCIF3){
		DMA1_Stream3->CR 	&= ~DMA_SxCR_EN;
		while(DMA1_Stream3->CR & DMA_SxCR_EN){}
		TIM5->DIER 			&= ~TIM_DIER_CC4DE;
		DMA1->LIFCR			|= DMA_LIFCR_CTCIF3;
		DMA1_Stream3->NDTR	= DSHOT_DMA_BUFFER_SIZE;
	}
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
