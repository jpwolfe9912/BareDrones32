/** @file 		battery.c
 *  @brief
 *  	This file initializes the battery monitor and provides functions to process the data.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 MAR 2022
 */

#include "board.h"

/* Global Functions */
float battVoltage;
battCells_e cells;
bool battLow = false;
bool battEmpty = false;

/* Static Function Prototypes */
static battCells_e batt_cells(void);
static void batt_warning(void);

/** @brief Initializes the battery monitor.
 *
 *  @return Void.
 */
void
battMonInit(void)
{
	cells = batt_cells();
	if(cells != 0)
		printf("\n%d cells detected\n", cells);
	else
		printf("\nNo battery detected\n");
}

/** @brief Starts the ADC conversion and processes the value.
 *
 *  @return Void.
 */
void
battMonRead(void)
{
	adc1Ch8Start();
	battVoltage = *rawADC * 0.0088623f;
	batt_warning();
}

/** @brief Determines the amount of cells in the battery based on the voltage.
 *
 *  @return battCell_e Cell count of the battery.
 */
static battCells_e
batt_cells(void)
{
	float summedVoltage = 0;
	float avgVoltage = 0;
	for(uint8_t i = 0; i < 10; i++)
	{
		battMonRead();
		summedVoltage += battVoltage;
		delay(1);
	}
	avgVoltage = summedVoltage / 10.0f;

	if((avgVoltage >= 10.0f) && (avgVoltage < 13.3))
		return THREE;
	else if((avgVoltage >= 13.3) && (avgVoltage < 17.15))
		return FOUR;
	else if((avgVoltage >= 17.15) && (avgVoltage < 21))
		return FIVE;
	else if(avgVoltage >= 21)
		return SIX;
	else
		return NO_BATT;

}

/** @brief Warns the user if voltage falls below 3.7V and 3.5V per cell.
 *
 *  @return Void.
 */
static void
batt_warning(void)
{
	if(cells != 0)
	{
		if(battVoltage < (cells * 3.5)){
			color(RED, YES);
			printf("\nBATTERY EMPTY\n");
			colorDefault();
			battEmpty = true;
		}
		else if(battVoltage < (cells * 3.7)){
			color(YELLOW, YES);
			printf("\nBATTERY LOW\n");
			colorDefault();
			battLow = true;
		}
		else
		{
			battLow = false;
			battEmpty = false;
		}
	}
}
