/** @file 		drv_spi.c
 *  @brief
 *  	This file initializes the SPI peripheral as well as writes
 *  	and reads to another device over SPI
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

 /* Includes */
#include "drv_spi3.h"

#include "drv_usart3.h"

static volatile bool spi3_transfer_complete = false;

static void spi3_transfer(const uint8_t* tx, uint8_t* rx, uint16_t size);
static void spi3_transfer_complete_callback(void);

/** @brief Initializes SPI3.
 *
 *  @return Void.
 */
void spi3Init(void)
{
    printf("[INIT] SPI3................ ");

    /* GPIO INIT */
    // enable clock for GPIOC/B
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // set mode, speed, type, pull, AF
    /* CS Pin PC4*/
    GPIOC->BSRR |= GPIO_BSRR_BS4;
    GPIOC->MODER &= ~GPIO_MODER_MODER4;
    GPIOC->MODER |= GPIO_MODER_MODER4_0; // Output mode
    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR4;
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT4;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR4_0; // pull up
    GPIOC->AFR[0] &= ~GPIO_AFRL_AFRL4;

    /* SCK Pin PC10*/
    GPIOC->MODER &= ~GPIO_MODER_MODER10;
    GPIOC->MODER |= GPIO_MODER_MODER10_1; // AF Mode
    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR10;
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT10;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR10_1; // pull up
    GPIOC->AFR[1] &= ~GPIO_AFRH_AFRH2;
    GPIOC->AFR[1] |= (0x6 << (4U * 2U)); // AF 6

    /* MISO Pin PC11*/
    GPIOC->MODER &= ~GPIO_MODER_MODER11;
    GPIOC->MODER |= GPIO_MODER_MODER11_1; // AF mode
    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR11;
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT11;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR11_0; // pull up
    GPIOC->AFR[1] &= ~GPIO_AFRH_AFRH3;
    GPIOC->AFR[1] |= (0x6 << (4U * 3U)); // AF 6

    /* MOSI Pin PB2*/
    GPIOB->MODER &= ~GPIO_MODER_MODER2;
    GPIOB->MODER |= GPIO_MODER_MODER2_1; // AF mode
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR2;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT2;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR2_1; // pull up
    GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL2;
    GPIOB->AFR[0] |= (0x7 << (4U * 2U)); // AF 7

    /* SPI INIT */
    // enable clock for SPI3
    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

    SPI3->CR1 &= ~SPI_CR1_SPE;

    /* SPI mode 0, master, 8-bit, software NSS */
    SPI3->CR1 = SPI_CR1_MSTR |
        SPI_CR1_SSM |
        SPI_CR1_SSI |
        SPI_BR_PRESCALER_128;

    SPI3->CR2 = (7U << SPI_CR2_DS_Pos) |
        SPI_CR2_FRXTH |
        SPI_CR2_RXDMAEN |
        SPI_CR2_TXDMAEN;

    /////////////////DMA INIT///////////////////
    /*
     * SPI3_RX on DMA1_Stream0_CH0
     * SPI3_TX on DMA1_Stream7_CH0
     */

     // disable DMA stream 0
    DMA1_Stream0->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream0->CR & DMA_SxCR_EN)
        ;
    DMA1_Stream0->CR = 0;
    DMA1_Stream0->NDTR = 0;
    DMA1_Stream0->PAR = 0;
    DMA1_Stream0->M0AR = 0;
    DMA1_Stream0->M1AR = 0;
    DMA1_Stream0->FCR = 0x00000021U;
    DMA1_Stream0->CR &= ~DMA_SxCR_CHSEL;
    DMA1->LIFCR = DMA_LIFCR_CFEIF0 |
        DMA_LIFCR_CDMEIF0 |
        DMA_LIFCR_CTEIF0 |
        DMA_LIFCR_CHTIF0 |
        DMA_LIFCR_CTCIF0;
    // disable DMA stream 7
    DMA1_Stream7->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream7->CR & DMA_SxCR_EN)
        ;
    DMA1_Stream7->CR = 0;
    DMA1_Stream7->NDTR = 0;
    DMA1_Stream7->PAR = 0;
    DMA1_Stream7->M0AR = 0;
    DMA1_Stream7->M1AR = 0;
    DMA1_Stream7->FCR = 0x00000021U;
    DMA1_Stream7->CR &= ~DMA_SxCR_CHSEL;
    DMA1->HIFCR = DMA_HIFCR_CFEIF7 |
        DMA_HIFCR_CDMEIF7 |
        DMA_HIFCR_CTEIF7 |
        DMA_HIFCR_CHTIF7 |
        DMA_HIFCR_CTCIF7;

    //* RX DMA settings
    DMA1_Stream0->CR &= ~DMA_SxCR_CHSEL_Msk;
    DMA1_Stream0->M0AR = 0;
    DMA1_Stream0->CR &= ~DMA_SxCR_DIR;     // per to mem
    DMA1_Stream0->FCR &= ~DMA_SxFCR_DMDIS; // fifo dis
    DMA1_Stream0->FCR &= ~DMA_SxFCR_FTH;   // 1/4 full
    DMA1_Stream0->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream0->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream0->PAR = (uint32_t)(&(SPI3->DR));
    DMA1_Stream0->NDTR = 0;
    DMA1_Stream0->CR &= ~DMA_SxCR_PINC;
    DMA1_Stream0->CR |= DMA_SxCR_MINC;
    DMA1_Stream0->CR &= ~DMA_SxCR_MSIZE;
    DMA1_Stream0->CR &= ~DMA_SxCR_PSIZE;
    DMA1_Stream0->CR &= ~DMA_SxCR_CIRC;
    DMA1_Stream0->CR |= DMA_SxCR_PL;
    // DMA transfer complete interrupt enable
    DMA1_Stream0->CR |= DMA_SxCR_TCIE;

    //* TX DMA settings
    DMA1_Stream7->CR &= ~DMA_SxCR_CHSEL_Msk;
    DMA1_Stream7->M0AR = 0;
    DMA1_Stream7->CR |= DMA_SxCR_DIR_0;    // mem to per
    DMA1_Stream7->FCR &= ~DMA_SxFCR_DMDIS; // fifo dis
    DMA1_Stream7->FCR &= ~DMA_SxFCR_FTH;   // 1/4 full
    DMA1_Stream7->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream7->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream7->PAR = (uint32_t)(&(SPI3->DR));
    DMA1_Stream7->NDTR = 0;
    DMA1_Stream7->CR &= ~DMA_SxCR_PINC;
    DMA1_Stream7->CR |= DMA_SxCR_MINC;
    DMA1_Stream7->CR &= ~DMA_SxCR_MSIZE;
    DMA1_Stream7->CR &= ~DMA_SxCR_PSIZE;
    DMA1_Stream7->CR &= ~DMA_SxCR_CIRC;
    DMA1_Stream7->CR |= DMA_SxCR_PL;
    // DMA transfer complete interrupt enable
    DMA1_Stream7->CR |= DMA_SxCR_TCIE;

    printf("OK\n");
}

