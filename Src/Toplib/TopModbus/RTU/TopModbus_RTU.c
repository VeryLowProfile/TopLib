/**
  ******************************************************************************
  * @file           : TopModbus_RTU.c
  * @author         : Enrico
  * @date           : Aug 27, 2021
  ******************************************************************************
*/

/* ------------------------- System includes --------------------------------*/
#include "stm32g4xx_hal.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "genericDefines.h"

/* -------------------- TopModbus_RTU includes ------------------------------*/
#include "TopModbus_RTU.h"

/* ----------------------- Extern variables ---------------------------------*/
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

/* ----------------------- Static variables ---------------------------------*/
enum
{
    STATE_DISABLED,
	STATE_ENABLED,
    STATE_INITIALIZED,
    STATE_NOT_INITIALIZED,
} MB_RTU_State = STATE_DISABLED;

enum
{
    EV_FRAME_RECEIVED,
    EV_EXECUTE,
	EV_SEND,
    EV_FRAME_SENT,
	EV_ERROR_EXCEPTION_FRAME,
	EV_ERROR_NO_EXCEPTION_FRAME,
	EV_NONE,
} MB_RTU_Event;

//Flags
uint8_t MB_Busy;
//Buffers
uint8_t RTU_Receive_Buf[MB_RTU_BUF_SIZE];
uint8_t RTU_Working_Buf[MB_RTU_BUF_SIZE];
uint16_t RTU_Receive_Lenght;
uint8_t RTU_Send_Buf[MB_RTU_BUF_SIZE];
uint16_t RTU_Send_Lenght;
//Header
uint8_t MB_RTU_Frame_Data_UID;
uint8_t MB_RTU_Frame_Data_FUNC;

/* ------------------------- Modbus Registers --------------------------------*/
static int16_t usRegInputBuf[MB_RTU_REG_INPUT_NREGS];
static int16_t usRegHoldingBuf[MB_RTU_REG_HOLDING_NREGS];
static uint8_t usRegCoilsBuf[MB_RTU_REG_COILS_SIZE/8];
static uint8_t usRegDiscreteBuf[MB_RTU_REG_DISCRETE_SIZE/8];

/*****************************************************************************/
/* ------------------------ implementation ----------------------------------*/
/*****************************************************************************/

/* ------------------------ Init Functions ----------------------------------*/
//--> Enable Protocol
void
MB_RTU_Enable ( void )
{
	if (MB_RTU_State == STATE_NOT_INITIALIZED || MB_RTU_State == STATE_DISABLED)
	{
		MB_RTU_State = STATE_ENABLED;
	}
}

//--> Disable Protocol
void
MB_RTU_Disable ( void )
{
	if (MB_RTU_State == STATE_INITIALIZED || MB_RTU_State == STATE_ENABLED)
	{
		MB_RTU_State = STATE_DISABLED;
	}
}

//--> Init Protocol
MB_RTU_ErrorCode
MB_RTU_Init ( void )
{
	MB_RTU_ErrorCode errorCode = MB_RTU_NO_ERROR;

	//Clear Event Queue
	MB_RTU_Event = EV_NONE;

	if (MB_RTU_State == STATE_ENABLED)
	{
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart1, (uint8_t*)RTU_Receive_Buf, MB_RTU_BUF_SIZE);

		MB_RTU_State = STATE_INITIALIZED;
	}
	else
	{
		errorCode = MB_RTU_NOT_ENABLED;
	}

	return errorCode;

}

/* ------------------------ USART Functions ----------------------------------*/
//--> Callback Function For Idle Interrupt
void MB_RTU_Idle_Callback(UART_HandleTypeDef *huart)
{
	//Stop this DMA transmission
	HAL_UART_DMAStop(&huart1);

	//Receive the Modbus frame
	if (MB_RTU_State == STATE_INITIALIZED)
	{
		if (!MB_Busy)
		{
			//Calculate the length of the received data
			RTU_Receive_Lenght  = MB_RTU_BUF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

			//Copy Received Buffer To Working Buffer
			memcpy((uint8_t*)RTU_Working_Buf, (uint8_t*)RTU_Receive_Buf, MB_RTU_BUF_SIZE);

			//Reset Receive Buffer
			memset(RTU_Receive_Buf, 0, MB_RTU_BUF_SIZE);

			//Set Frame Received Event
			MB_RTU_Event = EV_FRAME_RECEIVED;

        	//Set Busy
        	MB_Busy = 1;
		}
	}

	//Reset Receive Buffer
	memset(RTU_Receive_Buf, 0, MB_RTU_BUF_SIZE);

	//Restart to start DMA transmission of 255 bytes of data at a time
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)RTU_Receive_Buf, MB_RTU_BUF_SIZE);
}

