/** @file 		drv_usart3.c
 *  @brief
 *  	This file enables reading of usart data
 *  	for use with usart3.
 *
 *  Based on which device you are using (Nucleo or Autodrone PCB),
 *  enable the corresponding Include in the preprocessor
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

 /* Includes */
#include "drv_usart3.h"

#include "stm32f7xx.h"
#include "feature_config.h"
#include "baredrones32.h"

/* Global Variables */
Usart3Buffs_t Buff_3;

uint8_t temp;
uint8_t usart3Buf[100];
volatile bool endOfString;
uint8_t usart3Index = 0;

/* Static Variables*/
static volatile bool utx3_finished = true;

static volatile uint32_t rxOverflowCount = 0;
static volatile uint32_t dmaBytesProcessed = 0;
static volatile uint32_t ringBytesWritten = 0;

/* Static Function Prototypes */
static usart3TxCallback_t usart3TxCb = NULL;
static void usart3_begin_rx(void);
static void usart_rx_check(void);
static void usart_process_data(const void* data, size_t len);


/** @brief Initializes the low level uart registers in order to use usart3
 *
 *  @return Void.
 */
#ifdef USE_NUCLEO
void usart3Init(uint32_t baudrate)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  // enable the clock for port D
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // enable the clock for UART3

    GPIOD->AFR[1] |= (0x7 << (4 * 0U)); // set pin D8 as alternate function
    GPIOD->AFR[1] |= (0x7 << (4 * 1U)); // set pin D9 as alternate function

    GPIOD->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
    GPIOD->MODER |= (GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1); // set PD8,9 as alternate function

    NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART3_IRQn);

    USART3->BRR = 54000000 / baudrate;                                       // set baud rate to 115200
    USART3->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; // enable the receiver, transmitter, and USART
}
#endif

#ifdef USE_BAREDRONES

void usart3Init(uint32_t baudrate)
{
    /* TX3 : PB10 : AF7 : DMA1 Stream3 Ch4
       RX3 : PB11 : AF7 : DMA1 Stream1 Ch4 */
    printf("\nInitializing USART 3\n");

    /* GPIO INIT */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // enable the clock for port B
    // set mode, speed, type, pull, AF
    GPIOB->MODER &= ~(GPIO_MODER_MODER10 |
                      GPIO_MODER_MODER11);
    GPIOB->MODER |= (GPIO_MODER_MODER10_1 |
                     GPIO_MODER_MODER11_1); // set PB10/11 as alternate function
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR10 |
                       GPIO_OSPEEDR_OSPEEDR11);
    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFRH2 |
                       GPIO_AFRH_AFRH3);
    GPIOB->AFR[1] |= (0x7 << (2 * 4U)) |
        (0x7 << (3 * 4U)); // set PB10/11 to AF7

    /* USART INIT */
    NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART3_IRQn);

    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // enable the clock for USART3

    USART3->CR1 &= ~USART_CR1_UE;
    USART3->BRR = 54000000 / baudrate;
    USART3->CR2 |= USART_CR2_SWAP;                    //!! swap TX and RX pins because I'm stupid
    USART3->CR1 &= ~USART_CR1_M;  // 8 bit transfer
    USART3->CR2 &= ~USART_CR2_STOP;
    USART3->CR1 &= ~USART_CR1_PCE;
    USART3->CR1 |= (USART_CR1_TE |
                    USART_CR1_RE);
    USART3->CR3 &= ~(USART_CR3_CTSE |
                     USART_CR3_RTSE);
    USART3->CR1 &= ~USART_CR1_OVER8;
    USART3->CR1 |= USART_CR1_UE; // enable usart

    /* DMA INIT */
    /* USART3 RX DMA Init */
    DMA1_Stream1->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream1->CR & DMA_SxCR_EN)
    {
    }
    DMA1_Stream1->CR |= (0x4 << DMA_SxCR_CHSEL_Pos);       // set DMA channel ch 4
    DMA1_Stream1->CR &= ~DMA_SxCR_DIR;    // per to mem
    DMA1_Stream1->FCR &= ~DMA_SxFCR_DMDIS; // fifo dis
    DMA1_Stream1->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream1->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream1->PAR = (uint32_t)(&(USART3->RDR)); // set per address
    DMA1_Stream1->CR &= ~DMA_SxCR_PINC;             // don't inc per
    DMA1_Stream1->CR |= DMA_SxCR_MINC;              // increment mem
    DMA1_Stream1->CR &= ~DMA_SxCR_MSIZE;            // 8 bit size
    DMA1_Stream1->CR &= ~DMA_SxCR_PSIZE;            // 8 bit size
    DMA1_Stream1->CR |= DMA_SxCR_CIRC;             // circ mode dis
    DMA1_Stream1->CR |= DMA_SxCR_PL;                // medium priority

    /* USART3 TX DMA Init */
    DMA1_Stream3->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream3->CR & DMA_SxCR_EN)
    {
    }
    DMA1_Stream3->CR |= (0x4 << DMA_SxCR_CHSEL_Pos);               // set DMA channel ch 4
    DMA1_Stream3->CR |= DMA_SxCR_DIR_0;             // mem to per
    DMA1_Stream3->FCR &= ~DMA_SxFCR_DMDIS;          // fifo dis
    DMA1_Stream3->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream3->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream3->PAR = (uint32_t)(&(USART3->TDR)); // set per address
    DMA1_Stream3->CR &= ~DMA_SxCR_PINC;             // don't inc per
    DMA1_Stream3->CR |= DMA_SxCR_MINC;              // increment mem
    DMA1_Stream3->CR &= ~DMA_SxCR_MSIZE;            // 8 bit size
    DMA1_Stream3->CR &= ~DMA_SxCR_PSIZE;            // 8 bit size
    DMA1_Stream3->CR &= ~DMA_SxCR_CIRC;             // circ mode dis
    DMA1_Stream3->CR |= DMA_SxCR_PL;                // medium priority
    DMA1_Stream3->CR |= DMA_SxCR_TCIE;              // set transfer complete interrupts

    /* Begin Data Processing */
    lwrb_init(&Buff_3.RxBuffer, (void*)Buff_3.RxBuffer_Data, sizeof(Buff_3.RxBuffer_Data));

    usart3_begin_rx();
}
#endif

