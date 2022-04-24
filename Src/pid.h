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
#ifndef BAD_PID
typedef struct PIDdata {
  float   P, I, D, Limit;
  float   integratorState;
  float   filterState;
  uint8_t prevResetState;
} PIDdata_t;
#else
typedef struct fastPID{
	float error;
	float Kp;
	float Ki;
	float Kd;
	float integral;
	float derivative;
	float last_error;
	float pid_output;
	float integral_limit;
	float output_limit;
}fastPID;
#endif

/* Global Variables */
extern uint8_t pidReset;
#ifdef BAD_PID
extern fastPID attPIDdata;
extern fastPID ratePIDdata;
#endif

/* Function Prototypes */
void initPID(void);
#ifndef BAD_PID
float updatePID(float error, float deltaT, uint8_t reset, struct PIDdata *PIDparameters);
#else
float updatePID(struct fastPID *pidnow, float actual, float target);
#endif
void initPIDvalues(void);
void setPIDstates(uint8_t IDPid, float value);
void zeroPIDstates(void);

#endif /* __PID_H__ */
