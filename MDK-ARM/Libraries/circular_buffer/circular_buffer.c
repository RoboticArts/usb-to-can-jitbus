

#include "circular_buffer.h"
#include "stdlib.h"


// ============================================================================================ //


// ============================================================================================ //

void initCircularBuffer(CircularBuffer *circularBuffer){
	
	circularBuffer->pointerWrite = 0;
	circularBuffer->pointerRead = 0;
	circularBuffer->flagEOV = false;
	
}

//Write
void push_buffer(CircularBuffer *rx_buffer, unsigned char data){
	
	
	if(rx_buffer->flagEOV == false){
		
		rx_buffer->buffer[rx_buffer->pointerWrite] = data;
		rx_buffer->pointerWrite++;
	}
	
	else{
		
		if(rx_buffer->pointerWrite == rx_buffer->pointerRead ){
			//Elimina el dato mas antiguo para guardar el dato mas reciente
			pop_buffer(rx_buffer);
		}
		
		rx_buffer->buffer[rx_buffer->pointerWrite] = data;
		rx_buffer->pointerWrite++;
		
	}
	
	if(rx_buffer->pointerWrite == rx_buffer->pointerRead-1){
		rx_buffer->flagEOV = true;
	}

	if(rx_buffer->pointerWrite >= SIZE_BUFFER){
		
		rx_buffer->pointerWrite = 0;
		rx_buffer->flagEOV = true; //End Of Vector Flag
		
	}
	
}


//Read
unsigned char pop_buffer(CircularBuffer *rx_buffer){

	unsigned char data = 0;
	
	if(rx_buffer->pointerRead - rx_buffer->pointerWrite > 0){
		rx_buffer->flagEOV = false;
	}
	
		
	data = rx_buffer->buffer[rx_buffer->pointerRead];
	rx_buffer->pointerRead++;

	if(rx_buffer->pointerRead >= SIZE_BUFFER){
		
		rx_buffer->pointerRead = 0;
	}
	
	return data;
}


//Returns the number of unread bytes
unsigned long inWaiting(CircularBuffer *rx_buffer){
	
	unsigned long unreadBytes;
	
	
			if((rx_buffer->pointerWrite == rx_buffer->pointerRead) && (rx_buffer->flagEOV == false)){
				//Buffer vacio
				unreadBytes = 0;
			}
			
			if((rx_buffer->pointerWrite == rx_buffer->pointerRead) && (rx_buffer->flagEOV == true)){
				//Buffer vacio
				unreadBytes = SIZE_BUFFER;
			}
						
				
			if(rx_buffer->pointerWrite > rx_buffer->pointerRead){
				
				unreadBytes = rx_buffer->pointerWrite - rx_buffer->pointerRead;
			}
		
			if(rx_buffer->pointerWrite < rx_buffer->pointerRead){
			
				unreadBytes =  (SIZE_BUFFER - rx_buffer->pointerRead) + rx_buffer->pointerWrite;
			}
	
	return unreadBytes;
}


// ============================================================================================ //

// ============================================================================================ //


void initFloat_CircularBuffer(FloatCircularBuffer *circularBuffer){
	
	circularBuffer->pointerWrite = 0;
	circularBuffer->pointerRead = 0;
	circularBuffer->flagEOV = false;
	
}


//Write
void pushFloat_buffer(FloatCircularBuffer *rx_buffer, float data){
	
	
	if(rx_buffer->flagEOV == false){
		
		rx_buffer->buffer[rx_buffer->pointerWrite] = data;
		rx_buffer->pointerWrite++;
	}
	
	else{
		
		if(rx_buffer->pointerWrite == rx_buffer->pointerRead ){
			//Elimina el dato mas antiguo para guardar el dato mas reciente
			popFloat_buffer(rx_buffer);
		}
		
		rx_buffer->buffer[rx_buffer->pointerWrite] = data;
		rx_buffer->pointerWrite++;
		
	}
	
	if(rx_buffer->pointerWrite == rx_buffer->pointerRead-1){
		rx_buffer->flagEOV = true;
	}

	if(rx_buffer->pointerWrite >= FLOAT_BUFFER){
		
		rx_buffer->pointerWrite = 0;
		rx_buffer->flagEOV = true; //End Of Vector Flag
		
	}
	
}


float popFloat_buffer(FloatCircularBuffer *rx_buffer){

	float data = 0;
	
	if(rx_buffer->pointerRead - rx_buffer->pointerWrite > 0){
		rx_buffer->flagEOV = false;
	}
	
		
	data = rx_buffer->buffer[rx_buffer->pointerRead];
	rx_buffer->pointerRead++;

	if(rx_buffer->pointerRead >= FLOAT_BUFFER){
		
		rx_buffer->pointerRead = 0;
	}
	
	return data;
}


unsigned long inWaitingFloat(FloatCircularBuffer *rx_buffer){
	
	unsigned long unreadBytes;
	
	
			if((rx_buffer->pointerWrite == rx_buffer->pointerRead) && (rx_buffer->flagEOV == false)){
				//Buffer vacio
				unreadBytes = 0;
			}
			
			if((rx_buffer->pointerWrite == rx_buffer->pointerRead) && (rx_buffer->flagEOV == true)){
				//Buffer vacio
				unreadBytes = FLOAT_BUFFER;
			}
						
				
			if(rx_buffer->pointerWrite > rx_buffer->pointerRead){
				
				unreadBytes = rx_buffer->pointerWrite - rx_buffer->pointerRead;
			}
		
			if(rx_buffer->pointerWrite < rx_buffer->pointerRead){
			
				unreadBytes =  (FLOAT_BUFFER - rx_buffer->pointerRead) + rx_buffer->pointerWrite;
			}
	
	return unreadBytes;
}



