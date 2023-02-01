/** @file 		drv_led.c
 *  @brief
 *  	This file contains functions to activate the 5 LEDs onboard.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		19 MAR 2022
 */

#include "board.h"

void ledInit(void)
{
    /*
     * 	LEDs are PC0-4
     */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    GPIOC->MODER &= ~(GPIO_MODER_MODER0 |
                      GPIO_MODER_MODER1 |
                      GPIO_MODER_MODER2 |
                      GPIO_MODER_MODER3 |
                      GPIO_MODER_MODER4);

    GPIOC->MODER |= GPIO_MODER_MODER0_0 |
                    GPIO_MODER_MODER1_0 |
                    GPIO_MODER_MODER2_0 |
                    GPIO_MODER_MODER3_0 |
                    GPIO_MODER_MODER4_0;

    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR0_1 |
                    GPIO_PUPDR_PUPDR1_1 |
                    GPIO_PUPDR_PUPDR2_1 |
                    GPIO_PUPDR_PUPDR3_1 |
                    GPIO_PUPDR_PUPDR4_1;
}

void led1ON(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BS0;
}

void led1OFF(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BR0;
}

void led1TOGGLE(void)
{
    GPIOC->ODR ^= GPIO_ODR_OD0;
}

void led2ON(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BS1;
}

void led2OFF(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BR1;
}

void led2TOGGLE(void)
{
    GPIOC->ODR ^= GPIO_ODR_OD1;
}

void led3ON(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BS2;
}

void led3OFF(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BR2;
}

void led3TOGGLE(void)
{
    GPIOC->ODR ^= GPIO_ODR_OD2;
}

void led4ON(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BS3;
}

void led4OFF(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BR3;
}

void led4TOGGLE(void)
{
    GPIOC->ODR ^= GPIO_ODR_OD3;
}

void led5ON(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BS4;
}

void led5OFF(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BR4;
}

void led5TOGGLE(void)
{
    GPIOC->ODR ^= GPIO_ODR_OD4;
}

void ledsSet(void)
{
    if (armed) // 1 - Armed
        led1ON();
    else
        led1OFF();
    if (systemReady) // 2 - System Initialized
        led2ON();
    else
        led2OFF();
    if (rcActive) // 3 - Receiver Connected
        led3ON();
    else
        led3OFF();
    if (battLow) // 4 - Battery Low
        led4TOGGLE();
    else
        led4OFF();
    if (mode == FLIGHT) // 5(toggle) - Flight Mode
        led5TOGGLE();
    else if (mode == ROVER) // 5(on) - Rover Mode
        led5ON();
    else
        led5OFF();
}