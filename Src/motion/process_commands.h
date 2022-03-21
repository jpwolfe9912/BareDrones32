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

/* Defines */
#define DEADBAND       24
#define DEADBAND_SLOPE (1000/(1000-DEADBAND))

/* Global Variables */
extern float rxCommand[12];

extern uint8_t commandInDetent[3];
extern uint8_t previousCommandInDetent[3];

extern modes_e mode;

extern semaphore_t armed;
extern uint8_t     armingTimer;
extern uint8_t     disarmingTimer;

extern int16_t rxCommands[8];

void processCommands(void);

#endif /* __FLIGHT_COMMAND_H__ */
