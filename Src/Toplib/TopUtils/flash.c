/**
 ******************************************************************************
 * @file           : flash.c
 * @author         : Davide
 * @date           : 21 gen 2022
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* Functions -----------------------------------------------------------*/
FlashError FLASH_Init( void ){

	//Enable RCC CLK
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	//Enable FLASH CLK
	__HAL_RCC_FLASH_CLK_ENABLE();

	uint32_t errorCode = FLASH_NO_ERROR;

	//Unlock FLASH
	errorCode = HAL_FLASH_Unlock();

	if (errorCode != FLASH_NO_ERROR){ //UNLOCK ERROR

		//errorCode = UNLOCK error
		errorCode = FLASH_UNLOCK_ERROR;

	}

	//Clear pending flags
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	//Lock FLASH
	errorCode = HAL_FLASH_Lock();

	if (errorCode != FLASH_NO_ERROR){ //LOCK ERROR

		//errorCode = LOCK error
		errorCode = FLASH_LOCK_ERROR;

	}

	return errorCode;

}

FlashError FLASH_ErasePage_with_address(uint32_t address){

	uint32_t errorCode = FLASH_NO_ERROR;

	//Unlock FLASH
	errorCode = HAL_FLASH_Unlock();

	if (errorCode != FLASH_NO_ERROR){ //UNLOCK ERROR

		//errorCode = UNLOCK error
		errorCode = FLASH_UNLOCK_ERROR;

	}

	//Clear pending flags
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	//pageCalc
	uint32_t pageCalc = (address - FLASH_START_ADDR)/FLASH_PAGE_BYTES_N + FLASH_PAGE_OFFSET;

	//Configure erase structure
	FLASH_EraseInitTypeDef flash_erase_init =
	{
			.TypeErase = FLASH_TYPEERASE_PAGES,
			.Banks = FLASH_BANK,
			.Page = pageCalc,
			.NbPages = 1
	};

	uint32_t flash_page_erase_error = 0;

	//ERASE
	errorCode = HAL_FLASHEx_Erase(&flash_erase_init, &flash_page_erase_error);

	if (errorCode != FLASH_NO_ERROR){ //ERASE ERROR

		//errorCode = ERASE error
		errorCode = FLASH_ERASE_ERROR;

	}

	//Lock FLASH
	errorCode = HAL_FLASH_Lock();

	if (errorCode != FLASH_NO_ERROR){ //LOCK ERROR

		//errorCode = LOCK error
		errorCode = FLASH_LOCK_ERROR;

	}

	return errorCode;


}


FlashError FLASH_ErasePage_with_pageNb(uint32_t pageNb){


	uint32_t errorCode = FLASH_NO_ERROR;

	if ( pageNb > FLASH_MAX_PAGE || pageNb < FLASH_PAGE_OFFSET ){  	//Is not allowed to cancel pageNb

		//errorCode = ERASE error
		errorCode = FLASH_ERASE_ERROR;
	}

	else {

		//Unlock FLASH
		errorCode = HAL_FLASH_Unlock();

		if (errorCode != FLASH_NO_ERROR){ //UNLOCK ERROR

			//errorCode = UNLOCK error
			errorCode = FLASH_UNLOCK_ERROR;

		}

		//Clear pending flags
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

		//Configure erase structure
		FLASH_EraseInitTypeDef flash_erase_init =
		{
				.TypeErase = FLASH_TYPEERASE_PAGES,
				.Banks = FLASH_BANK,
				.Page = pageNb,
				.NbPages = 1
		};

		uint32_t flash_page_erase_error = 0;

		//ERASE
		errorCode = HAL_FLASHEx_Erase(&flash_erase_init, &flash_page_erase_error);

		if (errorCode != FLASH_NO_ERROR){ //ERASE ERROR

			//errorCode = ERASE error
			errorCode = FLASH_ERASE_ERROR;

		}

		//Lock FLASH
		errorCode = HAL_FLASH_Lock();

		if (errorCode != FLASH_NO_ERROR){ //LOCK ERROR

			//errorCode = LOCK error
			errorCode = FLASH_LOCK_ERROR;

		}
	}

	return errorCode;

}

