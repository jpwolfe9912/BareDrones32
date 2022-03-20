/** @file 		drv_led.h
 *  @brief
 *  	This file contains functions to activate the 5 LEDs onboard.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		19 MAR 2022
 */

#ifndef __LED_H__
#define __LED_H__

/* Function Prototypes */
void ledInit(void);

void led1ON(void);
void led1OFF(void);
void led1TOGGLE(void);

void led2ON(void);
void led2OFF(void);
void led2TOGGLE(void);

void led3ON(void);
void led3OFF(void);
void led3TOGGLE(void);

void led4ON(void);
void led4OFF(void);
void led4TOGGLE(void);

void led5ON(void);
void led5OFF(void);
void led5TOGGLE(void);

#endif /* __LED_H__ */
