/** @file 		drv_dshot.c
 *  @brief
 *  	This files enables the gpio, timer, and dma peripherals to send dshot commands with pwm.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#include "board.h"

/* Static Variables */
static uint32_t motor1_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
static uint32_t motor2_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
static uint32_t motor3_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
static uint32_t motor4_dmabuffer[DSHOT_DMA_BUFFER_SIZE];

volatile uint16_t dshot_command_count = 0;

bool dshot_telemetry = false;

/* Static Function Prototypes */
static uint32_t dshot_choose_type(dshot_type_e dshot_type);
static void dshot_prepare_dmabuffer_all(uint16_t *motor_value);
static void dshot_prepare_dmabuffer(uint32_t *motor_dmabuffer, uint16_t value);
static uint16_t dshot_prepare_packet(uint16_t value);
static void dshot_dma_start(void);
static void dshot_enable_dma_request(void);

/* Functions */

/** @brief Initializes the low level registers to set up
 *  the timer with PWM DMA.
 *  @note M1: PB4 / M2: PB5 / M3: PB0 / M4: PB1
 *
 *  @param dshot_type DSHOT150/300/600. Allows you to adjust the speed.
 *  DSHOT600 is preferred
 *  @return Void.
 */
void dshotInit(dshot_type_e dshot_type)
{
    printf("\nInitializing DSHOT\n");
    /* GPIO INIT */
    // enable clock for GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    /* output mode to AF */
    GPIOB->MODER &= ~(GPIO_MODER_MODER0 &
                      GPIO_MODER_MODER1 &
                      GPIO_MODER_MODER4 &
                      GPIO_MODER_MODER5);
    GPIOB->MODER |= (GPIO_MODER_MODER0_1 |
                     GPIO_MODER_MODER1_1 |
                     GPIO_MODER_MODER4_1 |
                     GPIO_MODER_MODER5_1);
    /* speed to max */
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR0 |
                       GPIO_OSPEEDR_OSPEEDR1 |
                       GPIO_OSPEEDR_OSPEEDR4 |
                       GPIO_OSPEEDR_OSPEEDR5);
    /* output type to push-pull */
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 &
                       GPIO_OTYPER_OT1 &
                       GPIO_OTYPER_OT4 &
                       GPIO_OTYPER_OT5);
    /* pull down */
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR0_1 |
                     GPIO_PUPDR_PUPDR1_1 |
                     GPIO_PUPDR_PUPDR4_1 |
                     GPIO_PUPDR_PUPDR5_1);
    /* alternate function to AF2 */
    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL0 &
                       GPIO_AFRL_AFRL1 &
                       GPIO_AFRL_AFRL4 &
                       GPIO_AFRL_AFRL5);
    GPIOB->AFR[0] |= (0x2 << (4U * 0U) |
                      0x2 << (4U * 1U) |
                      0x2 << (4U * 4U) |
                      0x2 << (4U * 5U));

    /* Timer Init */
    // M1: PB4 : TIM3 CH1 : DMA1 Ch5 Stream4
    // M2: PB5 : TIM3 CH2 : DMA1 Ch5 Stream5
    // M3: PB0 : TIM3 CH3 : DMA1 Ch5 Stream7
    // M4: PB1 : TIM3 CH4 : DMA1 Ch5 Stream2
    uint16_t dshot_psc = (uint16_t)((float)TIMER_CLOCK / dshot_choose_type(dshot_type) + 0.01f) - 1;
    // enable clock for TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->CR1 &= ~TIM_CR1_CEN;
    // set PSC, AR, clock div, cnt, cnt mode
    TIM3->PSC = dshot_psc;
    TIM3->ARR = MOTOR_BITLENGTH;
    TIM3->CR1 &= ~TIM_CR1_CKD;
    TIM3->CR1 &= ~TIM_CR1_DIR;
    // set output compare mode
    // channel 1
    TIM3->CCER &= ~TIM_CCER_CC1E; // disable the channel
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 |
                   TIM_CCMR1_OC1M_2; // set mode to PWM
    TIM3->CCER &= ~TIM_CCER_CC1P;    // disable output compare
    TIM3->CCR1 = 0;                  // set compare value to 0
    TIM3->CCER |= TIM_CCER_CC1E;     // enable the channel
    // channel 2
    TIM3->CCER &= ~TIM_CCER_CC2E;
    TIM3->CCMR1 |= TIM_CCMR1_OC2M_1 |
                   TIM_CCMR1_OC2M_2;
    TIM3->CCER &= ~TIM_CCER_CC2P;
    TIM3->CCR2 = 0;
    TIM3->CCER |= TIM_CCER_CC2E;
    // channel 3
    TIM3->CCER &= ~TIM_CCER_CC3E;
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 |
                   TIM_CCMR2_OC3M_2;
    TIM3->CCER &= ~TIM_CCER_CC3P;
    TIM3->CCR3 = 0;
    TIM3->CCER |= TIM_CCER_CC3E;
    // channel 4
    TIM3->CCER &= ~TIM_CCER_CC4E;
    TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 |
                   TIM_CCMR2_OC4M_2;
    TIM3->CCER &= ~TIM_CCER_CC4P;
    TIM3->CCR4 = 0;
    TIM3->CCER |= TIM_CCER_CC4E;
    // enable preload
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM3->CCMR1 |= TIM_CCMR1_OC2PE;
    TIM3->CCMR2 |= TIM_CCMR2_OC3PE;
    TIM3->CCMR2 |= TIM_CCMR2_OC4PE;
    // auto-reload preload enable
    TIM3->CR1 |= TIM_CR1_ARPE;
    // enable the counter
    TIM3->CR1 |= TIM_CR1_CEN;

    /* DMA Init */
    // M1: PB4 : TIM3 CH1 : DMA1 Ch5 Stream4
    // M2: PB5 : TIM3 CH2 : DMA1 Ch5 Stream5
    // M3: PB0 : TIM3 CH3 : DMA1 Ch5 Stream7
    // M4: PB1 : TIM3 CH4 : DMA1 Ch5 Stream2

    // disable DMA stream 4
    DMA1_Stream4->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream4->CR & DMA_SxCR_EN)
    {
    }
    DMA1_Stream4->CR = 0;
    DMA1_Stream4->NDTR = 0;
    DMA1_Stream4->PAR = 0;
    DMA1_Stream4->M0AR = 0;
    DMA1_Stream4->M1AR = 0;
    DMA1_Stream4->FCR = 0x00000021U;
    DMA1_Stream4->CR &= ~DMA_SxCR_CHSEL;
    DMA1->LIFCR |= 0x003F0000U;
    // disable DMA stream 5
    DMA1_Stream5->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream5->CR & DMA_SxCR_EN)
    {
    }
    DMA1_Stream5->CR = 0;
    DMA1_Stream5->NDTR = 0;
    DMA1_Stream5->PAR = 0;
    DMA1_Stream5->M0AR = 0;
    DMA1_Stream5->M1AR = 0;
    DMA1_Stream5->FCR = 0x00000021U;
    DMA1_Stream5->CR &= ~DMA_SxCR_CHSEL;
    DMA1->HIFCR |= 0x0000003FU;
    // disable DMA stream 7
    DMA1_Stream7->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream7->CR & DMA_SxCR_EN)
    {
    }
    DMA1_Stream7->CR = 0;
    DMA1_Stream7->NDTR = 0;
    DMA1_Stream7->PAR = 0;
    DMA1_Stream7->M0AR = 0;
    DMA1_Stream7->M1AR = 0;
    DMA1_Stream7->FCR = 0x00000021U;
    DMA1_Stream7->CR &= ~DMA_SxCR_CHSEL;
    DMA1->LIFCR |= 0x0000003FU;
    // disable DMA stream 2
    DMA1_Stream2->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream2->CR & DMA_SxCR_EN)
    {
    }
    DMA1_Stream2->CR = 0;
    DMA1_Stream2->NDTR = 0;
    DMA1_Stream2->PAR = 0;
    DMA1_Stream2->M0AR = 0;
    DMA1_Stream2->M1AR = 0;
    DMA1_Stream2->FCR = 0x00000021U;
    DMA1_Stream2->CR &= ~DMA_SxCR_CHSEL;
    DMA1->LIFCR |= 0x0F400000U;

    // motor 1 DMA settings
    DMA1_Stream4->CR |= (0x5 << 25U); // set channel 5
    DMA1_Stream4->M0AR = (uint32_t)motor1_dmabuffer;    // memory address to the dma buffer
    DMA1_Stream4->CR |= DMA_SxCR_DIR_0;   // memory to peripheral
    DMA1_Stream4->FCR |= DMA_SxFCR_DMDIS; // fifo enable
    DMA1_Stream4->FCR &= ~DMA_SxFCR_FTH;  // 1/4 full
    DMA1_Stream4->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream4->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream4->PAR = (uint32_t)(&(TIM3->CCR1)); // peripheral address to the timer
    DMA1_Stream4->NDTR = DSHOT_DMA_BUFFER_SIZE;
    DMA1_Stream4->CR &= ~DMA_SxCR_PINC;
    DMA1_Stream4->CR |= DMA_SxCR_MINC;
    DMA1_Stream4->CR |= DMA_SxCR_MSIZE_1;
    DMA1_Stream4->CR |= DMA_SxCR_PSIZE_1;
    DMA1_Stream4->CR &= ~DMA_SxCR_CIRC;
    DMA1_Stream4->CR |= DMA_SxCR_PL_0;
    // DMA transfer complete interrupt enable
    DMA1_Stream4->CR |= DMA_SxCR_TCIE;

    // motor 2 DMA settings
    DMA1_Stream5->CR |= (0x5 << 25U);
    DMA1_Stream5->M0AR = (uint32_t)motor2_dmabuffer;
    DMA1_Stream5->CR |= DMA_SxCR_DIR_0;   // mem to per
    DMA1_Stream5->FCR |= DMA_SxFCR_DMDIS; // fifo en
    DMA1_Stream5->FCR &= ~DMA_SxFCR_FTH;  // 1/4 full
    DMA1_Stream5->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream5->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream5->PAR = (uint32_t)(&(TIM3->CCR2));
    DMA1_Stream5->NDTR = DSHOT_DMA_BUFFER_SIZE;
    DMA1_Stream5->CR &= ~DMA_SxCR_PINC;
    DMA1_Stream5->CR |= DMA_SxCR_MINC;
    DMA1_Stream5->CR |= DMA_SxCR_MSIZE_1;
    DMA1_Stream5->CR |= DMA_SxCR_PSIZE_1;
    DMA1_Stream5->CR &= ~DMA_SxCR_CIRC;
    DMA1_Stream5->CR |= DMA_SxCR_PL;
    // DMA transfer complete interrupt enable
    DMA1_Stream5->CR |= DMA_SxCR_TCIE;

    // motor 3 DMA settings
    DMA1_Stream7->CR |= (0x5 << 25U);
    DMA1_Stream7->M0AR = (uint32_t)motor3_dmabuffer;
    DMA1_Stream7->CR |= DMA_SxCR_DIR_0;   // mem to per
    DMA1_Stream7->FCR |= DMA_SxFCR_DMDIS; // fifo en
    DMA1_Stream7->FCR &= ~DMA_SxFCR_FTH;  // 1/4 full
    DMA1_Stream7->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream7->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream7->PAR = (uint32_t)(&(TIM3->CCR3));
    DMA1_Stream7->NDTR = DSHOT_DMA_BUFFER_SIZE;
    DMA1_Stream7->CR &= ~DMA_SxCR_PINC;
    DMA1_Stream7->CR |= DMA_SxCR_MINC;
    DMA1_Stream7->CR |= DMA_SxCR_MSIZE_1;
    DMA1_Stream7->CR |= DMA_SxCR_PSIZE_1;
    DMA1_Stream7->CR &= ~DMA_SxCR_CIRC;
    DMA1_Stream7->CR |= DMA_SxCR_PL_0;
    // DMA transfer complete interrupt enable
    DMA1_Stream7->CR |= DMA_SxCR_TCIE;
    
    // motor 4 DMA settings
    DMA1_Stream2->CR |= (0x5 << 25U);
    DMA1_Stream2->M0AR = (uint32_t)motor4_dmabuffer;
    DMA1_Stream2->CR |= DMA_SxCR_DIR_0;   // mem to per
    DMA1_Stream2->FCR |= DMA_SxFCR_DMDIS; // fifo en
    DMA1_Stream2->FCR &= ~DMA_SxFCR_FTH;  // 1/4 full
    DMA1_Stream2->CR &= ~DMA_SxCR_MBURST;
    DMA1_Stream2->CR &= ~DMA_SxCR_PBURST;
    DMA1_Stream2->PAR = (uint32_t)(&(TIM3->CCR4));
    DMA1_Stream2->NDTR = DSHOT_DMA_BUFFER_SIZE;
    DMA1_Stream2->CR &= ~DMA_SxCR_PINC;
    DMA1_Stream2->CR |= DMA_SxCR_MINC;
    DMA1_Stream2->CR |= DMA_SxCR_MSIZE_1;
    DMA1_Stream2->CR |= DMA_SxCR_PSIZE_1;
    DMA1_Stream2->CR &= ~DMA_SxCR_CIRC;
    DMA1_Stream2->CR |= DMA_SxCR_PL_0;
    // DMA transfer complete interrupt enable
    DMA1_Stream2->CR |= DMA_SxCR_TCIE;
}

