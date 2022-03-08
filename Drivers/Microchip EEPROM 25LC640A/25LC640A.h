/**
  ******************************************************************************
  * @file           : 25LC640A.h
  * @author         : Davide
  * @date           : 27 gen 2022
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Defines -----------------------------------------------------------*/
//CS config: PB6
#define GPIO_PORT				GPIOB
#define GPIO_PIN_NB				GPIO_PIN_6

//PROTOCOL
#define INSTRUCTION_BYTES 		1			//8bit instruction
#define ADDR_BYTES 				2			//16bit addr
#define DATA_BYTES 				1   		//8bit data

/* Declarations -----------------------------------------------------------*/
typedef enum
{
	NO_ERROR,
	WRITE_ERROR,
	WRITE_TOO_MUCH_BYTES,
	READ_ERROR

} ErrorCode_EEPROM;

//TO BE CONFIGURED: it defines the dimension of the data to be processed
#define MAX_ENB_OBJ 					16 																	//max number of object to be stored
#define MAX_ENB_PROP					32   																//max number of properties for every object to be stored

typedef uint8_t EEPROM_DATATYPE[MAX_ENB_OBJ][MAX_ENB_PROP]; 												//Define data structure for data to be written

/* Prototypes -----------------------------------------------------------*/
uint8_t readRegEEPROM ( void );
void writeEEPROM_Init_CS( void );
uint8_t readEEPROM_SingleValue( uint16_t address );
ErrorCode_EEPROM writeEEPROM_SingleValue( uint16_t address, uint8_t dataTransmitted );
ErrorCode_EEPROM writeEEPROM_MultipleValues( uint16_t * ptr_address,  uint8_t * ptr_data, uint16_t Nb );
uint8_t * readEEPROM_MultipleValues( uint16_t * ptr_address, uint16_t Nb );
ErrorCode_EEPROM writeEEPROM_SingleFloat( uint16_t * ptr_address,  float a);
float readEEPROM_SingleFloat( uint16_t * ptr_address);

