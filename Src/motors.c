/*
 * motors.c
 *
 *  Created on: Feb 23, 2022
 *      Author: jeremywolfe
 */

#include "board.h"

uint16_t motor_value[4];

semaphore_t motor_initialized = 0;

void motorInit(void)
{
	printf("\nInitializing Motors\n");

	motor_value[0] = 0;
	motor_value[1] = 0;
	motor_value[2] = 0;
	motor_value[3] = 0;
//	for(uint16_t i = 0; i < 2000; i++){
//		dshotWrite(motor_value);
//		delayMicroseconds(1000);
//	}
	delay(1500);
	motor_initialized = 1;
}
