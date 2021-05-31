#include <iostream>
#include <cstring>
#include <functional>

using namespace std;

int main(void) 
{
	// Crea un bus local de direcciones 16 bits
	LocalBus16 *addressBus = new LocalBus16();
	
	// Crea un bus local de datos
	LocalBus8 *dataBus = new LocalBus8();
	
	// Crea la memoria de la máquina de 64 bytes
	Ram *ram = new Ram(addressBus, dataBus, 1024 * 64);
	
	// Crea el procesador
	z80 *cpu = new z80(addressBus, dataBus, ram);
	
	// Crea la memoria RAM
	memoria *ram = new Memoria()
	
	// Carga la ROM de la máquina
	ram->load("48.rom");
	
	// Crea los buses
	/*Bus *addressBus = new Bus(16);
	Bus *dataBus = new Bus(8);
	
	addressBus->add(cpu);
	addressBus->add(ram);
	
	while(!exit) {
		cpu->Run();
	}*/
	
	for(int i=1;i<10;i++) {
		cpu->Run();
	}
}
	