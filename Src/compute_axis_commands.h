/** @file 		compute_axis_commands.h
 *  @brief
 *  	This file creates the inputs to the PID controller and sends values to the mixer.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

#ifndef __COMPUTE_AXIS_COMMANDS_H__
#define __COMPUTE_AXIS_COMMANDS_H__

/* Global Variables */
extern float   attCmd[3];
extern float   attPID[3];
extern float   ratePID[3];
extern float   rateCmd[3];

/* Function Prototypes */
void computeAxisCommands(float dt);

#endif /* __COMPUTE_AXIS_COMMANDS_H__ */
