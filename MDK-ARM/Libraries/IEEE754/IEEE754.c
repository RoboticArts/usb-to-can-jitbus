
#include "math.h"
#include "IEEE754.h"


unsigned int float2binary(float value) {
	
	// Function to convert real value  to IEEE foating point representation 
	myfloat var;
	var.f = value;
	unsigned int data;
	
	data =  (var.raw.sign << 31) | (var.raw.exponent << 23 ) | (var.raw.mantissa);
	
	
	return data;
}




float binary2float(unsigned int value){
	
	myfloat var;

	unsigned int m;
	unsigned int e;
	unsigned int s;
	
	m =  value & 0x7FFFFF;
	e =  (value >> 23) & 0xFF;
	s = value >> 31;
	
	var.raw.mantissa = m;
	var.raw.exponent = e;
	var.raw.sign = s;

	return var.f;
}






