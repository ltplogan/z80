// Constructor

z80::z80(LocalBus16 *addressBus, LocalBus8 *dataBus, Ram *memoria) {
	this->address = addressBus;
	this->dataBus = dataBus;
	this->memoria = memoria;
	
	// Dirección de memoria de reset
	// ROM -> Sistema operativo + Basic    
	regs.PC = 0; // Indica en qué celdilla de memoria va a leer la instrucción
	// Y hay que pasarlo al bus de direcciones, le diría a la memoria que haga un RD (READ)
	Tstates = 0;
	opcode = 0;
}

/*
	bits
	7 6 5 4 3 2 1 0
	
	y 0 1 0 desplaza 3 veces 
	
	y -> 7 6 5 4 3 2 1 0
	     0 0 0 0 0 0 1 0
*/

void z80::descodificar(byte opcode) {
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

void z80::Run() 
{
	byte opcode;
	
	// 1. Ciclo de búsqueda (Fetch)
	// Escribe en el bus de direcciones el contenido del registro PC
	address->Write(regs.PC);
	
	// Notifica a la memoria que atienda al bus de direcciones
	memoria->RD(); // Ese valor aparecerá en el bus de datos
	
	// Lee el bus de datos en el que la memoria ha dejado una instrucción
	opcode = dataBus->Read();
	
	// Descodifica la instrucción leída
	// Afecta a las variables x, y, z, p, q
	decodificar(opcode)
	
	// Accede a memoria y lee un opcode 
	//opcode = read(regs.PC);
	
	// Apunta a la siguiente instrucción
	regs.PC++;
	
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
				
				case 1:
					switch(q)
					{
						case 0:
							emular_LD_RR_nn(byte p);  //LD BC, nm
							break;
						case 1:
							emular_ADD_HL_nn(byte p);
							break;
					}
				
				case 2:
					switch(q) 
					{
						case 0:
							switch(p)
							{
								case 0:
									emular_LD_BC_A();  
									break;
								case 1:
									emular_LD_DE_A();  
									break;
								case 2:
									emular_LD_NN_HL();  
									break;
								case 3:
									emular_LD_NN_A();  
									break;
							}
							break;
						case 1:
							break;
					}
			}
	}
}

void z80::emular_nop()
{
	Tstates += 4;
	regs.PC += 1;
}

void z80::emular_ex_af_af()
{
	word tmpReg = regs.AF;
	regs.AF = regs._AF;
	regs._AF = tmpReg;
	Tstates += 4;
	regs.PC += 1;
	
	/*temp = regs.AF;
	regs.AF = regs._AF;
	regs._AF = temp;
	Tstates += 4;*/
}

void z80::emular_DJNZ()
{
	// Toma el operando para saltar o no
	addressBus->Write(regs.PC + 1);
	
	// Notifica a la RAM que lea esa posición y me lo devuelve en el bus de datos
	memoria->RD();
	
	byte e = dataBus->read();
	
	// Decrementa el registro B
	regs.B--;
	if(regs.B == 0) {
		r.PC += 2;
		// Continue significa que tiene que continuar ejecutando por el siguiente
		// byte que está detrás del desplazamiento.
		// En memoria hay instrucción y datos. Tiene que continuar por el que viene detrás
		
		Tstates += 8;
	} else {
		regs.PC += e;
		Tstates += 13;
	}
		
}
	
void z80::emular_JR_d()
{
	// PC = PC + d
	byte d = RAM.read();
	
	regs.PC += d; 
}

void z80::emular_JR_Z()
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

void z80::emular_JR_NZ()
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

void z80::emular_JR_NC() // JR NC, e
{
	// if C = 1, continue
	// if C = 0, PC <- PC + e
	/*
		Si la bandera = 1, la siguiente instrucción ejecutada se toma de la ubicación que sigue a esta
		instrucción
	*/
	
	if(regs.ZC == 1) {
		regs.PC++;
	} else {
		byte e = RAM.read();
		regs.PC += e;
	}
}

void z80::emular_JR_C() {
	// if C = 0, continue
	// if C = 1, PC <- PC + e
	
	if(regs.ZC == 0) {
		regs.PC++;
	} else {
		byte e = RAM.read();
		regs.PC += e;
	}
}

/*
	LD RR, nn
	Operation 
		RR <- nn Le asignamos los 2 bytes de memoria. Es un registro de 16 bit.
		Pero la memoria es de 8 bit dos celdillas de 8 bit = 16 bit
		
		Regs p
		BC 00
		DE 01
		HL 02
		
		Registro de pila
		SP 03
*/

