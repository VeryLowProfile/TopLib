/**
  ******************************************************************************
  * @file           : TopModbus_TCP.c
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

/* --------------------- Wiznet w5500  includes -----------------------------*/
#include "socket.h"

/* -------------------- TopModbus_TCP includes ------------------------------*/
#include "TopModbus_TCP.h"

/* ----------------------- Static variables ---------------------------------*/
static enum
{
    STATE_DISABLED,
	STATE_ENABLED,
    STATE_INITIALIZED,
    STATE_NOT_INITIALIZED,
} MB_TCP_State = STATE_DISABLED;

static enum
{
    EV_READY,
    EV_FRAME_RECEIVED,
    EV_EXECUTE,
	EV_SEND,
    EV_FRAME_SENT,
	EV_ERROR_EXCEPTION_FRAME,
	EV_ERROR_NO_EXCEPTION_FRAME,
	EV_NONE,
} MB_TCP_Event;

//Flags
static uint8_t MB_Busy;
//Buffers
static uint8_t TCP_Receive_Buf[MB_TCP_BUF_SIZE];
static uint16_t TCP_Receive_Lenght;
static uint8_t TCP_Send_Buf[MB_TCP_BUF_SIZE];
static uint16_t TCP_Send_Lenght;
//Frame Data
static uint16_t MB_TCP_Frame_Data_PID;
static uint8_t MB_TCP_Frame_Data_LEN;
static uint8_t MB_TCP_Frame_Data_UID;
static uint8_t MB_TCP_Frame_Data_FUNC;

/* ------------------------- Modbus Registers --------------------------------*/
static int16_t usRegInputBuf[MB_TCP_REG_INPUT_NREGS];
static int16_t usRegHoldingBuf[MB_TCP_REG_HOLDING_NREGS];
static uint8_t usRegCoilsBuf[MB_TCP_REG_COILS_SIZE/8];
static uint8_t usRegDiscreteBuf[MB_TCP_REG_DISCRETE_SIZE/8];

/*****************************************************************************/
/* ------------------------ implementation ----------------------------------*/
/*****************************************************************************/

/* ------------------------ Init Functions ----------------------------------*/
//--> Enable Protocol
void
MB_TCP_Enable ( void )
{
	if (MB_TCP_State == STATE_NOT_INITIALIZED || MB_TCP_State == STATE_DISABLED)
	{
		MB_TCP_State = STATE_ENABLED;
	}
}

//--> Disable Protocol
void
MB_TCP_Disable ( void )
{
	if (MB_TCP_State == STATE_INITIALIZED || MB_TCP_State == STATE_ENABLED)
	{
		MB_TCP_Close_Session();
		MB_TCP_State = STATE_DISABLED;
	}
}

//--> Init Protocol
MB_TCP_ErrorCode
MB_TCP_Init ( void )
{
	MB_TCP_ErrorCode errorCode = MB_TCP_NO_ERROR;

	//Clear Event Queue
	MB_TCP_Event = EV_NONE;

	if (MB_TCP_State == STATE_INITIALIZED)
	{
		MB_TCP_Close_Session();
	}

	if (MB_TCP_State == STATE_ENABLED)
	{
		if (MB_TCP_SOCKET == socket(MB_TCP_SOCKET, Sn_MR_TCP, MB_TCP_PORT, 0))
		{
			MB_TCP_State = STATE_INITIALIZED;
		}
		else
		{
			errorCode = MB_TCP_LISTEN_ERROR;
		}
	}
	else
	{
		errorCode = MB_TCP_LISTEN_ERROR;
	}

	return errorCode;

}


/* ------------------------ TCP Functions ----------------------------------*/
//--> Set Socket To Listen Mode
MB_TCP_ErrorCode
MB_TCP_Listen ( void )
{
	MB_TCP_ErrorCode errorCode = MB_TCP_NO_ERROR;

	if (MB_TCP_State == STATE_INITIALIZED)
	{
		if (listen(MB_TCP_SOCKET) < 0)
		{
			MB_TCP_Close_Session();
			errorCode = MB_TCP_INIT_ERROR;
		}
		else
		{
			//Add Event to Queue
			MB_TCP_Event = EV_READY;
		}
	}
	else
	{
		errorCode = MB_TCP_INIT_ERROR;
	}

	return errorCode;

}

