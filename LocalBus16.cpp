#include "LocalBus16.h"

LocalBus16::LocalBus16() 
{
	valor = 0; // Cuando se arranca la máquina y crea un bus local para que empieze en la posición 0
}

LocalBus16::~LocalBus16()
{
	
}

word LocalBus16::Read() 
{
	return valor;
}

word LocalBus16::Write(word data) 
{
	valor = data;
}