/** @brief Writes to the DMA buffer and starts the DMA stream.
 *
 *  @param *motor_value A four length array with elements from 0 to 2047.
 *  @return Void.
 */
void dshotWrite(void)
{
    dshot_prepare_dmabuffer_all(motor_value);
    dshot_enable_dma_request();
    dshot_dma_start();

    dshot_command_count++;
}

/** @brief Waits for a certain amount of dshot commands.
 *
 *  @param wait_counts Number of commands to wait for.
 *  @return Void.
 */
void dshotWait(uint16_t wait_counts)
{
    dshot_command_count = 0;

    if (wait_counts == DSHOT_ARM_COUNT)
    {
        SysTick->LOAD = (uint32_t)((SystemCoreClock / 10000) - 1UL);
        wait_counts *= 10;
    }

    while (dshot_command_count < wait_counts)
        ;

    SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000) - 1UL);
}

/* Static Functions */

/** @brief Chooses the dshot speed.
 *
 *  @param dshot_type DSHOT150/300/600. Allows you to adjust the speed.
 *  DSHOT600 is preferred
 *  @return Void.
 */
static uint32_t
dshot_choose_type(dshot_type_e dshot_type)
{
    switch (dshot_type)
    {
    case (DSHOT600):
        return DSHOT600_HZ;

    case (DSHOT300):
        return DSHOT300_HZ;

    default:
        return DSHOT150_HZ;
    }
}