//--> Receive incoming TCP frame
MB_TCP_ErrorCode
MB_TCP_Receive ( void )
{
	MB_TCP_ErrorCode errorCode = MB_TCP_NO_ERROR;

	if (MB_TCP_State == STATE_INITIALIZED)
	{
		//Check if protocol is not busy
		if (!MB_Busy)
		{
			//Check if new frame is present
			TCP_Receive_Lenght = getSn_RX_RSR(MB_TCP_SOCKET);
			if (TCP_Receive_Lenght > 0)
			{
				//Receive Frame if present
				if (recv(MB_TCP_SOCKET, (uint8_t *) TCP_Receive_Buf, TCP_Receive_Lenght))
				{
					//Add Event to Queue
					MB_TCP_Event = EV_FRAME_RECEIVED;

                	//Set Busy
                	MB_Busy = 1;
				}
				else
				{
					errorCode = MB_TCP_RECEIVE_FAILED;
				}
			}
		}
	}
	else
	{
		errorCode = MB_TCP_RECEIVE_FAILED;
	}

	return errorCode;

}

//--> Send TCP frame
MB_TCP_ErrorCode
MB_TCP_Send ( void )
{
	MB_TCP_ErrorCode errorCode = MB_TCP_NO_ERROR;

	if (MB_TCP_State == STATE_INITIALIZED)
	{
		memcpy(TCP_Send_Buf, TCP_Receive_Buf, TCP_Send_Lenght);

		if(send(MB_TCP_SOCKET, TCP_Send_Buf, TCP_Send_Lenght))
		{
			//Add Event to Queue
			MB_TCP_Event = EV_FRAME_SENT;
		}
		else
		{
			errorCode = MB_TCP_SENT_FAILED;
		}
	}
	else
	{
		errorCode = MB_TCP_SENT_FAILED;
	}

	return errorCode;

}

//--> Close Tcp Socket
MB_TCP_ErrorCode
MB_TCP_Close_Session ( void )
{
	MB_TCP_ErrorCode errorCode = MB_TCP_NO_ERROR;

	if (disconnect(MB_TCP_SOCKET) > 0)
	{
		close(MB_TCP_SOCKET);
		MB_TCP_State = STATE_ENABLED;
	}
	else
	{
		errorCode = MB_TCP_CLOSE_FAILED;
	}

	return errorCode;

}


/* ------------------- TCP Management and Poll Loop -------------------------*/
//--> TCP socket management
void
MB_TCP_Sequencer( void )
{
	if (MB_TCP_State == STATE_ENABLED || MB_TCP_State == STATE_INITIALIZED)
	{
		int socketStatus = getSn_SR(MB_TCP_SOCKET);

		switch (socketStatus)
		{
			case SOCK_CLOSED:
				MB_TCP_Init();
			  break;

			case SOCK_INIT:
				MB_TCP_Listen();
			  break;

			case SOCK_LISTEN:
				//Wait incoming Connection
				break;

			case SOCK_ESTABLISHED:
				MB_TCP_Receive();
			  break;
		}
	}
}

