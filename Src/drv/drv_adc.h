/** @file 		drv_adc.h
 *  @brief
 *  	This file initializes and enables the adc for reading the battery voltage.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 MAR 2022
 */

#ifndef INC_DRV_ADC_H_
#define INC_DRV_ADC_H_

/* Enumerations */
typedef enum
{
	NO_BATT,
	THREE = 3,
	FOUR,
	FIVE,
	SIX
}battCells_e;

/* Global Variables */
extern uint16_t *val;
extern float battVoltage;
extern battCells_e cells;
extern bool battLow;
extern bool battEmpty;

/* Function Prototypes */
void adc1Ch8Init(void);
void batMonRead(void);

#endif /* INC_DRV_ADC_H_ */
