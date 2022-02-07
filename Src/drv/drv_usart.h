/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_lwrb.h"

#define RXBUF_SIZE 					32

void usart1Init(void);
void usart1Read(uint8_t *pData, uint8_t size);

void uart5Init(void);
int uart5Write(int ch);

void usart3Init(void);
int usart3Write(int ch);


/*		Global Variables	*/
extern uint8_t rxBuf[RXBUF_SIZE];
extern lwrb_t rxRingBuf;
extern uint8_t rxRingBufData[RXBUF_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