//--> Poll Modbus TCP Protocol
void
MB_TCP_Poll( void )
{

	/* ------------------------ DEBUG ----------------------------------*/

	usRegHoldingBuf[0]++;
	usRegHoldingBuf[1] = usRegHoldingBuf[0] + 1;
	usRegHoldingBuf[2] = usRegHoldingBuf[1] + 1;
	usRegHoldingBuf[3] = usRegHoldingBuf[2] + 1;
	usRegHoldingBuf[4] = usRegHoldingBuf[3] + 1;
	usRegHoldingBuf[5] = usRegHoldingBuf[4] + 1;
	usRegHoldingBuf[6] = usRegHoldingBuf[5] + 1;
	usRegHoldingBuf[7] = usRegHoldingBuf[6] + 1;

	usRegInputBuf[0]++;
	usRegInputBuf[1] = usRegInputBuf[0] + 1;
	usRegInputBuf[2] = usRegInputBuf[1] + 1;
	usRegInputBuf[3] = usRegInputBuf[2] + 1;
	usRegInputBuf[4] = usRegInputBuf[3] + 1;
	usRegInputBuf[5] = usRegInputBuf[4] + 1;
	usRegInputBuf[6] = usRegInputBuf[5] + 1;
	usRegInputBuf[7] = usRegInputBuf[6] + 1;

	usRegDiscreteBuf[0]++;
	usRegCoilsBuf[0]++;

	/* ------------------------ DEBUG ----------------------------------*/

	MB_TCP_ErrorCode errorCode = MB_TCP_NO_ERROR;
	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;

	//Call MB TCP Sequencer
	MB_TCP_Sequencer();

	//Check If Event is present
	if (MB_TCP_Event != EV_NONE)
	{
        switch (MB_TCP_Event)
        {
			case EV_READY:

				break;

	        case EV_FRAME_RECEIVED:

	        	//Process The Received Frame (Extract Header)
	        	errorCode = MB_TCP_Get_Frame_Header();

	            if( errorCode == MB_TCP_NO_ERROR )
	            {
	                /* Check if the frame is for us. If not ignore the frame. */
	                if( ( MB_TCP_Frame_Data_UID == MB_TCP_ADDRESS ) )
	                {
	        			//Add Event to Queue
	                	MB_TCP_Event = EV_EXECUTE;
	                }
	            }
	            else
	            {
	            	//Add error event to queue
	            	MB_TCP_Event = EV_ERROR_NO_EXCEPTION_FRAME;
	            }

	            break;

	        case EV_EXECUTE:

	        	//Execute Function Based On Function Code
	        	switch (MB_TCP_Frame_Data_FUNC)
	        	{

					//MB_TCP_FUNC_READ_COILS 0x01
					case MB_TCP_FUNC_READ_COILS:

							//Get Status and Build Answer Frame
							exceptionCode = MB_TCP_Read_Coils();

						break;

					//MB_TCP_FUNC_READ_DISCRETE 0x02
					case MB_TCP_FUNC_READ_DISCRETE_INPUTS:

							//Get Status and Build Answer Frame
							exceptionCode = MB_TCP_Read_Discrete();

						break;

					//MB_TCP_FUNC_READ_HOLDING_REGISTER 0x03
					case MB_TCP_FUNC_READ_HOLDING_REGISTER:

							//Get Status and Build Answer Frame
							exceptionCode = MB_TCP_Read_Holding();

						break;

					//MB_TCP_FUNC_READ_INPUT_REGISTER   0x04
					case MB_TCP_FUNC_READ_INPUT_REGISTER:

							//Get Status and Build Answer Frame
							exceptionCode = MB_TCP_Read_Input();

						break;

					//MB_TCP_FUNC_WRITE_SINGLE_COIL   0x05
					case MB_TCP_FUNC_WRITE_SINGLE_COIL:

							//Get Status and Build Answer Frame
							exceptionCode = MB_TCP_Write_Single_Coil();

						break;

					//MB_TCP_FUNC_WRITE_REGISTER   0x06
					case MB_TCP_FUNC_WRITE_REGISTER:

							//Get Status and Build Answer Frame
							exceptionCode = MB_TCP_Write_Single_Holding();

						break;

					//MB_TCP_FUNC_WRITE_MULTIPLE_COILS   0x15
					case MB_TCP_FUNC_WRITE_MULTIPLE_COILS:

							//Get Status and Build Answer Frame
							exceptionCode = MB_TCP_Write_Multiple_Coils();

						break;

					//MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS   0x16
					case MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS:

							//Get Status and Build Answer Frame
							exceptionCode = MB_TCP_Write_Multiple_Holdings();

						break;

					//NOT DEFINED FUNC
					default:

						//Not Defined Function
						exceptionCode = MB_TCP_EX_ILLEGAL_FUNCTION;

						break;


	        	}

    			//Check if exception, in go to error event
    			if ( exceptionCode != MB_TCP_EX_NONE )
    			{
	            	//Add error event to queue
    				MB_TCP_Event = EV_ERROR_EXCEPTION_FRAME;

    			}
    			else
    			{
	            	//Add send event to queue
    				MB_TCP_Event = EV_SEND;
    			}

	        	break;

		   case EV_SEND:

					//Send Back Answer frame
					errorCode = MB_TCP_Send();

					//Check if comm ok
					if( errorCode == MB_TCP_NO_ERROR )
					{
						MB_TCP_Event = EV_FRAME_SENT;
					}
					else
					{
						MB_TCP_Event = EV_ERROR_NO_EXCEPTION_FRAME;
					}

				break;

		   case EV_FRAME_SENT:

				//Reset Busy
				MB_Busy = 0;

				//Delete Event From Queue
				MB_TCP_Event = EV_NONE;

			   break;

		   case EV_ERROR_EXCEPTION_FRAME:

			    //Build Exception frame
			    MB_TCP_Build_Exception_Frame( errorCode, exceptionCode );

            	//Add send event to queue
			    MB_TCP_Event = EV_SEND;

			   break;

		   case EV_ERROR_NO_EXCEPTION_FRAME:

				//Reset Busy
				MB_Busy = 0;

				//Delete Event From Queue
				MB_TCP_Event = EV_NONE;

			   break;

		   case EV_NONE:

			   break;

        }
	}
}

