#include "jitbus.h"

//#define RECEIVE_SIZE 7 //Número de bytes que se leeran por el puerto hasta que salte la interrupcion

volatile bool USART1TransferCompleted = true;
volatile bool USART1ReceptionCompleted = true;

int64_t maxDataLimit = 0xFFFFFFFF;
uint16_t debugVariable[8] = {0,0,0,0,0,0,0,0};

uint8_t* sendMsg;	
uint8_t* vectorMessage;
uint8_t* sendPacket;

uint8_t* rawBytes;


#define DMA_RX_BUFFER 14		//Buffer para el DMA
#define RAW_BUFFER 256			//Buffer para los bytes del puerto serie


char rx_dma_buffer[DMA_RX_BUFFER];
char rx_buffer[RAW_BUFFER];
int counter = 0;
int pointer = 0;
bool pointerOverflow = false;
int num_bytes_received = 0;
int i = 0;
int j = 0; 



volatile JB_UART_HandleTypeDef JB_STATE = { ._TX = JB_UART_TX_READY, ._RX = JB_UART_RX_READY  };

extern UART_HandleTypeDef huart1;
extern CircularBuffer RX_buffer;

extern _JB_FloatCircularBuffer JB_BufferFloat; 
extern _JB_IntCircularBuffer JB_BufferInt;
extern _JB_UInt32CircularBuffer JB_BufferUInt32;



void JB_UART_Init(UART_HandleTypeDef *huart){
	
		//Se habilita la interrucion cuando no se reciben datos UART tras esperar 1 byte
		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
	
	
		//Pone en marcha la recepcion UART por DMA en modo automatico
		HAL_UART_Receive_DMA(huart, (uint8_t *)rx_dma_buffer, DMA_RX_BUFFER);
	
		//Inicializa el buffer circular RX para almacenar los bytes en raw
		initCircularBuffer(&RX_buffer);
	
		//Inicializa el buffer circular para almacenar los mensajes JITBUS encontrados
		//JB_initCircularBuffer(_JB_MsgBuffer, JB_MSG_BUFFER);
		
		initInt_CircularBuffer(&(JB_BufferInt.id));
		initInt_CircularBuffer(&(JB_BufferInt.dataInt));
	
		initInt_CircularBuffer(&(JB_BufferFloat.id));
		initFloat_CircularBuffer(&(JB_BufferFloat.dataFloat));
	
		initUInt32_CircularBuffer(&(JB_BufferUInt32.id));
		initUInt32_CircularBuffer(&(JB_BufferUInt32.dataUInt32));

}



// Fuentes de disparo:
//					- Envio de datos TX
//					- IDLE en la linea RX
void USART1_IRQHandler(void){
	
	HAL_UART_IRQHandler(&huart1);
	
	// Condicion que salta cuando no se reciben datos durante 1 byte o más.
  // Se debe definir previamente el flag __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE)
  // Se debe comentar la funcion USART1_IRQHandler en el archivo stm32f1xx_it.c
	if (USART1->SR & USART_SR_IDLE) {

	
			num_bytes_received = sizeof (rx_dma_buffer) - huart1.hdmarx -> Instance -> CNDTR;
					
					
				for(i = j; i < num_bytes_received; i++){
				
							rx_buffer[counter] = rx_dma_buffer[i];
							push_buffer(&RX_buffer, rx_dma_buffer[i]);
							counter++;
							j++;
							
							if(counter >= RAW_BUFFER){
							counter = 0;
							}
				 }
				// Clear Interrupt via dummy read
				(void) USART1->DR;	
	}
}


void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart){
	
			for(i = j; i < DMA_RX_BUFFER/2; i++){
			
					rx_buffer[counter] = rx_dma_buffer[i];
					push_buffer(&RX_buffer, rx_dma_buffer[i]);
					counter++;
					j++;
				
					if(counter >= RAW_BUFFER){
					counter = 0;
					}
			
		  }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
		
			for(i = j; i < DMA_RX_BUFFER; i++){
			
					rx_buffer[counter] = rx_dma_buffer[i];
					push_buffer(&RX_buffer, rx_dma_buffer[i]);
					counter++;
					j++;
				
					if(counter >= RAW_BUFFER){
					counter = 0;
					}
			}
				
			j = 0;	
}


uint16_t __serialAvailable(void){
	
	
	return inWaiting(&RX_buffer);
	
}

uint8_t __serialRead(void){

	
	return pop_buffer(&RX_buffer);
}


