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
/* Prototypes */

/** @brief Initializes the low level uart registers in order to use printf
 *
 *  @return Void.
 */
void serialInit(void);

/** @brief Uses polling to write data to the transmit buffer.
 *
 *  @param ch The character to send.
 *  @return Void.
 */
void serialWrite(int ch);

/** @brief Waits for a character.
 *
 *  @param wait Character to wait for.
 *  @return bool True or False based on whether or not the character.
 *  received is the input to the function.
 */
bool serialWaitFor(char wait);

/*		Global Variables	*/

#ifdef __cplusplus
}
#endif

#endif /* __DRV_SERIAL_H__ */
