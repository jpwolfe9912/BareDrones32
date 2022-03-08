/** @file 		board.h
 *  @brief		This file bundles all the header files. This allows us to
 *  			just include "board.h" in each file and have access to all
 *  			of the library's functions
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 *  @bug
 */

#ifndef __BOARD_H__
#define __BOARD_H__

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

#include "pid.h"

#include "Autodrone32.h"
#include "main.h"
#include "config.h"
#include "utilities.h"
#include "mixer.h"

#include "drv_color.h"
#include "drv_system.h"
#include "drv_rcc.h"
#include "drv_spi.h"
#include "drv_usart.h"
#include "drv_serial.h"
#include "drv_adc.h"
#include "drv_dma.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "drv_tim.h"
#include "drv_usb_otg.h"

#include "drv_lwrb.h"
#include "drv_dshot.h"
#include "drv_ibus.h"

#include "process_commands.h"
#include "compute_axis_commands.h"
#include "motors.h"

#include "mpu6000.h"
#include "orientation.h"
#include "sensor_fusion.h"
#include "mpu6000_calibration.h"
#include "accel_calibration_mpu.h"

#endif /* __BOARD_H__ */
