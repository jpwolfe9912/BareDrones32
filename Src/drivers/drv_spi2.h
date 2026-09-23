/** @file 		drv_spi.h
 *  @brief
 *  	This file initializes the SPI peripheral as well as writes
 *  	and reads to another device over SPI
 *
 *  Still needs to implement SPI2 for OSD chip
 *  Find a way to reuse write and read functions for SPI1 and SPI2
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#ifndef __DRV_SPI2_H__
#define __DRV_SPI2_H__

#include <stdint.h>
#include <stdbool.h>

#include "stm32f7xx.h"

 /* Defines */
#define SPI_BR_PRESCALER_2		   ~(0x7 << 3U)     // need to &= to clear bits
#define SPI_BR_PRESCALER_4			(0x1 << 3U)
#define SPI_BR_PRESCALER_8			(0x2 << 3U)
#define SPI_BR_PRESCALER_16			(0x3 << 3U)
#define SPI_BR_PRESCALER_32			(0x4 << 3U)
#define SPI_BR_PRESCALER_64			(0x5 << 3U)
#define SPI_BR_PRESCALER_128		(0x6 << 3U)
#define SPI_BR_PRESCALER_256		(0x7 << 3U)

#define FLASH_CS_LOW                GPIOB->BSRR |= GPIO_BSRR_BR12
#define FLASH_CS_HIGH               GPIOB->BSRR |= GPIO_BSRR_BS12

/* Prototypes */
void spi2Init(void);
void spi2WriteBytesDMA(uint8_t* data, uint16_t length);
bool spi2DMAWriteComplete(void);
void spi2EndDMAWrite(void);
void spi2WriteBytes(uint8_t* data, uint16_t length);
void spi2ReadBytes(uint8_t* data, uint16_t length);
uint8_t spi2TransferByte(uint8_t data);

#endif /* __DRV_SPI2_H__ */
