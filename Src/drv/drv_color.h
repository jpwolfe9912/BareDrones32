/** @file 		drv_color.h
 *  @brief
 *  	This file is used to change the output color to the terminal
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 22
 */

#ifndef __COLOR_H__
#define __COLOR_H__

/* Global Enums */
typedef enum
{
	BLACK,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	PURPLE,
	CYAN,
	WHITE
}color_e;

typedef enum
{
	NO,
	YES
}bf_e;

/* Function Prototypes */
void color(uint8_t color, uint8_t bf);
void colorDefault(void);
void drawAutodrone(void);

#endif /* __COLOR_H__ */
