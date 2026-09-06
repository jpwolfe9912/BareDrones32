/** @file 		drv_usart.c
 *  @brief
 *  	This file enables reading of usart data for use with a serial receiver.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

 /* Includes */
#include "drv_usart2.h"

#include "stm32f7xx.h"
#include "feature_config.h"
#include "baredrones32.h"
#include "drv_printf.h"

/**
 * @brief Calculate length of statically allocated array
 */

 /* Global Variables */
volatile bool utx2_finished = false;
Usart2Buffs_t Buff_2;

// volatile uint8_t testbyte;
// volatile uint32_t rxCount = 0;

/* Static Function Prototypes */
static void usart_rx_check(void);
static void usart_process_data(const void* data, size_t len);

/** @brief Initializes the low level registers for usart2.
 *
 *  @return Void.
 */
void usart2Init(uint32_t baudrate)
{
    printf("\nInitializing USART 2\n");
    /* GPIO INIT */
    // enable clock for GPIOA PA3
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // set mode, speed, type, pull, AF
    GPIOA->MODER &= ~GPIO_MODER_MODER3;
    GPIOA->MODER |= GPIO_MODER_MODER3_1;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR3;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT3;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR3;
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL3;
    GPIOA->AFR[0] |= (0x7 << (4U * 3U));

    NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART2_IRQn);

    /* USART INIT */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    USART2->CR1 &= ~USART_CR1_UE; // disable usart
    // USART2->BRR = 0x1D5;          // 115200 BR
    USART2->BRR = 54000000 / baudrate;          // 420000 BR
    USART2->CR1 &= ~USART_CR1_M; // 8 bit transfer
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->CR1 &= ~USART_CR1_PCE;
    USART2->CR1 |= USART_CR1_RE |
        USART_CR1_TE;
    USART2->CR3 &= ~(USART_CR3_CTSE |
                     USART_CR3_RTSE);
    USART2->CR1 &= ~USART_CR1_OVER8;

    /* DMA INIT */

    /* USART2 RX DMA Init */
    DMA1_Stream5->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream5->CR & DMA_SxCR_EN)
    {
    }
    DMA1_Stream5->CR &= ~DMA_SxCR_DIR;     // per to mem
    DMA1_Stream5->FCR &= ~DMA_SxFCR_DMDIS; // fifo dis
    DMA1_Stream5->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream5->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream5->PAR = (uint32_t)(&(USART2->RDR)); // set per address
    DMA1_Stream5->CR &= ~DMA_SxCR_PINC;             // don't inc per
    DMA1_Stream5->CR |= DMA_SxCR_MINC;              // increment mem
    DMA1_Stream5->CR &= ~DMA_SxCR_MSIZE;            // 8 bit size
    DMA1_Stream5->CR &= ~DMA_SxCR_PSIZE;            // 8 bit size
    DMA1_Stream5->CR |= DMA_SxCR_CIRC;              // circ mode en
    DMA1_Stream5->CR |= DMA_SxCR_PL;                // medium priority

    /* USART TX DMA Init */
    DMA1_Stream6->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream6->CR & DMA_SxCR_EN)
    {
    }
    DMA1_Stream6->CR |= DMA_SxCR_DIR;      // per to mem
    DMA1_Stream6->FCR &= ~DMA_SxFCR_DMDIS; // fifo dis
    DMA1_Stream6->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream6->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream6->PAR = (uint32_t)(&(USART2->TDR)); // set per address
    DMA1_Stream6->CR &= ~DMA_SxCR_PINC;             // don't inc per
    DMA1_Stream6->CR |= DMA_SxCR_MINC;              // increment mem
    DMA1_Stream6->CR &= ~DMA_SxCR_MSIZE;            // 8 bit size
    DMA1_Stream6->CR &= ~DMA_SxCR_PSIZE;            // 8 bit size
    DMA1_Stream6->CR &= ~DMA_SxCR_CIRC;             // normal mode en
    DMA1_Stream6->CR |= DMA_SxCR_PL;                // medium priority
}

/** @brief Reads in data form usart2 with DMA.
 *
 *  @param *pData A pointer to location where you want to read data to.
 *  @param size The amount of bytes to be read.
 *  @return Void.
 */
void usart2BeginRx(void)
{
    if (!(USART2->ISR & USART_ISR_BUSY))
    {                                     // wait for UART to be ready
        DMA1_Stream5->CR &= ~DMA_SxCR_EN; // disable DMA
        while (DMA1_Stream5->CR & DMA_SxCR_EN)
            ;
        DMA1_Stream5->CR |= (0x4 << DMA_SxCR_CHSEL_Pos);                   // set DMA channel
        DMA1_Stream5->M0AR = (uint32_t)Buff_2.RxBuffer_DMA;  // set memory address
        DMA1_Stream5->NDTR = ARRAY_LEN(Buff_2.RxBuffer_DMA); // set transfer size

        DMA1->HIFCR |= (0x3F << 16U); // clear flags

        DMA1_Stream5->CR |= DMA_SxCR_TCIE; // set transfer complete interrupts
        DMA1_Stream5->CR |= DMA_SxCR_HTIE; // set transfer complete interrupts

        DMA1_Stream5->CR |= DMA_SxCR_EN; // enable DMA

        USART2->CR1 |= USART_CR1_IDLEIE;// | USART_CR1_RXNEIE;
        USART2->CR3 |= USART_CR3_DMAR; // enable DMA for UART
        USART2->CR1 |= USART_CR1_UE;   // enable usart
    }
}

/**
 * @brief           Send string to USART
 * @param[in]       str: String to send
 */
