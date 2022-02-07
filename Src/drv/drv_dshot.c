#include "board.h"

static uint32_t motor1_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
static uint32_t motor2_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
static uint32_t motor3_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
static uint32_t motor4_dmabuffer[DSHOT_DMA_BUFFER_SIZE];

static uint16_t dshot_prepare_packet(uint16_t value);
static void dshot_prepare_dmabuffer_all(uint16_t *motor_value);
static void dshot_prepare_dmabuffer(uint32_t *motor_dmabuffer, uint16_t value);
static uint16_t dshot_prepare_packet(uint16_t value);
static void dshot_dma_start(void);
static void dshot_enable_dma_request(void);

static uint32_t dshot_choose_type(dshot_type_e dshot_type);

// FUNCTIONS
void dshot_init(dshot_type_e dshot_type){
	/////////////////GPIO INIT///////////////////
	// enable clock for GPIOA
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	// set mode, speed, type, pull, AF
	GPIOA->MODER 	&= ~GPIO_MODER_MODER0;
	GPIOA->MODER 	|= GPIO_MODER_MODER0_1;
	GPIOA->OSPEEDR	&= ~GPIO_OSPEEDR_OSPEEDR0;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT0;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR0;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL0;
	GPIOA->AFR[0] 	|= (GPIO_AF2_TIM5 << (4U * 0U));

	GPIOA->MODER 	&= ~GPIO_MODER_MODER1;
	GPIOA->MODER 	|= GPIO_MODER_MODER1_1;
	GPIOA->OSPEEDR	&= ~GPIO_OSPEEDR_OSPEEDR1;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT1;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR1;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL1;
	GPIOA->AFR[0] 	|= (GPIO_AF2_TIM5 << (4U * 1U));

	GPIOA->MODER 	&= ~GPIO_MODER_MODER2;
	GPIOA->MODER 	|= GPIO_MODER_MODER2_1;
	GPIOA->OSPEEDR	&= ~GPIO_OSPEEDR_OSPEEDR2;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT2;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR2;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL2;
	GPIOA->AFR[0] 	|= (GPIO_AF2_TIM5 << (4U * 2U));

	GPIOA->MODER 	&= ~GPIO_MODER_MODER3;
	GPIOA->MODER 	|= GPIO_MODER_MODER3_1;
	GPIOA->OSPEEDR	&= ~GPIO_OSPEEDR_OSPEEDR3;
	GPIOA->OTYPER	&= ~GPIO_OTYPER_OT3;
	GPIOA->PUPDR	&= ~GPIO_PUPDR_PUPDR3;
	GPIOA->AFR[0]	&= ~GPIO_AFRL_AFRL3;
	GPIOA->AFR[0] 	|= (GPIO_AF2_TIM5 << (4U * 3U));

	/////////////////TIMER INIT///////////////////
	uint16_t dshot_psc = (uint16_t)((float)TIMER_CLOCK / dshot_choose_type(dshot_type) + 0.01f) - 1;
	// enable clock for TIM2
	RCC->APB1ENR 	|= RCC_APB1ENR_TIM5EN;
	TIM5->CR1		&= ~TIM_CR1_CEN;
	// set PSC, AR, clock div, cnt, cnt mode
	TIM5->PSC 		= dshot_psc;	//(uint16_t)((float) TIMER_CLOCK / 12000000) - 1;
	TIM5->ARR		= MOTOR_BITLENGTH;
	TIM5->CR1		&= ~TIM_CR1_CKD;
	TIM5->CR1		&= ~TIM_CR1_DIR;
	// set output compare mode
	// channel 1
	TIM5->CCER		&= ~TIM_CCER_CC1E;
	TIM5->CCMR1		|= TIM_CCMR1_OC1M_1 |
			TIM_CCMR1_OC1M_2;
	TIM5->CCER		&= ~TIM_CCER_CC1P;
	TIM5->CCR1		= 0;
	TIM5->CCER		|= TIM_CCER_CC1E;
	// channel 2
	TIM5->CCER		&= ~TIM_CCER_CC2E;
	TIM5->CCMR1		|= TIM_CCMR1_OC2M_1 |
			TIM_CCMR1_OC2M_2;
	TIM5->CCER		&= ~TIM_CCER_CC2P;
	TIM5->CCR2		= 0;
	TIM5->CCER		|= TIM_CCER_CC2E;
	// channel 3
	TIM5->CCER		&= ~TIM_CCER_CC3E;
	TIM5->CCMR2		|= TIM_CCMR2_OC3M_1 |
			TIM_CCMR2_OC3M_2;
	TIM5->CCER		&= ~TIM_CCER_CC3P;
	TIM5->CCR3		= 0;
	TIM5->CCER		|= TIM_CCER_CC3E;
	// channel 4
	TIM5->CCER		&= ~TIM_CCER_CC4E;
	TIM5->CCMR2		|= TIM_CCMR2_OC4M_1 |
			TIM_CCMR2_OC4M_2;
	TIM5->CCER		&= ~TIM_CCER_CC4P;
	TIM5->CCR4		= 0;
	TIM5->CCER		|= TIM_CCER_CC4E;
	// enable preload
	TIM5->CCMR1		|= TIM_CCMR1_OC1PE;
	TIM5->CCMR1		|= TIM_CCMR1_OC2PE;
	TIM5->CCMR2		|= TIM_CCMR2_OC3PE;
	TIM5->CCMR2		|= TIM_CCMR2_OC4PE;
	//
	TIM5->CR1		|= TIM_CR1_ARPE;
	// enable the counter
	TIM5->CR1		|= TIM_CR1_CEN;

	/////////////////DMA INIT///////////////////
	/*
	 * TIM2_CH1 on Stream2_CH6
	 * TIM2_CH2 on Stream4_CH6
	 * TIM2_CH3 on Stream0_CH6
	 * TIM2_CH4 on Stream3_CH6
	 */

	// reset DMA
	RCC->AHB1RSTR		|= RCC_AHB1RSTR_DMA1RST;
	RCC->AHB1RSTR		&= ~RCC_AHB1RSTR_DMA1RST;
	// disable DMA stream 5
	DMA1_Stream2->CR 	&= ~DMA_SxCR_EN;
	while(DMA1_Stream2->CR & DMA_SxCR_EN){}
	DMA1_Stream2->CR	= 0;
	DMA1_Stream2->NDTR	= 0;
	DMA1_Stream2->PAR	= 0;
	DMA1_Stream2->M0AR	= 0;
	DMA1_Stream2->M1AR	= 0;
	DMA1_Stream2->FCR	= 0x00000021U;
	DMA1_Stream2->CR	&= ~DMA_SxCR_CHSEL;
	DMA1->LIFCR			|= 0x003F0000U;
	// disable DMA stream 6
	DMA1_Stream4->CR 	&= ~DMA_SxCR_EN;
	while(DMA1_Stream4->CR & DMA_SxCR_EN){}
	DMA1_Stream4->CR	= 0;
	DMA1_Stream4->NDTR	= 0;
	DMA1_Stream4->PAR	= 0;
	DMA1_Stream4->M0AR	= 0;
	DMA1_Stream4->M1AR	= 0;
	DMA1_Stream4->FCR	= 0x00000021U;
	DMA1_Stream4->CR	&= ~DMA_SxCR_CHSEL;
	DMA1->HIFCR			|= 0x0000003FU;
	// disable DMA stream 1
	DMA1_Stream0->CR 	&= ~DMA_SxCR_EN;
	while(DMA1_Stream0->CR & DMA_SxCR_EN){}
	DMA1_Stream0->CR	= 0;
	DMA1_Stream0->NDTR	= 0;
	DMA1_Stream0->PAR	= 0;
	DMA1_Stream0->M0AR	= 0;
	DMA1_Stream0->M1AR	= 0;
	DMA1_Stream0->FCR	= 0x00000021U;
	DMA1_Stream0->CR	&= ~DMA_SxCR_CHSEL;
	DMA1->LIFCR			|= 0x0000003FU;
	// disable DMA stream 7
	DMA1_Stream3->CR 	&= ~DMA_SxCR_EN;
	while(DMA1_Stream3->CR & DMA_SxCR_EN){}
	DMA1_Stream3->CR	= 0;
	DMA1_Stream3->NDTR	= 0;
	DMA1_Stream3->PAR	= 0;
	DMA1_Stream3->M0AR	= 0;
	DMA1_Stream3->M1AR	= 0;
	DMA1_Stream3->FCR	= 0x00000021U;
	DMA1_Stream3->CR	&= ~DMA_SxCR_CHSEL;
	DMA1->LIFCR			|= 0x0F400000U;
	// enable clock
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	// enable interrupts
	NVIC_SetPriority(DMA1_Stream2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(DMA1_Stream2_IRQn);

	NVIC_SetPriority(DMA1_Stream4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);

	NVIC_SetPriority(DMA1_Stream0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(DMA1_Stream0_IRQn);

	NVIC_SetPriority(DMA1_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	// motor 4 DMA settings
	DMA1_Stream2->CR	|= (0x6 << 25U);
	DMA1_Stream2->M0AR 	= (uint32_t)motor4_dmabuffer;
	DMA1_Stream2->CR 	|= DMA_SxCR_DIR_0;			// mem to per
	DMA1_Stream2->FCR	|= DMA_SxFCR_DMDIS;			// fifo en
	DMA1_Stream2->FCR	&= ~DMA_SxFCR_FTH;			//1/4 full
	DMA1_Stream2->CR 	&= ~DMA_SxCR_MBURST;
	DMA1_Stream2->CR 	&= ~DMA_SxCR_PBURST;
	DMA1_Stream2->PAR 	= (uint32_t)(&(TIM5->CCR1));
	DMA1_Stream2->NDTR	= DSHOT_DMA_BUFFER_SIZE;
	DMA1_Stream2->CR 	&= ~DMA_SxCR_PINC;
	DMA1_Stream2->CR 	|= DMA_SxCR_MINC;
	DMA1_Stream2->CR 	|= DMA_SxCR_MSIZE_1;
	DMA1_Stream2->CR 	|= DMA_SxCR_PSIZE_1;
	DMA1_Stream2->CR 	&= ~DMA_SxCR_CIRC;
	DMA1_Stream2->CR 	|= DMA_SxCR_PL_0;
	// DMA transfer complete interrupt enable
	DMA1_Stream2->CR 	|= DMA_SxCR_TCIE;
	// motor 4 DMA settings
	DMA1_Stream4->CR	|= (0x6 << 25U);
	DMA1_Stream4->M0AR 	= (uint32_t)motor3_dmabuffer;
	DMA1_Stream4->CR 	|= DMA_SxCR_DIR_0;			// mem to per
	DMA1_Stream4->FCR	|= DMA_SxFCR_DMDIS;			// fifo en
	DMA1_Stream4->FCR	&= ~DMA_SxFCR_FTH;			//1/4 full
	DMA1_Stream4->CR 	&= ~DMA_SxCR_MBURST;
	DMA1_Stream4->CR 	&= ~DMA_SxCR_PBURST;
	DMA1_Stream4->PAR 	= (uint32_t)(&(TIM5->CCR2));
	DMA1_Stream4->NDTR	= DSHOT_DMA_BUFFER_SIZE;
	DMA1_Stream4->CR 	&= ~DMA_SxCR_PINC;
	DMA1_Stream4->CR 	|= DMA_SxCR_MINC;
	DMA1_Stream4->CR 	|= DMA_SxCR_MSIZE_1;
	DMA1_Stream4->CR 	|= DMA_SxCR_PSIZE_1;
	DMA1_Stream4->CR 	&= ~DMA_SxCR_CIRC;
	DMA1_Stream4->CR 	|= DMA_SxCR_PL;
	// DMA transfer complete interrupt enable
	DMA1_Stream4->CR 	|= DMA_SxCR_TCIE;
	// motor 2 DMA settings
	DMA1_Stream0->CR	|= (0x6 << 25U);
	DMA1_Stream0->M0AR 	= (uint32_t)motor2_dmabuffer;
	DMA1_Stream0->CR 	|= DMA_SxCR_DIR_0;			// mem to per
	DMA1_Stream0->FCR	|= DMA_SxFCR_DMDIS;			// fifo en
	DMA1_Stream0->FCR	&= ~DMA_SxFCR_FTH;			//1/4 full
	DMA1_Stream0->CR 	&= ~DMA_SxCR_MBURST;
	DMA1_Stream0->CR 	&= ~DMA_SxCR_PBURST;
	DMA1_Stream0->PAR 	= (uint32_t)(&(TIM5->CCR3));
	DMA1_Stream0->NDTR	= DSHOT_DMA_BUFFER_SIZE;
	DMA1_Stream0->CR 	&= ~DMA_SxCR_PINC;
	DMA1_Stream0->CR 	|= DMA_SxCR_MINC;
	DMA1_Stream0->CR 	|= DMA_SxCR_MSIZE_1;
	DMA1_Stream0->CR 	|= DMA_SxCR_PSIZE_1;
	DMA1_Stream0->CR 	&= ~DMA_SxCR_CIRC;
	DMA1_Stream0->CR 	|= DMA_SxCR_PL_0;
	// DMA transfer complete interrupt enable
	DMA1_Stream0->CR 	|= DMA_SxCR_TCIE;
	// motor 4 DMA settings
	DMA1_Stream3->CR	|= (0x6 << 25U);
	DMA1_Stream3->M0AR 	= (uint32_t)motor1_dmabuffer;
	DMA1_Stream3->CR 	|= DMA_SxCR_DIR_0;			// mem to per
	DMA1_Stream3->FCR	|= DMA_SxFCR_DMDIS;			// fifo en
	DMA1_Stream3->FCR	&= ~DMA_SxFCR_FTH;			//1/4 full
	DMA1_Stream3->CR 	&= ~DMA_SxCR_MBURST;
	DMA1_Stream3->CR 	&= ~DMA_SxCR_PBURST;
	DMA1_Stream3->PAR 	= (uint32_t)(&(TIM5->CCR4));
	DMA1_Stream3->NDTR	= DSHOT_DMA_BUFFER_SIZE;
	DMA1_Stream3->CR 	&= ~DMA_SxCR_PINC;
	DMA1_Stream3->CR 	|= DMA_SxCR_MINC;
	DMA1_Stream3->CR 	|= DMA_SxCR_MSIZE_1;
	DMA1_Stream3->CR 	|= DMA_SxCR_PSIZE_1;
	DMA1_Stream3->CR 	&= ~DMA_SxCR_CIRC;
	DMA1_Stream3->CR 	|= DMA_SxCR_PL_0;
	// DMA transfer complete interrupt enable
	DMA1_Stream3->CR 	|= DMA_SxCR_TCIE;
}

void dshot_write(uint16_t *motor_value){
	dshot_prepare_dmabuffer_all(motor_value);
	dshot_enable_dma_request();
	dshot_dma_start();
}

static void dshot_prepare_dmabuffer_all(uint16_t *motor_value){
	dshot_prepare_dmabuffer(motor1_dmabuffer, motor_value[0]);
	dshot_prepare_dmabuffer(motor2_dmabuffer, motor_value[1]);
	dshot_prepare_dmabuffer(motor3_dmabuffer, motor_value[2]);
	dshot_prepare_dmabuffer(motor4_dmabuffer, motor_value[3]);
}

// STATIC FUNCTIONS
static uint32_t dshot_choose_type(dshot_type_e dshot_type){
	switch (dshot_type)
	{
	case(DSHOT600):
		return DSHOT600_HZ;

	case(DSHOT300):
		return DSHOT300_HZ;

	default:
		return DSHOT150_HZ;
	}
}

static void dshot_prepare_dmabuffer(uint32_t *motor_dmabuffer, uint16_t value)
{
	uint16_t packet;
	packet = dshot_prepare_packet(value);

	for(int i = 0; i < 16; i++)
	{
		motor_dmabuffer[i] = (packet & 0x8000) ? MOTOR_BIT_1 : MOTOR_BIT_0;
		packet <<= 1;
	}

	motor_dmabuffer[16] = 0;
	motor_dmabuffer[17] = 0;

}

static uint16_t dshot_prepare_packet(uint16_t value){
	uint16_t packet;
	bool dshot_telemetry = false;

	packet = (value << 1) | (dshot_telemetry ? 1 : 0);

	// compute checksum
	unsigned csum = 0;
	unsigned csum_data = packet;

	for(int i = 0; i < 3; i++)
	{
		csum ^=  csum_data; // xor data by nibbles
		csum_data >>= 4;
	}

	csum &= 0xf;
	packet = (packet << 4) | csum;

	return packet;
}

static void dshot_dma_start(void){
	TIM5->CNT			= 0;

	TIM5->DIER			|= TIM_DIER_CC1DE;
	TIM5->DIER			|= TIM_DIER_CC2DE;
	TIM5->DIER			|= TIM_DIER_CC3DE;
	TIM5->DIER			|= TIM_DIER_CC4DE;
}

static void dshot_enable_dma_request(void){

	DMA1_Stream2->CR 	|= DMA_SxCR_EN;
	DMA1_Stream4->CR 	|= DMA_SxCR_EN;
	DMA1_Stream0->CR 	|= DMA_SxCR_EN;
	DMA1_Stream3->CR 	|= DMA_SxCR_EN;
}
