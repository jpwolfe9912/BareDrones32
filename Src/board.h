/** @file 		board.h
 *  @brief		This file bundles all the header files. This allows us to
 *  			just include "board.h" in each file and have access to all
 *  			of the library's functions
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#ifndef __BOARD_H__
#define __BOARD_H__

/* C or System */
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"
#include "arm_math.h"

/* General */
#include "pid.h"
#include "Autodrone32.h"
#include "main.h"
#include "config.h"
#include "utilities.h"
#include "battery.h"
#include "logging.h"

/* Drivers */
#include "drv_led.h"
#include "drv_color.h"
#include "drv_system.h"
#include "drv_rcc.h"
#include "drv_spi.h"
#include "drv_usart.h"
#include "drv_serial.h"
#include "drv_adc.h"
#include "drv_dma.h"
#include "drv_tim.h"
#include "drv_lwrb.h"
#include "drv_worm.h"
#include "drv_dshot.h"
#include "drv_lager.h"

/* Motion */
#include "process_commands.h"
#include "compute_axis_commands.h"
#include "mixer.h"
#include "motors.h"
#include "rotations.h"
#include "transition.h"

/* Sensors */
#include "ibus.h"
#include "mpu6000.h"
#include "madgwick.h"
#include "orientation.h"
#include "mpu6000_calibration.h"
#include "accel_calibration_mpu.h"

#endif /* __BOARD_H__ */
