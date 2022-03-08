/*
  ******************************************************************************
  * @file           : genericDefines.h
  * @author         : Enrico
  * @date           : 27 ago 2021
  ******************************************************************************
*/

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


