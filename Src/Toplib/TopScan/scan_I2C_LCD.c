/**
  ******************************************************************************
  * @file           : scan_I2C_LCD.c
  * @author         : Enrico
  * @date           : 22 dic 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "scan_I2C_LCD.h"
#include "stm32g4xx_hal.h"

/* Body ------------------------------------------------------------------*/
void Lcd_Init (void)
{
	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	Lcd_Send_Cmd (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	Lcd_Send_Cmd (0x30);
	HAL_Delay(1);  // wait for >100us
	Lcd_Send_Cmd (0x30);
	HAL_Delay(10);
	Lcd_Send_Cmd (0x20);  // 4bit mode
	HAL_Delay(10);

  // dislay initialisation
	Lcd_Send_Cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	Lcd_Send_Cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	Lcd_Send_Cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	Lcd_Send_Cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	Lcd_Send_Cmd ( 0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

//void Lcd_Send_Cmd (char cmd)
//{
//  char data_u, data_l;
//	uint8_t data_t[4];
//	data_u = (cmd&0xf0);
//	data_l = ((cmd<<4)&0xf0);
//	data_t[0] = data_u|0x0C;  //en=1, rs=0
//	data_t[1] = data_u|0x08;  //en=0, rs=0
//	data_t[2] = data_l|0x0C;  //en=1, rs=0
//	data_t[3] = data_l|0x08;  //en=0, rs=0
//	HAL_I2C_Master_Transmit (&LCD_I2C_HANDLE, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
//}

void Lcd_Set_Cursor(int Row, int Col)
{
  switch(Row)
  {
     case 1:
    	 Lcd_Send_Cmd(0x80 + Col-1);
	  break;
     case 2:
    	 Lcd_Send_Cmd(0xC0 + Col-1);
      break;
     case 3:
    	 Lcd_Send_Cmd(0x94 + Col-1);
      break;
     case 4:
    	 Lcd_Send_Cmd(0xD4 + Col-1);
      break;
  }
}

void Lcd_Send_Char (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1
	data_t[1] = data_u|0x09;  //en=0, rs=1
	data_t[2] = data_l|0x0D;  //en=1, rs=1
	data_t[3] = data_l|0x09;  //en=0, rs=1
	HAL_I2C_Master_Transmit (&LCD_I2C_HANDLE, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void Lcd_Send_String (char *str)
{
	while (*str) Lcd_Send_Char(*str++);
}

void Lcd_Clear(void)
{
	Lcd_Send_Cmd(LCD_CLEAR);
}