/** @brief Registers a callback function for USART3 transmission completion.
 *
 *  @param cb The callback function to register.
 *  @return Void.
 */
void
usart3RegisterCallback(usart3TxCallback_t cb)
{
    usart3TxCb = cb;
}

/** @brief Reads in data form usart3 with DMA.
 *
 *  @param *pData A pointer to location where you want to read data to.
 *  @param size The amount of bytes to be read.
 *  @return Void.
 */
static void
usart3_begin_rx(void)
{
    if (!(USART3->ISR & USART_ISR_BUSY))
    {                                     // wait for UART to be ready
        DMA1_Stream1->CR &= ~DMA_SxCR_EN; // disable DMA
        while (DMA1_Stream1->CR & DMA_SxCR_EN)
            ;
        DMA1_Stream1->M0AR = (uint32_t)Buff_3.RxBuffer_DMA;  // set memory address
        DMA1_Stream1->NDTR = ARRAY_LEN(Buff_3.RxBuffer_DMA); // set transfer size

        DMA1->LIFCR |= (0x3F << 6U); // clear flags

        DMA1_Stream1->CR |= DMA_SxCR_TCIE; // set transfer complete interrupts
        DMA1_Stream1->CR |= DMA_SxCR_HTIE; // set transfer complete interrupts

        DMA1_Stream1->CR |= DMA_SxCR_EN; // enable DMA

        USART3->CR1 |= USART_CR1_IDLEIE;// | USART_CR1_RXNEIE;
        USART3->CR3 |= USART_CR3_DMAR; // enable DMA for UART
    }
}

/** @brief Writes date over USART3 with DMA.
 *
 *  @param *pData A pointer to data you want to send.
 *  @param size The amount of bytes to be send.
 *  @return Bool. Successful or not
 */
bool usart3Tx(const char* str, size_t size)
{
    if (!utx3_finished)
        return false;
    utx3_finished = false;

    DMA1_Stream3->CR &= ~DMA_SxCR_EN; // disable DMA
    while (DMA1_Stream3->CR & DMA_SxCR_EN)
        ;
    DMA1_Stream3->NDTR = size;        // set transfer size
    DMA1_Stream3->M0AR = (uint32_t)str;

    DMA1->LIFCR |= (0x3F << 22U); // clear flags

    DMA1_Stream3->CR |= DMA_SxCR_EN; // enable DMA
    USART3->CR3 |= USART_CR3_DMAT; // enable DMA for UART

    USART3->ICR |= USART_ICR_TCCF;
    USART3->CR1 |= USART_CR1_UE; // enable usart

    return true;
}

