#ifndef _Z80_
#define _Z80_
#include "zxTypes.h"

class z80
{
	// Atributos 
	public:
		RegisterBank regs;
		
		// Control de los ciclos de ejeecución
		word Tstates;
		
		// Interfaz privada
		private: 
		// variables internas para descodificación
		byte x, y, z, p, q
		void decodificar(byte opcode);
		
		// Métodos que emulan instrucciones.
		void emular_nop();
		void emular_ex_af_af();
		void emular_djnz();
		void emular_JR_d();
		void emular_JR_Z();
		void emular_JR_NZ();
		
		// Interfaz pública del Z80
		z80();
		void Run();
};
	
#endif