// ============================================================================================ //

// ============================================================================================ //



void initInt_CircularBuffer(IntCircularBuffer *circularBuffer){
	
	circularBuffer->pointerWrite = 0;
	circularBuffer->pointerRead = 0;
	circularBuffer->flagEOV = false;
	
}


//Write
void pushInt_buffer(IntCircularBuffer *rx_buffer, int data){
	
	
	if(rx_buffer->flagEOV == false){
		
		rx_buffer->buffer[rx_buffer->pointerWrite] = data;
		rx_buffer->pointerWrite++;
	}
	
	else{
		
		if(rx_buffer->pointerWrite == rx_buffer->pointerRead ){
			//Elimina el dato mas antiguo para guardar el dato mas reciente
			popInt_buffer(rx_buffer);
		}
		
		rx_buffer->buffer[rx_buffer->pointerWrite] = data;
		rx_buffer->pointerWrite++;
		
	}
	
	if(rx_buffer->pointerWrite == rx_buffer->pointerRead-1){
		rx_buffer->flagEOV = true;
	}

	if(rx_buffer->pointerWrite >= INT_BUFFER){
		
		rx_buffer->pointerWrite = 0;
		rx_buffer->flagEOV = true; //End Of Vector Flag
		
	}
	
}



int popInt_buffer(IntCircularBuffer *rx_buffer){

	int data = 0;
	
	if(rx_buffer->pointerRead - rx_buffer->pointerWrite > 0){
		rx_buffer->flagEOV = false;
	}
	
		
	data = rx_buffer->buffer[rx_buffer->pointerRead];
	rx_buffer->pointerRead++;

	if(rx_buffer->pointerRead >= INT_BUFFER){
		
		rx_buffer->pointerRead = 0;
	}
	
	return data;
}


unsigned long inWaitingInt(IntCircularBuffer *rx_buffer){
	
	unsigned long unreadBytes;
	
	
			if((rx_buffer->pointerWrite == rx_buffer->pointerRead) && (rx_buffer->flagEOV == false)){
				//Buffer vacio
				unreadBytes = 0;
			}
			
			if((rx_buffer->pointerWrite == rx_buffer->pointerRead) && (rx_buffer->flagEOV == true)){
				//Buffer vacio
				unreadBytes = INT_BUFFER;
			}
						
				
			if(rx_buffer->pointerWrite > rx_buffer->pointerRead){
				
				unreadBytes = rx_buffer->pointerWrite - rx_buffer->pointerRead;
			}
		
			if(rx_buffer->pointerWrite < rx_buffer->pointerRead){
			
				unreadBytes =  (INT_BUFFER - rx_buffer->pointerRead) + rx_buffer->pointerWrite;
			}
	
	return unreadBytes;
}


// ============================================================================================ //

// ============================================================================================ //


void initUInt32_CircularBuffer(UInt32CircularBuffer *circularBuffer){
	
	circularBuffer->pointerWrite = 0;
	circularBuffer->pointerRead = 0;
	circularBuffer->flagEOV = false;
	
}

//Write
void pushUInt32_buffer(UInt32CircularBuffer *rx_buffer, unsigned int data){
	
	
	if(rx_buffer->flagEOV == false){
		
		rx_buffer->buffer[rx_buffer->pointerWrite] = data;
		rx_buffer->pointerWrite++;
	}
	
	else{
		
		if(rx_buffer->pointerWrite == rx_buffer->pointerRead ){
			//Elimina el dato mas antiguo para guardar el dato mas reciente
			popUInt32_buffer(rx_buffer);
		}
		
		rx_buffer->buffer[rx_buffer->pointerWrite] = data;
		rx_buffer->pointerWrite++;
		
	}
	
	if(rx_buffer->pointerWrite == rx_buffer->pointerRead-1){
		rx_buffer->flagEOV = true;
	}

	if(rx_buffer->pointerWrite >= UINT32_BUFFER){
		
		rx_buffer->pointerWrite = 0;
		rx_buffer->flagEOV = true; //End Of Vector Flag
		
	}
	
}


unsigned int popUInt32_buffer(UInt32CircularBuffer *rx_buffer){

	unsigned int data = 0;
	
	if(rx_buffer->pointerRead - rx_buffer->pointerWrite > 0){
		rx_buffer->flagEOV = false;
	}
	
		
	data = rx_buffer->buffer[rx_buffer->pointerRead];
	rx_buffer->pointerRead++;

	if(rx_buffer->pointerRead >= UINT32_BUFFER){
		
		rx_buffer->pointerRead = 0;
	}
	
	return data;
}



unsigned long inWaitingUInt32(UInt32CircularBuffer *rx_buffer){
	
	unsigned long unreadBytes;
	
	
			if((rx_buffer->pointerWrite == rx_buffer->pointerRead) && (rx_buffer->flagEOV == false)){
				//Buffer vacio
				unreadBytes = 0;
			}
			
			if((rx_buffer->pointerWrite == rx_buffer->pointerRead) && (rx_buffer->flagEOV == true)){
				//Buffer vacio
				unreadBytes = UINT32_BUFFER;
			}
						
				
			if(rx_buffer->pointerWrite > rx_buffer->pointerRead){
				
				unreadBytes = rx_buffer->pointerWrite - rx_buffer->pointerRead;
			}
		
			if(rx_buffer->pointerWrite < rx_buffer->pointerRead){
			
				unreadBytes =  (UINT32_BUFFER - rx_buffer->pointerRead) + rx_buffer->pointerWrite;
			}
	
	return unreadBytes;
}

