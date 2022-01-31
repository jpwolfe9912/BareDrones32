/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../Libraries/STM32F7xx_HAL_Driver/Inc/stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Limit1_Int_Pin GPIO_PIN_13
#define Limit1_Int_GPIO_Port GPIOC
#define Limit1_Int_EXTI_IRQn EXTI15_10_IRQn
#define Limit2_Int_Pin GPIO_PIN_14
#define Limit2_Int_GPIO_Port GPIOC
#define Limit2_Int_EXTI_IRQn EXTI15_10_IRQn
#define LED_1_Pin GPIO_PIN_0
#define LED_1_GPIO_Port GPIOC
#define LED_2_Pin GPIO_PIN_1
#define LED_2_GPIO_Port GPIOC
#define LED_3_Pin GPIO_PIN_2
#define LED_3_GPIO_Port GPIOC
#define LED_4_Pin GPIO_PIN_3
#define LED_4_GPIO_Port GPIOC
#define M4_Pin GPIO_PIN_0
#define M4_GPIO_Port GPIOA
#define M3_Pin GPIO_PIN_1
#define M3_GPIO_Port GPIOA
#define M2_Pin GPIO_PIN_2
#define M2_GPIO_Port GPIOA
#define M1_Pin GPIO_PIN_3
#define M1_GPIO_Port GPIOA
#define GYRO_CS_Pin GPIO_PIN_4
#define GYRO_CS_GPIO_Port GPIOA
#define CURR_SENSE_Pin GPIO_PIN_4
#define CURR_SENSE_GPIO_Port GPIOC
#define VCC_SENSE_Pin GPIO_PIN_0
#define VCC_SENSE_GPIO_Port GPIOB
#define CURR_SENSEB1_Pin GPIO_PIN_1
#define CURR_SENSEB1_GPIO_Port GPIOB
#define OSD_CS_Pin GPIO_PIN_12
#define OSD_CS_GPIO_Port GPIOB
#define WORM1_DIR_Pin GPIO_PIN_8
#define WORM1_DIR_GPIO_Port GPIOC
#define WORM2_DIR_Pin GPIO_PIN_9
#define WORM2_DIR_GPIO_Port GPIOC
#define WORM1_PWM_Pin GPIO_PIN_8
#define WORM1_PWM_GPIO_Port GPIOA
#define WORM2_PWM_Pin GPIO_PIN_9
#define WORM2_PWM_GPIO_Port GPIOA
#define BUZZ_Pin GPIO_PIN_4
#define BUZZ_GPIO_Port GPIOB
#define LED_STRIP_Pin GPIO_PIN_5
#define LED_STRIP_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
