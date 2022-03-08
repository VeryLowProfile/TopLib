/**
  ******************************************************************************
  * @file           : TopModbus_RTU.h
  * @author         : Enrico
  * @date           : Aug 27, 2021
  ******************************************************************************
*/

#ifndef CMSIS_TOPMODBUS_RTU_TOPMODBUS_RTU_H_
#define CMSIS_TOPMODBUS_RTU_TOPMODBUS_RTU_H_

/* ----------------------- Defines ----------------------------------*/

/* ----------------- MB TCP Protocol Definition --------------------------*/
/* ------------------------ DO NOT MODIFY --------------------------------*/
/*
 * <------------------------ MODBUS SERIAL LINE PDU (1) ------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+----------------------------+-------------+
 *  | Address   | Function Code | Data                       | CRC/LRC     |
 *  +-----------+---------------+----------------------------+-------------+
 *  |           |               |                                   |
 * (2)        (3/2')           (3')                                (4)
 *
 * (2)  ... MB_RTU_TID          = 0 (Transaction Identifier - 2 Byte)
 * (3)  ... MB_RTU_PID          = 2 (Protocol Identifier - 2 Byte)
 * (4)  ... MB_RTU_LEN          = 4 (Number of bytes - 2 Byte)
 * (5)  ... MB_RTU_UID          = 6 (Unit Identifier - 1 Byte)
 * (6)  ... MB_RTU_FUNC         = 7 (Modbus Function Code)
 *
 * (1)  ... Modbus TCP/IP Application Data Unit
 * (1') ... Modbus Protocol Data Unit
 */

//Transaction Identifier Byte pos in pdu - 2 Byte
#define MB_RTU_UID          					 (	0 )

//Modbus Function Byte pos in pdu Code
#define MB_RTU_FUNC         					 (	1 )

//Modbus Data Byte pos in pdu Code
#define MB_RTU_DATA         					 (	2 )

/* ----------------- MB TCP Func Codes -----------------------------*/
#define MB_RTU_FUNC_NONE                          (  0 )
#define MB_RTU_FUNC_READ_COILS                    (  1 )
#define MB_RTU_FUNC_READ_DISCRETE_INPUTS          (  2 )
#define MB_RTU_FUNC_WRITE_SINGLE_COIL             (  5 )
#define MB_RTU_FUNC_WRITE_MULTIPLE_COILS          ( 15 )
#define MB_RTU_FUNC_READ_HOLDING_REGISTER         (  3 )
#define MB_RTU_FUNC_READ_INPUT_REGISTER           (  4 )
#define MB_RTU_FUNC_WRITE_REGISTER                (  6 )
#define MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS      ( 16 )
#define MB_RTU_FUNC_DIAG_DIAGNOSTIC               (  8 )
#define MB_RTU_FUNC_ERROR                         ( 128 )

/* --------------- Reg Manipulation Offset Define ------------------*/
/* ------------------------- Request -------------------------------*/

//MB_RTU_FUNC_READ_COILS
#define MB_RTU_FUNC_READ_COILS_PDU_SIZE           	( 8 )     //LENGHT IS PDU (6) + CRC (2)
#define MB_RTU_FUNC_READ_COILS_NUMBER             	( 4 )

//MB_RTU_FUNC_READ_DISCRETE
#define MB_RTU_FUNC_READ_DISCRETE_PDU_SIZE        	( 8 )	  //LENGHT IS PDU (6) + CRC (2)
#define MB_RTU_FUNC_READ_DISCRETE_NUMBER     	  	( 4 )

//MB_RTU_FUNC_READ_HOLDING_REGISTER
#define MB_RTU_FUNC_READ_REGISTER_PDU_SIZE        	( 8 )	  //LENGHT IS PDU (6) + CRC (2)
#define MB_RTU_FUNC_READ_REGISTER_NUMBER          	( 4 )

//MB_RTU_FUNC_READ_INPUT_REGISTER
#define MB_RTU_FUNC_READ_INPUT_PDU_SIZE          	( 8 )	  //LENGHT IS PDU (6) + CRC (2)
#define MB_RTU_FUNC_READ_INPUT_NUMBER            	( 4 )

//MB_RTU_FUNC_WRITE_SINGLE_COIL
#define MB_RTU_FUNC_WRITE_SINGLE_COIL_PDU_SIZE   	( 8 )     //LENGHT IS PDU (6) + CRC (2)
#define MB_RTU_FUNC_WRITE_SINGLE_COIL_VALUE		    ( 4 )

//MB_RTU_FUNC_WRITE_SINGLE_REGISTER
#define MB_RTU_FUNC_WRITE_SINGLE_REGISTER_PDU_SIZE	( 8 )	  //LENGHT IS PDU (6) + CRC (2)
#define MB_RTU_FUNC_WRITE_SINGLE_REGISTER_VALUE		( 4 )

