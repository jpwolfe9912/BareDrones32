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
#include "logging.h"

/* Global Variables */
uint8_t temp;
uint8_t usart3Buf[100];
volatile bool endOfString;
uint8_t usart3Index = 0;

volatile bool usart3TxBusy = false;

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
    printf("\nInitializing USART 3\n");
    // TX3 : PB10 : AF7
    // RX3 : PB11 : AF7
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // enable the clock for port B
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // enable the clock for USART3

    /* */
    GPIOB->MODER &= ~(GPIO_MODER_MODER10 |
                      GPIO_MODER_MODER11);
    GPIOB->MODER |= GPIO_MODER_MODER10_1 |
        GPIO_MODER_MODER11_1; // set PB10/11 as alternate function
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR10 |
                       GPIO_OSPEEDR_OSPEEDR11);
    GPIOB->AFR[1] |= (0x7 << (2 * 4U)) |
        (0x7 << (3 * 4U)); // set PB10/11 to AF7

    NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(USART3_IRQn);

    USART3->CR1 &= ~USART_CR1_UE;
    USART3->BRR = 54000000 / baudrate;                                       // set baud rate to 115200
    USART3->CR2 |= USART_CR2_SWAP;                             // swap TX and RX pins   
    USART3->CR1 &= ~USART_CR1_M;  // 8 bit transfer
    USART3->CR2 &= ~USART_CR2_STOP;
    USART3->CR1 &= ~USART_CR1_PCE;
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART3->CR3 &= ~(USART_CR3_CTSE |
                     USART_CR3_RTSE);
    USART3->CR1 &= ~USART_CR1_OVER8;
    USART3->CR1 |= USART_CR1_UE; // enable usart

    // USART3_TX on DMA1 Stream3 Ch4
    // USART3_RX on DMA1 Stream1 Ch4
    // disable DMA 1 stream 3
    DMA1_Stream3->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream3->CR & DMA_SxCR_EN)
    {
    }
    DMA1_Stream3->CR = 0;
    DMA1_Stream3->NDTR = 0;
    DMA1_Stream3->PAR = 0;
    DMA1_Stream3->M0AR = 0;
    DMA1_Stream3->M1AR = 0;
    DMA1_Stream3->FCR = 0x00000021U;
    DMA1_Stream3->CR &= ~DMA_SxCR_CHSEL;
    DMA2->HIFCR |= (0x3F << 16U); // 0x00000F40U;

    // stream 3 ch 4 DMA settings
    DMA1_Stream3->CR |= DMA_SxCR_DIR_0;    // mem to per
    DMA1_Stream3->FCR &= ~DMA_SxFCR_DMDIS; // fifo dis
    DMA1_Stream3->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream3->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream3->PAR = (uint32_t)(&(USART3->TDR)); // set per address
    DMA1_Stream3->CR &= ~DMA_SxCR_PINC;             // don't inc per
    DMA1_Stream3->CR |= DMA_SxCR_MINC;              // increment mem
    DMA1_Stream3->CR &= ~DMA_SxCR_MSIZE;            // 8 bit size
    DMA1_Stream3->CR &= ~DMA_SxCR_PSIZE;            // 8 bit size
    DMA1_Stream3->CR &= ~DMA_SxCR_CIRC;             // circ mode dis
    DMA1_Stream3->CR |= DMA_SxCR_PL;                // medium priority
}
#endif

/** @brief Writes date over USART3 with DMA.
 *
 *  @param *pData A pointer to data you want to send.
 *  @param size The amount of bytes to be send.
 *  @return Bool. Successful or not
 */
bool usart3Write(uint8_t* pData, uint8_t size)
{
    if (usart3TxBusy)
        return false;
    usart3TxBusy = true;

    DMA1_Stream3->CR &= ~DMA_SxCR_EN; // disable DMA
    while (DMA1_Stream3->CR & DMA_SxCR_EN)
        ;
    DMA1_Stream3->M0AR = (uint32_t)pData;
    DMA1_Stream3->CR |= (0x4 << 25U); // set DMA channel
    DMA1_Stream3->NDTR = size;        // set transfer size

    DMA1->LIFCR |= (0x3F << 22U); // clear flags

    DMA1_Stream3->CR |= DMA_SxCR_TCIE; // set transfer complete interrupts

    DMA1_Stream3->CR |= DMA_SxCR_EN; // enable DMA

    USART3->CR3 |= USART_CR3_DMAT; // enable DMA for UART

    USART3->ICR |= USART_ICR_TCCF;
    USART3->CR1 |= USART_CR1_UE; // enable usart

    return true;
}

