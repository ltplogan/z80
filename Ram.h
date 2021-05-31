#include "LocalBus8.h"
#include "LocalBus16.h"

// Un objeto para manejar la memoria y enlazar esa memoria con el procesador
class Ram 
{
	// Cuando el procesador escriba en el bus le llegue a la memoria y cuando active la línea READ
	// la memoria ponga en el bus de datos el contenido de esa posición de memoria y el procesador lo pueda leer
	private:
		// Dos buses
		LocalBus16 *addressBus; // Atributos
		LocalBus8 *dataBus;
		
		// Array de bytes de 64kbytes
		// Dinámico
		byte *memoria; // No apunta a nada hasta que se le de el tamaño
		
		public:
			Ram(LocalBus16 *addressBus, LocalBus8 *dataBus, int memSize); //Instancias
			~Ram();
			
			// Métodos
			void loadROM(char *rom_file); // Cargaría la ROM en la parte baja
			// Dirección 0 hasta la 65535
			
			// Línea de control para lectura en el bus
			void RD();
			
			// Línea de control para escribir en el bus
			void WR();
}

/*
	La memoria a través de los buses se va a comunicar con el procesador
	va a tener un array de 64 k de memoria
*/