//--> Get the frame Header Data
MB_TCP_ErrorCode
MB_TCP_Get_Frame_Header()
{
	MB_TCP_ErrorCode errorCode = MB_TCP_NO_ERROR;

    //PID
    MB_TCP_Frame_Data_PID = TCP_Receive_Buf[MB_TCP_PID] << 8U;
    MB_TCP_Frame_Data_PID |= TCP_Receive_Buf[MB_TCP_PID + 1];

    //LEN
    MB_TCP_Frame_Data_LEN = TCP_Receive_Buf[MB_TCP_LEN] << 8U;
    MB_TCP_Frame_Data_LEN |= TCP_Receive_Buf[MB_TCP_LEN + 1];

    //UID
    MB_TCP_Frame_Data_UID = TCP_Receive_Buf[MB_TCP_UID];

    //FUNC
    MB_TCP_Frame_Data_FUNC = TCP_Receive_Buf[MB_TCP_FUNC];

	if( MB_TCP_Frame_Data_PID != MB_TCP_PROTOCOL_ID )
    {
		errorCode = MB_TCP_PID_ERROR;
    }

	return errorCode;

}

//--> Build exception frame
void
MB_TCP_Build_Exception_Frame( MB_TCP_ErrorCode eCode, MB_TCP_Exception exCode )
{
	TCP_Receive_Buf[MB_TCP_FUNC] = (  MB_TCP_FUNC_ERROR );
	TCP_Receive_Buf[MB_TCP_FUNC + 1] = (  exCode );
	TCP_Send_Lenght = MB_TCP_FUNC + 2;
}

