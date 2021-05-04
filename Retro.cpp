int main() 
{
	// Crea el procesador
	z80 *cpu = new Cpu();
	
	// Crea la memoria RAM
	memoria *ram = new Memoria()
	
	// Carga la ROM del sistema
	ram->load("ROM.BIN");
	
	// Crea los buses
	Bus *addressBus = new Bus(16);
	Bus *dataBus = new Bus(8);
	
	addressBus->add(cpu);
	addressBus->add(ram);
	
	while(!exit) {
		cpu->Run();
	}
}
	