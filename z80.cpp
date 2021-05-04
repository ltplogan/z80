// Constructor
z80::z80() {
	// Dirección de memoria de reset
	// ROM -> Sistema operativo + Basic
	regs.PC = 0;
}

/*
	bits
	7 6 5 4 3 2 1 0
	
	y 0 1 0 desplaza 3 veces 
	
	y -> 7 6 5 4 3 2 1 0
	     0 0 0 0 0 0 1 0
*/

z80::descodificar(byte opcode) {
	// Toma los bits 7 y 6
	x = (opcode & 192) >> 6;
	
	// Toma los bits 5, 4, 3
	y = (opcode & 56) >> 3;
	
	// Toma los bits 2, 1, 0
	z = opcode & 7;
	
	// Toma los bits 5, 4
	p = y >> 1;
	
	// Toma el bit 3
	// máscara y & 254
	q = y % 2;
}

// Ejecuta la instrucción decodificada
/*
	Ciclo de funcionamiento:
		1. Busqueda en memoria (Accede a memoria y lee un byte)(PC)
		2. Decodificar opcode
		3. Ejecutar la instrucción correspondiente.
		4. goto 1.
*/

z80::Run() 
{
	byte opcode;
	
	// Accede a memoria y lee un opcode 
	opcode = read(regs.PC);
	
	// Apunta a la siguiente instrucción
	regs.PC++;
	
	// Descodifica la instrucción leída
	// Afecta a las variables x, y, z, p, q
	decodificar(opcode)
	
	//Ejecución 
	switch(x) 
	{
		// x = 0
		case 0: // valor de Z
			switch(z) {
				case 0: // Valor de Y
					switch(y) 
					{
						case 0: // NOP
							// Emular NOP
							break;
						case 1: // EX AF, AF'
							// Cócdigo que emula  EX AF, AF'
							emular_EX_AF_AF();
							break;
						case 2: // DJNZ, e
							emular_DJNZ();
							break;
						case 3: // JR d salto relativo a un desplazamiento de 8 bit 
							emular_JR_d();
							break;
						case 4: // JR NZ, d salto condicional
							emular_JR_NZ();
							break;
						case 5: // JR Z, d
							emular_JR_Z();
							break;
						case 6: // JR NC, d
							emular_JR_NC();
							break;
						case 7: // JR C, d
							emular_JR_C();
							break;
					}			
			}
	}
}

z80::emular_ex_af_af()
{
	temp = regs.AF;
	regs.AF = regs._AF;
	regs._AF = temp;
	Tstates += 4;
}

z80::emular_nop()
{
	Tstates += 4;
}

z80::emular_DJNZ()
{
	byte e = RAM.read();
	
	regs.B--;
	if(B != 0) {
		regs.PC += e;
	}
}
	
z80::emular_JR_d()
{
	// PC = PC + d
	byte d = RAM.read();
	
	regs.PC += d; 
}

z80::emular_JR_Z()
{
	// if Z = 0, continue
	// if Z = 1, PC <- PC + e
	
	if(regs.ZF == 0) {
		regs.PC++;
	} else {
		byte e = RAM.read();
		regs.PC += e;
	}
}

z80::emular_JR_NZ()
{
	// if Z = 1, continue
	// if Z = 0, PC <- PC + e
	
	if(regs.ZF == 1) {
		regs.PC++;
	} else {
		byte e = RAM.read();
		regs.PC += e;
	}
}







