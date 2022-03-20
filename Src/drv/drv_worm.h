/** @file 		drv_worm.h
 *  @brief
 *  	This file initializes the timer and GPIO interrupt for the arm rotation.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		17 MAR 2022
 */

#ifndef SRC_DRV_DRV_WORM_H_
#define SRC_DRV_DRV_WORM_H_

/* Global Variables */
extern bool arm1Hit;
extern bool arm2Hit;

/* Function Prototypes */
void wormInit(void);
void wormDrive(void);

#endif /* SRC_DRV_DRV_WORM_H_ */
