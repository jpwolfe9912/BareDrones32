/** @file 		drv_serial.h
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
#ifndef __DRV_SERIAL_H__
#define __DRV_SERIAL_H__

/* Function Prototypes */
void serialInit(void);
void serialWrite(uint8_t ch);
void serialRead8(uint8_t *num);
void serialReadPID(float *P, float *I, float *D);
bool serialWaitFor(char wait);

#endif /* __DRV_SERIAL_H__ */
