/**
 ******************************************************************************
 * @file           : 25LC640A.c
 * @author         : Davide
 * @date           : 27 gen 2022
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "25LC640A.h"

extern SPI_HandleTypeDef hspi1;

//SPI
#define SPI_HANDLE						hspi1
#define SPI_TIMEOUT 				    100

// instructions 25LC640A
const uint8_t EEPROM_READ = 0b00000011;
const uint8_t EEPROM_WRITE = 0b00000010;
const uint8_t EEPROM_WRDI = 0b00000100;
const uint8_t EEPROM_WREN = 0b00000110;
const uint8_t EEPROM_RDSR = 0b00000101;
const uint8_t EEPROM_WRSR = 0b00000001;

//-----------------FUNCTIONS--------------------------------------------------//

uint8_t readRegEEPROM ( void ){

	uint8_t regData = 0;

	// CS pin should default high
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_SET);

	// Read status register
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&SPI_HANDLE, (uint8_t *)&EEPROM_RDSR, INSTRUCTION_BYTES, SPI_TIMEOUT);
	HAL_SPI_Receive(&SPI_HANDLE, &regData, DATA_BYTES, SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_SET);

	return regData;

}


void writeEEPROM_Init_CS( void ){

	// CS pin should default high
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_SET);

}

uint8_t readEEPROM_SingleValue( uint16_t address ){

	// Variables declaration
	uint8_t wip_bit = 1;
	uint8_t regData = 0;
	uint8_t dataRead = 0;

	// Wait until WIP bit is cleared
	while (wip_bit)
	{
		// Read status register
		regData = readRegEEPROM();

		// Mask out WIP bit
		wip_bit = regData & 0b00000001;
	}

	// Read the byte back
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&SPI_HANDLE, (uint8_t *)&EEPROM_READ, INSTRUCTION_BYTES, SPI_TIMEOUT);
	HAL_SPI_Transmit(&SPI_HANDLE, (uint8_t *)&address, ADDR_BYTES, SPI_TIMEOUT);
	HAL_SPI_Receive(&SPI_HANDLE, &dataRead, DATA_BYTES, SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_SET);

	return dataRead;

}


ErrorCode_EEPROM writeEEPROM_SingleValue( uint16_t address, uint8_t dataTransmitted ){

	ErrorCode_EEPROM error = NO_ERROR;
	uint8_t wip_bit = 1;
	uint8_t regData = 0;

	// Wait until WIP bit is cleared
	while (wip_bit)
	{
		// Read status register
		regData = readRegEEPROM();

		// Mask out WIP bit
		wip_bit = regData & 0b00000001;
	}

	// Write ENABLE
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&SPI_HANDLE, (uint8_t *)&EEPROM_WREN, INSTRUCTION_BYTES, SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_SET);

	//WRITE
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&SPI_HANDLE, (uint8_t *)&EEPROM_WRITE, INSTRUCTION_BYTES, SPI_TIMEOUT);
	HAL_SPI_Transmit(&SPI_HANDLE, (uint8_t *)&address, ADDR_BYTES, SPI_TIMEOUT);
	HAL_SPI_Transmit(&SPI_HANDLE, &dataTransmitted, DATA_BYTES, SPI_TIMEOUT);
	HAL_GPIO_WritePin(GPIO_PORT, GPIO_PIN_NB, GPIO_PIN_SET);


	if (readEEPROM_SingleValue(address) != dataTransmitted){
		error = WRITE_ERROR;
	}

	return error;

}



ErrorCode_EEPROM writeEEPROM_MultipleValues( uint16_t * ptr_address,  uint8_t * ptr_data, uint16_t Nb ){

	ErrorCode_EEPROM error = NO_ERROR;
	ErrorCode_EEPROM e = NO_ERROR;

	if(Nb<32){
		//WRITEMULTIPLE
		for (uint16_t i=0; i<Nb; i++){
			e = writeEEPROM_SingleValue(*(ptr_address+i),*(ptr_data+i));
			if(e != NO_ERROR){
				error = WRITE_ERROR;
			}
		}
	}
	else {
		error = WRITE_TOO_MUCH_BYTES;
	}

	return error;

}



ErrorCode_EEPROM writeEEPROM_SingleFloat( uint16_t * ptr_address,  float a){

	ErrorCode_EEPROM error = NO_ERROR;
	ErrorCode_EEPROM e = NO_ERROR;
	uint8_t a8[sizeof(float)];

	//FROM FLOAT TO BYTES
	union {
		float f;
		char c[sizeof(float)]; // Edit: changed to this
	} u;

	u.f = a;
	for ( int i = sizeof(float) -1 ; i > -1 ; i-- ){
		a8[i] = u.c[i] & 0x00FF;                               //a8[0]=LSB; a8[3]=MSB
	}

	//WRITEMULTIPLE
	for (uint16_t i=0; i<sizeof(float); i++){
		e = writeEEPROM_SingleValue(*(ptr_address+i),a8[sizeof(float)-i]);   //WRITING THEM FROM MSB to LSB
		if(e != NO_ERROR){
			error = WRITE_ERROR;
		}

	}

	return error;

}


float readEEPROM_SingleFloat( uint16_t * ptr_address){

	float a;
	int8_t a8[sizeof(float)];

	for (uint16_t i=0; i<sizeof(float); i++){
		a8[sizeof(float)-i-1] = 0;
		a8[sizeof(float)-i-1] = readEEPROM_SingleValue(*(ptr_address+i+1));
	}

	//FROM BYTES TO FLOAT
	union {
		float f;
		char b[4];
	} u;

	u.b[3] = a8[3];
	u.b[2] = a8[2];
	u.b[1] = a8[1];
	u.b[0] = a8[0];

	a= u.f;

	return a;


}


uint8_t * readEEPROM_MultipleValues( uint16_t * ptr_address, uint16_t Nb ){

	uint8_t * ptr_data;
	uint8_t dataBuffer[Nb];

	for (uint16_t i=0; i<Nb; i++){
		dataBuffer[i] = readEEPROM_SingleValue(*(ptr_address+i));
	}

	ptr_data = dataBuffer;

	return ptr_data;

}