//MB_RTU_FUNC_WRITE_MULTIPLE_COILS
#define MB_RTU_FUNC_WRITE_MULTIPLE_COILS_NUMBER     ( 4 )
#define MB_RTU_FUNC_WRITE_MULTIPLE_COILS_VALUE		( 7 )

//MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS
#define MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS_NUMBER ( 4 )
#define MB_RTU_FUNC_WRITE_MULTIPLE_REGISTERS_VALUE  ( 7 )

/* ------------------------ Types ----------------------------------*/

typedef enum
{
	MB_RTU_NO_ERROR,
	MB_RTU_NOT_ENABLED,
	MB_RTU_CRC_ERROR,
	MB_RTU_SEND_ERROR
} MB_RTU_ErrorCode;

typedef enum
{
	MB_RTU_EX_NONE = 0x00,
	MB_RTU_EX_ILLEGAL_FUNCTION = 0x01,
	MB_RTU_EX_ILLEGAL_DATA_ADDRESS = 0x02,
	MB_RTU_EX_ILLEGAL_DATA_VALUE = 0x03,
	MB_RTU_EX_SLAVE_DEVICE_FAILURE = 0x04,
	MB_RTU_EX_ACKNOWLEDGE = 0x05,
	MB_RTU_EX_SLAVE_BUSY = 0x06,
	MB_RTU_EX_MEMORY_PARITY_ERROR = 0x08,
	MB_RTU_EX_GATEWAY_PATH_FAILED = 0x0A,
	MB_RTU_EX_GATEWAY_TGT_FAILED = 0x0B
} MB_RTU_Exception;

typedef enum
{
    MB_RTU_REG_READ,                /*!< Read register values and pass to protocol stack. */
    MB_RTU_REG_WRITE                /*!< Update register values. */
}RegisterMode_RTU;

/* ------------------------ Config ----------------------------------*/
//Station Address
#define MB_RTU_ADDRESS								(  5 )

//TCP Socket Buffer Size
#define MB_RTU_BUF_SIZE								(  259 )

//TCP Socket Buffer Size
#define MB_RTU_CRC_SWAP								(  1 )

//Coil Reg Size
#define MB_RTU_REG_COILS_SIZE        				(  16 )

//Coil Reg Start
#define MB_RTU_REG_COILS_START       				(	1 )

//Discrete Size
#define MB_RTU_REG_DISCRETE_SIZE     				(  16 )

//Discrete Start
#define MB_RTU_REG_DISCRETE_START    				(	1 )

//Input Reg Size
#define MB_RTU_REG_INPUT_NREGS       				(	8 )

//Input Reg Start
#define MB_RTU_REG_INPUT_START 	  					(	1 )

//Holding Reg Size
#define MB_RTU_REG_HOLDING_NREGS     				(	8 )

//Holding Reg Start
#define MB_RTU_REG_HOLDING_START 	  				(	1 )

/* ----------------------- Proto ----------------------------------*/
void MB_RTU_Enable ( void );
void MB_RTU_Disable ( void );
void MB_RTU_Idle_Callback(UART_HandleTypeDef *huart);
void MB_RTU_Idle_Handler(UART_HandleTypeDef *huart);
void MB_RTU_Poll( void );
void MB_RTU_Get_Frame_Header();
void MB_RTU_Get_CRC();
void MB_RTU_Build_Exception_Frame( MB_RTU_ErrorCode eCode, MB_RTU_Exception exCode );
void MB_RTU_Calculate_and_Add_CRC();
MB_RTU_ErrorCode MB_RTU_Init ( void );
MB_RTU_ErrorCode MB_RTU_Send( void );
MB_RTU_ErrorCode MB_RTU_Check_CRC();

MB_RTU_Exception MB_RTU_Read_Coils( void );
MB_RTU_Exception MB_RTU_Write_Single_Coil( void );
MB_RTU_Exception MB_RTU_Write_Multiple_Coils( void );
MB_RTU_Exception MB_RTU_Coils_Management( uint8_t * pToFrame, uint16_t sAddress, int16_t number, RegisterMode_RTU eMode );

MB_RTU_Exception MB_RTU_Read_Discrete( void );
MB_RTU_Exception MB_RTU_Discrete_Management( uint8_t * pToFrame, uint16_t sAddress, int16_t number );

MB_RTU_Exception MB_RTU_Read_Holding ( void );
MB_RTU_Exception MB_RTU_Write_Single_Holding( void );
MB_RTU_Exception MB_RTU_Write_Multiple_Holdings( void );
MB_RTU_Exception MB_RTU_Holding_Management( uint8_t * pToFrame, uint16_t sAddress, int16_t number, RegisterMode_RTU eMode );

MB_RTU_Exception MB_RTU_Read_Input ( void );
MB_RTU_Exception MB_RTU_Input_Management( uint8_t * pToFrame, uint16_t sAddress, int16_t number );

#endif /* CMSIS_TOPMODBUS_RTU_TOPMODBUS_RTU_H_ */
