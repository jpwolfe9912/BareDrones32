/** @file 		drv_usart.h
 *  @brief This file enables reading of usart data
 *  	for use with a serial receiver
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_USART6_H__
#define __DRV_USART6_H__

/* Prototypes */
void usart6Init(void);
void usart6Write(char *pData, uint8_t size);

#endif /* __USART_H__ */
