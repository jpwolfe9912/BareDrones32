/** @file 		drv_usart.h
 *  @brief This file enables reading of usart data
 *  	for use with a serial receiver
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#include "lwrb.h"

/* Defines */
#define RX_DMA_SIZE 1024
#define RX_RB_SIZE 2048

/* Global Variables */
typedef struct 
{
    uint8_t RxBuffer_DMA[RX_DMA_SIZE];
    uint8_t RxBuffer_Data[RX_RB_SIZE];
    lwrb_t RxBuffer;
} Usart1Buffs_t;

extern Usart1Buffs_t Buffs;

/* Prototypes */
void usart1Init(void);
void usart1BeginRx(void);


#endif /* __USART_H__ */
