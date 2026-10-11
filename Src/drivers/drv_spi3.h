/** @file 		drv_spi.h
 *  @brief
 *  	This file initializes the SPI peripheral as well as writes
 *  	and reads to another device over SPI
 *
 *  Still needs to implement SPI2 for OSD chip
 *  Find a way to reuse write and read functions for SPI3 and SPI2
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#ifndef __DRV_SPI3_H__
#define __DRV_SPI3_H__

#include <stdint.h>
#include <stdbool.h>

#include "stm32f7xx.h"

 /* Defines */
#define SPI3_ENABLE					GPIOC->BSRR |= GPIO_BSRR_BR4
#define SPI3_DISABLE				GPIOC->BSRR |= GPIO_BSRR_BS4

#define SPI_BR_PRESCALER_8			(0x2 << 3U)
#define SPI_BR_PRESCALER_16			(0x3 << 3U)
#define SPI_BR_PRESCALER_128		(0x6 << 3U)

#define SPI3_MAX_TRANSFER 1024U

/* Prototypes */
void spi3Init(void);
void spi3ReadBytes(uint8_t reg, uint8_t* pData, uint16_t size);
void spi3WriteBytes(uint8_t* pData, uint16_t size);
void spi3ReadOneByte(uint8_t reg, uint8_t* pData);
void spi3WriteOneByte(uint8_t reg, uint8_t data);

#endif /* __DRV_SPI3_H__ */