//Procesa los bytes en raw y busca mensajes. Devuelve el numero de mensajes encontrados
uint16_t JB_inWaiting(void){

	uint16_t n;
	uint8_t k;
	int i;
	
	uint8_t ib;
	uint8_t rawLengthDataBytes;
	uint8_t lengthDataBytes;
	uint8_t lengthBytes;
	uint8_t* checkMessage;
	uint8_t pCheck = 0;
	uint8_t pBytes;
	
	int receiveId;
	uint8_t receiveType;
	uint8_t receiveSign;
	uint32_t receiveBinaryData;
	uint64_t receiveDataInt;
	float receiveDataFloat;
	
	
	n = __serialAvailable();
	debugVariable[0] = n;
	
	
	//Comprueba si hay bytes disponibles en el buffer
	if(n > 0){
			
			//Reserva espacio para procesar la informacion
			rawBytes = (uint8_t*)malloc(n);
		
			//Se lee del buffer del puerto y se guarda en un vector, una posicion ocupa 8 bits
			for(k = 0; k < n; k++){
			
				rawBytes[k] = __serialRead();
			
			}
			
			//Desde 0 hasta que queden al menos 4 bytes (tamaño minimo del mensaje) en el buffer
			i = 0;
			while(i < (n-3) ){
			
					//Se busca el bit de inicio
					ib = rawBytes[i] >> 7;
					
				  //Si se encuentra un supuesto bit de inicio, se continua
					if (ib == 1){
					
							rawLengthDataBytes = ((rawBytes[i] << 1) & 0xFF) >> 6;
							
							switch(rawLengthDataBytes){
								
								case 0: lengthDataBytes = 1; break;
								case 1: lengthDataBytes = 2; break;
								case 2: lengthDataBytes = 3; break;
								case 3: lengthDataBytes = 4; break;
							
							}
              
							//Se suman los bytes de los datos mas los bytes resultado de: ib+tb+sb+id+crc
							lengthBytes = lengthDataBytes + 3;
							
						
							//El numero de bytes del supuesto mensaje debe ser menor que el numero de bytes
              //que QUEDAN por comprobar en el buffer
							
							if(lengthBytes <= (n-i) ){
									
									checkMessage = (uint8_t*)malloc(lengthBytes);
									pCheck = 0;
             
								  //Con la informacion anterior, se reconstruye el supuesto mensaje
									for (pBytes = i; pBytes < lengthBytes+i; pBytes++){
									
											checkMessage[pCheck] = rawBytes[pBytes]; 
											pCheck++;
									
									}
									
									uint8_t crcReceived;
									uint8_t  crcCalculated;
									
									//El byte del CRC es el ultimo byte de todo el mensaje
									crcReceived = checkMessage[lengthBytes-1];
									
									//Se comprueba el CRC del supuesto mensaje
									//Se debe pasar como parametros el mensaje sin el CRC y su longitud
									//Se pasa el mensaje completo y la longitud maxima se le resta 1 bytes (el del crc)
									crcCalculated= crc8(checkMessage, lengthBytes-1);
									
									if(crcCalculated == crcReceived){
									
											// CRC correcto, el mensaje es válido
											HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
										
											receiveId =  ((checkMessage[0] << 6) | (checkMessage[1] >> 2)) & 0x7FF; 
											receiveType = (checkMessage[1] >> 1) & 0x1;
											receiveSign = checkMessage[1] & 0x1;
										
										  receiveBinaryData = 0;
											
										  //Se construye el dato en formato binario:
											for(pCheck = 2; pCheck < lengthBytes-1; pCheck++){
												    
														receiveBinaryData = receiveBinaryData << 8;
                            receiveBinaryData = receiveBinaryData | checkMessage[pCheck];
											}
											
											//Se guarda en un buffer el dato en binario
											pushUInt32_buffer(&JB_BufferUInt32.id, receiveId);
											pushUInt32_buffer(&JB_BufferUInt32.dataUInt32, receiveBinaryData);
																																			
											if(receiveType == 1){
												
														receiveDataFloat = binary2float (receiveBinaryData);
												
														pushInt_buffer(&JB_BufferFloat.id, receiveId);
														pushFloat_buffer(&JB_BufferFloat.dataFloat, receiveDataFloat);
											}
											else{
												
														receiveDataInt = receiveBinaryData;
												
														if(receiveSign == 1){
														
																receiveDataInt = -receiveDataInt;
														}
																												
														pushInt_buffer(&JB_BufferInt.id, receiveId);
														pushInt_buffer(&JB_BufferInt.dataInt,receiveDataInt);
											}
												
											//Como el dato es correcto, se saltan el numero de bytes del mensaje correcto
											i = (i + lengthBytes)-1;
									}
									
									else{
									
											// CRC incorrecto, mensaje erroneo
									}
									
								  free(checkMessage);
							} //End process data
							
					
					} // End "ib" IF
					
					i++;
				
			} //End while
			
			
			free(rawBytes);
	
	}
	
	uint16_t unreadJitbusMsg;
	
	unreadJitbusMsg = inWaitingFloat(&JB_BufferFloat.dataFloat) + inWaitingInt(&JB_BufferInt.dataInt);
  

 return unreadJitbusMsg;
}


uint16_t JB_inWaitingFloat(void){

	return inWaitingFloat(&JB_BufferFloat.dataFloat);
}


JB_MsgFloat JB_getFloatMsg(void){

	JB_MsgFloat newMsg;
	
	newMsg.id = popInt_buffer(&JB_BufferFloat.id);
	newMsg.dataFloat = popFloat_buffer(&JB_BufferFloat.dataFloat);
								
	return newMsg;
}


