/** @file 		drv_color.c
 *  @brief
 *  	This file is used to change the output color to the terminal
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 22
 *  @bug 
 */

/* Includes */
#include "board.h"

/* Functions */

/** @brief Changes color of print text
 *
 *  @param color The color you want to change to.
 *  @param bf If you want bf or not
 *  @return Void.
 */
void
color(uint8_t color, uint8_t bf)
{
	printf("\033[%d;3%dm", bf, color);
}

/** @brief Changes color of print back to white with no bf
 *
 *  @return Void.
 */
void
colorDefault(void)
{
	printf("\033[0;37m");
}
