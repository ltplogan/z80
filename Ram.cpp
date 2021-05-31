#include "Ram.h"

Ram:Ram(LocalBus16 *addressBus, LocalBus8 *dataBus, int memSize)
{
	// Asigna conecta los buses de datos al chip de memoria RAM
	this->addressBus = addressBus;
	this->dataBus = dataBus;
	
	// Crea el array de memoria
	memoria = (byte *) malloc(memSize);
}

Ram::~Ram()
{
	// Libera la memoria
	free(memoria);
}

/*
	Lee la posición de memoria (celdilla de memoria) dentro de la RAM indica en el bus de direcciones
	Y coloca su contenido en el bus de datos
*/

void Ram::RD()
{
	// Notación punteros
	dataBus->Write(*(memoria + addressBus->Read()));
	
	// Notación array
	dataBus->Write(memoria[addressBus->Read()]);
}

/*
	Escribe el valor presente en el bus de datos en la celdilla de memoria indicada por el valor presente
	en el bus de direcciones-
*/

void Ram::WR()
{
	// Notación array
	memoria[addressBus->Read()] = dataBus->Read();
	
	// Notación puntero
	*(memoria + addressBus->Read()) = dataBus->Read();
}

/*Carga la ROM en los primeros 16 Kbytes de RAM*/
void Ram::loadROM(char *rom_file)
{
	FILE *fd;
	
	fd = fopen(rom_file, "rb");
	if(fd == NULL) {
		return;
	}
	
	// Lee el fichero binario completo
	fread(memoria, 1, 16384, fd);
	
	fclose(fd);
	
	// Debug
	/*LD A, 5
	LD HL, 0
	memoria = {62, 5, 33, 0, 0}*/
}




























