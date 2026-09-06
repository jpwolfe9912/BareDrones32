/** @file 		drv_color.c
 *  @brief
 *  	This file is used to change the output color to the terminal
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 22
 */

/* Includes */
#include "drv_color.h"

#include "drv_system.h"
#include "drv_printf.h"

/** @brief Changes color of print text
 *
 *  @param uint8_t color The color you want to change to.
 *  @param uint8_t bf If you want bf or not.
 *  @return Void.
 */
void color(color_e color, bf_e bf)
{
	printf("\033[%d;3%dm", bf, color);
}

/** @brief Changes color of print to white with no bf
 *
 *  @return Void.
 */
void colorDefault(void)
{
	printf("\033[0;37m");
}

/** @brief Draws "AUTODRONE" in different colors.
 *
 *  @return Void.
 */
void drawAutodrone(void)
{
	color(RED, YES);
	printf("\n               ,---.                      ,----.                             _,.---._    .-._           ,----.    ,-,--.  ");
	delay(100);
	color(YELLOW, YES);
	printf("\n    _..---.  .--.'  \\      .-.,.---.   ,-.--` , \\  _,..---._   .-.,.---.   ,-.' , -  `. /==/ \\  .-._ ,-.--` , \\ ,-.'-  _\\ ");
	delay(100);
	color(GREEN, YES);
	printf("\n  .' .'.-. \\ \\==\\-/\\ \\    /==/  `   \\ |==|-  _.-`/==/,   -  \\ /==/  `   \\ /==/_,  ,  - \\|==|, \\/ /, /==|-  _.-`/==/_ ,_.' ");
	delay(100);
	color(CYAN, YES);
	printf("\n /==/- '=' / /==/-|_\\ |  |==|-, .=., ||==|   `.-.|==|   _   _\\==|-, .=., |==|   .=.     |==|-  \\|  ||==|   `.-.\\==\\  \\    ");
	delay(100);
	color(BLUE, YES);
	printf("\n |==|-,   '  \\==\\,   - \\ |==|   '='  /==/_ ,    /|==|  .=.   |==|   '='  /==|_ : ;=:  - |==| ,  | -/==/_ ,    / \\==\\ -\\   ");
	delay(100);
	color(CYAN, YES);
	printf("\n |==|  .=. \\ /==/ -   ,| |==|- ,   .'|==|    .-' |==|,|   | -|==|- ,   .'|==| , '='     |==| -   _ |==|    .-'  _\\==\\ ,\\  ");
	delay(100);
	color(GREEN, YES);
	printf("\n /==/- '=' ,/==/-  /\\ - \\|==|_  . ,'.|==|_  ,`-._|==|  '='   /==|_  . ,'. \\==\\ -    ,_ /|==|  /\\ , |==|_  ,`-._/==/\\/ _ | ");
	delay(100);
	color(YELLOW, YES);
	printf("\n|==|   -   /\\==\\ _.\\=\\.-'/==/  /\\ ,  )==/ ,     /|==|-,   _`//==/  /\\ ,  ) '.='. -   .' /==/, | |- /==/ ,     /\\==\\ - , / ");
	delay(100);
	color(RED, YES);
	printf("\n`-._`.___,'  `--`        `--`-`--`--'`--`-----`` `-.`.____.' `--`-`--`--'    `--`--''   `--`./  `--`--`-----``  `--`---'  ");
	colorDefault();
}
