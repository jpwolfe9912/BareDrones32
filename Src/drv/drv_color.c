/** @file 		drv_color.c
 *  @brief 
 *
 *  @author 	jeremywolfe
 *  @date 		Mar 2, 2022
 *  @bug 
 */

#include "board.h"

void color(uint8_t color, uint8_t bf)
{
	printf("\033[%d;3%dm", bf, color);
}

//void yellow(void)
//{
//	printf("\033[1;33m");
//}
//
//void yellow(void)
//{
//	printf("\033[1;33m");
//}
//
//void yellow(void)
//{
//	printf("\033[1;33m");
//}
//
//void yellow(void)
//{
//	printf("\033[1;33m");
//}
//
//void yellow(void)
//{
//	printf("\033[1;33m");
//}
