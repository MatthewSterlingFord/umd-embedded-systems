#include "lcd44780.h"

/****************************************************************************
 Function Name : delay()
 Description :This function suspends the tasks for specified ticks.	
 Input :  ticks:no of ticks in multiple of 1 usec
 task: task to be suspended
 Output : void
 Note :
 *****************************************************************************/
void delay(int count) {
  int j = 0, i = 0;
  for (j = 0; j < count; j++) {
    /* At 60Mhz, the below loop introduces delay of 10 us */
    for (i = 0; i < 1200; i++) {
      __NOP();
    }
  }
}

/*******************************************************************************************
 Function Name :	wait_lcd()
 Description   :	
 Input         :	
 Output        :	Void
 Note          :
 ****************************************************************************************/
void wait_lcd(void) {/*
 LCD_DATA_DIR &= ~LCD_DATA_MASK;
 LCD_CTRL_CLR |=  LCDRS;	
 LCD_CTRL_SET |=  LCDRW |LCDEN;
 while(LPC_GPIO0->FIOPIN & LCD_BUSY_FLAG);
 
 LCD_CTRL_CLR |= LCDEN | LCDRW;
 LCD_DATA_DIR |= LCD_DATA_MASK;
 */
  delay(500);
}

/******************************************************************************************
 Function Name :	lcd_command_write()
 Description   :	
 Input         :	
 Output        :	Void
 Note          :
 ****************************************************************************************/
#define DATA_POS 4
#define DELAY_COUNT 100
void lcd_write_nibble(unsigned char command) {
  if (command & 1) {
    LCD_DATA_SET |= LCD_D4;
  } else {
    LCD_DATA_CLR |= LCD_D4;
  }

  if (command & 2) {
    LCD_DATA_SET |= LCD_D5;
  } else {
    LCD_DATA_CLR |= LCD_D5;
  }

  if (command & 4) {
    LCD_DATA_SET |= LCD_D6;
  } else {
    LCD_DATA_CLR |= LCD_D6;
  }

  if (command & 8) {
    LCD_DATA_SET |= LCD_D7;
  } else {
    LCD_DATA_CLR |= LCD_D7;
  }

  LCD_CTRL_CLR |= LCDEN;
  delay(2);
  LCD_CTRL_SET |= LCDEN;
  delay(10);
  LCD_CTRL_CLR |= LCDEN;
  delay(300);
}

void lcd_command_write(unsigned char command) {
  LCD_CTRL_CLR |= LCDRS;
  lcd_write_nibble(command >> 4);

  delay(1);
  lcd_write_nibble(command);

  delay(1);
  wait_lcd();
}

void lcd_command_write2(unsigned char command) {
  unsigned char temp = 0;
  unsigned int temp1 = 0;

  temp = (command >> 4) & 0x0F;
  temp1 = (temp << DATA_POS);

  LCD_CTRL_CLR = LCDRS;
  LCD_CTRL_SET = LCDEN;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1 & LCD_DATA_MASK;
  delay(DELAY_COUNT);
  LCD_CTRL_CLR = LCDEN;

  temp = command & 0x0F;
  temp1 = (temp << DATA_POS) & LCD_DATA_MASK;
  delay(DELAY_COUNT);

  LCD_CTRL_CLR |= LCDRS;
  LCD_CTRL_SET |= LCDEN;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay(DELAY_COUNT);
  LCD_CTRL_CLR |= LCDEN;
  wait_lcd();
}

/*******************************************************************************************
 Function Name :	set_lcd_port_output()
 Description   :	
 Input         :	
 Output        :	Void
 Note          :
 *********************************************************************************************/
void set_lcd_port_output(void) {
  LCD_CTRL_DIR |= (LCDEN | LCDRS | LCDRW | LCD_BACKLIGHT);
  LCD_CTRL_CLR |= (LCDEN | LCDRS | LCDRW | LCD_BACKLIGHT);
  LCD_DATA_DIR |= LCD_DATA_MASK;
}

/*****************************************************************************************
 Function Name :	lcd_clear()
 Description   :	
 Input         :	
 Output        :	Void
 Note          :
 *****************************************************************************************/
