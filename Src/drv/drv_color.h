/** @file 		drv_color.h
 *  @brief
 *  	This file is used to change the output color to the terminal
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 22
 *  @bug
 */

#ifndef __COLOR_H__
#define __COLOR_H__

/* Global Enums */
enum color
{
	BLACK,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	PURPLE,
	CYAN,
	WHITE
};

enum bf
{
	NO,
	YES
};

/* Function Prototypes */
void color(uint8_t color, uint8_t bf);
void colorDefault(void);

#endif /* __COLOR_H__ */