/** @brief Uses polling to write data to the transmit buffer.
 *
 *  @param ch The character to send.
 *  @return Void.
 */
void usart3WriteOneByte(uint8_t ch)
{
    while (!(USART3->ISR & USART_ISR_TXE))
    {
    } // waits for TX buffer to become empty
    USART3->TDR = ch; // transfers the value of the data register into ch
}

/** @brief Uses interrupts to read uint8 data to the receive buffer.
 *
 *  @param uint8_t *num Pointer to the location you want to store the received number
 *  @return Void.
 */
void usart3Read8(uint8_t* num)
{
    USART3->CR1 |= USART_CR1_RXNEIE;
    while (!temp)
        ;

    *num = (uint8_t)temp;
    USART3->CR1 &= ~USART_CR1_RXNEIE;
}

/** @brief Uses interrupts to read a string of PID values.
 *
 *  @param * Pointer to the location you want to store the received number
 *  @return Void.
 */
void usart3ReadPID(float* P, float* I, float* D)
{
    usart3Index = 0;
    memset(usart3Buf, '\0', sizeof(usart3Buf));

    endOfString = false;
    USART3->CR1 |= USART_CR1_RXNEIE;
    while (!endOfString)
        ;

    sscanf((char*)usart3Buf, "%f, %f, %f", P, I, D);

    USART3->CR1 &= ~USART_CR1_RXNEIE;
    memset(usart3Buf, '\0', sizeof(usart3Buf));
}

/** @brief Waits for a character.
 *
 *  @param wait Character to wait for.
 *  @return bool True or False based on whether or not the character.
 *  received is the input to the function.
 */
bool usart3WaitFor(char wait)
{
    USART3->CR1 |= USART_CR1_RXNEIE;
    while (!temp)
        ;
    if (temp == wait)
    {
        temp = '\0';
        USART3->CR1 &= ~USART_CR1_RXNEIE;
        return true;
    }
    else
    {
        temp = '\0';
        USART3->CR1 &= ~USART_CR1_RXNEIE;
        return false;
    }
}

/* Interrupt Handlers */

/**
 * @brief This function handles UART3 global interrupt.
 */
void USART3_IRQHandler(void)
{
    if ((USART3->ISR & USART_ISR_RXNE) && (USART3->CR1 & USART_CR1_RXNEIE))
    {
        temp = USART3->RDR;
        USART3->TDR = temp;
        if (temp == '\r')
        {
            endOfString = true;
            temp = '\0';
        }
        else
        {
            usart3Buf[usart3Index] = temp;
            usart3Index++;
        }
    }
    if (USART3->ISR & USART_ISR_ORE)
        USART3->ICR |= USART_ICR_ORECF;

    if ((USART3->ISR & USART_ISR_TC) && (USART3->CR1 & USART_CR1_TCIE))
        USART3->ICR |= USART_ICR_TCCF; /* Clear IDLE line flag */
}

/** @brief	DMA1_Stream3 global interrupt handler for USART6 TX
 *
 * 	@return Void.
 */
void DMA1_Stream3_IRQHandler(void)
{
    /* Check half-transfer complete interrupt */
    if (DMA1->LISR & DMA_LISR_TCIF3)
    {
        DMA1->LIFCR |= DMA_LIFCR_CTCIF3; /* Clear half-transfer complete flag */
        usart3TxBusy = false;
#ifdef WIRED_LOGGING
        loggerComplete();
#endif
    }
}

/*	This is required to use usart3											*/
/*	This basically tells the compiler what to do when it encounters usart3	*/
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
    usart3WriteOneByte(ch);
    return ch;
}