//--> Callback Function For Idle Interrupt
void MB_RTU_Idle_Handler(UART_HandleTypeDef *huart)
 {
     if(USART1 == huart1.Instance)                                   //Determine whether it is serial port 1
     {
         if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //Judging whether it is idle interruption
         {
             __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //Clear idle interrupt sign (otherwise it will continue to enter interrupt)

             MB_RTU_Idle_Callback(huart);                            //Call interrupt handler
         }
     }
 }

//--> Send answer frame
MB_RTU_ErrorCode
MB_RTU_Send( void )
{
	MB_RTU_ErrorCode errorCode = MB_RTU_NO_ERROR;

	//Copy Working Buffer To Send Buffer
	memcpy((uint8_t*)RTU_Send_Buf, (uint8_t*)RTU_Working_Buf, MB_RTU_BUF_SIZE);

	//Reset Working Buffer
	memset(RTU_Working_Buf, 0, MB_RTU_BUF_SIZE);

	//Send Buffer To UART
	if (HAL_UART_Transmit(&huart1, (uint8_t*)RTU_Send_Buf, RTU_Send_Lenght, HAL_MAX_DELAY) != HAL_OK)
	{
		errorCode = MB_RTU_SEND_ERROR;
	}
	else
	{

		//Reset Send Buffer
		memset(RTU_Send_Buf, 0, MB_RTU_BUF_SIZE);

		//Set Frame Sent Event
		MB_RTU_Event = EV_FRAME_SENT;

	}

	return errorCode;
}


/* ------------------------ RTU Functions ----------------------------------*/
//--> Poll the Modbus event machine
void
MB_RTU_Poll( void )
{

//	/* ------------------------ DEBUG ----------------------------------*/
//
//	usRegHoldingBuf[0]++;
//	usRegHoldingBuf[1] = usRegHoldingBuf[0] + 1;
//	usRegHoldingBuf[2] = usRegHoldingBuf[1] + 1;
//	usRegHoldingBuf[3] = usRegHoldingBuf[2] + 1;
//	usRegHoldingBuf[4] = usRegHoldingBuf[3] + 1;
//	usRegHoldingBuf[5] = usRegHoldingBuf[4] + 1;
//	usRegHoldingBuf[6] = usRegHoldingBuf[5] + 1;
//	usRegHoldingBuf[7] = usRegHoldingBuf[6] + 1;
//
//	usRegInputBuf[0]++;
//	usRegInputBuf[1] = usRegInputBuf[0] + 1;
//	usRegInputBuf[2] = usRegInputBuf[1] + 1;
//	usRegInputBuf[3] = usRegInputBuf[2] + 1;
//	usRegInputBuf[4] = usRegInputBuf[3] + 1;
//	usRegInputBuf[5] = usRegInputBuf[4] + 1;
//	usRegInputBuf[6] = usRegInputBuf[5] + 1;
//	usRegInputBuf[7] = usRegInputBuf[6] + 1;
//
//	usRegDiscreteBuf[0]++;
//	usRegCoilsBuf[0]++;
//
//	/* ------------------------ DEBUG ----------------------------------*/

	MB_RTU_ErrorCode errorCode = MB_RTU_NO_ERROR;
	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

	//Check If Event is present
	if (MB_RTU_Event != EV_NONE)
	{
        switch (MB_RTU_Event)
        {

	        case EV_FRAME_RECEIVED:

	        	//Process The Received Frame (Extract Header)
	        	MB_RTU_Get_Frame_Header();

	        	//Check CRC
	        	errorCode = MB_RTU_Check_CRC();

	        	//Evaluate CRC error and UID from answer frame
	            if( errorCode == MB_RTU_NO_ERROR )
	            {
	                /* Check if the frame is for us. If not ignore the frame. */
	                if( ( MB_RTU_Frame_Data_UID == MB_RTU_ADDRESS ) )
	                {
	        			//Add Execute event to Queue
	                	MB_RTU_Event = EV_EXECUTE;
	                }
	            }
	            else
	            {
	            	//Add error event to queue
	            	MB_RTU_Event = EV_ERROR_NO_EXCEPTION_FRAME;
	            }

	            break;

	        case EV_EXECUTE:

	        	//Execute Function Based On Function Code
	        	switch (MB_RTU_Frame_Data_FUNC)
	        	{

					//MB_RTU_FUNC_READ_COILS 0x01
					case MB_RTU_FUNC_READ_COILS:

							//Get Status and Build Answer Frame
							exceptionCode = MB_RTU_Read_Coils();

						break;

					//MB_RTU_FUNC_READ_DISCRETE 0x02
					case MB_RTU_FUNC_READ_DISCRETE_INPUTS:

							//Get Status and Build Answer Frame
							exceptionCode = MB_RTU_Read_Discrete();

						break;

					//MB_RTU_FUNC_READ_HOLDING_REGISTER 0x03
					case MB_RTU_FUNC_READ_HOLDING_REGISTER:

							//Get Status and Build Answer Frame
							exceptionCode = MB_RTU_Read_Holding();

						break;

					//MB_RTU_FUNC_READ_INPUT_REGISTER   0x04
					case MB_RTU_FUNC_READ_INPUT_REGISTER:

							//Get Status and Build Answer Frame
							exceptionCode = MB_RTU_Read_Input();

						break;

					//MB_RTU_FUNC_WRITE_SINGLE_COIL   0x05
					case MB_RTU_FUNC_WRITE_SINGLE_COIL:

							//Get Status and Build Answer Frame
							exceptionCode = MB_RTU_Write_Single_Coil();

						break;

					//MB_RTU_FUNC_WRITE_REGISTER   0x06
					case MB_RTU_FUNC_WRITE_REGISTER:

							//Get Status and Build Answer Frame
							exceptionCode = MB_RTU_Write_Single_Holding();

						break;

					//MB_RTU_FUNC_WRITE_MULTIPLE_COILS   0x15
					case MB_RTU_FUNC_WRITE_MULTIPLE_COILS:

							//Get Status and Build Answer Frame
							exceptionCode = MB_RTU_Write_Multiple_Coils();

						break;

					//MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS   0x16
					case MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS:

							//Get Status and Build Answer Frame
							exceptionCode = MB_RTU_Write_Multiple_Holdings();

						break;

					//NOT DEFINED FUNC
					default:

						//Not Defined Function
						exceptionCode = MB_RTU_EX_ILLEGAL_FUNCTION;

						break;

	        	}

    			//Check if exception, in go to error event
    			if ( exceptionCode != MB_RTU_EX_NONE )
    			{
	            	//Add error event to queue
	            	MB_RTU_Event = EV_ERROR_EXCEPTION_FRAME;

    			}
    			else
    			{
	            	//Add send event to queue
	            	MB_RTU_Event = EV_SEND;
    			}

	        	break;

	       case EV_SEND:

					//Send Back Answer frame
					errorCode = MB_RTU_Send();

					//Check if comm ok
					if( errorCode == MB_RTU_NO_ERROR )
					{
						MB_RTU_Event = EV_FRAME_SENT;
					}
					else
					{
						MB_RTU_Event = EV_ERROR_NO_EXCEPTION_FRAME;
					}

				break;

		   case EV_FRAME_SENT:

			    //Reset Receive Lenght and Send Lenght
			    RTU_Receive_Lenght = 0;
			    RTU_Send_Lenght = 0;

				//Reset Busy
				MB_Busy = 0;

				//Delete Event From Queue
				MB_RTU_Event = EV_NONE;

			   break;

		   case EV_ERROR_EXCEPTION_FRAME:

			    //Build Exception frame
				MB_RTU_Build_Exception_Frame( errorCode, exceptionCode );

            	//Add send event to queue
            	MB_RTU_Event = EV_SEND;

			   break;

		   case EV_ERROR_NO_EXCEPTION_FRAME:

			    //Reset Receive Lenght and Send Lenght
			    RTU_Receive_Lenght = 0;
			    RTU_Send_Lenght = 0;

				//Reset Busy
				MB_Busy = 0;

				//Delete Event From Queue
				MB_RTU_Event = EV_NONE;

			   break;

		   case EV_NONE:

			   break;

        }
	}
}