uint16_t JB_inWaitingInt(void){


	return inWaitingInt(&JB_BufferInt.dataInt);
}


JB_MsgInt JB_getIntMsg(void){

	JB_MsgInt newMsg;
	
	newMsg.id = popInt_buffer(&JB_BufferInt.id);
	newMsg.dataInt = popInt_buffer(&JB_BufferInt.dataInt);
	
	return newMsg;
}


uint16_t JB_inWaitingBinary(void){


	return inWaitingUInt32(&JB_BufferUInt32.dataUInt32);
}


JB_MsgBinary JB_getBinaryMsg(void){

	JB_MsgBinary newMsg;
	
	newMsg.id = popUInt32_buffer(&JB_BufferUInt32.id);
	newMsg.dataUInt32 = popUInt32_buffer(&JB_BufferUInt32.dataUInt32);
	
	return newMsg;
}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	 if (huart->Instance == USART1){ //Comprueba que quien ha acabado es el USART1
		 
			//Cuando UART_Transmit envia el mensaje completo (todos los bytes indicados) se desbloquea el recurso
			USART1TransferCompleted = true;
			JB_STATE._TX = JB_UART_TX_READY;
	 }
		
		__NOP();
}

/*
JB_UART_StateTypeDef JB_UART_State(void){

	return JB_STATE._TX; ////////////////////////// R E V I S A R
}
*/

void JB_UART_SendMsgInt(UART_HandleTypeDef *huart, uint16_t idx, int64_t datax){
	

			if(USART1TransferCompleted == true){
				
				free(sendMsg);
		    free(vectorMessage);
				USART1TransferCompleted = false; //Debe ir antes de enviar los datos para evitar problemas
				JB_STATE._TX = JB_UART_TX_BUSY;
				__build_and_Transmit_Msg(huart, idx, datax, false);		
			}

}

	
void JB_UART_SendMsgFloat(UART_HandleTypeDef *huart, uint16_t idx, float datax){

		uint32_t binaryFloatData;
	
		binaryFloatData = float2binary(datax);
		
		if(USART1TransferCompleted == true){
				
				free(sendMsg);
		    free(vectorMessage);
				USART1TransferCompleted = false; //Debe ir antes de enviar los datos para evitar problemas
			  JB_STATE._TX = JB_UART_TX_BUSY;
				__build_and_Transmit_Msg(huart, idx, binaryFloatData, true);		
		}
			
}

void JB_UART_SendAllBinaryMsgs(UART_HandleTypeDef *huart, JB_MsgBinary *pBinaryMessage, uint16_t size){

		if(USART1TransferCompleted == true){
			
				free(sendPacket);
				USART1TransferCompleted = false;
				JB_STATE._TX = JB_UART_RX_BUSY;
				__build_and_Transmit_All_BinaryMsgs(huart,pBinaryMessage, size );
		}
		
}

	
uint64_t __build_and_Transmit_Msg(UART_HandleTypeDef *huart, uint16_t idx, int64_t datax, bool isFloat){
		
		
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
		
		// ==================================================================== //
		
			//Se envia el mensaje por el puerto serie
			HAL_UART_Transmit_DMA(huart, (uint8_t*)sendMsg, lengthBytesSendMsg );
		
		// ==================================================================== //
	
	//Devuelve el mensaje sin el CRC, empleado con fines de depuracion
	return message;
}



void  __build_and_Transmit_All_BinaryMsgs(UART_HandleTypeDef *huart, JB_MsgBinary *pBinaryMessage, uint16_t size){

		//Variables del protocolo
		uint8_t initBit  = 0x1;
		uint8_t typeBit = 0; //Como solo interesa transmisitir los datos en binario sin importar el tipo 
		uint8_t signBit = 0;// o el signo, los valores se ponen por defecto como type = 0 y sign = 0
		uint16_t id;
		int64_t data;
		uint8_t lengthDataBytes;
		uint8_t crc;
		uint8_t lengthBytesSendMsg;
		
		
		//Variable auxiliares
		uint64_t message = 0;
		uint8_t lengthBit = 0;
		uint8_t lengthBytesMsg = 0;
		int i = 0;
		int k = 0;
		int pPacket = 0;
		
	  //Se reserva espacio para el numero de mensajes indicado. Se considera que cada mensaje
		//ocupa 11 bytes ya que es la longitud máxima posible.
		sendPacket = (uint8_t*)malloc(size*11);  
	
		for(k = 0; k < size; k++){
	
			
				id = pBinaryMessage[k].id;  
				data = pBinaryMessage[k].dataUInt32;
				lengthDataBytes = __numberOfBytes(data); 
				
			
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
				
				for(i = 0; i < lengthBytesSendMsg; i++){
				
						sendPacket[pPacket]= sendMsg[i];
						pPacket++;
				}
				
				free(sendMsg);
				free(vectorMessage);
	
		}
		
		HAL_UART_Transmit_DMA(huart, (uint8_t*)sendPacket, pPacket+1 );
		
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

