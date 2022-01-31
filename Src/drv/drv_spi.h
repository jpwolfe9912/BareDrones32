/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN Private defines */
#define SPI1_ENABLE					GPIOA->BSRR |= GPIO_BSRR_BR4
#define SPI1_DISABLE				GPIOA->BSRR |= GPIO_BSRR_BS4

#define SPI_BR_PRESCALER_16			(0x3 << 3U)
#define SPI_BR_PRESCALER_128		(0x6 << 3U)
/* USER CODE END Private defines */

void MX_SPI1_Init(void);
void MX_SPI2_Init(void);

/* USER CODE BEGIN Prototypes */
void spiReadBytes(uint8_t reg, uint8_t *pData, uint8_t size);
void spiWriteBytes(uint8_t *pData, uint8_t size);
void spiReadOneByte(uint8_t reg, uint8_t *pData);
void spiWriteOneByte(uint8_t reg, uint8_t data);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