/** @brief Reads multiple bytes of data in.
 *
 *  @param reg The hex value of the first register to read from.
 *  @param *pData A pointer to location where you want to read data to.
 *  @param size The amount of bytes to be read.
 *  @return Void.
 */

void spi3ReadBytes(uint8_t reg, uint8_t* pData, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        spi3ReadOneByte((uint8_t)(reg + i), &pData[i]);
    }
}

/** @brief Writes multiple bytes of data in.
 *
 *  @param reg The hex value of the register to write to.
 *  @param pData A pointer to the data you want to write.
 *  @param size The amount of bytes to write.
 *  @return Void.
 */
void spi3WriteBytes(uint8_t* pData, uint16_t size)
{
    if (size == 0)
        return;

    if (size > SPI3_MAX_TRANSFER)
        return;

    uint8_t rx[SPI3_MAX_TRANSFER] = { 0 };

    spi3_transfer(pData, rx, size);
}

/** @brief Reads one byte of data in.
 *
 *  @param reg The hex value of the register to read from.
 *  @param *pData A pointer to location where you want to read data to.
 *  @return Void.
 */
void spi3ReadOneByte(uint8_t reg, uint8_t* pData)
{
    uint8_t tx[2] = { (uint8_t)(reg | 0x80), 0x00 };
    uint8_t rx[2] = { 0 };

    spi3_transfer(tx, rx, 2);

    *pData = rx[1];
}

/** @brief Writes one byte of data.
 *
 *  @param reg The hex value of the register to write to.
 *  @param data The data to write.
 *  @return Void.
 */
void spi3WriteOneByte(uint8_t reg, uint8_t data)
{
    uint8_t tx[2] = { reg, data };
    uint8_t rx[2] = { 0 };

    spi3_transfer(tx, rx, 2);
}

static void
spi3_transfer(const uint8_t* tx, uint8_t* rx, uint16_t size)
{
    if (size == 0)
        return;

    spi3_transfer_complete = false;

    /* Disable DMA streams */
    DMA1_Stream0->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream0->CR & DMA_SxCR_EN)
        ;

    DMA1_Stream7->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream7->CR & DMA_SxCR_EN)
        ;

    /* Clear pending DMA flags */
    DMA1->LIFCR = DMA_LIFCR_CFEIF0 |
        DMA_LIFCR_CDMEIF0 |
        DMA_LIFCR_CTEIF0 |
        DMA_LIFCR_CHTIF0 |
        DMA_LIFCR_CTCIF0;

    DMA1->HIFCR = DMA_HIFCR_CFEIF7 |
        DMA_HIFCR_CDMEIF7 |
        DMA_HIFCR_CTEIF7 |
        DMA_HIFCR_CHTIF7 |
        DMA_HIFCR_CTCIF7;

    /* Configure DMA */
    DMA1_Stream0->NDTR = size;
    DMA1_Stream0->M0AR = (uint32_t)rx;

    DMA1_Stream7->NDTR = size;
    DMA1_Stream7->M0AR = (uint32_t)tx;

    /* Assert chip select */
    SPI3_ENABLE;

    /* Enable SPI */
    SPI3->CR1 |= SPI_CR1_SPE;

    /* Start RX before TX */
    DMA1_Stream0->CR |= DMA_SxCR_EN;
    DMA1_Stream7->CR |= DMA_SxCR_EN;

    spi3_transfer_complete_callback();

    /* Release chip select */
    SPI3_DISABLE;
}

static void
spi3_transfer_complete_callback(void)
{
    while (!spi3_transfer_complete)
        ;
    while (SPI3->SR & SPI_SR_BSY)
        ;
    SPI3->CR1 &= ~SPI_CR1_SPE;
    spi3_transfer_complete = false;
}

/* Interrupt Handlers */

/**
 * @brief This function handles DMA1 stream1 global interrupt.
 */
void DMA1_Stream0_IRQHandler(void)
{
    if (DMA1->LISR & DMA_LISR_TCIF0)
    {
        DMA1->LIFCR |= DMA_LIFCR_CTCIF0;
        spi3_transfer_complete = true;
    }
}

/**
 * @brief This function handles DMA1 stream3 global interrupt.
 */
void DMA1_Stream7_IRQHandler(void)
{
    if (DMA1->HISR & DMA_HISR_TCIF7)
    {
        DMA1->HIFCR |= DMA_HIFCR_CTCIF7;
    }
}