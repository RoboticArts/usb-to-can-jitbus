

#include "crc8.h"
#include <stddef.h> //Importa NULL como elemento vacio

//Se implementa un CRC-8 con polinomio generador 0x07, Initial Value: 0x0, Final Xor Value: 0x0
 
unsigned char crc8(unsigned char const *data, unsigned char len)
{
	unsigned char crc = 0;

    if (data == NULL)
        return 0;
    crc &= 0xff;
    unsigned char const *end = data + len;
    while (data < end)
        crc = crc8_table[crc ^ *data++];
    return crc;
}
