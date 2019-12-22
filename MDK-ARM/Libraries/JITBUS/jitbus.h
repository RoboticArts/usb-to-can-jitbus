
#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include "stdbool.h"
#include <stddef.h> 
#include "IEEE754.h"
#include "crc8.h"
#include "circular_buffer.h"


//typedef enum {
//	
//    JB_UART_TX_READY=1,                  /*!< Transmision TX disponible*/
//		JB_UART_TX_BUSY=2,									 /*!< Transmision TX no disponible*/
//		JB_UART_RX_READY=3,									 /*!< Transmision RX disponible*/
//		JB_UART_RX_BUSY=4										 /*!< Transmision RX no disponible*/
//	
//} JB_UART_StateTypeDef;


#define JB_UART_TX_READY 1
#define JB_UART_TX_BUSY  2
#define JB_UART_RX_READY 3
#define JB_UART_RX_BUSY  4




typedef struct{
	
	 int _TX;
	 int _RX;

} JB_UART_HandleTypeDef;



// Buffer circular donde se guardaran todos los mensajes de tipo FLOAT
typedef struct {
	
	IntCircularBuffer  			id;
	FloatCircularBuffer     dataFloat;

} _JB_FloatCircularBuffer;


// Buffer circular donde se guardaran todos los mensajes de tipo INT
typedef struct {
	
	IntCircularBuffer  			id;
	IntCircularBuffer       dataInt;

} _JB_IntCircularBuffer;


// Buffer circular donde se guardaran todos los mensajes de tipo UINT32
typedef struct {
	
	UInt32CircularBuffer  			id;
	UInt32CircularBuffer        dataUInt32;

} _JB_UInt32CircularBuffer;





//Estructura donde se guardara un mensaje de tipo FLOAT JITBUS 
typedef struct {
	
	uint16_t  			id;
	float			      dataFloat;

} JB_MsgFloat;


//Estructura donde se guardara un mensaje de tipo INT JITBUS 
typedef struct {
	
	uint16_t  			id;
	int       			dataInt;

} JB_MsgInt;


//Estructura donde se guardara un JITBUS de forma binaria  
typedef struct {
	
	uint16_t  			id;
	unsigned int    dataUInt32;

} JB_MsgBinary;



uint64_t __build_and_Transmit_Msg(UART_HandleTypeDef *huart, uint16_t idx, int64_t datax, bool isFloat);

void  __build_and_Transmit_All_BinaryMsgs(UART_HandleTypeDef *huart, JB_MsgBinary *pBinaryMessage, uint16_t size);

void JB_UART_SendMsgInt(UART_HandleTypeDef *huart, uint16_t idx, int64_t datax);

void JB_UART_SendMsgFloat(UART_HandleTypeDef *huart, uint16_t idx, float datax);

void JB_UART_SendAllBinaryMsgs(UART_HandleTypeDef *huart, JB_MsgBinary *pBinaryMessage, uint16_t size);

void JB_UART_inWaiting(UART_HandleTypeDef *huart);

//JB_UART_StateTypeDef JB_UART_State(void);

void JB_UART_Init(UART_HandleTypeDef *huart);

uint8_t __numberOfBytes(uint32_t value);


uint16_t __serialAvailable(void);

uint16_t JB_inWaiting(void);



uint16_t JB_inWaitingFloat(void);
JB_MsgFloat JB_getFloatMsg(void);

uint16_t JB_inWaitingInt(void);
JB_MsgInt JB_getIntMsg(void);

uint16_t JB_inWaitingBinary(void);
JB_MsgBinary JB_getBinaryMsg(void);

