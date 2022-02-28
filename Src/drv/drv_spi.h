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
 *  @bug
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Defines */
#define SPI1_ENABLE					GPIOA->BSRR |= GPIO_BSRR_BR4
#define SPI1_DISABLE				GPIOA->BSRR |= GPIO_BSRR_BS4

#define SPI_BR_PRESCALER_16			(0x3 << 3U)
#define SPI_BR_PRESCALER_128		(0x6 << 3U)

/* Prototypes */
void spi1Init(void);
void spiReadBytes(uint8_t reg, uint8_t *pData, uint8_t size);
void spiWriteBytes(uint8_t *pData, uint8_t size);
void spiReadOneByte(uint8_t reg, uint8_t *pData);
void spiWriteOneByte(uint8_t reg, uint8_t data);

void DMA2_Stream0_IRQHandler(void);
void DMA2_Stream3_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_SPI_H__ */
