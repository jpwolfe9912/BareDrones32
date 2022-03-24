/** @file 		drv_adc.h
 *  @brief
 *  	This file initializes and enables the adc for reading the battery voltage.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 MAR 2022
 */

#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

/* Global Variables */
extern uint16_t *rawADC;

/* Function Prototypes */
void adc1Ch8Init(void);
void adc1Ch8Start(void);

#endif /* __DRV_ADC_H__ */
