/** @file 		battery.h
 *  @brief
 *  	This file initializes the battery monitor and provides functions to process the data.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 MAR 2022
 */

#ifndef SRC_BATTERY_H_
#define SRC_BATTERY_H_

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
extern float battVoltage;
extern battCells_e cells;
extern bool battLow;
extern bool battEmpty;

/* Function Prototypes */
void battMonInit(void);
void battMonRead(void);

#endif /* SRC_BATTERY_H_ */
