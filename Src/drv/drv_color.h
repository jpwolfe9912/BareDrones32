/*
 * drv_color.h
 *
 *  Created on: Mar 2, 2022
 *      Author: jeremywolfe
 */

#ifndef SRC_DRV_DRV_COLOR_H_
#define SRC_DRV_DRV_COLOR_H_

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

#endif /* SRC_DRV_DRV_COLOR_H_ */