/** @brief Prepares all the DMA buffers.
 *
 *  @param *motor_value A four length array with elements from 0 to 2047.
 *  @return Void.
 */
static void
dshot_prepare_dmabuffer_all(uint16_t *motor_value)
{
    dshot_prepare_dmabuffer(motor1_dmabuffer, motor_value[0]);
    dshot_prepare_dmabuffer(motor2_dmabuffer, motor_value[1]);
    dshot_prepare_dmabuffer(motor3_dmabuffer, motor_value[2]);
    dshot_prepare_dmabuffer(motor4_dmabuffer, motor_value[3]);
}

/** @brief Based on whethere there is a 1 or 0, this loads the
 *  autoreload value into the buffer.
 *
 *  @param *motor_dmabuffer A pointer to the location in memory
 *  that the buffer with the values is located.
 *  @param value Numeric value to send from 0 to 2047.
 *  @return Void.
 */
static void
dshot_prepare_dmabuffer(uint32_t *motor_dmabuffer, uint16_t value)
{
    uint16_t packet;
    packet = dshot_prepare_packet(value);

    for (int i = 0; i < 16; i++)
    {
        motor_dmabuffer[i] = (packet & 0x8000) ? MOTOR_BIT_1 : MOTOR_BIT_0;
        packet <<= 1;
    }

    motor_dmabuffer[16] = 0;
    motor_dmabuffer[17] = 0;
}

