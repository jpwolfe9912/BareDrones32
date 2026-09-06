/** @file 		feature_config.h
 *  @brief
 *  	This file runs all the other functions and carries out the scheduling of tasks.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#ifndef __FEATURE_CONFIG_H__
#define __FEATURE_CONFIG_H__


/* Defines */
#define STLINK
// #define SANDBOX
#define OPENLAGER
#define USE_BAREDRONES
// #define USE_NUCLEO
#define USE_EEPROM

/* Features */
// #define USE_SPI1_DRIVER
// #define USE_USART1_DRIVER
// #define USE_USART2_DRIVER
// #define USE_USART6_DRIVER

// #define USE_MOTION_PROCESSING
// #define USE_MPU6000
#define USE_DSHOT
// #define USE_IBUS 115200U
// #define USE_CRSF 416666U
// #define USE_LOGGING
// #define USE_BATT_MON
// #define USE_LEDS

#endif /* __FEATURE_CONFIG_H__ */