/** @file 		orientation.h
 *  @brief
 *  	This file contains a function to orient the board.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		06 MAR 2022
 */

#ifndef __ORIENTATION_H__
#define __ORIENTATION_H__

/* Global Variables */
extern float mpuOrientationMatrix[9];

/* Function Prototypes */
void orientSensors(void);

#endif /* __ORIENTATION_H__ */
