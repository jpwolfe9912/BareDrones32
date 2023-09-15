/** @file 		drv_usart.c
 *  @brief
 *  	This file enables reading of usart data for use with a serial receiver.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Includes */
#include "board.h"

/**
 * @brief Calculate length of statically allocated array
 */
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

/* Global Variables */
volatile bool utx_finished = false;
Usart1Buffs_t Buffs;

/* Static Function Prototypes */
static void usart_rx_check(void);
static void usart_process_data(const void *data, size_t len);

/** @brief Initializes the low level registers for usart1.
 *
 *  @return Void.
 */
void usart1Init(void)
{
    printf("\nInitializing USART 1\n");
    /////////////////GPIO INIT///////////////////
    // enable clock for GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // set mode, speed, type, pull, AF
    GPIOB->MODER &= ~GPIO_MODER_MODER7;
    GPIOB->MODER |= GPIO_MODER_MODER7_1;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR7;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT7;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR7;
    GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL7;
    GPIOB->AFR[0] |= (0x7 << (4U * 7U));

    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART1_IRQn);

    /////////////////USART INIT///////////////////
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    USART1->CR1 &= ~USART_CR1_UE; // disable usart
    // USART1->BRR = 0x3AA;          // 115200 BR
    USART1->BRR = 0x1E;          // 400000 BR
    USART1->CR1 &= ~USART_CR1_M;  // 8 bit transfer
    USART1->CR2 &= ~USART_CR2_STOP;
    USART1->CR1 &= ~USART_CR1_PCE;
    USART1->CR1 |= USART_CR1_RE |
                   USART_CR1_TE;
    USART1->CR3 &= ~(USART_CR3_CTSE |
                     USART_CR3_RTSE);
    USART1->CR1 &= ~USART_CR1_OVER8;

    /////////////////DMA INIT///////////////////

    /* USART1 RX DMA Init */
    DMA2_Stream2->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream2->CR & DMA_SxCR_EN)
    {
    }
    DMA2_Stream2->CR &= ~DMA_SxCR_DIR;     // per to mem
    DMA2_Stream2->FCR &= ~DMA_SxFCR_DMDIS; // fifo dis
    DMA2_Stream2->CR &= ~DMA_SxCR_MBURST;
    DMA2_Stream2->CR &= ~DMA_SxCR_PBURST;
    DMA2_Stream2->PAR = (uint32_t)(&(USART1->RDR)); // set per address
    DMA2_Stream2->CR &= ~DMA_SxCR_PINC;             // don't inc per
    DMA2_Stream2->CR |= DMA_SxCR_MINC;              // increment mem
    DMA2_Stream2->CR &= ~DMA_SxCR_MSIZE;            // 8 bit size
    DMA2_Stream2->CR &= ~DMA_SxCR_PSIZE;            // 8 bit size
    DMA2_Stream2->CR |= DMA_SxCR_CIRC;              // circ mode en
    DMA2_Stream2->CR |= DMA_SxCR_PL;                // medium priority

    /* USART TX DMA Init */
    DMA2_Stream7->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream7->CR & DMA_SxCR_EN)
    {
    }
    DMA2_Stream7->CR |= DMA_SxCR_DIR;      // per to mem
    DMA2_Stream7->FCR &= ~DMA_SxFCR_DMDIS; // fifo dis
    DMA2_Stream7->CR &= ~DMA_SxCR_MBURST;
    DMA2_Stream7->CR &= ~DMA_SxCR_PBURST;
    DMA2_Stream7->PAR = (uint32_t)(&(USART1->TDR)); // set per address
    DMA2_Stream7->CR &= ~DMA_SxCR_PINC;             // don't inc per
    DMA2_Stream7->CR |= DMA_SxCR_MINC;              // increment mem
    DMA2_Stream7->CR &= ~DMA_SxCR_MSIZE;            // 8 bit size
    DMA2_Stream7->CR &= ~DMA_SxCR_PSIZE;            // 8 bit size
    DMA2_Stream7->CR &= ~DMA_SxCR_CIRC;             // normal mode en
    DMA2_Stream7->CR |= DMA_SxCR_PL;                // medium priority
}

/** @brief Reads in data form usart1 with DMA.
 *
 *  @param *pData A pointer to location where you want to read data to.
 *  @param size The amount of bytes to be read.
 *  @return Void.
 */
