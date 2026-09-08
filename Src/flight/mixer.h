/** @file 		mixer.h
 *  @brief
 *  	This file takes the output of the PID controller and assigns values to the motors.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

#ifndef __MIXER_H__
#define __MIXER_H__

#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "drv_dshot_burst.h"
#include "compute_axis_commands.h"

 /* Defines */
#define PIDMIXFLIGHT(X,Y,Z,T) 	(ratePID[ROLL] * (X) + ratePID[PITCH] * (Y) + eepromConfig.yawDirection * ratePID[YAW] * (Z) + throttleCmd * (T))

#define DSHOT_THROTTLE_OFFSET		DSHOT_MIN_THROTTLE
#define DSHOT_THROTTLE_SCALE	(DSHOT_MAX_THROTTLE - DSHOT_IDLE_THROTTLE) / (MAXCOMMAND - eepromConfig.minThrottle * 1.0f)

/* Global Variables */
extern uint8_t numberMotor;
extern uint16_t throttleCmd;
extern int16_t motor_temp[4];

/* Function Prototypes */
void pulseMotors(void);
void mixTable(void);

#endif /* __MIXER_H__ */
