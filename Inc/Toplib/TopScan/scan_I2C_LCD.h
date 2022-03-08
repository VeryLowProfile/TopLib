/**
  ******************************************************************************
  * @file           : scan_I2C_LCD.h
  * @author         : Enrico
  * @date           : 22 dic 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "genericDefines.h"
#include "stm32g4xx_hal.h"

/* Defines -----------------------------------------------------------*/
//Config
#define LCD_I2C_HANDLE        hi2c1

#define SLAVE_ADDRESS_LCD     0x4E
#define LCD_TYPE              2 // 0 -> 5x7 | 1 -> 5x10 | 2 -> 2 lines
#define LCD_COLUMNS		      20
#define LCD_ROWS			  4

//Command codes
#define LCD_BACKLIGHT         0x08
#define LCD_NOBACKLIGHT       0x00
#define LCD_CLEAR             0x01
#define LCD_RETURN_HOME       0x02
#define LCD_ENTRY_MODE_SET    0x04
#define LCD_CURSOR_OFF        0x0C
#define LCD_UNDERLINE_ON      0x0E
#define LCD_BLINK_CURSOR_ON   0x0F
#define LCD_MOVE_CURSOR_LEFT  0x10
#define LCD_MOVE_CURSOR_RIGHT 0x14
#define LCD_TURN_ON           0x0C
#define LCD_TURN_OFF          0x08
#define LCD_SHIFT_LEFT        0x18
#define LCD_SHIFT_RIGHT       0x1E

//Variables
extern SPI_HandleTypeDef LCD_I2C_HANDLE;

//Functions
void Lcd_Init (void);
void Lcd_Send_Cmd (char cmd);
void Lcd_Set_Cursor(int Row, int Col);
void Lcd_Send_Char (char data);
void Lcd_Send_String (char *str);
void Lcd_Clear(void);
