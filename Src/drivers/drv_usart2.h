/** @file 		drv_usart2.h
 *  @brief This file enables reading of usart data
 *  	for use with a serial receiver
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART2_H__
#define __USART2_H__

#include <stdint.h>
#include <stdbool.h>

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
} Usart2Buffs_t;

extern Usart2Buffs_t Buff_2;

/* Prototypes */
void usart2Init(uint32_t baudrate);
void usart2BeginRx(void);


#endif /* __USART2_H__ */