/** @brief Puts a 1 or 0 into each index in the buffer based on what
 *  we want to send. Also calculates the checksum.
 *
 *  @param *motor_dmabuffer A pointer to the location in memory
 *  that the buffer with the values is located.
 *  @param value Numeric value to send from 0 to 2047.
 *  @return Void.
 */
static uint16_t
dshot_prepare_packet(uint16_t value)
{
    uint16_t packet;

    packet = (value << 1) | (dshot_telemetry ? 1 : 0);

    // compute checksum
    unsigned csum = 0;
    unsigned csum_data = packet;

    for (int i = 0; i < 3; i++)
    {
        csum ^= csum_data; // xor data by nibbles
        csum_data >>= 4;
    }

    csum &= 0xf;

    packet = (packet << 4) | csum;

    return packet;
}

/** @brief Starts dma for each timer.
 *
 *  @return Void.
 */
static void
dshot_dma_start(void)
{
    TIM3->CNT = 0;

    TIM3->DIER |= TIM_DIER_CC1DE;
    TIM3->DIER |= TIM_DIER_CC2DE;
    TIM3->DIER |= TIM_DIER_CC3DE;
    TIM3->DIER |= TIM_DIER_CC4DE;
}

/** @brief Enables each dma stream.
 *
 *  @return Void.
 */
