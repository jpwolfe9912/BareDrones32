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
// extern uint16_t rawADC;//uint16_t *rawADC[2];
extern uint16_t rawADC[2];

/* Function Prototypes */
void adc1Init(void);
void adc1Start(void);

#endif /* __DRV_ADC_H__ */
