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

/* Defines */
#define RX1BUF_SIZE 				32

/* Global Variables */
extern uint8_t rx1Buf[RX1BUF_SIZE];

/* Prototypes */
void usart1Init(void);
void usart1Read(uint8_t *pData, uint8_t size);
void usart6Init(void);
void usart6Write(char *pData, uint8_t size);

#endif /* __USART_H__ */
