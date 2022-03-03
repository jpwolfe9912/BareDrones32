/*
 * motors.h
 *
 *  Created on: Feb 23, 2022
 *      Author: jeremywolfe
 */

#ifndef SRC_MOTORS_H_
#define SRC_MOTORS_H_

void motorInit(void);

extern uint16_t motor_value[4];

extern semaphore_t motor_initialized;

#endif /* SRC_MOTORS_H_ */
