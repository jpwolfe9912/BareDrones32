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
extern uint16_t frameCounter;

// extern semaphore_t frame_1000Hz;
// extern semaphore_t frame_500Hz;
// extern semaphore_t frame_200Hz;
// extern semaphore_t frame_100Hz;
// extern semaphore_t frame_50Hz;
// extern semaphore_t frame_10Hz;
// extern semaphore_t frame_5Hz;
// extern semaphore_t frame_1Hz;

// extern uint32_t deltaTime1000Hz, executionTime1000Hz, previous1000HzTime;
// extern uint32_t deltaTime500Hz, executionTime500Hz, previous500HzTime;
// extern uint32_t deltaTime200Hz, executionTime200Hz, previous200HzTime;
// extern uint32_t deltaTime100Hz, executionTime100Hz, previous100HzTime;
// extern uint32_t deltaTime50Hz, executionTime50Hz, previous50HzTime;
// extern uint32_t deltaTime10Hz, executionTime10Hz, previous10HzTime;
// extern uint32_t deltaTime5Hz, executionTime5Hz, previous5HzTime;
// extern uint32_t deltaTime1Hz, executionTime1Hz, previous1HzTime;

extern float dt8000Hz;

extern semaphore_t systemReady;

extern semaphore_t execUp;

extern volatile uint16_t loopMask;
extern volatile bool loopsChecked;

/* Function Prototypes */
void systemInit(void);
void SystemClock_Config(void);

void delayMicroseconds(uint32_t us);
void delay(int32_t ms);

uint32_t micros(void);
uint32_t millis(void);

// void cycleCounterInit(void);

void SysTick_Handler(void);

#endif /* DRV_DRV_SYSTEM_H_ */