void usart2Tx(const char* str)
{
    size_t size = strlen(str);
    DMA1_Stream6->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream6->CR & DMA_SxCR_EN)
        ;
    DMA1_Stream6->NDTR = size;
    DMA1_Stream6->M0AR = (uint32_t)str;

    USART2->CR3 |= USART_CR3_DMAT;

    DMA1_Stream6->CR |= DMA_SxCR_EN;

    while (!utx2_finished)
        ;
    utx2_finished = false;
}

/* Static Functions */

/**
 * @brief           Check for new data received with DMA
 * @note
 * User must select context to call this function from:
 * - Only interrupts (DMA HT, DMA TC, UART IDLE) with same preemption priority level
 * - Only thread context (outside interrupts)
 *
 * If called from both context-es, exclusive access protection must be implemented
 * This mode is not advised as it usually means architecture design problems
 *
 * When IDLE interrupt is not present, application must rely only on thread context,
 * by manually calling function as quickly as possible, to make sure
 * data are read from raw buffer and processed.
 *
 * Not doing reads fast enough may cause DMA to overflow unread received bytes,
 * hence application will lost useful data.
 *
 * Solutions to this are:
 * - Improve architecture design to achieve faster reads
 * - Increase raw buffer size and allow DMA to write more data before this function is called
 */
static void
usart_rx_check(void)
{
    static size_t old_pos;
    size_t pos;

    /* Calculate current position in buffer and check for new data available */
    pos = ARRAY_LEN(Buff_2.RxBuffer_DMA) - DMA1_Stream5->NDTR; // LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);
    if (pos != old_pos)
    { /* Check change in received data */
        if (pos > old_pos)
        { /* Current position is over previous one */
            /*
             * Processing is done in "linear" mode.
             *
             * Application processing is fast with single data block,
             * length is simply calculated by subtracting pointers
             *
             * [   0   ]
             * [   1   ] <- old_pos |------------------------------------|
             * [   2   ]            |                                    |
             * [   3   ]            | Single block (len = pos - old_pos) |
             * [   4   ]            |                                    |
             * [   5   ]            |------------------------------------|
             * [   6   ] <- pos
             * [   7   ]
             * [ N - 1 ]
             */
            usart_process_data(&Buff_2.RxBuffer_DMA[old_pos], pos - old_pos);
        }
        else
        {
            /*
             * Processing is done in "overflow" mode..
             *
             * Application must process data twice,
             * since there are 2 linear memory blocks to handle
             *
             * [   0   ]            |---------------------------------|
             * [   1   ]            | Second block (len = pos)        |
             * [   2   ]            |---------------------------------|
             * [   3   ] <- pos
             * [   4   ] <- old_pos |---------------------------------|
             * [   5   ]            |                                 |
             * [   6   ]            | First block (len = N - old_pos) |
             * [   7   ]            |                                 |
             * [ N - 1 ]            |---------------------------------|
             */
            usart_process_data(&Buff_2.RxBuffer_DMA[old_pos], ARRAY_LEN(Buff_2.RxBuffer_DMA) - old_pos);
            if (pos > 0)
            {
                usart_process_data(&Buff_2.RxBuffer_DMA[0], pos);
            }
        }
        old_pos = pos; /* Save current position as old for next transfers */
    }
}

/**
 * @brief           Process received data over UART
 * @note            Either process them directly or copy to other bigger buffer
 * @param[in]       data: Data to process
 * @param[in]       len: Length in units of bytes
 */
volatile uint32_t rxOverflowCount = 0;
volatile uint32_t dmaBytesProcessed = 0;
volatile uint32_t ringBytesWritten = 0;

static void
usart_process_data(const void* data, size_t len)
{
    dmaBytesProcessed += len;

    size_t written = lwrb_write(&Buff_2.RxBuffer, data, len);
    ringBytesWritten += written;

    if (written != len)
        rxOverflowCount++;
}

/* Interrupt Handlers */

/** @brief  USART2 global interrupt handler
 *
 *  @return Void.
 */
void USART2_IRQHandler(void)
{
    if ((USART2->CR1 & USART_CR1_IDLEIE) && (USART2->ISR & USART_ISR_IDLE))
    {
        USART2->ICR |= USART_ICR_IDLECF;
        usart_rx_check();
    }
    if (USART2->ISR & USART_ISR_ORE)
        USART2->ICR |= USART_ICR_ORECF; // clear overrun flag
}

/** @brief	DMA1_Stream5 global interrupt handler for USART2 RX
 *
 * 	@return Void.
 */
void DMA1_Stream5_IRQHandler(void)
{
    /* Check transfer complete interrupt */
    if ((DMA1->HISR & DMA_HISR_TCIF5) && (DMA1_Stream5->CR & DMA_SxCR_TCIE))
    {
        DMA1->HIFCR |= DMA_HIFCR_CTCIF5; /* Clear half-transfer complete flag */
        usart_rx_check();
    }
    /* Check half-transfer complete interrupt */
    if ((DMA1->HISR & DMA_HISR_HTIF5) && (DMA1_Stream5->CR & DMA_SxCR_HTIE))
    {
        DMA1->HIFCR |= DMA_HIFCR_CHTIF5; /* Clear half-transfer complete flag */
        usart_rx_check();
    }
}

/** @brief	DMA1_Stream5 global interrupt handler for USART2 TX
 *
 * 	@return Void.
 */
void DMA1_Stream6_IRQHandler(void)
{
    /* Check half-transfer complete interrupt */
    if ((DMA1->HISR & DMA_HISR_TCIF6) && (DMA1_Stream6->CR & DMA_SxCR_TCIE))
    {
        DMA1->HIFCR |= DMA_HIFCR_CTCIF6; /* Clear half-transfer complete flag */
        utx2_finished = true;
    }
}