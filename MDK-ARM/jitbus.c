

#include "jitbus.h"



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart1)
{
		__NOP();
}
	


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart1)
{
		USART1TransferCompleted = true;
		__NOP();
}


	
uint64_t JB_UART_SendMsgInt(UART_HandleTypeDef *huart, uint16_t idx, int64_t datax){
	
			static uint64_t value=0;
		
			if(USART1TransferCompleted == true){
				
				free(sendMsg);
		    free(vectorMessage);
				USART1TransferCompleted = false; //Debe ir antes de enviar los datos para evitar problemas
				value = __buildInt32Msg(huart, idx, datax, false);			
			}
			
			return value;
	}
	
	
uint64_t __buildInt32Msg (UART_HandleTypeDef *huart, uint16_t idx, int64_t datax, bool isFloat){
		
		
		//Variables del protocolo
		uint8_t initBit  = 0;
		uint8_t typeBit = 0;
		uint8_t signBit = 0;
		uint16_t id = 0;
		int64_t data = 0;
		uint8_t lengthDataBytes = 0;
		uint8_t crc;
		uint8_t lengthBytesSendMsg;
		
		
		//Variable auxiliares
		uint64_t message = 0;
		uint8_t lengthBit = 0;
		uint8_t lengthBytesMsg = 0;
		int i = 0;
		
	
		//Si la variable es mayor de 32 bits, se evita el desbordamiento
		if (datax > maxDataLimit)
			datax = maxDataLimit;
		else if(datax < -maxDataLimit)
			datax = -maxDataLimit;
		
		
		//Se halla el signo del dato a enviar
		if(datax < 0){
			//Signo negativo
			signBit = 0x1;
			datax = -datax; //Se convierte en valor positivo
		}
		else{
			//Signo positivo
			signBit = 0x0;
		}
		
		//Determina si el dato es de tipo entero o decimal
		if(isFloat)
			typeBit = 0x1;
		else
			typeBit = 0x0;
		
		
		//Si la variable es mayor de 11 bits, se evita el desbordamiento
		//El caso por debajo de cero no se contempla porque la variable es unsigned
		if(idx > 0x7FF)
			idx = 0x7FF;
		
		
		initBit = 0x1;  // 1 bit
		typeBit = typeBit;  // 1 bit, 0x0 indica tipo entero, 0x01 indica tipo decimal
		signBit = signBit;  // 1 bit, 0x0 indica signo positivo, 0x1 indica signo negativo
		id = idx;  // 11 bits, tamaño fijo
		data = datax;  // 32 bits máximos, tamaño variable, en este caso se usan 14 bits
		lengthDataBytes = __numberOfBytes(data);  // 2 bits fijos, tamaño en bytes de data
		
	
		//Se cambia de sistema de referencia, "0b00" es 1 bytes y "0b11" son 4 bytes
		switch(lengthDataBytes){
			case 1: lengthBit = 0x0; break;
			case 2: lengthBit = 0x1; break;
			case 3: lengthBit = 0x2;  break;
			case 4: lengthBit = 0x3;  break;
		}
		
		message = (uint64_t)initBit << (2 + 1 + 1 + 11 + lengthDataBytes * 8);
		message = message | ((uint64_t)lengthBit << (1 + 1 + 11 + lengthDataBytes * 8));
		message = message | ((uint64_t)id << (1 + 1 + lengthDataBytes * 8));
		message = message | ((uint64_t)typeBit << (1 + lengthDataBytes * 8));
		message = message | ((uint64_t)signBit << (lengthDataBytes * 8));
		message = message | (uint64_t) data;
		
		
	  //Lonigitud total en bytes  del mensaje. La suma de ib, tb, sb, id son 2 bytes (el crc no se incluye todavia)
		lengthBytesMsg = lengthDataBytes + 2;  
		
		
		//El mensaje se almacena en un vector de 8 para poder calcular el CRC
		vectorMessage = (uint8_t*)malloc(lengthBytesMsg);
		
		for(i = 0; i< lengthBytesMsg; i++){
			
			vectorMessage[(lengthBytesMsg-1)-i] = (message >> (i*8)) & 0xFF;
		}
		
		crc = crc8(vectorMessage, lengthBytesMsg);
			
		//Bytes totales del mensaje incluyendo el CRC
		lengthBytesSendMsg = lengthBytesMsg + 1;
		
		//Se añade 1 byte mas en la memoria para añadir del CRC
		sendMsg = (uint8_t*)realloc(vectorMessage, lengthBytesSendMsg);
			
		//Se añade el CRC en el bit menos significativo
		sendMsg[lengthBytesSendMsg-1] = crc;
		
		/*
		for(i = 0; i < lengthBytesSendMsg; i++){
		
			debugVaraible[i] = sendMsg[i];
		}
		*/
		
		//Se envia el mensaje por el puerto serie
		HAL_UART_Transmit_IT(huart, (uint8_t*)sendMsg, lengthBytesSendMsg );
		
		
	return message;
}

	
	
uint8_t __numberOfBytes(uint32_t value){
		
		uint8_t valueBits;
		
		if (value != 0){
		
			valueBits = (uint32_t)log2(value)+1; 
			
			if(valueBits % 8 != 0)		
				valueBits = (valueBits/8) + 1;
			else
				valueBits = valueBits/8;	
		}
		else{
			valueBits = 1;
		}
		
		
	return valueBits;
}
	