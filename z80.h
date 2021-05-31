#ifndef _Z80_
#define _Z80_
#include "zxTypes.h"

class z80
{
	// Interfaz privada
	private: 
		// Banco de registro
		LocalBus16 *addressBus;
		LocalBus8 *dataBus;
		byte opcode;
		Ram *memoria; // Referencia a la memoria
		
	private: 
		// Listado de todas las instrucciones
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
		void emular_JR_NC();
		void emular_JR_C();
		void emular_LD_RR_nn(byte p);
		void emular_ADD_HL(byte p);
		void emular_LD_BC_A();  
		void emular_LD_DE_A();  
		void emular_LD_NN_HL();  
		void emular_LD_NN_A();  
		
	// Atributos 
	public:
		RegisterBank regs;
		// Control de los ciclos de ejecución
		word Tstates;
		// Interfaz pública del Z80
		z80();
		void Run();
};
	
#endif

