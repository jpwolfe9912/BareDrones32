/** @file 		drv_printf.h
 *  @brief
 *  	This file enables reading of usart data
 *  	for use with printf.
 *
 *  Based on which device you are using (Nucleo or Autodrone PCB),
 *  enable the corresponding Include in the preprocessor
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_PRINTF_H__
#define __DRV_PRINTF_H__

/* Function Prototypes */
void printfInit(void);
void printfWrite(uint8_t ch);
void printfRead8(uint8_t *num);
void printfReadPID(float *P, float *I, float *D);
bool printfWaitFor(char wait);

#endif /* __DRV_printf_H__ */
