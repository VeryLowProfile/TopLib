/*
  ******************************************************************************
  * @file           : genericDefines.h
  * @author         : Enrico
  * @date           : 27 ago 2021
  ******************************************************************************
*/

#ifndef TOPLIB_INC_GENERICDEFINES_H_
#define TOPLIB_INC_GENERICDEFINES_H_

/* Includes ------------------------------------------------------------------*/

/* Defines -----------------------------------------------------------*/
typedef uint8_t BOOL;

#ifndef TRUE
	#define TRUE            1
#endif

#ifndef FALSE
	#define FALSE           0
#endif

//Generic
#define NO 0;
#define YES 1;

//Clock
#define SYS_CLOCK 170000000
#define SYS_PERIOD (float)1/(float)170000000

//Generic GPIO
#define OFF 0
#define ON 1

#define PORT_A GPIOA
#define PORT_B GPIOB
#define PORT_C GPIOC
#define PORT_D GPIOD
#define PORT_E GPIOE
#define PORT_F GPIOF
#define PORT_G GPIOG

#define PIN_0 0
#define PIN_1 2
#define PIN_2 4
#define PIN_3 8
#define PIN_4 16
#define PIN_5 32
#define PIN_6 64
#define PIN_7 128
#define PIN_8 256
#define PIN_9 512
#define PIN_10 1024
#define PIN_11 2048
#define PIN_12 4096
#define PIN_13 8192
#define PIN_14 16384
#define PIN_15 32768

/* Declarations -----------------------------------------------------------*/

#endif /* TOPLIB_INC_GENERICDEFINES_H_ */