//--> Get the frame Header Data
void
MB_RTU_Get_Frame_Header()
{

    //UID
    MB_RTU_Frame_Data_UID = RTU_Working_Buf[MB_RTU_UID];

    //FUNC
    MB_RTU_Frame_Data_FUNC = RTU_Working_Buf[MB_RTU_FUNC];

}

//--> Check the CRC with the frame
MB_RTU_ErrorCode
MB_RTU_Check_CRC()
{

	MB_RTU_ErrorCode errorCode = MB_RTU_NO_ERROR;

	uint16_t actualCrc = 0;
	uint16_t crc = 0xFFFF;
	uint16_t swappedCrc = 0;

	//Get Actual CRC from Frame
	actualCrc = ( uint16_t )( RTU_Working_Buf[RTU_Receive_Lenght - 2] << 8 );
	actualCrc |= ( uint16_t )( RTU_Working_Buf[RTU_Receive_Lenght - 1] );

	//Calculate CRC from frame
	for (int pos = 0; pos < RTU_Receive_Lenght - 2; pos++)
	{
		crc ^= ( uint16_t )RTU_Working_Buf[pos];          // XOR byte into least sig. byte of crc

		for (int i = 8; i != 0; i--)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;                    // If the LSB is set
				crc ^= 0xA001;				  // Shift right and XOR 0xA001
			}
			else
			{
				crc >>= 1;                    // Else LSB is not set Just shift right
			}
		}

	}

	//Check if calculated CRC need to be swapped and add check it with CRC taken from the frame
	if (MB_RTU_CRC_SWAP)
	{
		swappedCrc = ( uint16_t )(crc >> 8);
		swappedCrc |= ( uint16_t )((crc & 255) << 8);

		if (actualCrc != swappedCrc)
		{
			errorCode = MB_RTU_CRC_ERROR;
		}

	}
	else
	{
		if (actualCrc != crc)
		{
			errorCode = MB_RTU_CRC_ERROR;
		}
	}

	return errorCode;

}