void z80::emular_LD_RR_nn(byte p) {
	// En cada celdilla cabe 1 byte
	// Pero los tipos de datos son mayores que 1 byte. Si no cabe en una celdilla se utilizan dos.
	// Y si no caben en dos se utilizan 4
	// 16 bit son dos bytes
	
	switch(p) 
	{
		// BC C el menos significativo
		case 0:
			addressBus->Write(regs.PC + 1);
			memoria->RD();
			regs.C = dataBus->Read();
			
			addressBus->Write(regs.PC + 2);
			memoria->RD();
			regs.B = dataBus->Read();
			break;
		case 1:
		// DE E el menos significativo
			addressBus->Write(regs.PC + 1);
			memoria->RD();
			regs.E = dataBus->Read();
			
			addressBus->Write(regs.PC + 2);
			memoria->RD();
			regs.D = dataBus->Read();
			break;
		case 2:
		// HL L el menos significativo
			addressBus->Write(regs.PC + 1);
			memoria->RD();
			regs.L = dataBus->Read();
			
			addressBus->Write(regs.PC + 2);
			memoria->RD();
			regs.H = dataBus->Read();
			break;
		case 3:
		//SP es entero hay que multiplicar la parte alta y la parte baja
			byte low = readByteFromMemory(regs.PC + 1);
			byte high = readByteFromMemory(regs.PC + 2);
	
			word nn = high * 256 + low;
			addressBus->Write(nn);
			memoria->RD();
			regs.SP = dataBus->Read();;
			break;
	}
	
	Tstates += 10; // Hay que accerder a memoria 2 veces porque el bus es de 8
	regs.PC += 3;
}

void emular_ADD_HL(byte p) 
{
	/*
	El contenido del par de registros ss se agrega al contenido del par de registros HL y el resultado se almacena en HL. Si el par de registros HL contiene el número entero 4242h y el par de registros DE contiene 1111h, entonces tras la ejecución de una instrucción ADD HL, DE, el par de registros HL contiene 5353h.
	*/
	switch(p) 
	{
		// add hl,bc
		case 0:
			
			break;	
		// add hl,de
		case 1:
			
			break;
		// add hl,hl
		case 2:
			
			break;
		// add hl,sp
		case 3:
			
			break;
	}
	
	Tstates += 11; 
}

void emular_LD_BC_A() 
{
	/*
	El contenido del acumulador se carga en la ubicación de memoria especificada por el contenido del par de registros BC.
	*/
	addressBus->Write(regs.BC, regs.A);
			
	Tstates += 7;
}

void emular_LD_DE_A()
{
	addressBus->Write(regs.DE, regs.A);
			
	Tstates += 7;
}

void emular_LD_NN_HL()
{
	/*
		El contenido de la porción de orden inferior del par de registros HL (Registro L) se carga en la dirección de memoria (nn) y el contenido de la porción de orden superior de HL (Registro H) se carga a la siguiente dirección de memoria más alta (nn + 1). El primer operando n después del código de operación es el Byte de orden inferior de nn.
	*/
	
	// (nn + 1) ← H, (nn) ← L
	
	byte low = readByteFromMemory(regs.PC + 1);
	byte high = readByteFromMemory(regs.PC + 2);
	
	word nn = high * 256 + low;
	
	addressBus->Write(nn);
	addressBus->Write(regs.L);
	dataBus->Write(regs.L;
	
	addressBus->Write(nn + 1);
	addressBus->Write(regs.H);
	dataBus->Write(regs.H);
	
	memoria->WR();
	regs.PC += 3;
	Tstates += 16;
}
								
void emular_LD_NN_A()
{
	/*
		El contenido del acumulador se carga en la dirección de memoria especificada por el operando nn. El primer operando n después del código de operación es el byte de orden inferior de nn.
	*/
	
	// (nn) ← A)
	// Lee el operando nn
	byte low = readByteFromMemory(regs.PC + 1);
	byte high = readByteFromMemory(regs.PC + 2);
	
	// Es una palabra de 16 bits, según little endian
	word nn = high * 256 + low;
	
	//Escribe registro A
	addressBus->Write(nn);
	dataBus->Write(regs.A);
	memoria->WR();
	regs.PC += 3;
	Tstates += 13;
}



