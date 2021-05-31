#include "LocalBus8.h"

LocalBus8::LocalBus8() 
{
	valor = 0; 
}

LocalBus8::~LocalBus8()
{
	
}

byte LocalBus8::Read() // En la lectura me devuelve un 1 byte
{
	return valor;
}

word LocalBus8::Write(byte data)  // Solo puedo escribir un 1 byte
{
	valor = data;
}