/*******************************************************************************************************************************************************/
/* ---------------------------------------------------------------------- COILS -----------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/

//--> MB_TCP_FUNC_READ_COILS
MB_TCP_Exception
MB_TCP_Read_Coils( void )
{

	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;

    uint16_t startAddress;
    uint16_t answerDataLenght;
    uint16_t Number;
    uint8_t Bytes;
    uint8_t *pointerToFrame;

    if( MB_TCP_Frame_Data_LEN == MB_TCP_FUNC_READ_COILS_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS] << 8 );
    	startAddress |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_TCP_REG_COILS_START ) && ( startAddress <= MB_TCP_REG_COILS_SIZE ) )
    	{
            //Check if the number of registers to read is valid.
            //If not return Modbus illegal data value exception.

    		Number = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_READ_COILS_NUMBER] << 8 );
    		Number |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_READ_COILS_NUMBER + 1] );

            if( ( Number >= 1 ) && ( Number <= (MB_TCP_REG_COILS_SIZE - startAddress + 1) ) )
            {

                // Set the pointerToFrame to the beginning of the data area ("TCP_Receive_Buf" is used to build "TCP_Send_Buf", it will be copied when ready)
            	pointerToFrame = &TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_BYTE_NUMBER];

            	// Set the current PDU data lenght as defined */
            	answerDataLenght = MB_TCP_FUNC_ALL_PDU_INITIAL_DATA_LENGHT;

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

                // Set the updated data lenght on "TCP_Receive_Buf" (is used to build "TCP_Send_Buf")
                TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_MESSGE_LENGHT] = (uint8_t) answerDataLenght >> 8;
                TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_MESSGE_LENGHT + 1] = (uint8_t) answerDataLenght & 255;

                // Set the total lenght of the frame, this will be used by MB_TCP_Send()
                TCP_Send_Lenght = MB_TCP_FUNC_ALL_PDU_HEADER_SIZE + answerDataLenght;

                MB_TCP_Bit_Reg_Management(pointerToFrame, (uint8_t *) usRegCoilsBuf, 0, startAddress, Number,  MB_TCP_REG_COILS_START , MB_TCP_REG_COILS_SIZE, MB_TCP_REG_READ);

            }
            else
            {
            	exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
            }
    	}
    	else
    	{
    		exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_TCP_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> MB_TCP_FUNC_WRITE_SINGLE_COIL
MB_TCP_Exception
MB_TCP_Write_Single_Coil( void )
{

	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;

    uint16_t startAddress;
    uint16_t Number;
    uint8_t *pointerToFrame;

    if( MB_TCP_Frame_Data_LEN == MB_TCP_FUNC_WRITE_SINGLE_COIL_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS] << 8 );
    	startAddress |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_TCP_REG_COILS_START ) && ( startAddress <= MB_TCP_REG_COILS_SIZE ) )
    	{

			//Single coil
			Number = 1;

			//this is not used in "MB_TCP_Coils_Management" when "MB_MB_TCP_REG_WRITE", it is initialized just to remove compiler warning!!!
			pointerToFrame = &TCP_Receive_Buf[0];

			// Set the total lenght of the frame, this will be used by MB_TCP_Send(), in this case it is equal to received frame
			TCP_Send_Lenght = MB_TCP_FUNC_WRITE_SINGLE_COIL_FRAME_LENGHT;

            // Apply command or read information from working registers, also build the answer frame
            MB_TCP_Bit_Reg_Management(pointerToFrame, (uint8_t *) usRegCoilsBuf, 0, startAddress, Number,  MB_TCP_REG_COILS_START , MB_TCP_REG_COILS_SIZE, MB_TCP_REG_WRITE);

    	}
    	else
    	{
    		exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_TCP_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> MB_TCP_FUNC_WRITE_MULTIPLE_COILS
MB_TCP_Exception
MB_TCP_Write_Multiple_Coils( void )
{

	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;

    uint16_t startAddress;
    uint16_t Number;
    uint8_t *pointerToFrame;

	//Check if the stert register is valid.
	//If not return Modbus illegal data value exception.

	startAddress = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS] << 8 );
	startAddress |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS + 1] );
	startAddress++;

	if (  ( startAddress >= MB_TCP_REG_COILS_START ) && ( startAddress <= MB_TCP_REG_COILS_SIZE ) )
	{

		//Set number of coils
		Number = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_WRITE_MULTIPLE_COILS_NUMBER] << 8 );
		Number |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_WRITE_MULTIPLE_COILS_NUMBER + 1] );

		if( ( Number >= 1 ) && ( Number <= (MB_TCP_REG_COILS_SIZE - startAddress + 1) ) )
		{

			//this is not used in "MB_TCP_Coils_Management" when "MB_MB_TCP_REG_WRITE", it is initialized just to remove compiler warning!!!
			pointerToFrame = &TCP_Receive_Buf[0];

			// Set the total lenght of the frame, this will be used by MB_TCP_Send(), in this case it is equal to received frame
			TCP_Send_Lenght = MB_TCP_FUNC_WRITE_SINGLE_COIL_FRAME_LENGHT;

            // Apply command or read information from working registers, also build the answer frame
            MB_TCP_Bit_Reg_Management(pointerToFrame, (uint8_t *) usRegCoilsBuf, 0, startAddress, Number,  MB_TCP_REG_COILS_START , MB_TCP_REG_COILS_SIZE, MB_TCP_REG_WRITE);

		}
		else
    	{
    		exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
    	}
	}
	else
	{
		exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
	}


    return exceptionCode;

}

