/** @file 		transition.c
 *  @brief
 *  	This file allows for the transition between flight modes.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		16 MAR 2022
 */

/* Includes */
#include "board.h"

/*
 * PA8 - Worm 1 PWM
 * PA9 - Worm 2 PWM
 * PC8 - Worm 1 Dir
 * PC9 - Worm 2 Dir
 *
 * */

void
modeTransition(void)
{
	systemReady = false;
	motor_initialized = false;

	wormDrive();

	motors3dOn();

	while(!(arm1Hit & arm2Hit));	// wait for both arms to hit

	if(mode == TRANS_FLIGHT)
		mode = FLIGHT;
	else if(mode == TRANS_ROVER)
		mode = ROVER;

	arm1Hit = false;
	arm2Hit = false;

	systemReady = true;
	motor_initialized = true;

}
