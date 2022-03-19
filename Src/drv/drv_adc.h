/** @file 		drv_adc.h
 *  @brief
 *  	This file initializes and enables the adc for reading the battery voltage.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 MAR 2022
 */

#ifndef INC_DRV_ADC_H_
#define INC_DRV_ADC_H_

extern uint16_t *val;
extern float battVoltage;

typedef enum
{
	NO_BATT,
	THREE = 3,
	FOUR,
	FIVE,
	SIX
}battCells_e;

void adc1Ch8Init(void);
void batMonRead(void);

#endif /* INC_DRV_ADC_H_ */
