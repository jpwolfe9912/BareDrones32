/** @file 		drv_usart.h
 *  @brief This file enables reading of usart data
 *  	for use with a serial receiver
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 *  @bug
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "drv_lwrb.h"

/* Defines */
#define RXBUF_SIZE 					32

/* Prototypes */
void usart1Init(void);
void usart1Read(uint8_t *pData, uint8_t size);

/*		Global Variables	*/
extern uint8_t rxBuf[RXBUF_SIZE];
extern lwrb_t rxRingBuf;
extern uint8_t rxRingBufData[RXBUF_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