static void
dshot_enable_dma_request(void)
{

    DMA1_Stream4->CR |= DMA_SxCR_EN;
    DMA1_Stream5->CR |= DMA_SxCR_EN;
    DMA1_Stream7->CR |= DMA_SxCR_EN;
    DMA1_Stream2->CR |= DMA_SxCR_EN;
}

/* Interrupt Handlers */

/**
 * @brief This function handles DMA1 Stream4 global interrupt.
 */
void DMA1_Stream4_IRQHandler(void)
{
    if (DMA1->HISR & DMA_HISR_TCIF4)
    {
        DMA1_Stream4->CR &= ~DMA_SxCR_EN;
        while (DMA1_Stream4->CR & DMA_SxCR_EN)
        {
        }
        TIM3->DIER &= ~TIM_DIER_CC1DE;
        DMA1->HIFCR |= DMA_HIFCR_CTCIF4;
        DMA1_Stream4->NDTR = DSHOT_DMA_BUFFER_SIZE;
    }
}

/**
 * @brief This function handles DMA1 Stream5 global interrupt.
 */
void DMA1_Stream5_IRQHandler(void)
{
    if (DMA1->HISR & DMA_HISR_TCIF5)
    {
        DMA1_Stream5->CR &= ~DMA_SxCR_EN;
        while (DMA1_Stream5->CR & DMA_SxCR_EN)
        {
        }
        TIM3->DIER &= ~TIM_DIER_CC2DE;
        DMA1->HIFCR |= DMA_HIFCR_CTCIF5;
        DMA1_Stream5->NDTR = DSHOT_DMA_BUFFER_SIZE;
    }
}

/**
 * @brief This function handles DMA1 Stream7 global interrupt.
 */
void DMA1_Stream7_IRQHandler(void)
{
    if (DMA1->HISR & DMA_HISR_TCIF7)
    {
        DMA1_Stream7->CR &= ~DMA_SxCR_EN;
        while (DMA1_Stream7->CR & DMA_SxCR_EN)
        {
        }
        TIM3->DIER &= ~TIM_DIER_CC3DE;
        DMA1->HIFCR |= DMA_HIFCR_CTCIF7;
        DMA1_Stream7->NDTR = DSHOT_DMA_BUFFER_SIZE;
    }
}

/**
 * @brief This function handles DMA1 Stream2 global interrupt.
 */
void DMA1_Stream2_IRQHandler(void)
{
    if (DMA1->LISR & DMA_LISR_TCIF2)
    {
        DMA1_Stream2->CR &= ~DMA_SxCR_EN;
        while (DMA1_Stream2->CR & DMA_SxCR_EN)
        {
        }
        TIM3->DIER &= ~TIM_DIER_CC4DE;
        DMA1->LIFCR |= DMA_LIFCR_CTCIF2;
        DMA1_Stream2->NDTR = DSHOT_DMA_BUFFER_SIZE;
    }
}
