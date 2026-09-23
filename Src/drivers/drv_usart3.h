/** @file 		drv_usart3.h
 *  @brief
 *  	This file enables reading of usart data
 *  	for use with usart3.
 *
 *  Based on which device you are using (Nucleo or Autodrone PCB),
 *  enable the corresponding Include in the preprocessor
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_USART3_H__
#define __DRV_USART3_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define WIREDLOGGING_BAUDRATE 115200U

/* Function Prototypes */
void usart3Init(uint32_t baudrate);
bool usart3Write(uint8_t* pData, uint8_t size);
void usart3WriteOneByte(uint8_t ch);
void usart3Read8(uint8_t *num);
void usart3ReadPID(float *P, float *I, float *D);
bool usart3WaitFor(char wait);

#endif /* __DRV_USART3_H__ */