FlashError FLASH_WriteSingle(uint32_t address, uint64_t data){

	uint32_t errorCode = FLASH_NO_ERROR;

	//Unlock FLASH
	errorCode = HAL_FLASH_Unlock();

	if (errorCode != FLASH_NO_ERROR){ //UNLOCK ERROR

		//errorCode = UNLOCK error
		errorCode = FLASH_UNLOCK_ERROR;

	}

	//WRITE
	errorCode = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, (uint64_t)(data));

	if (errorCode != FLASH_NO_ERROR){ //WRITE ERROR

		//errorCode = WRITE error
		errorCode = FLASH_WRITE_ERROR;

	}

	//Lock FLASH
	errorCode = HAL_FLASH_Lock();

	if (errorCode != FLASH_NO_ERROR){ //LOCK ERROR

		//errorCode = LOCK error
		errorCode = FLASH_LOCK_ERROR;

	}

	return errorCode;

}

uint64_t FLASH_ReadSingle(uint32_t address){

	uint32_t errorCode = FLASH_NO_ERROR;

	//Unlock FLASH
	errorCode = HAL_FLASH_Unlock();

	if (errorCode != FLASH_NO_ERROR){ //UNLOCK ERROR

		//errorCode = UNLOCK error
		errorCode = FLASH_UNLOCK_ERROR;

	}

	//READ
	uint64_t *value_read;
	value_read = (uint64_t *)address;

	//Lock FLASH
	errorCode = HAL_FLASH_Lock();

	if (errorCode != FLASH_NO_ERROR){ //LOCK ERROR

		//errorCode = LOCK error
		errorCode = FLASH_LOCK_ERROR;

	}

	return *value_read;

}

FlashError FLASH_EraseAll( void ){

	uint32_t errorCode = FLASH_NO_ERROR;

	//Unlock FLASH
	errorCode = HAL_FLASH_Unlock();

	if (errorCode != FLASH_NO_ERROR){ //UNLOCK ERROR

		//errorCode = UNLOCK error
		errorCode = FLASH_UNLOCK_ERROR;

	}

	//Clear pending flags
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	//Configure erase structure
	FLASH_EraseInitTypeDef flash_erase_init =
	{
			.TypeErase = FLASH_TYPEERASE_PAGES,
			.Banks = FLASH_BANK,
			.Page = FLASH_PAGE_OFFSET,
			.NbPages = (FLASH_MAX_PAGE - FLASH_PAGE_OFFSET)
	};

	uint32_t flash_page_erase_error = 0;

	//ERASE
	errorCode = HAL_FLASHEx_Erase(&flash_erase_init, &flash_page_erase_error);

	if (errorCode != FLASH_NO_ERROR){ //ERASE ERROR

		//errorCode = ERASE error
		errorCode = FLASH_ERASE_ERROR;

	}

	//Lock FLASH
	errorCode = HAL_FLASH_Lock();

	if (errorCode != FLASH_NO_ERROR){ //LOCK ERROR

		//errorCode = LOCK error
		errorCode = FLASH_LOCK_ERROR;

	}

	return errorCode;

}

FlashError FLASH_WriteAll(FLASH_Data_type data){

	uint32_t errorCode = FLASH_NO_ERROR;

	if (FLASH_START_ADDR + DOUBLEWORD_BYTES_N*MAX_NB_OBJ*MAX_NB_PROP < FLASH_END_ADDR){

		//Unlock FLASH
		errorCode = HAL_FLASH_Unlock();

		if (errorCode != FLASH_NO_ERROR){ //UNLOCK ERROR

			//errorCode = UNLOCK error
			errorCode = FLASH_UNLOCK_ERROR;

		}

		//WRITE

		for (int i = 0; i < MAX_NB_OBJ; i++){		//cycle trough OBJECTS
			for (int j = 0; j < MAX_NB_PROP; j++){	//cycle trough PROPERTIES

				errorCode = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_START_ADDR + DOUBLEWORD_BYTES_N*MAX_NB_PROP*i + DOUBLEWORD_BYTES_N*j, (uint64_t)(data[i][j]));

				if (errorCode != FLASH_NO_ERROR){ //WRITE ERROR

					//errorCode = WRITE error
					errorCode = FLASH_WRITE_ERROR;

				}
			}
		}

		//Lock FLASH
		errorCode = HAL_FLASH_Lock();

		if (errorCode != FLASH_NO_ERROR){ //LOCK ERROR

			//errorCode = LOCK error
			errorCode = FLASH_LOCK_ERROR;

		}
	}
	else{

		//errorCode = WRITE ALL error
		errorCode = FLASH_WRITE_ALL_ERROR;

	}

	return errorCode;

}

