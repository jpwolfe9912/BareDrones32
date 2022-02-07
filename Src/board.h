/*
 * board.h
 *
 *  Created on: Dec 24, 2021
 *      Author: jeremywolfe
 */

#ifndef INC_BOARD_H_
#define INC_BOARD_H_

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

#include "Autodrone32.h"
#include "main.h"

#include "drv_spi.h"
#include "drv_system.h"
#include "drv_usart.h"
#include "drv_adc.h"
#include "drv_dma.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "drv_tim.h"
#include "drv_usb_otg.h"
#include "drv_lwrb.h"
#include "drv_dshot.h"
#include "drv_ibus.h"

#include "flightCommand.h"
#include "pid.h"

#include "mpu6000.h"
#include "orientation.h"

#endif /* INC_BOARD_H_ */