void usart1BeginRx(void)
{
    if (!(USART1->ISR & USART_ISR_BUSY))
    {                                     // wait for UART to be ready
        DMA2_Stream2->CR &= ~DMA_SxCR_EN; // disable DMA
        while (DMA2_Stream2->CR & DMA_SxCR_EN)
            ;
        DMA2_Stream2->CR |= (0x4 << 25U);                   // set DMA channel
        DMA2_Stream2->M0AR = (uint32_t)Buffs.RxBuffer_DMA;  // set memory address
        DMA2_Stream2->NDTR = ARRAY_LEN(Buffs.RxBuffer_DMA); // set transfer size

        DMA2->LIFCR |= (0x3F << 16U); // clear flags

        DMA2_Stream2->CR |= DMA_SxCR_TCIE; // set transfer complete interrupts
        DMA2_Stream2->CR |= DMA_SxCR_HTIE; // set transfer complete interrupts

        DMA2_Stream2->CR |= DMA_SxCR_EN; // enable DMA

        USART1->CR1 |= USART_CR1_IDLEIE;
        USART1->CR3 |= USART_CR3_DMAR; // enable DMA for UART
        USART1->CR1 |= USART_CR1_UE;   // enable usart
    }
}

/**
 * @brief           Send string to USART
 * @param[in]       str: String to send
 */
void usart1Tx(const char *str)
{
    size_t size = strlen(str);
    DMA2_Stream7->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream7->CR & DMA_SxCR_EN)
        ;
    DMA2_Stream7->NDTR = size;
    DMA2_Stream7->M0AR = (uint32_t)str;

    USART1->CR3 |= USART_CR3_DMAT;

    DMA2_Stream7->CR |= DMA_SxCR_EN;

    while (!utx_finished)
        ;
    utx_finished = false;
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
    pos = ARRAY_LEN(Buffs.RxBuffer_DMA) - DMA2_Stream2->NDTR; //LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);
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
            usart_process_data(&Buffs.RxBuffer_DMA[old_pos], pos - old_pos);
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
            usart_process_data(&Buffs.RxBuffer_DMA[old_pos], ARRAY_LEN(Buffs.RxBuffer_DMA) - old_pos);
            if (pos > 0)
            {
                usart_process_data(&Buffs.RxBuffer_DMA[0], pos);
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
static void
usart_process_data(const void *data, size_t len)
{
    /* Write data to buffer */
    lwrb_write(&Buffs.RxBuffer, data, len);
}

/* Interrupt Handlers */

/** @brief  USART1 global interrupt handler
 *
 *  @return Void.
 */
void USART1_IRQHandler(void)
{
    if ((USART1->CR1 & USART_CR1_IDLEIE) && (USART1->ISR & USART_ISR_IDLE))
    {
        USART1->ICR |= USART_ICR_IDLECF;
        usart_rx_check();
    }
    if (USART1->ISR & USART_ISR_ORE)
        USART1->ICR |= USART_ICR_ORECF; // clear overrun flag
}

/** @brief	DMA2_Stream2 global interrupt handler for USART1 RX
 *
 * 	@return Void.
 */
void DMA2_Stream2_IRQHandler(void)
{
    /* Check transfer complete interrupt */
    if ((DMA2->LISR & DMA_LISR_TCIF2) && (DMA2_Stream2->CR & DMA_SxCR_TCIE))
    {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF2; /* Clear half-transfer complete flag */
    }
    /* Check half-transfer complete interrupt */
    if ((DMA2->LISR & DMA_LISR_HTIF2) && (DMA2_Stream2->CR & DMA_SxCR_HTIE))
    {
        DMA2->LIFCR |= DMA_LIFCR_CHTIF2; /* Clear half-transfer complete flag */
    }
}

/** @brief	DMA2_Stream2 global interrupt handler for USART1 TX
 *
 * 	@return Void.
 */
void DMA2_Stream7_IRQHandler(void)
{
    /* Check half-transfer complete interrupt */
    if ((DMA2->LISR & DMA_LISR_TCIF2) && (DMA2_Stream2->CR & DMA_SxCR_TCIE))
    {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF2; /* Clear half-transfer complete flag */
        utx_finished = true;
    }
}