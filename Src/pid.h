/** @file 		pid.h
 *  @brief
 *  	This files initializes and updates the PID controller.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

#ifndef __PID_H__
#define __PID_H__

/* Global Structs */
typedef struct PIDdata {
  float   P, I, D, Limit;
  float   integratorState;
  float   filterState;
  uint8_t prevResetState;
} PIDdata_t;

/* Global Variables */
extern uint8_t pidReset;

/* Function Prototypes */
void initPID(void);
float updatePID(float error, float deltaT, uint8_t reset, struct PIDdata *PIDparameters);
void initPIDvalues(void);
void setPIDstates(uint8_t IDPid, float value);
void zeroPIDstates(void);

#endif /* __PID_H__ */
