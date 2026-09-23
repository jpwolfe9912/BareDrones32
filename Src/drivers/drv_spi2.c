/** @file 		drv_spi.c
 *  @brief
 *  	This file initializes the SPI peripheral as well as writes
 *  	and reads to another device over SPI
 *
 *  Still needs to implement SPI2 for OSD chip
 *  Find a way to reuse write and read functions for SPI2 and SPI2
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

 /* Includes */
#include "drv_spi2.h"

#include "drv_usart3.h"

static volatile bool spi2_transfer_complete = false;

static void spi2_transfer_complete_callback(void);
static void spi2_clear_rx(void);

/** @brief Initializes SPI2.
 *
 *  @return Void.
 */
void spi2Init(void)
{
    printf("\nInitializing SPI 2\n");

    /* GPIO INIT */
    // enable clock for GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // set mode, speed, type, pull, AF
    /* CS Pin PB12 */
    GPIOB->BSRR |= GPIO_BSRR_BS12;
    GPIOB->MODER &= ~GPIO_MODER_MODER12;
    GPIOB->MODER |= GPIO_MODER_MODER12_0; // output mode
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR12;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT12;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR12_0; // no pull
    GPIOB->AFR[1] &= ~GPIO_AFRH_AFRH4;
    // GPIOB->AFR[1] |= (0x5 << GPIO_AFRH_AFRH4_Pos); // AF 5

    /* SCK Pin PB13 */
    GPIOB->MODER &= ~GPIO_MODER_MODER13;
    GPIOB->MODER |= GPIO_MODER_MODER13_1; // AF Mode
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR13;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT13;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR13_0; // no pull
    GPIOB->AFR[1] &= ~GPIO_AFRH_AFRH5;
    GPIOB->AFR[1] |= (0x5 << GPIO_AFRH_AFRH5_Pos); // AF 5

    /* MISO Pin PB14 */
    GPIOB->MODER &= ~GPIO_MODER_MODER14;
    GPIOB->MODER |= GPIO_MODER_MODER14_1; // AF mode
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR14;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT14;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR14_0; // no pull
    GPIOB->AFR[1] &= ~GPIO_AFRH_AFRH6;
    GPIOB->AFR[1] |= (0x5 << GPIO_AFRH_AFRH6_Pos); // AF 5

    /* MOSI Pin PB15 */
    GPIOB->MODER &= ~GPIO_MODER_MODER15;
    GPIOB->MODER |= GPIO_MODER_MODER15_1; // AF mode
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR15;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT15;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR15_0; // no pull
    GPIOB->AFR[1] &= ~GPIO_AFRH_AFRH7;
    GPIOB->AFR[1] |= (0x5 << GPIO_AFRH_AFRH7_Pos); // AF 5

    /* SPI INIT */
    // enable clock for SPI2
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    SPI2->CR1 &= ~SPI_CR1_SPE;      // disable SPI
    SPI2->CR1 &= ~SPI_CR1_BIDIMODE; // rx and tx
    SPI2->CR1 |= SPI_CR1_MSTR;      // SPI master
    SPI2->CR2 |= (0x7 << 8U);       // byte transfer size
    SPI2->CR1 &= ~SPI_CR1_CPHA;      // low polarity
    SPI2->CR1 &= ~SPI_CR1_CPOL;      // rising edge
    SPI2->CR1 |= SPI_CR1_SSM;      // software slave management
    SPI2->CR1 |= SPI_CR1_SSI;
    SPI2->CR2 &= ~SPI_CR2_SSOE;
    SPI2->CR1 &= SPI_BR_PRESCALER_2; // BR < 1MHz for init
    SPI2->CR1 &= ~SPI_CR1_LSBFIRST;    // MSB first
    SPI2->CR1 &= ~SPI_CR1_CRCEN;       // disable CRC
    SPI2->CR2 |= SPI_CR2_TXDMAEN;
    SPI2->CR2 |= SPI_CR2_FRXTH;
    SPI2->CR1 |= SPI_CR1_SPE;

    /////////////////DMA INIT///////////////////
    /*
     * SPI2_TX on DMA1 Stream4 CH0
     * SPI2_RX unused due to conflict with USART3_TX needed for logging
     */

     // disable DMA1 stream 4
    DMA1_Stream4->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream4->CR & DMA_SxCR_EN)
        ;
    DMA1_Stream4->CR = 0;
    DMA1_Stream4->NDTR = 0;
    DMA1_Stream4->PAR = 0;
    DMA1_Stream4->M0AR = 0;
    DMA1_Stream4->M1AR = 0;
    DMA1_Stream4->FCR = 0x00000021U;
    DMA1_Stream4->CR &= ~DMA_SxCR_CHSEL;
    DMA1->LIFCR |= 0x0000003FU;

    // TX DMA settings
    DMA1_Stream4->CR &= ~DMA_SxCR_CHSEL_Msk;
    DMA1_Stream4->M0AR = 0;
    DMA1_Stream4->CR |= DMA_SxCR_DIR_0;    // mem to per
    DMA1_Stream4->FCR &= ~DMA_SxFCR_DMDIS; // fifo dis
    DMA1_Stream4->FCR &= ~DMA_SxFCR_FTH;   // 1/4 full
    DMA1_Stream4->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream4->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream4->PAR = (uint32_t)(&(SPI2->DR));
    DMA1_Stream4->NDTR = 0;
    DMA1_Stream4->CR &= ~DMA_SxCR_PINC;
    DMA1_Stream4->CR |= DMA_SxCR_MINC;
    DMA1_Stream4->CR &= ~DMA_SxCR_MSIZE;
    DMA1_Stream4->CR &= ~DMA_SxCR_PSIZE;
    DMA1_Stream4->CR &= ~DMA_SxCR_CIRC;
    DMA1_Stream4->CR |= DMA_SxCR_PL;
    // DMA transfer complete interrupt enable
    DMA1_Stream4->CR |= DMA_SxCR_TCIE;
}

