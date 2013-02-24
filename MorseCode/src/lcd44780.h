#ifndef _LCD44780_H
#define _LCD44780_H

#include "lpc17xx.h"

#define LCDGPIO LPC_GPIO2		//Change this to change the gpio IO port
#define LCDDATA LPC_GPIO0		//Change this to change the data IO port
#define MAX_CHAR_IN_ONE_LINE 24

#define LCD_BACK_LIGHT_TIMEOUT 1000

#define LCD_BACKLIGHT (1 << 29)

#define LCD_BACK_LIGHT_DIR LPC_GPIO4->FIODIR
#define LCD_BACK_LIGHT_SET LPC_GPIO4->FIOSET
#define LCD_BACK_LIGHT_CLR LPC_GPIO4->FIOCLR

#define LCD_DATA_DIR	   LCDDATA->FIODIR
#define LCD_DATA_SET	   LCDDATA->FIOSET
#define LCD_DATA_CLR	   LCDDATA->FIOCLR

#define LCD_CTRL_DIR	   LCDGPIO->FIODIR
#define LCD_CTRL_SET       LCDGPIO->FIOSET
#define LCD_CTRL_CLR       LCDGPIO->FIOCLR

#define LCDRS	           (1 << 3)				//Change this to change the RS data pin
#define LCDRW	           (1 << 4)				//Change this to change the RW data pin
#define LCDEN	           (1 << 5)				//Change this to change the EN data pin
#define LCD_D4 (1 << 4)							//Change this to change the D4 gpio pin
#define LCD_D5 (1 << 5)							//Change this to change the D5 gpio pin
#define LCD_D6 (1 << 6)							//Change this to change the D6 gpio pin
#define LCD_D7 (1 << 7)							//Change this to change the D7 gpio pin
#define LCD_DATA_MASK           (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7)
#define LCD_BUSY_FLAG           LCD_D7

//#define LCD_CONTROL_MASK        0x01C00000

enum ROW_NUMBERS {
  LINE1, LINE2
};

void init_lcd(void);
void lcd_putstring(unsigned char line, char *string);
void lcd_clear(void);
void lcd_backlight_on(void);
void lcd_backlight_off(void);
int lcd_gotoxy(unsigned int x, unsigned int y);
void lcd_putchar(int c);
void lcd_command_write(unsigned char command);

#endif