bool
usart3TxBusy(void)
{
    return !utx3_finished;
}

/** @brief Uses polling to write data to the transmit buffer. Mostly for slow printf
 *
 *  @param ch The character to send.
 *  @return Void.
 */
void usart3TxOneByte(uint8_t ch)
{
    while (!(USART3->ISR & USART_ISR_TXE))
    {
    } // waits for TX buffer to become empty
    USART3->TDR = ch; // transfers the value of the data register into ch
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
    pos = ARRAY_LEN(Buff_3.RxBuffer_DMA) - DMA1_Stream1->NDTR;
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
            usart_process_data(&Buff_3.RxBuffer_DMA[old_pos], pos - old_pos);
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
            usart_process_data(&Buff_3.RxBuffer_DMA[old_pos], ARRAY_LEN(Buff_3.RxBuffer_DMA) - old_pos);
            if (pos > 0)
            {
                usart_process_data(&Buff_3.RxBuffer_DMA[0], pos);
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
usart_process_data(const void* data, size_t len)
{
    dmaBytesProcessed += len;

    size_t written = lwrb_write(&Buff_3.RxBuffer, data, len);
    ringBytesWritten += written;

    if (written != len)
        rxOverflowCount++;
}

void usart3Read8(uint8_t* num) {}
void usart3ReadPID(float* P, float* I, float* D) {}
bool usart3WaitFor(char wait) {}

/* Interrupt Handlers */

/**
 * @brief This function handles UART3 global interrupt.
 */
void USART3_IRQHandler(void)
{
    if ((USART3->CR1 & USART_CR1_IDLEIE) && (USART3->ISR & USART_ISR_IDLE))
    {
        USART3->ICR |= USART_ICR_IDLECF;
        usart_rx_check();
    }

    if (USART3->ISR & USART_ISR_ORE)
        USART3->ICR |= USART_ICR_ORECF;

    if ((USART3->ISR & USART_ISR_TC) && (USART3->CR1 & USART_CR1_TCIE))
        USART3->ICR |= USART_ICR_TCCF; /* Clear IDLE line flag */
}

/** @brief	DMA1_Stream1 global interrupt handler for USART3 RX
 *
 * 	@return Void.
 */
void DMA1_Stream1_IRQHandler(void)
{
    /* Check transfer complete interrupt */
    if ((DMA1->LISR & DMA_LISR_TCIF1) && (DMA1_Stream1->CR & DMA_SxCR_TCIE))
    {
        DMA1->LIFCR |= DMA_LIFCR_CTCIF1; /* Clear half-transfer complete flag */
        usart_rx_check();
    }
    /* Check half-transfer complete interrupt */
    if ((DMA1->LISR & DMA_LISR_HTIF1) && (DMA1_Stream1->CR & DMA_SxCR_HTIE))
    {
        DMA1->LIFCR |= DMA_LIFCR_CHTIF1; /* Clear half-transfer complete flag */
        usart_rx_check();
    }
}

/** @brief	DMA1_Stream3 global interrupt handler for USART3 TX
 *
 * 	@return Void.
 */
void DMA1_Stream3_IRQHandler(void)
{
    /* Check half-transfer complete interrupt */
    if ((DMA1->LISR & DMA_LISR_TCIF3) && (DMA1_Stream3->CR & DMA_SxCR_TCIE))
    {
        DMA1->LIFCR |= DMA_LIFCR_CTCIF3; /* Clear half-transfer complete flag */
        utx3_finished = true;

        if (usart3TxCb != NULL)
            usart3TxCb();
    }
}

/*	This is required to use printf											*/
/*	This basically tells the compiler what to do when it encounters printf	*/
/*	I honestly can't fully explain what is going on but it works			*/
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//	#define GETCHAR_PROTOTYPE int __io_getchar (void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//	#define GETCHAR_PROTOTYPE int fgetc(FILE * f)
#endif

PUTCHAR_PROTOTYPE
{
    usart3TxOneByte(ch);
    return ch;
}