//--> Calculate and add CRC to the frame
void
MB_RTU_Calculate_and_Add_CRC()
{

	uint16_t crc = 0xFFFF;
	uint16_t swappedCrc = 0;

	for (int pos = 0; pos < RTU_Send_Lenght - 2; pos++)
	{
		crc ^= ( uint16_t )RTU_Working_Buf[pos];          // XOR byte into least sig. byte of crc

		for (int i = 8; i != 0; i--)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;                    // If the LSB is set
				crc ^= 0xA001;				  // Shift right and XOR 0xA001
			}
			else
			{
				crc >>= 1;                    // Else LSB is not set Just shift right
			}
		}

	}

	//Check if CRC need to be swapped and add CRC at the end of the frame
	if (MB_RTU_CRC_SWAP)
	{
		//Swap CRC
		swappedCrc = ( uint16_t )(crc >> 8);
		swappedCrc |= ( uint16_t )((crc & 255) << 8);

	    RTU_Working_Buf[RTU_Send_Lenght - 2] = ( uint8_t )(swappedCrc >> 8);
	    RTU_Working_Buf[RTU_Send_Lenght - 1] = ( uint8_t )(swappedCrc & 255);
	}
	else
	{
	    RTU_Working_Buf[RTU_Send_Lenght - 2] = ( uint8_t )(crc >> 8);
	    RTU_Working_Buf[RTU_Send_Lenght - 1] = ( uint8_t )(crc & 255);
	}


}

//--> Build exception frame
void
MB_RTU_Build_Exception_Frame( MB_RTU_ErrorCode eCode, MB_RTU_Exception exCode )
{
	RTU_Working_Buf[MB_RTU_FUNC] = (  MB_RTU_FUNC_ERROR );
	RTU_Working_Buf[MB_RTU_DATA] = (  exCode );
	RTU_Send_Lenght = 5;

	//Calculate and add CRC to the Exception frame
	MB_RTU_Calculate_and_Add_CRC();
}

