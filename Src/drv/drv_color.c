/** @file 		drv_color.c
 *  @brief
 *  	This file is used to change the output color to the terminal
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 22
 */

/* Includes */
#include "board.h"

/** @brief Changes color of print text
 *
 *  @param uint8_t color The color you want to change to.
 *  @param uint8_t bf If you want bf or not.
 *  @return Void.
 */
void
color(color_e color, bf_e bf)
{
	printf("\033[%d;3%dm", bf, color);
}

/** @brief Changes color of print to white with no bf
 *
 *  @return Void.
 */
void
colorDefault(void)
{
	printf("\033[0;37m");
}

/** @brief Draws "AUTODRONE" in different colors.
 *
 *  @return Void.
 */
void
drawAutodrone(void)
{
	color(RED, YES);
	printf("\n    __       _     _     _______      _____     ");
	printf("_______     _______       _____      __    _     _______  ");
	color(PURPLE, YES);
	printf("\n   /  \\     | |   | |   |_______|    /  _  \\    ");
	printf("|   _  \\    |   _  \\     /  _  \\    |  \\  | |   |   ____| ");
	color(BLUE, YES);
	printf("\n  /    \\    | |   | |      | |      /  / \\  \\   ");
	printf("|  | \\  |   |  | \\  |   /  / \\  \\   |   \\ | |   |  |      ");
	color(CYAN, YES);
	printf("\n /  /\\  \\   | |   | |      | |      |  | |  |   ");
	printf("|  | |  |   |  |_/  /   |  | |  |   |    \\| |   |  |____  ");
	color(CYAN, YES);
	printf("\n/  /__\\  \\  | |   | |      | |      |  | |  |   ");
	printf("|  | |  |   |      /    |  | |  |   | |\\    |   |   ____| ");
	color(BLUE, YES);
	printf("\n|   __   |  | |   | |      | |      |  | |  |   ");
	printf("|  | |  |   |  |\\  \\    |  | |  |   | | \\   |   |  |      ");
	color(PURPLE, YES);
	printf("\n|  |  |  |  \\ \\___/ /      | |      \\  \\_/  /   ");
	printf("|  |_/  /   |  | \\  \\   \\  \\_/  /   | |  \\  |   |  |____  ");
	color(RED, YES);
	printf("\n|__|  |__|   \\_____/       | |       \\_____/    ");
	printf("|______/    |__| |__|    \\_____/    |_|   \\_|   |_______| ");
	colorDefault();
}
