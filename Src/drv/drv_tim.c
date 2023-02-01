/** @file 		drv_tim.c
 *  @brief
 *  	This file enables use of a timer to ensure accurate timing of the PID loop.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

/* Includes */
#include "board.h"

/** @brief Initializes the low level registers to set up timer 9.
 *
 *  @return Void.
 */
void tim9Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;

    TIM9->PSC = 108 - 1;
    TIM9->ARR = 0xFFFF;
    TIM9->CNT = 4000;
}

/** @brief Gets the counter value.
 *
 *  @return Void.
 */
uint32_t
tim9GetCnt(void)
{
    return TIM9->CNT;
}

/** @brief Resets the counter.
 *
 *  @return Void.
 */
void tim9ResetCnt(void)
{
    TIM9->CNT = 0;
}

/** @brief Enables the timer.
 *
 *  @return Void.
 */
void tim9Enable(void)
{
    TIM9->CR1 |= TIM_CR1_CEN;
}

/** @brief Disables the timer.
 *
 *  @return Void.
 */
void tim9Disable(void)
{
    TIM9->CR1 &= ~TIM_CR1_CEN;
}

uint16_t getTimerValue(void)
{
    uint16_t timerValue;
    
    tim9Disable();
    timerValue = tim9GetCnt();
    tim9ResetCnt();
    tim9Enable();

    return timerValue;
}