/*******************************************************************************************************************************************************/
/* ---------------------------------------------------------------------- DISCRETE --------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/

//--> MB_TCP_FUNC_READ_DISCRETE
MB_TCP_Exception
MB_TCP_Read_Discrete( void )
{

	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;

    uint16_t startAddress;
    uint16_t answerDataLenght;
    uint16_t Number;
    uint8_t Bytes;
    uint8_t *pointerToFrame;

    if( MB_TCP_Frame_Data_LEN == MB_TCP_FUNC_READ_DISCRETE_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS] << 8 );
    	startAddress |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_TCP_REG_DISCRETE_START ) && ( startAddress <= MB_TCP_REG_DISCRETE_SIZE ) )
    	{
            //Check if the number of registers to read is valid.
            //If not return Modbus illegal data value exception.

    		Number = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_READ_DISCRETE_NUMBER] << 8 );
    		Number |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_READ_DISCRETE_NUMBER + 1] );

            if( ( Number >= 1 ) && ( Number <= (MB_TCP_REG_DISCRETE_SIZE - startAddress + 1) ) )
            {

                // Set the pointerToFrame to the beginning of the data area ("TCP_Receive_Buf" is used to build "TCP_Send_Buf", it will be copied when ready)
            	pointerToFrame = &TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_BYTE_NUMBER];

            	// Set the current PDU data lenght as defined */
            	answerDataLenght = MB_TCP_FUNC_ALL_PDU_INITIAL_DATA_LENGHT;

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

                // Set the updated data lenght on "TCP_Receive_Buf" (is used to build "TCP_Send_Buf")
                TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_MESSGE_LENGHT] = (uint8_t) answerDataLenght >> 8;
                TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_MESSGE_LENGHT + 1] = (uint8_t) answerDataLenght & 255;

                // Set the total lenght of the frame, this will be used by MB_TCP_Send()
                TCP_Send_Lenght = MB_TCP_FUNC_ALL_PDU_HEADER_SIZE + answerDataLenght;

                // Apply command or read information from working registers, also build the answer frame
                MB_TCP_Bit_Reg_Management(pointerToFrame, (uint8_t *) usRegDiscreteBuf, 0, startAddress, Number,  MB_TCP_REG_DISCRETE_START , MB_TCP_REG_DISCRETE_SIZE, MB_TCP_REG_READ);

            }
            else
            {
            	exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
            }
    	}
    	else
    	{
    		exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_TCP_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

/*******************************************************************************************************************************************************/
/* ---------------------------------------------------------------------- HOLDING ---------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/

//--> MB_TCP_FUNC_READ_HOLDING_REGISTER
MB_TCP_Exception
MB_TCP_Read_Holding ( void )
{

	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;
    uint16_t startAddress;
    uint16_t answerDataLenght;
    uint16_t Number;
    uint8_t *pointerToFrame;

    if( MB_TCP_Frame_Data_LEN == MB_TCP_FUNC_READ_REGISTER_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS] << 8 );
    	startAddress |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_TCP_REG_HOLDING_START ) && ( startAddress <= MB_TCP_REG_HOLDING_NREGS ) )
    	{
            //Check if the number of registers to read is valid.
            //If not return Modbus illegal data value exception.

    		Number = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_READ_REGISTER_NUMBER] << 8 );
    		Number |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_READ_REGISTER_NUMBER + 1] );

            if( ( Number >= 1 ) && ( Number <= (MB_TCP_REG_HOLDING_NREGS - startAddress + 1) ) )
            {

                // Set the pointerToFrame to the beginning of the data area ("TCP_Receive_Buf" is used to build "TCP_Send_Buf", it will be copied when ready)
            	pointerToFrame = &TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_BYTE_NUMBER];

            	// Set the current PDU data lenght as defined */
            	answerDataLenght = MB_TCP_FUNC_ALL_PDU_INITIAL_DATA_LENGHT;

                // Set The lenght of the answer.
                *pointerToFrame++ = Number * 2;
                answerDataLenght += Number * 2;

                // Set the updated data lenght on "TCP_Receive_Buf" (is used to build "TCP_Send_Buf")
                TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_MESSGE_LENGHT] = (uint8_t) answerDataLenght >> 8;
                TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_MESSGE_LENGHT + 1] = (uint8_t) answerDataLenght & 255;

                // Set the total lenght of the frame, this will be used by MB_TCP_Send()
                TCP_Send_Lenght = MB_TCP_FUNC_ALL_PDU_HEADER_SIZE + answerDataLenght;

                // Apply command or read information from working registers, also build the answer frame
                MB_TCP_Word_Reg_Management( pointerToFrame, (uint16_t *) usRegHoldingBuf, startAddress, Number, MB_TCP_REG_HOLDING_START, MB_TCP_REG_HOLDING_NREGS, MB_TCP_REG_READ );

            }
            else
            {
            	exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
            }
    	}
    	else
    	{
    		exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_TCP_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> MB_TCP_FUNC_WRITE_SINGLE_REGISTER
MB_TCP_Exception
MB_TCP_Write_Single_Holding( void )
{

	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;

    uint16_t startAddress;
    int16_t Number;
    uint8_t *pointerToFrame;

    if( MB_TCP_Frame_Data_LEN == MB_TCP_FUNC_WRITE_SINGLE_REGISTER_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS] << 8 );
    	startAddress |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_TCP_REG_HOLDING_START ) && ( startAddress <= MB_TCP_REG_HOLDING_NREGS ) )
    	{

			//Single Holding
			Number = 1;

			// Set the pointerToFrame to the beginning of the data area
			pointerToFrame = &TCP_Receive_Buf[MB_TCP_FUNC_WRITE_SINGLE_REGISTER_VALUE];

			// Set the total lenght of the frame, this will be used by MB_TCP_Send(), in this case it is equal to received frame
			TCP_Send_Lenght = MB_TCP_FUNC_WRITE_SINGLE_REGISTER_FRAME_LENGHT;

            MB_TCP_Word_Reg_Management( pointerToFrame, (uint16_t *) usRegHoldingBuf, startAddress, Number, MB_TCP_REG_HOLDING_START, MB_TCP_REG_HOLDING_NREGS, MB_TCP_REG_WRITE );

    	}
    	else
    	{
    		exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_TCP_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS
MB_TCP_Exception
MB_TCP_Write_Multiple_Holdings( void )
{

	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;

    uint16_t startAddress;
    uint16_t Number;
    uint8_t *pointerToFrame;


	//Check if the stert register is valid.
	//If not return Modbus illegal data value exception.

	startAddress = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS] << 8 );
	startAddress |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS + 1] );
	startAddress++;

	if (  ( startAddress >= MB_TCP_REG_HOLDING_START ) && ( startAddress <= MB_TCP_REG_HOLDING_NREGS ) )
	{

		Number = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS_NUMBER] << 8 );
		Number |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS_NUMBER + 1] );

		if( ( Number >= 1 ) && ( Number <= (MB_TCP_REG_HOLDING_NREGS - startAddress + 1) ) )
		{

			// Set the pointerToFrame to the beginning of the data area
			pointerToFrame = &TCP_Receive_Buf[MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS_VALUE];

			// Set the total lenght of the frame, this will be used by MB_TCP_Send(), in this case it is equal to received frame
			TCP_Send_Lenght = MB_TCP_FUNC_WRITE_SINGLE_REGISTER_FRAME_LENGHT;

            // Apply command or read information from working registers, also build the answer frame
            MB_TCP_Word_Reg_Management( pointerToFrame, (uint16_t *) usRegHoldingBuf, startAddress, Number, MB_TCP_REG_HOLDING_START, MB_TCP_REG_HOLDING_NREGS, MB_TCP_REG_WRITE );

		}
		else
		{
			exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
		}

	}
	else
	{
		exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
	}

    return exceptionCode;

}