void lcd_clear(void) {
  lcd_command_write(0x01);
}

/*******************************************************************************************
 Function Name :	lcd_gotoxy()
 Description   :	
 Input         :
 Output        :	Void
 Note          :
 *********************************************************************************************/
int lcd_gotoxy(unsigned int x, unsigned int y) {
  int retval = 0;

  if ((x > 1) && (y > 15)) {
    retval = -1;
  } else {
    if (x == 0) {
      lcd_command_write(0x80 + y); /* command - position cursor at 0x00 (0x80 + 0x00 ) */
    } else if (x == 1) {
      lcd_command_write(0xC0 + y); /* command - position cursor at 0x40 (0x80 + 0x00 ) */
    }
  }
  return retval;
}

/*******************************************************************************************
 Function Name :	lcd_data_write()
 Description   :	
 Input         :	
 Output        :	Void
 Note          :
 ********************************************************************************************/
void lcd_data_write(unsigned char data) {
  LCD_CTRL_SET |= LCDRS;
  lcd_write_nibble(data >> 4);

  delay(1);
  lcd_write_nibble(data & 0x0F);

  delay(1);
  wait_lcd();

  return;

  unsigned char temp = 0;
  unsigned int temp1 = 0;

  temp = data;
  temp = (temp >> 4) & 0x0F;
  temp1 = (temp << DATA_POS) & LCD_DATA_MASK;

  LCD_CTRL_SET |= LCDEN | LCDRS;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET |= temp1;
  LCD_CTRL_CLR |= LCDEN;

  temp = data;
  temp &= 0x0F;
  temp1 = (temp << DATA_POS) & LCD_DATA_MASK;

  LCD_CTRL_SET |= LCDEN | LCDRS;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET |= temp1;
  LCD_CTRL_CLR |= LCDEN;
  wait_lcd();
}

/**********************************************************************************************
 Function Name :	lcd_putchar()
 Description   :	
 Input         :	
 Output        :	Void
 Note          :
 *********************************************************************************************/
void lcd_putchar(int c) {
  lcd_data_write(c);
}

/**********************************************************************************************
 Function Name :	lcd_putstring()
 Description   :	
 Input         :	
 Output        :	Void
 Note          :
 *********************************************************************************************/
void lcd_putstring(unsigned char line, char *string) {
  unsigned char len = MAX_CHAR_IN_ONE_LINE;

  lcd_gotoxy(line, 0);
  while (*string != '\0' && len--) {
    lcd_putchar(*string);
    string++;
  }
}

/**********************************************************************************************
 Function Name :	lcd_backlight_on()
 Description   :	
 Input         :	
 Output        :	Void
 Note          :
 *********************************************************************************************/
void lcd_backlight_on() {
  LCD_BACK_LIGHT_DIR |= LCD_BACKLIGHT;
  LCD_BACK_LIGHT_SET |= LCD_BACKLIGHT;
}

/*********************************************************************************************
 Function Name : turn_off_lcd_back_light()
 Description   :	
 Input         :	
 Output        : Void
 Note          :
 *********************************************************************************************/
void lcd_backlight_off(void) {
  LCD_BACK_LIGHT_DIR |= LCD_BACKLIGHT;
  LCD_BACK_LIGHT_CLR |= LCD_BACKLIGHT;
}

/**********************************************************************************************
 Function Name :	init_lcd()
 Description   :	
 Input         :	
 Output        :	Void
 Note          :
 *********************************************************************************************/
void init_lcd(void) {
  set_lcd_port_output();
  lcd_write_nibble(0x03);
  delay(450);
  lcd_write_nibble(0x03);
  delay(450);
  lcd_write_nibble(0x03);
  delay(180);

  lcd_write_nibble(0x02);
  delay(10);

  lcd_command_write(0x28); /*   4-bit interface, two line, 5X7 dots.   */
//  lcd_clear() ;                /*   LCD clear                            */
  lcd_command_write(0x0C); /*   display on      */
  lcd_command_write(0x02); /*   cursor home                      */
  lcd_command_write(0x06); /*   cursor move direction       */

  lcd_gotoxy(0, 0);
  lcd_clear();
}