/*******************************************************************************************************************************************************/
/* ---------------------------------------------------------------------- COILS -----------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/

//--> MB_RTU_FUNC_READ_COILS
MB_RTU_Exception
MB_RTU_Read_Coils( void )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

    uint16_t startAddress;
    uint16_t answerDataLenght;
    uint16_t Number;
    uint8_t Bytes;
    uint8_t *pointerToFrame;

    if( RTU_Receive_Lenght == MB_RTU_FUNC_READ_COILS_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA] << 8 );
    	startAddress |= ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_RTU_REG_COILS_START ) && ( startAddress <= MB_RTU_REG_COILS_SIZE ) )
    	{
            //Check if the number of registers to read is valid.
            //If not return Modbus illegal data value exception.

    		Number = ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_READ_COILS_NUMBER] << 8 );
    		Number |= ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_READ_COILS_NUMBER + 1] );

            if( ( Number >= 1 ) && ( Number <= (MB_RTU_REG_COILS_SIZE - startAddress + 1) ) )
            {

                // Set the pointerToFrame to the beginning of the data area ("RTU_Receive_Buf" is used to build "RTU_Send_Buf", it will be copied when ready)
            	pointerToFrame = &RTU_Working_Buf[MB_RTU_DATA];
            	answerDataLenght = MB_RTU_DATA + 3; //Add 2 bytes of standard protocol + 1 bytes indicating data lenght + 2 bytes CRC


                // Test if the quantity of coils is a multiple of 8.
                // If not last byte is only partially field with unused coils set to zero.
                if( ( Number & 0x0007 ) != 0 )
                {
                	Bytes = ( uint8_t )( Number / 8 + 1 );
                }
                else
                {
                	Bytes = ( uint8_t )( Number / 8 );
                }
                *pointerToFrame++ = Bytes;
                answerDataLenght += Bytes;

                // Set the total lenght of the frame, this will be used by MB_RTU_Send()
                RTU_Send_Lenght = answerDataLenght;

                // Apply command or read information from working registers, also build the answer frame
                MB_RTU_Coils_Management( pointerToFrame, startAddress, Number, MB_RTU_REG_READ );

                // Calculate and add CRC to the answer frame
                MB_RTU_Calculate_and_Add_CRC();

            }
            else
            {
            	exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
            }
    	}
    	else
    	{
    		exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> MB_RTU_FUNC_WRITE_SINGLE_COIL
MB_RTU_Exception
MB_RTU_Write_Single_Coil( void )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

    uint16_t startAddress;
    uint16_t Number;
    uint8_t *pointerToFrame;

    if( RTU_Receive_Lenght == MB_RTU_FUNC_WRITE_SINGLE_COIL_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA] << 8 );
    	startAddress |= ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_RTU_REG_COILS_START ) && ( startAddress <= MB_RTU_REG_COILS_SIZE ) )
    	{

			//Single coil
			Number = 1;

			//this is not used in "MB_RTU_Coils_Management" when "MB_MB_RTU_REG_WRITE", it is initialized just to remove compiler warning!!!
			pointerToFrame = &RTU_Working_Buf[0];

			// Set the total lenght of the frame, this will be used by MB_RTU_Send(), in this case it is equal to received frame
			RTU_Send_Lenght = RTU_Receive_Lenght;

			MB_RTU_Coils_Management( pointerToFrame, startAddress, Number, MB_RTU_REG_WRITE );

            // Calculate and add CRC to the answer frame
            MB_RTU_Calculate_and_Add_CRC();

    	}
    	else
    	{
    		exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> MB_RTU_FUNC_WRITE_MULTIPLE_COILS
MB_RTU_Exception
MB_RTU_Write_Multiple_Coils( void )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

    uint16_t startAddress;
    uint16_t Number;
    uint8_t *pointerToFrame;

	//Check if the stert register is valid.
	//If not return Modbus illegal data value exception.

	startAddress = ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA] << 8 );
	startAddress |= ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA + 1] );
	startAddress++;

	if (  ( startAddress >= MB_RTU_REG_COILS_START ) && ( startAddress <= MB_RTU_REG_COILS_SIZE ) )
	{

		//Set number of coils
		Number = ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_WRITE_MULTIPLE_COILS_NUMBER] << 8 );
		Number |= ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_WRITE_MULTIPLE_COILS_NUMBER + 1] );

		if( ( Number >= 1 ) && ( Number <= (MB_RTU_REG_COILS_SIZE - startAddress + 1) ) )
		{

			//this is not used in "MB_RTU_Coils_Management" when "MB_MB_RTU_REG_WRITE", it is initialized just to remove compiler warning!!!
			pointerToFrame = &RTU_Working_Buf[0];

			// Set the total lenght of the frame, this will be used by MB_RTU_Send()
			RTU_Send_Lenght = MB_RTU_FUNC_READ_COILS_PDU_SIZE;

			MB_RTU_Coils_Management( pointerToFrame, startAddress, Number, MB_RTU_REG_WRITE );

            // Calculate and add CRC to the answer frame
            MB_RTU_Calculate_and_Add_CRC();

		}
		else
		{
			exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
		}
	}
	else
	{
		exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
	}

    return exceptionCode;

}

//--> Write/Read coils status from/to working registers
MB_RTU_Exception
MB_RTU_Coils_Management( uint8_t * pToFrame, uint16_t sAddress, int16_t number, RegisterMode_RTU eMode )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

    int	iRegIndex;

    if( ( sAddress >= MB_RTU_REG_COILS_START ) && ( sAddress + number <= MB_RTU_REG_COILS_START + MB_RTU_REG_COILS_SIZE ) )
    {
        iRegIndex = ( int )( sAddress - MB_RTU_REG_COILS_START );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_RTU_REG_READ:
			while( number > 0 )
			{
				*pToFrame = usRegCoilsBuf[iRegIndex];
				number -= 8;
				iRegIndex++;
				pToFrame++;
			}
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_RTU_REG_WRITE:
			if ((RTU_Working_Buf[MB_RTU_FUNC_WRITE_SINGLE_COIL_VALUE] == 255 && number == 1) || (RTU_Working_Buf[MB_RTU_FUNC_WRITE_MULTIPLE_COILS_VALUE] == 255 && number >= 1))
			{
				int managedCoils = 0;
				while( managedCoils < number )
				{
					switch (((sAddress - 1) + managedCoils) % 8)
					{
						case 0:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] |= 1UL << 0;
							managedCoils++;
							break;

						case 1:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] |= 1UL << 1;
							managedCoils++;
							break;

						case 2:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] |= 1UL << 2;
							managedCoils++;
							break;

						case 3:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] |= 1UL << 3;
							managedCoils++;
							break;

						case 4:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] |= 1UL << 4;
							managedCoils++;
							break;

						case 5:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] |= 1UL << 5;
							managedCoils++;
							break;

						case 6:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] |= 1UL << 6;
							managedCoils++;
							break;

						case 7:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] |= 1UL << 7;
							managedCoils++;
							break;

						default:
							break;
					}
				}
			}
			else
			{
				int managedCoils = 0;
				while( managedCoils < number )
				{
					switch (((sAddress - 1) + managedCoils) % 8)
					{
						case 0:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] &= ~(1UL << 0);
							managedCoils++;
							break;

						case 1:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] &= ~(1UL << 1);
							managedCoils++;
							break;

						case 2:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] &= ~(1UL << 2);
							managedCoils++;
							break;

						case 3:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] &= ~(1UL << 3);
							managedCoils++;
							break;

						case 4:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] &= ~(1UL << 4);
							managedCoils++;
							break;

						case 5:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] &= ~(1UL << 5);
							managedCoils++;
							break;

						case 6:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] &= ~(1UL << 6);
							managedCoils++;
							break;

						case 7:
							usRegCoilsBuf[(sAddress + managedCoils - 1) / 8] &= ~(1UL << 7);
							managedCoils++;
							break;

						default:
							break;
					}
				}
			}
        }
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

/*******************************************************************************************************************************************************/
/* ---------------------------------------------------------------------- DISCRETE --------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/

//--> MB_RTU_FUNC_READ_DISCRETE
MB_RTU_Exception
MB_RTU_Read_Discrete( void )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

    uint16_t startAddress;
    uint16_t answerDataLenght;
    uint16_t Number;
    uint8_t Bytes;
    uint8_t *pointerToFrame;

    if( RTU_Receive_Lenght == MB_RTU_FUNC_READ_DISCRETE_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA] << 8 );
    	startAddress |= ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_RTU_REG_COILS_START ) && ( startAddress <= MB_RTU_REG_COILS_SIZE ) )
    	{
            //Check if the number of registers to read is valid.
            //If not return Modbus illegal data value exception.

    		Number = ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_READ_DISCRETE_NUMBER] << 8 );
    		Number |= ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_READ_DISCRETE_NUMBER + 1] );

            if( ( Number >= 1 ) && ( Number <= (MB_RTU_REG_COILS_SIZE - startAddress + 1) ) )
            {

                // Set the pointerToFrame to the beginning of the data area ("RTU_Receive_Buf" is used to build "RTU_Send_Buf", it will be copied when ready)
            	pointerToFrame = &RTU_Working_Buf[MB_RTU_DATA];
            	answerDataLenght = MB_RTU_DATA + 3; //Add 2 bytes of standard protocol + 1 bytes indicating data lenght + 2 bytes CRC


                // Test if the quantity of coils is a multiple of 8.
                // If not last byte is only partially field with unused coils set to zero.
                if( ( Number & 0x0007 ) != 0 )
                {
                	Bytes = ( uint8_t )( Number / 8 + 1 );
                }
                else
                {
                	Bytes = ( uint8_t )( Number / 8 );
                }
                *pointerToFrame++ = Bytes;
                answerDataLenght += Bytes;

                // Set the total lenght of the frame, this will be used by MB_RTU_Send()
                RTU_Send_Lenght = answerDataLenght;

                MB_RTU_Discrete_Management( pointerToFrame, startAddress, Number );

                // Calculate and add CRC to the answer frame
                MB_RTU_Calculate_and_Add_CRC();

            }
            else
            {
            	exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
            }
    	}
    	else
    	{
    		exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> Write/Read discrete status from/to working registers
MB_RTU_Exception
MB_RTU_Discrete_Management( uint8_t * pToFrame, uint16_t sAddress, int16_t number )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

    int	iRegIndex;

    if( ( sAddress >= MB_RTU_REG_DISCRETE_START ) && ( sAddress + number <= MB_RTU_REG_DISCRETE_START + MB_RTU_REG_DISCRETE_SIZE ) )
    {
        iRegIndex = ( int )( sAddress - MB_RTU_REG_DISCRETE_START );

        while( number > 0 )
		{
			*pToFrame = usRegDiscreteBuf[iRegIndex];
			number -= 8;
			iRegIndex++;
			pToFrame++;
		}
    }
	else
	{
		exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
	}

    return exceptionCode;

}

/*******************************************************************************************************************************************************/
/* ---------------------------------------------------------------------- HOLDING ---------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/

//--> MB_RTU_FUNC_READ_HOLDING_REGISTER
MB_RTU_Exception
MB_RTU_Read_Holding ( void )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;
    uint16_t startAddress;
    uint16_t answerDataLenght;
    uint16_t Number;
    uint8_t *pointerToFrame;

    if( RTU_Receive_Lenght == MB_RTU_FUNC_READ_REGISTER_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA] << 8 );
    	startAddress |= ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_RTU_REG_HOLDING_START ) && ( startAddress <= MB_RTU_REG_HOLDING_NREGS ) )
    	{
            //Check if the number of registers to read is valid.
            //If not return Modbus illegal data value exception.

    		Number = ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_READ_REGISTER_NUMBER] << 8 );
    		Number |= ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_READ_REGISTER_NUMBER + 1] );

            if( ( Number >= 1 ) && ( Number <= (MB_RTU_REG_HOLDING_NREGS - startAddress + 1) ) )
            {

                // Set the pointerToFrame to the beginning of the data area ("RTU_Receive_Buf" is used to build "RTU_Send_Buf", it will be copied when ready)
            	pointerToFrame = &RTU_Working_Buf[MB_RTU_DATA];
            	answerDataLenght = MB_RTU_DATA + 3; //Add 2 bytes of standard protocol + 1 bytes indicating data lenght + 2 bytes CRC

                // Set The lenght of the answer.
                *pointerToFrame++ = Number * 2;
                answerDataLenght += Number * 2;

                // Set the total lenght of the frame, this will be used by MB_RTU_Send()
                RTU_Send_Lenght = answerDataLenght;

                MB_RTU_Holding_Management( pointerToFrame, startAddress, Number, MB_RTU_REG_READ);

                // Calculate and add CRC to the answer frame
                MB_RTU_Calculate_and_Add_CRC();

            }
            else
            {
            	exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
            }
    	}
    	else
    	{
    		exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> MB_RTU_FUNC_WRITE_SINGLE_REGISTER
MB_RTU_Exception
MB_RTU_Write_Single_Holding( void )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

    uint16_t startAddress;
    uint16_t Number;
    uint8_t *pointerToFrame;

    if( RTU_Receive_Lenght == MB_RTU_FUNC_WRITE_SINGLE_REGISTER_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA] << 8 );
    	startAddress |= ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA + 1] );
    	startAddress++;

    	if ( ( startAddress >= MB_RTU_REG_HOLDING_START ) && ( startAddress <= MB_RTU_REG_HOLDING_NREGS ) )
    	{

			//Single Holding
			Number = 1;

			// Set the pointerToFrame to the beginning of the data area
			pointerToFrame = &RTU_Working_Buf[MB_RTU_FUNC_WRITE_SINGLE_REGISTER_VALUE];

			// Set the total lenght of the frame, this will be used by MB_RTU_Send(), in this case it is equal to received frame
			RTU_Send_Lenght = RTU_Receive_Lenght;

			MB_RTU_Holding_Management( pointerToFrame, startAddress, Number, MB_RTU_REG_WRITE );

            // Calculate and add CRC to the answer frame
            MB_RTU_Calculate_and_Add_CRC();

    	}
    	else
    	{
    		exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS
MB_RTU_Exception
MB_RTU_Write_Multiple_Holdings( void )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

    uint16_t startAddress;
    uint16_t Number;
    uint8_t *pointerToFrame;


	//Check if the stert register is valid.
	//If not return Modbus illegal data value exception.

	startAddress = ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA] << 8 );
	startAddress |= ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA + 1] );
	startAddress++;

	if (  ( startAddress >= MB_RTU_REG_HOLDING_START ) && ( startAddress <= MB_RTU_REG_HOLDING_NREGS ) )
	{

		Number = ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS_NUMBER] << 8 );
		Number |= ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS_NUMBER + 1] );

		if( ( Number >= 1 ) && ( Number <= (MB_RTU_REG_HOLDING_NREGS - startAddress + 1) ) )
		{

			// Set the pointerToFrame to the beginning of the data area
			pointerToFrame = &RTU_Working_Buf[MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS_VALUE];

			// Set the total lenght of the frame, this will be used by MB_RTU_Send(), in this case it is equal to received frame
			RTU_Send_Lenght = MB_RTU_FUNC_WRITE_SINGLE_REGISTER_PDU_SIZE;

			MB_RTU_Holding_Management( pointerToFrame, startAddress, Number, MB_RTU_REG_WRITE );

            // Calculate and add CRC to the answer frame
            MB_RTU_Calculate_and_Add_CRC();
		}
		else
		{
			exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
		}

	}
	else
	{
		exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
	}

    return exceptionCode;

}

//--> Write/Read Holding status from/to working registers
MB_RTU_Exception
MB_RTU_Holding_Management( uint8_t * pToFrame, uint16_t sAddress, int16_t number, RegisterMode_RTU eMode )
{

	MB_RTU_Exception 	exceptionCode = MB_RTU_EX_NONE;
    int             iRegIndex;

    if( ( sAddress >= MB_RTU_REG_HOLDING_START ) && ( sAddress + number <= MB_RTU_REG_HOLDING_START + MB_RTU_REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( sAddress - MB_RTU_REG_HOLDING_START );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_RTU_REG_READ:
            while( number > 0 )
            {
                *pToFrame++ = ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pToFrame++ = ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                number--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_RTU_REG_WRITE:
            while( number > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pToFrame++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pToFrame++;
                iRegIndex++;
                number--;
            }
        }
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
    }

    return exceptionCode;
}

/*******************************************************************************************************************************************************/
/* ---------------------------------------------------------------------- INPUT -----------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/
//-->> MB_RTU_FUNC_READ_INPUT_REGISTER
MB_RTU_Exception
MB_RTU_Read_Input ( void )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;
    uint16_t startAddress;
    uint16_t answerDataLenght;
    uint16_t Number;
    uint8_t *pointerToFrame;

    if( RTU_Receive_Lenght == MB_RTU_FUNC_READ_INPUT_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA] << 8 );
    	startAddress |= ( uint16_t )( RTU_Working_Buf[MB_RTU_DATA + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_RTU_REG_INPUT_START ) && ( startAddress <= MB_RTU_REG_INPUT_NREGS ) )
    	{
            //Check if the number of registers to read is valid.
            //If not return Modbus illegal data value exception.

    		Number = ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_READ_INPUT_NUMBER] << 8 );
    		Number |= ( uint16_t )( RTU_Working_Buf[MB_RTU_FUNC_READ_INPUT_NUMBER + 1] );

            if( ( Number >= 1 ) && ( Number <= (MB_RTU_REG_INPUT_NREGS - startAddress + 1) ) )
            {

                // Set the pointerToFrame to the beginning of the data area ("RTU_Receive_Buf" is used to build "RTU_Send_Buf", it will be copied when ready)
            	pointerToFrame = &RTU_Working_Buf[MB_RTU_DATA];
            	answerDataLenght = MB_RTU_DATA + 3; //Add 2 bytes of standard protocol + 1 bytes indicating data lenght + 2 bytes CRC

                // Set The lenght of the answer.
                *pointerToFrame++ = Number * 2;
                answerDataLenght += Number * 2;

                // Set the total lenght of the frame, this will be used by MB_RTU_Send()
                RTU_Send_Lenght = answerDataLenght;

                MB_RTU_Input_Management( pointerToFrame, startAddress, Number);

                // Calculate and add CRC to the answer frame
                MB_RTU_Calculate_and_Add_CRC();

            }
            else
            {
            	exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
            }
    	}
    	else
    	{
    		exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> Write/Read input status from/to working registers
MB_RTU_Exception
MB_RTU_Input_Management( uint8_t * pToFrame, uint16_t sAddress, int16_t number )
{

	MB_RTU_Exception 	exceptionCode = MB_RTU_EX_NONE;
    int             iRegIndex;

    if( ( sAddress >= MB_RTU_REG_INPUT_START ) && ( sAddress + number <= MB_RTU_REG_INPUT_START + MB_RTU_REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( sAddress - MB_RTU_REG_HOLDING_START );

		while( number > 0 )
		{
			*pToFrame++ = ( usRegInputBuf[iRegIndex] >> 8 );
			*pToFrame++ = ( usRegInputBuf[iRegIndex] & 0xFF );
			iRegIndex++;
			number--;
		}
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
    }

    return exceptionCode;
}

/*******************************************************************************************************************************************************/
/* ------------------------------------------------ NEW UNIV. FUNC TO BE TESTET -----------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/

//--> Write/Read bit status from/to working registers
MB_RTU_Exception
MB_RTU_Bit_Reg_Management( uint8_t * pToFrame, uint8_t * pToReg, uint8_t set, uint16_t sAddress, int16_t number, int16_t regStart, int16_t regSize, RegisterMode_RTU eMode )
{

	MB_RTU_Exception exceptionCode = MB_RTU_EX_NONE;

    int	iRegIndex;

    if( ( sAddress >= regStart ) && ( sAddress + number <= regStart + regSize ) )
    {

    	int managedBits = 0;

        switch ( eMode )
        {

        case MB_RTU_REG_READ:

            iRegIndex = ( int )( sAddress - regStart );

            pToReg += iRegIndex;

            while( number > 0 )
    		{
    			*pToFrame = *pToReg;
    			number -= 8;
    			pToReg++;
    			pToFrame++;
    		}

            break;

        case MB_RTU_REG_WRITE:

			if (set)
			{
				while( managedBits < number )
				{
					pToReg += ((sAddress + managedBits - 1) / 8);
					*pToReg |= 1UL << ((sAddress - 1 + managedBits) % 8);
					managedBits++;
				}
			}
			else
			{
				while( managedBits < number )
				{
					pToReg += ((sAddress + managedBits - 1) / 8);
					*pToReg &= ~(1UL << ((sAddress - 1 + managedBits) % 8));
					managedBits++;
				}
			}

			break;
        }
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> Write/Read Register status from/to working registers
MB_RTU_Exception
MB_RTU_Word_Reg_Management( uint8_t * pToFrame, uint16_t * pToReg, uint16_t sAddress, int16_t number, int16_t regStart, int16_t regSize, RegisterMode_RTU eMode )
{

	MB_RTU_Exception 	exceptionCode = MB_RTU_EX_NONE;
    int             iRegIndex;

    if( ( sAddress >= regStart ) && ( sAddress + number <= regStart + regSize ) )
    {
        iRegIndex = ( int )( sAddress - regStart );

        pToReg += iRegIndex;

        switch ( eMode )
        {

        case MB_RTU_REG_READ:
            while( number > 0 )
            {
                *pToFrame++ = ( *pToReg >> 8 );
                *pToFrame++ = ( *pToReg & 0xFF );
                iRegIndex++;
                number--;
            }
            break;

        case MB_RTU_REG_WRITE:
            while( number > 0 )
            {
            	*pToReg = *pToFrame++ << 8;
            	*pToReg |= *pToFrame++;
                iRegIndex++;
                number--;
            }
        }
    }
    else
    {
    	exceptionCode = MB_RTU_EX_ILLEGAL_DATA_ADDRESS;
    }

    return exceptionCode;
}
