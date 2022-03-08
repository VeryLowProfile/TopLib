/**
  ******************************************************************************
  * @file           : flash.h
  * @author         : Davide
  * @date           : 21 gen 2022
  ******************************************************************************
*/

//-----------------INFORMATION------------------------------------------------//
//
// FLASH_START_ADDR (OFFSET = PAGE 50 -> address 0x08019000)
//
// 0x08019000 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF   ^
// 0x08019020 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF	|
// 0x08019040 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF	|   PAGE 50
// ...																					|	1 Page every 800
// 0x08019780 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF	|_  the last byte is at the address 0x08019800-1 = 0x080197FF
//
// 0x08019800 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF   ^
// 0x08019820 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF   |
// 0x08019840 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF	|	PAGE 51
// ...																					|	1 Page every 800
// 0x08019FE0 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF   |_  the last byte is at the address 0x0801A000-1 = 0x08019FFF
//
// 0x0801A000 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF   ^
// 0x0801A020 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF   |
// 0x0801A040 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF   |   PAGE 52
// ...
//
//----------------------------------------------------------------------------//
// This library has this function:
//----------------------------------------------------------------------------//
//
// (1)
// FlashError FLASH_Init( void );
// Initialize the flash operation
//
// (2)
// FlashError FLASH_ErasePage_with_address(uint32_t address);
// Erase the page at the address inserted
// ex: FLASH_ErasePage_with_address(FLASH_START_ADDR + (0x800)*k): address MUST be in this format
//
// (3)
// FlashError FLASH_ErasePage_with_pageNb(uint32_t pageNb);
// Erase the page pageNb
//
// (4)
// FlashError FLASH_EraseAll( void );
// Erase all the pages from FLASH_MAX_PAGE to FLASH_PAGE_OFFSET
//
// (5)
// FlashError FLASH_WriteSingle(uint32_t address, uint64_t data);
// Write one doubleword at the address selected, the address MUST be in this format
// ex: FLASH_WriteSingle(FLASH_START_ADDR + DOUBLEWORD_BYTES_N*k, data_wr_test);
//
// (6)
// uint64_t   FLASH_ReadSingle(uint32_t address);
// Return the value stored at the FLASH address inserted
//
// (7)
// FlashError FLASH_WriteAll(FLASH_Data_type data);
// ex: FLASH_WriteAll(recipes); input MUST be a FLASH_Data_type format
// it represents an double dimensional array
// recipes[MAX_NB_OBJ][MAX_NB_PROP];  like a struct of OBJECTS with PROPERTIES
//
// OBJECT(0):				 OBJECT(1):						...			OBJECT(MAX_NB_OBJ):
//		PROP(0):					PROP(0):				...						PROP(0):
//		PROP(1):					PROP(1):				...						PROP(1):
//		PROP(2):					PROP(2):				...						PROP(2):
//		...  :						...    :				...						...    :
//		PROP(MAX_NB_PROP):			PROP(MAX_NB_PROP):		...						PROP(MAX_NB_PROP):
//
/* Includes ------------------------------------------------------------------*/
#include "genericDefines.h"
#include "stm32g4xx_hal.h"

/* Defines -----------------------------------------------------------*/
//FLASH ORGANIZATION DEFINE
#define FLASH_BANK 					FLASH_BANK_1 														// It means that we use the first BANK of the FLASH 0x00000001U
#define FLASH_BANK_ADDRESS			((FLASH_BANK == FLASH_BANK_1) ? 0x08000000 : 0x08080000) 			//switch between FLASH_BANKs
#define FLASH_PAGE_BYTES_N 			0x800 																// Represents the flash page size (2048 bit = 2 KB)
#define DOUBLEWORD_BYTES_N 			8 																	// 8*8 == 64 bit == size of uint64_t
#define DOUBLEWORD_IN_A_PAGE		FLASH_PAGE_BYTES_N / DOUBLEWORD_BYTES_N 							//256 doubleword in a PAGE
#define FLASH_START_ADDR 			(FLASH_BANK_ADDRESS + (FLASH_PAGE_OFFSET * FLASH_PAGE_BYTES_N)) 	//start address
#define FLASH_END_ADDR 				(FLASH_BANK_ADDRESS + (FLASH_MAX_PAGE * FLASH_PAGE_BYTES_N)) 		//end address: it stops before that address

//TO BE CONFIGURED: it defines the span between which the data are stored
#define FLASH_PAGE_OFFSET 			50 																	// FLASH PAGE goes from 0 to 63 in G431RB, we choose a start address to start locating data at 0x080190000
#define FLASH_MAX_PAGE 		     	54 																	//FLASH PAGE goes from 0 to 63
//TO BE CONFIGURED: it defines the dimension of the data to be processed
#define MAX_NB_OBJ 					16 																	//max number of object to be stored
#define MAX_NB_PROP					32   																//max number of properties for every object to be stored

/* Declarations -----------------------------------------------------------*/
typedef enum
{
	FLASH_NO_ERROR,
	FLASH_ERASE_ERROR,
	FLASH_WRITE_ERROR,
	FLASH_UNLOCK_ERROR,
	FLASH_LOCK_ERROR,
	FLASH_WRITE_ALL_ERROR

} FlashError;

typedef uint64_t FLASH_Data_type[MAX_NB_OBJ][MAX_NB_PROP]; //Define data structure for data to be written

/* Prototypes -----------------------------------------------------------*/
FlashError Flash_Init( void );
FlashError Flash_ErasePage_with_address(uint32_t address);
FlashError Flash_ErasePage_with_pageNb(uint32_t pageNb);
FlashError Flash_EraseAll( void );
FlashError Flash_WriteSingle(uint32_t address, uint64_t data);
uint64_t   Flash_ReadSingle(uint32_t address);
FlashError Flash_WriteAll(FLASH_Data_type data);



