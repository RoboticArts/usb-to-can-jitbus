
#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include "stdbool.h"
#include <stddef.h> 
#include "IEEE754.h"
#include "crc8.h"


	volatile bool USART1TransferCompleted = true;
	uint8_t* sendMsg;	
	uint8_t* vectorMessage;
	int64_t maxDataLimit;
	uint8_t debugVariable[7] = {0,0,0,0,0,0,0};
	//maxDataLimit = pow(2,32)-1;

	uint64_t __buildInt32Msg(UART_HandleTypeDef *huart, uint16_t idx, int64_t datax, bool isFloat);

	uint64_t JB_UART_SendMsgInt(UART_HandleTypeDef *huart, uint16_t idx, int64_t datax);

	uint8_t __numberOfBytes(uint32_t value);