/**
 ******************************************************************************
 * @file    tim.c
 * @brief   This file provides code for the configuration
 *          of the TIM instances.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"

void
tim9Init(void)
{
	RCC->APB2ENR	|= RCC_APB2ENR_TIM9EN;

	TIM9->PSC 	= 108 - 1;
	TIM9->ARR 	= 0xFFFF;
	TIM9->CNT	= 4000;
}

uint32_t
tim9GetCnt(void)
{
	return TIM9->CNT;
}

void
tim9ResetCnt(void)
{
	TIM9->CNT = 0;
}

void
tim9Enable(void)
{
	TIM9->CR1	|= TIM_CR1_CEN;
}

void
tim9Disable(void)
{
	TIM9->CR1	&= ~TIM_CR1_CEN;
}
