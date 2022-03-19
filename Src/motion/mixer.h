/** @file 		mixer.h
 *  @brief
 *  	This file takes the output of the PID controller and assigns values to the motors.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

#ifndef __MIXER_H__
#define __MIXER_H__

/* Defines */
#define PIDMIXFLIGHT(X,Y,Z,T) (ratePID[ROLL] * (X) + ratePID[PITCH] * (Y) + eepromConfig.yawDirection * ratePID[YAW] * (Z) + throttleCmd * (T))
#define PIDMIXROVER(X,T) (steerCmd * (X) + speedCmd * (T))

/* Global Variables */
extern uint8_t numberMotor;
extern uint16_t throttleCmd;
extern int16_t steerCmd, speedCmd;
extern int16_t motor_temp[4];

/* Function Prototypes */
void pulseMotors(void);
void mixTable(void);

#endif /* __MIXER_H__ */
