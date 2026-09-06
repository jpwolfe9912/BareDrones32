/** @file 		drv_led.c
 *  @brief
 *  	This file contains functions to activate the 5 LEDs onboard.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		19 MAR 2022
 */

#include "drv_led.h"

#include "stm32f7xx.h"
#include "drv_system.h"
#include "process_commands.h"

void ledInit(void)
{
    /*
     * 	LEDs are PC3-2
     */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    GPIOC->MODER &= ~(GPIO_MODER_MODER2 |
                      GPIO_MODER_MODER3);

    GPIOC->MODER |= GPIO_MODER_MODER2_0 |
                    GPIO_MODER_MODER3_0;

    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR2_1 |
                    GPIO_PUPDR_PUPDR3_1;
}

void led1ON(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BS3;
}

void led1OFF(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BR3;
}

void led1TOGGLE(void)
{
    GPIOC->ODR ^= GPIO_ODR_OD3;
}

void led2ON(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BS2;
}

void led2OFF(void)
{
    GPIOC->BSRR |= GPIO_BSRR_BR2;
}

void led2TOGGLE(void)
{
    GPIOC->ODR ^= GPIO_ODR_OD2;
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
}