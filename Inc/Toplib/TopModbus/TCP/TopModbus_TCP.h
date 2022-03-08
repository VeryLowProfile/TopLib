/**
  ******************************************************************************
  * @file           : TopModbus_TCP.h
  * @author         : Enrico
  * @date           : Aug 27, 2021
  ******************************************************************************
*/

#ifndef CMSIS_TOPMODBUS_TCP_TOPMODBUS_TCP_H_
#define CMSIS_TOPMODBUS_TCP_TOPMODBUS_TCP_H_

/* ----------------------- Defines ----------------------------------*/

/* ----------------- MB TCP Protocol Definition --------------------------*/
/* ------------------------ DO NOT MODIFY --------------------------------*/
/*
 * <------------------------ MODBUS TCP/IP ADU(1) ------------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+------------------------------------------+
 *  | TID | PID | Length | UID  |Code | Data                               |
 *  +-----------+---------------+------------------------------------------+
 *  |     |     |        |      |
 * (2)   (3)   (4)      (5)    (6)
 *
 * (2)  ... MB_TCP_TID          = 0 (Transaction Identifier - 2 Byte)
 * (3)  ... MB_TCP_PID          = 2 (Protocol Identifier - 2 Byte)
 * (4)  ... MB_TCP_LEN          = 4 (Number of bytes - 2 Byte)
 * (5)  ... MB_TCP_UID          = 6 (Unit Identifier - 1 Byte)
 * (6)  ... MB_TCP_FUNC         = 7 (Modbus Function Code)
 *
 * (1)  ... Modbus TCP/IP Application Data Unit
 * (1') ... Modbus Protocol Data Unit
 */

//Transaction Identifier Byte pos in pdu - 2 Byte
#define MB_TCP_TID          					 (	0 )

//Protocol Identifier Byte pos in pdu - 2 Byte
#define MB_TCP_PID          					 (  2 )

//Number of bytes Byte pos in pdu - 2 Byte
#define MB_TCP_LEN          					 (  4 )

//Unit Identifier Byte pos in pdu - 1 Byte
#define MB_TCP_UID          					 (	6 )

//Modbus Function Byte pos in pdu Code
#define MB_TCP_FUNC         					 (	7 )

//0 = Modbus Protocol
#define MB_TCP_PROTOCOL_ID  					 (	0 )

/* ----------------- MB TCP Func Codes -----------------------------*/
#define MB_TCP_FUNC_NONE                          (  0 )
#define MB_TCP_FUNC_READ_COILS                    (  1 )
#define MB_TCP_FUNC_READ_DISCRETE_INPUTS          (  2 )
#define MB_TCP_FUNC_WRITE_SINGLE_COIL             (  5 )
#define MB_TCP_FUNC_WRITE_MULTIPLE_COILS          ( 15 )
#define MB_TCP_FUNC_READ_HOLDING_REGISTER         (  3 )
#define MB_TCP_FUNC_READ_INPUT_REGISTER           (  4 )
#define MB_TCP_FUNC_WRITE_REGISTER                (  6 )
#define MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS      ( 16 )
#define MB_TCP_FUNC_DIAG_DIAGNOSTIC               (  8 )
#define MB_TCP_FUNC_ERROR                         ( 128 )

/* --------------- Reg Manipulation Offset Define ------------------*/
/* ------------------------- Request -------------------------------*/
//MB_TCP_FUNC_ALL
#define MB_TCP_FUNC_ALL_PDU_START_ADDRESS         	( 8 )

//MB_TCP_FUNC_READ_COILS
#define MB_TCP_FUNC_READ_COILS_PDU_SIZE           	( 6 )
#define MB_TCP_FUNC_READ_COILS_NUMBER             	( 10 )

//MB_TCP_FUNC_READ_DISCRETE
#define MB_TCP_FUNC_READ_DISCRETE_PDU_SIZE        	( 6 )
#define MB_TCP_FUNC_READ_DISCRETE_NUMBER     	  	( 10 )

//MB_TCP_FUNC_READ_HOLDING_REGISTER
#define MB_TCP_FUNC_READ_REGISTER_PDU_SIZE        	( 6 )
#define MB_TCP_FUNC_READ_REGISTER_NUMBER          	( 10 )

//MB_TCP_FUNC_READ_HOLDING_REGISTER
#define MB_TCP_FUNC_READ_INPUT_PDU_SIZE          	( 6 )
#define MB_TCP_FUNC_READ_INPUT_NUMBER            	( 10 )

//MB_TCP_FUNC_WRITE_SINGLE_COIL
#define MB_TCP_FUNC_WRITE_SINGLE_COIL_PDU_SIZE   	( 6 )
#define MB_TCP_FUNC_WRITE_SINGLE_COIL_VALUE		    ( 10 )

//MB_TCP_FUNC_WRITE_SINGLE_REGISTER
#define MB_TCP_FUNC_WRITE_SINGLE_REGISTER_PDU_SIZE	( 6 )
#define MB_TCP_FUNC_WRITE_SINGLE_REGISTER_VALUE		( 10 )

//MB_TCP_FUNC_WRITE_MULTIPLE_COILS
#define MB_TCP_FUNC_WRITE_MULTIPLE_COILS_NUMBER     ( 10 )
#define MB_TCP_FUNC_WRITE_MULTIPLE_COILS_VALUE		( 13 )

//MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS
#define MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS_NUMBER ( 10 )
#define MB_TCP_FUNC_WRITE_MULTIPLE_REGISTERS_VALUE  ( 13 )

