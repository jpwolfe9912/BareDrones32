/*
 * drv_color.h
 *
 *  Created on: Mar 2, 2022
 *      Author: jeremywolfe
 */

#ifndef __COLOR_H__
#define __COLOR_H__

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

void color(uint8_t color, uint8_t bf);
void colorDefault(void);

#endif /* __COLOR_H__ */
