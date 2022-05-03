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
		delay(100);
		printf("_______     _______       _____      __    _     _______  ");
		delay(100);
		color(PURPLE, YES);
		printf("\n   /  \\     | |   | |   |_______|    /  _  \\    ");
		delay(100);
		printf("|  __  \\    |   _  \\     /  _  \\    |  \\  | |   |   ____| ");
		delay(100);
		color(BLUE, YES);
		printf("\n  /    \\    | |   | |      | |      /  / \\  \\   ");
		delay(100);
		printf("|  | \\  |   |  | \\  |   /  / \\  \\   |   \\ | |   |  |      ");
		delay(100);
		color(CYAN, YES);
		printf("\n /  /\\  \\   | |   | |      | |      |  | |  |   ");
		delay(100);
		printf("|  | |  |   |  |_/  /   |  | |  |   |    \\| |   |  |____  ");
		delay(100);
		color(CYAN, YES);
		printf("\n/  /__\\  \\  | |   | |      | |      |  | |  |   ");
		delay(100);
		printf("|  | |  |   |      /    |  | |  |   | |\\    |   |   ____| ");
		delay(100);
		color(BLUE, YES);
		printf("\n|   __   |  | |   | |      | |      |  | |  |   ");
		delay(100);
		printf("|  | |  |   |  |\\  \\    |  | |  |   | | \\   |   |  |      ");
		delay(100);
		color(PURPLE, YES);
		printf("\n|  |  |  |  \\ \\___/ /      | |      \\  \\_/  /   ");
		delay(100);
		printf("|  |_/  /   |  | \\  \\   \\  \\_/  /   | |  \\  |   |  |____  ");
		delay(100);
		color(RED, YES);
		printf("\n|__|  |__|   \\_____/       |_|       \\_____/    ");
		delay(100);
		printf("|______/    |__| |__|    \\_____/    |_|   \\_|   |_______| ");
		delay(100);
		colorDefault();
}
