/** @file 		process_commands.h
 *  @brief
 *  	This file processes the receiver inputs to ensure they are in the correct
 *  	ranges for further computations.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

#ifndef __PROCESS_COMMANDS_H__
#define __PROCESS_COMMANDS_H__

#include <stdint.h>
#include <stdbool.h>

#include "baredrones32.h"

/* Defines */
#define DEADBAND       24
#define DEADBAND_SLOPE (1000.0f / (1000 - DEADBAND))

/* Global Variables */
extern flightModes_e flightMode;

extern volatile bool armed;

extern float rcCommands[16];

void processRcCommands(void);
void processSerialCommands(void);

#endif /* __FLIGHT_COMMAND_H__ */