/* ------------------------- Answer -------------------------------*/
//MB_TCP_FUNC_ALL
#define MB_TCP_FUNC_ALL_PDU_BYTE_NUMBER			  	( 8 )
#define MB_TCP_FUNC_ALL_PDU_INITIAL_DATA_LENGHT	  	( 3 )
#define MB_TCP_FUNC_ALL_PDU_MESSGE_LENGHT		  	( 4 )
#define MB_TCP_FUNC_ALL_PDU_HEADER_SIZE			  	( 6 )

//MB_TCP_FUNC_WRITE_SINGLE_COIL
#define MB_TCP_FUNC_WRITE_SINGLE_COIL_FRAME_LENGHT     ( 12 )

//MB_TCP_FUNC_WRITE_SINGLE_REGISTER
#define MB_TCP_FUNC_WRITE_SINGLE_REGISTER_FRAME_LENGHT ( 12 )

/* ------------------------ Types ----------------------------------*/

typedef enum
{
	MB_TCP_NO_ERROR,
    MB_TCP_INIT_ERROR,
	MB_TCP_LISTEN_ERROR,
	MB_TCP_RECEIVE_FAILED,
	MB_TCP_SENT_FAILED,
	MB_TCP_CLOSE_FAILED,
	MB_TCP_PID_ERROR
} MB_TCP_ErrorCode;

typedef enum
{
	MB_TCP_EX_NONE = 0x00,
	MB_TCP_EX_ILLEGAL_FUNCTION = 0x01,
	MB_TCP_EX_ILLEGAL_DATA_ADDRESS = 0x02,
	MB_TCP_EX_ILLEGAL_DATA_VALUE = 0x03,
	MB_TCP_EX_SLAVE_DEVICE_FAILURE = 0x04,
	MB_TCP_EX_ACKNOWLEDGE = 0x05,
	MB_TCP_EX_SLAVE_BUSY = 0x06,
	MB_TCP_EX_MEMORY_PARITY_ERROR = 0x08,
	MB_TCP_EX_GATEWAY_PATH_FAILED = 0x0A,
	MB_TCP_EX_GATEWAY_TGT_FAILED = 0x0B
} MB_TCP_Exception;

typedef enum
{
    MB_TCP_REG_READ,                /*!< Read register values and pass to protocol stack. */
    MB_TCP_REG_WRITE                /*!< Update register values. */
} RegisterMode_TCP;

/* ------------------------ Config ----------------------------------*/
//Station Address
#define MB_TCP_ADDRESS									(  1 )

//TCP Port
#define MB_TCP_PORT  									( 502 )

//TCP Socket Number
#define MB_TCP_SOCKET									(  0 ) //0 Default, 8 Max, Don't change it if you don't know what you are doing!!!!!!

//TCP Socket Buffer Size
#define MB_TCP_BUF_SIZE									(256 + 7)

//Coil Reg Size
#define MB_TCP_REG_COILS_SIZE        					(  16 )

//Coil Reg Start
#define MB_TCP_REG_COILS_START       					(	1 )

//Discrete Size
#define MB_TCP_REG_DISCRETE_SIZE     					(  16 )

//Discrete Start
#define MB_TCP_REG_DISCRETE_START    					(	1 )

//Input Reg Size
#define MB_TCP_REG_INPUT_NREGS       					(	8 )

//Input Reg Start
#define MB_TCP_REG_INPUT_START 	  						(	1 )

//Holding Reg Size
#define MB_TCP_REG_HOLDING_NREGS     					(	8 )

//Holding Reg Start
#define MB_TCP_REG_HOLDING_START 	  					(	1 )

/* ----------------------- Proto ----------------------------------*/
void MB_TCP_Enable ( void );
void MB_TCP_Disable ( void );
void MB_TCP_Sequencer( void );
void MB_TCP_Poll( void );
void MB_TCP_Build_Exception_Frame( MB_TCP_ErrorCode eCode, MB_TCP_Exception exCode );
MB_TCP_ErrorCode MB_TCP_Init ( void );
MB_TCP_ErrorCode MB_TCP_Listen ( void );
MB_TCP_ErrorCode MB_TCP_Receive ( void );
MB_TCP_ErrorCode MB_TCP_Close_Session ( void );
MB_TCP_ErrorCode MB_TCP_Get_Frame_Header();

MB_TCP_Exception MB_TCP_Read_Coils( void );
MB_TCP_Exception MB_TCP_Write_Single_Coil( void );
MB_TCP_Exception MB_TCP_Write_Multiple_Coils( void );

MB_TCP_Exception MB_TCP_Read_Discrete( void );

MB_TCP_Exception MB_TCP_Read_Holding ( void );
MB_TCP_Exception MB_TCP_Write_Single_Holding( void );
MB_TCP_Exception MB_TCP_Write_Multiple_Holdings( void );

MB_TCP_Exception MB_TCP_Read_Input ( void );

MB_TCP_Exception MB_TCP_Bit_Reg_Management( uint8_t * pToFrame, uint8_t * pToReg, uint8_t set, uint16_t sAddress, int16_t number, int16_t regStart, int16_t regSize, RegisterMode_TCP eMode );
MB_TCP_Exception MB_TCP_Word_Reg_Management( uint8_t * pToFrame, uint16_t * pToReg, uint16_t sAddress, int16_t number, int16_t regStart, int16_t regSize, RegisterMode_TCP eMode );

#endif /* CMSIS_TOPMODBUS_TCP_TOPMODBUS_TCP_H_ */
