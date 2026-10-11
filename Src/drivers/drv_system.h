/** @file 		drv_system.h
 *  @brief
 *  	This file contains all the basic functions to run the timing
 *  	of the system as well as initialize the system.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#ifndef __DRV_SYSTEM_H__
#define __DRV_SYSTEM_H__

#include <stdint.h>
#include <stdbool.h>

#include "baredrones32.h"

 /* Defines */
#define FRAME_COUNT 8000
#define SYSTICK_PERIOD_US (1000000U / FRAME_COUNT)

#define COUNT_8000HZ 1  // Number of 8000 Hz frames for 8000 Hz Loop
#define COUNT_4000HZ 2  // Number of 8000 Hz frames for 4000 Hz Loop
#define COUNT_2000HZ 4  // Number of 8000 Hz frames for 2000 Hz Loop
#define COUNT_1000HZ 8  // Number of 8000 Hz frames for  100 Hz Loop
#define COUNT_500HZ 16  // Number of 8000 Hz frames for  100 Hz Loop
#define COUNT_200HZ 40  // Number of 8000 Hz frames for  100 Hz Loop
#define COUNT_100HZ 80  // Number of 8000 Hz frames for  100 Hz Loop
#define COUNT_50HZ 160  // Number of 8000 Hz frames for   50 Hz Loop
#define COUNT_10HZ 800  // Number of 8000 Hz frames for   10 Hz Loop
#define COUNT_5HZ 1600  // Number of 8000 Hz frames for    5 Hz Loop
#define COUNT_1HZ 8000  // Number of 8000 Hz frames for    1 Hz Loop

/* Global Variables */
extern semaphore_t systemReady;     // TODO: Necessary variable but too many things abuse it

extern volatile uint16_t loopMask;
extern volatile bool loopsChecked;

/* Function Prototypes */
void systemInit(void);

void delayMicroseconds(uint32_t us);
void delay(int32_t ms);

uint32_t micros(void);
uint32_t millis(void);

void pauseSysTick(void);

void SysTick_Handler(void);

#endif /* DRV_DRV_SYSTEM_H_ */
