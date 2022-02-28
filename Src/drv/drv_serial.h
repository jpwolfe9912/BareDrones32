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
 *  @bug
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_SERIAL_H__
#define __DRV_SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Function Prototypes */
void serialInit(void);
void serialWrite(int ch);
bool serialWaitFor(char wait);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_SERIAL_H__ */