/*******************************************************************************************************************************************************/
/* ---------------------------------------------------------------------- INPUT -----------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/
//-->> MB_TCP_FUNC_READ_INPUT_REGISTER
MB_TCP_Exception
MB_TCP_Read_Input ( void )
{

	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;
    uint16_t startAddress;
    uint16_t answerDataLenght;
    uint16_t Number;
    uint8_t *pointerToFrame;

    if( MB_TCP_Frame_Data_LEN == MB_TCP_FUNC_READ_INPUT_PDU_SIZE )
    {

        //Check if the stert register is valid.
        //If not return Modbus illegal data value exception.

    	startAddress = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS] << 8 );
    	startAddress |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_START_ADDRESS + 1] );
    	startAddress++;

    	if (  ( startAddress >= MB_TCP_REG_INPUT_START ) && ( startAddress <= MB_TCP_REG_INPUT_NREGS ) )
    	{
            //Check if the number of registers to read is valid.
            //If not return Modbus illegal data value exception.

    		Number = ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_READ_INPUT_NUMBER] << 8 );
    		Number |= ( uint16_t )( TCP_Receive_Buf[MB_TCP_FUNC_READ_INPUT_NUMBER + 1] );

            if( ( Number >= 1 ) && ( Number <= (MB_TCP_REG_INPUT_NREGS - startAddress + 1) ) )
            {

                // Set the pointerToFrame to the beginning of the data area ("TCP_Receive_Buf" is used to build "TCP_Send_Buf", it will be copied when ready)
            	pointerToFrame = &TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_BYTE_NUMBER];

            	// Set the current PDU data lenght as defined */
            	answerDataLenght = MB_TCP_FUNC_ALL_PDU_INITIAL_DATA_LENGHT;

                // Set The lenght of the answer.
                *pointerToFrame++ = Number * 2;
                answerDataLenght += Number * 2;

                // Set the updated data lenght on "TCP_Receive_Buf" (is used to build "TCP_Send_Buf")
                TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_MESSGE_LENGHT] = (uint8_t) answerDataLenght >> 8;
                TCP_Receive_Buf[MB_TCP_FUNC_ALL_PDU_MESSGE_LENGHT + 1] = (uint8_t) answerDataLenght & 255;

                // Set the total lenght of the frame, this will be used by MB_TCP_Send()
                TCP_Send_Lenght = MB_TCP_FUNC_ALL_PDU_HEADER_SIZE + answerDataLenght;

                // Apply command or read information from working registers, also build the answer frame
                MB_TCP_Word_Reg_Management( pointerToFrame, (uint16_t *) usRegInputBuf, startAddress, Number, MB_TCP_REG_INPUT_START, MB_TCP_REG_INPUT_NREGS, MB_TCP_REG_READ );

            }
            else
            {
            	exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
            }
    	}
    	else
    	{
    		exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
    	}
    }
    else
    {
    	exceptionCode = MB_TCP_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

/*******************************************************************************************************************************************************/
/* ---------------------------------------------------------------- BUFFER FUNC -----------------------------------------------------------------------*/
/*******************************************************************************************************************************************************/

//--> Write/Read bit status from/to working registers
MB_TCP_Exception
MB_TCP_Bit_Reg_Management( uint8_t * pToFrame, uint8_t * pToReg, uint8_t set, uint16_t sAddress, int16_t number, int16_t regStart, int16_t regSize, RegisterMode_TCP eMode )
{

	MB_TCP_Exception exceptionCode = MB_TCP_EX_NONE;

    int	iRegIndex;

    if( ( sAddress >= regStart ) && ( sAddress + number <= regStart + regSize ) )
    {

    	int managedBits = 0;

        switch ( eMode )
        {

        case MB_TCP_REG_READ:

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

        case MB_TCP_REG_WRITE:

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
    	exceptionCode = MB_TCP_EX_ILLEGAL_FUNCTION;
    }

    return exceptionCode;

}

//--> Write/Read Register status from/to working registers
MB_TCP_Exception
MB_TCP_Word_Reg_Management( uint8_t * pToFrame, uint16_t * pToReg, uint16_t sAddress, int16_t number, int16_t regStart, int16_t regSize, RegisterMode_TCP eMode )
{

	MB_TCP_Exception 	exceptionCode = MB_TCP_EX_NONE;
    int             iRegIndex;

    if( ( sAddress >= regStart ) && ( sAddress + number <= regStart + regSize ) )
    {
        iRegIndex = ( int )( sAddress - regStart );

        pToReg += iRegIndex;

        switch ( eMode )
        {

        case MB_TCP_REG_READ:
            while( number > 0 )
            {
                *pToFrame++ = ( *pToReg >> 8 );
                *pToFrame++ = ( *pToReg & 0xFF );
                iRegIndex++;
                pToReg++;
                number--;
            }
            break;

        case MB_TCP_REG_WRITE:
            while( number > 0 )
            {
            	*pToReg = *pToFrame++ << 8;
            	*pToReg |= *pToFrame++;
                iRegIndex++;
                pToReg++;
                number--;
            }
        }
    }
    else
    {
    	exceptionCode = MB_TCP_EX_ILLEGAL_DATA_ADDRESS;
    }

    return exceptionCode;
}
