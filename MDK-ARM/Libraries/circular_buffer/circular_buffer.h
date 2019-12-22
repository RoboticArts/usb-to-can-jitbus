

#include "stdbool.h"


#define SIZE_BUFFER 256 		// Buffer para almacenar los bytes en raw que llegan del puerto serie 


#define FLOAT_BUFFER	8	//Buffer para almacenar los mensajes float JITBUS encontrados
#define INT_BUFFER 8		//Buffer para almacenar los mensajes int JITBUS encontrados
#define UINT32_BUFFER 8 //Buffer para almacenar los mensajes uint32 JITBUS encontrados


// ===================================================================================

#ifndef FLOAT_BUFFER
	#define FLOAT_BUFFER	32	//Buffer para almacenar los mensajes float JITBUS encontrados
#endif

#ifndef INT_BUFFER
	#define INT_BUFFER 32			//Buffer para almacenar los mensajes float JITBUS encontrados
#endif

#ifndef UINT32_BUFFER
	#define UINT32_BUFFER 32			//Buffer para almacenar los mensajes uint32 JITBUS encontrados
#endif

// ===================================================================================

typedef struct{
	unsigned char buffer[SIZE_BUFFER];
	int pointerWrite;
	int pointerRead;
	bool flagEOV;
		
} CircularBuffer;

void initCircularBuffer(CircularBuffer *circularBuffer);
void push_buffer(CircularBuffer *rx_buffer, unsigned char data);
unsigned char pop_buffer(CircularBuffer *rx_buffer);
unsigned long inWaiting(CircularBuffer *rx_buffer);

// ===================================================================================

typedef struct{
	float buffer[FLOAT_BUFFER];
	int pointerWrite;
	int pointerRead;
	bool flagEOV;
		
} FloatCircularBuffer;


void  initFloat_CircularBuffer(FloatCircularBuffer *circularBuffer);
void  pushFloat_buffer(FloatCircularBuffer *rx_buffer, float data);
float popFloat_buffer(FloatCircularBuffer *rx_buffer);
unsigned long inWaitingFloat(FloatCircularBuffer *rx_buffer);

// ===================================================================================

typedef struct{
	int buffer[INT_BUFFER];
	int pointerWrite;
	int pointerRead;
	bool flagEOV;
		
} IntCircularBuffer;

void initInt_CircularBuffer(IntCircularBuffer *circularBuffer);
void pushInt_buffer(IntCircularBuffer *rx_buffer, int data);
int  popInt_buffer(IntCircularBuffer *rx_buffer);
unsigned long inWaitingInt(IntCircularBuffer *rx_buffer);

// ===================================================================================


typedef struct{
	unsigned int buffer[UINT32_BUFFER];
	int 				 pointerWrite;
	int 				 pointerRead;
	bool 				 flagEOV;
		
} UInt32CircularBuffer;


void  initUInt32_CircularBuffer(UInt32CircularBuffer *circularBuffer);
void  pushUInt32_buffer(UInt32CircularBuffer *rx_buffer, unsigned int data);
unsigned int popUInt32_buffer(UInt32CircularBuffer *rx_buffer);
unsigned long inWaitingUInt32(UInt32CircularBuffer *rx_buffer);


// ===================================================================================

