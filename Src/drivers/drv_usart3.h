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

#include "lwrb.h"

/* Defines */
#define WIREDLOGGING_BAUDRATE 1000000U // 115200U

#define RX_DMA_SIZE 1024
#define RX_RB_SIZE 2048

/* Global Variables */
typedef struct 
{
    uint8_t RxBuffer_DMA[RX_DMA_SIZE];
    uint8_t RxBuffer_Data[RX_RB_SIZE];
    lwrb_t RxBuffer;
} Usart3Buffs_t;
extern Usart3Buffs_t Buff_3;

typedef void (*usart3TxCallback_t)(void);

/* Function Prototypes */
void usart3Init(uint32_t baudrate);
void usart3RegisterCallback(usart3TxCallback_t cb);
bool usart3Tx(const char* str, size_t size);
bool usart3TxBusy(void);
void usart3TxOneByte(uint8_t ch);

void usart3Read8(uint8_t *num);
void usart3ReadPID(float *P, float *I, float *D);
bool usart3WaitFor(char wait);


#endif /* __DRV_USART3_H__ */
