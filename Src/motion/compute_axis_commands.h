/** @file 		compute_axis_commands.h
 *  @brief
 *  	This file creates the inputs to the PID controller and sends values to the mixer.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

#ifndef __COMPUTE_AXIS_COMMANDS_H__
#define __COMPUTE_AXIS_COMMANDS_H__

/* Defines */
#define RATECURVE(x)	(((eepromConfig.rateCoeffAlpha * CUBE(x)) + (eepromConfig.rateCoeffBravo * x)) * PI / 180.0);

/* Global Variables */
extern float   attCmd[3];
extern float   attPID[3];
extern float   ratePID[3];
extern float   rateCmd[3];

/* Function Prototypes */
void computeAxisCommands(float dt);

#endif /* __COMPUTE_AXIS_COMMANDS_H__ */
