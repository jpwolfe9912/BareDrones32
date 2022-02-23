/*
 * motors.c
 *
 *  Created on: Feb 23, 2022
 *      Author: jeremywolfe
 */

#include "board.h"

extern uint16_t motor_value[4];

void motorInit(void)
{
	motor_value[0] = 0;
	motor_value[1] = 0;
	motor_value[2] = 0;
	motor_value[3] = 0;
	for(uint16_t i = 0; i < 4000; i++){
		dshotWrite(motor_value);
		delayMicroseconds(100);
	}
}