/** @brief Writes multiple bytes of data with DMA.
 *
 *  @param data A pointer to the data you want to write.
 *  @param length The amount of bytes to write.
 *  @return Void.
 */
void
spi2WriteBytesDMA(uint8_t* data, uint16_t length)
{
    spi2_transfer_complete = false;
    // TX Setup
    DMA1_Stream4->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream4->CR & DMA_SxCR_EN)
        ;

    DMA1->HIFCR = DMA_HIFCR_CFEIF4 |
        DMA_HIFCR_CDMEIF4 |
        DMA_HIFCR_CTEIF4 |
        DMA_HIFCR_CHTIF4 |
        DMA_HIFCR_CTCIF4;

    DMA1_Stream4->CR &= ~DMA_SxCR_CHSEL_Msk;
    DMA1_Stream4->NDTR = length;
    DMA1_Stream4->M0AR = (uint32_t)data;

    DMA1_Stream4->CR |= DMA_SxCR_EN;
}

bool
spi2DMAWriteComplete(void)
{
    return spi2_transfer_complete;
}

static void
spi2_clear_rx(void)
{
    volatile uint8_t dummy;

    while (SPI2->SR & SPI_SR_RXNE)
    {
        dummy = *((volatile uint8_t*)&SPI2->DR);
    }

    dummy = *((volatile uint8_t*)&SPI2->DR);
    dummy = (uint8_t)SPI2->SR;

    (void)dummy;
}

void
spi2EndDMAWrite(void)
{
    while (SPI2->SR & SPI_SR_BSY)
        ;

    spi2_clear_rx();
}

/** @brief Writes multiple bytes of data with polling.
 *
 *  @param data A pointer to the data you want to write.
 *  @param length The amount of bytes to write.
 *  @return Void.
 */
void
spi2WriteBytes(uint8_t* data, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
        spi2TransferByte(data[i]);
}

/** @brief Reads multiple bytes of data with polling.
 *
 *  @param data A pointer to the variable you want to read data into.
 *  @param length The amount of bytes to write.
 *  @return Void.
 */
void
spi2ReadBytes(uint8_t* data, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        data[i] = spi2TransferByte(0x00);
    }
}

/** @brief Transfers one byte of data with polling.
 *
 *  @param data The data to send.
 *  @return uint8_t The data received.
 */
uint8_t
spi2TransferByte(uint8_t data)
{
    while (!(SPI2->SR & SPI_SR_TXE))            // wait for transmitting to be done
        ;
    *((volatile uint8_t*)&SPI2->DR) = data;   // send one byte

    while (!(SPI2->SR & SPI_SR_RXNE))           // wait for something to appear in the receive register
        ;
    return *((volatile uint8_t*)&SPI2->DR);     // read the byte available
}

/* Interrupt Handlers */

/**
 * @brief This function handles DMA2 stream1 global interrupt.
 */
void DMA1_Stream4_IRQHandler(void)
{
    if (DMA1->HISR & DMA_HISR_TCIF4)
    {
        DMA1->HIFCR |= DMA_HIFCR_CTCIF4;
        spi2_transfer_complete = true;
    }
}