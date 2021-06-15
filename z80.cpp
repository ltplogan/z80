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
	
	byte INTERRUPT = 0;
	
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
					break;
				
				case 1:
					switch(q)
					{
						case 0:
							emular_LD_RR_nn(p);  //LD BC, nm
							break;
						case 1:
							emular_ADD_HL_nn(p);
							break;
					}
					break;
				
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
							switch(p) 
							{
								case 0:
									emular_LD_A_BC();
									break;
								case 1:
									emular_LD_A_DE();
									break;
								case 2:
									emular_LD_HL_NN();
									break;
								case 3:
									emular_LD_A_NN();
									break;
							}
							break;
					}
					break;
				case 3:
					switch(q)
					{
						case 0:
							emular_INC_RR(p);
							break;
						case 1:
							emular_DEC_RR(p);
							break;
					}
					break;
				case 4:
					emular_INC_R(y);
					break;
				case 5:
					emular_DEC_R(y);
					break;
				case 6:
					emular_LD_R_N(y);
					break;
				case 7:
					switch(y)
					{
						case 0:
							emular_RLCA();
							break;
						case 1:
							emular_RRCA();
							break;
						case 2:
							emular_RLA();
							break;
						case 3:
							emular_RRA();
							break;
						case 4:
							emular_DAA();
							break;
						case 5:
							emular_CPL();
							break;
						case 6:
							emular_SCF();
							break;
						case 7:
							emular_CCF();
							break;
					}
					break;
				
			} // SWITCH(z)
			
		break; // CASE 0
		
		case 1:
	} // SWITCH(x)
} // RUN

byte z80::readByteFromMemory(word direccion) {
	addressBus->Write(direccion);
	memoria->RD();
	
	return dataBus->Read();
}

byte z80::getRegisterFromIndex(byte index)
{
	byte valorRegister = 0;
	
	switch(index)
	{
		case 0: valorRegister = regs.B; break;
		case 1: valorRegister = regs.C; break;
	}
	
	return valorRegister;
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
	Tstates += 13;
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
			memoria->WR();
			regs.SP = dataBus->Read();
			break;
	}
	
	Tstates += 10; // Hay que accerder a memoria 2 veces porque el bus es de 8
	regs.PC += 3;
}

void z80::emular_ADD_HL_RR(byte p) // suma un HL con otro registo de 16 bit RR 
{
	/*
	El contenido del par de registros ss se agrega al contenido del par de registros HL y el resultado se almacena en HL. Si el par de registros HL contiene el número entero 4242h y el par de registros DE contiene 1111h, entonces tras la ejecución de una instrucción ADD HL, DE, el par de registros HL contiene 5353h.
	*/
	switch(p) 
	{
		// add hl,bc
		case 0:
			regs.HL += regs.BC; 
			break;	
		// add hl,de
		case 1:
			regs.HL += regs.DE;
			break;
		// add hl,hl
		case 2:
			regs.HL += regs.HL;
			break;
		// add hl,sp
		case 3:
			regs.HL += regs.SP;
			break;
	}
	
	// Al salir de el switch los FLAGS
	
	//regs.HF
	//regs.CF 
	regs.NF = 0; // reset = 0 y set = 1   
	Tstates += 11; 
}

void z80::emular_LD_BC_A() 
{
	/*
	El contenido del acumulador se carga en la ubicación de memoria especificada por el contenido del par de registros BC.
	*/
	
	// (BC) <- A
	// En el bus de direcciones hay que escribir el registro BC
	addressBus->Write(regs.BC);
	// En el bus de datos hay que poner el registro A
	dataBus->Write(regs.A);
	regs.PC += 1;
			
	Tstates += 7;
}

void z80::emular_LD_DE_A()
{
	// En el bus de direcciones hay que escribir el registro DE 
	addressBus->Write(regs.DE);
	// En el bus de datos hay que poner el registro A
	dataBus->Write(regs.A);
	regs.PC += 1;
	
	Tstates += 7;
}

void z80::emular_LD_NN_HL()
{
	/*
		El contenido de la porción de orden inferior del par de registros HL (Registro L) se carga en la dirección de memoria (nn) y el contenido de la porción de orden superior de HL (Registro H) se carga a la siguiente dirección de memoria más alta (nn + 1). El primer operando n después del código de operación es el Byte de orden inferior de nn.
	*/
	
	// (nn + 1) ← H, (nn) ← L
	
	byte low = readByteFromMemory(regs.PC + 1);
	byte high = readByteFromMemory(regs.PC + 2);
	
	word nn = high * 256 + low;
	
	addressBus->Write(nn); 
	dataBus->Write(regs.L;
	memoria->WR();
	addressBus->Write(nn + 1);
	dataBus->Write(regs.H);
	memoria->WR();
	regs.PC += 3;
	Tstates += 16;
}
								
void z80::emular_LD_NN_A()
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

void z80::emular_LD_A_BC() 
{
	// A <- (BC)
	addressBus->Write(regs.BC);
	dataBus->RD();
	regs.A = dataBus->Read(); // Lee lo que hay en el bus y lo mete en A
	regs.PC += 1;
	Tstates += 7;
}

void z80::emular_LD_A_DE() 
{
	// A <- (DE)
	addressBus->Write(regs.DE);
	dataBus->RD();
	regs.A = dataBus->Read(); // Lee lo que hay en el bus y lo mete en A
	regs.PC += 1;
	Tstates += 7;
}
								
void z80::emular_LD_HL_NN()
{
	/*addressBus->Write(regs.PC + 1);
	memoria->RD();
	regs.L = dataBus->Read();
	
	addressBus->Write(regs.PC + 2);
	memoria->RD();
	regs.h = dataBus->Read();
	*/
	
	regs.L = readByteFromMemory(regs.PC + 1);
	regs.H = readByteFromMemory(regs.PC + 2);
	
	regs.PC += 3;
	Tstates += 16;
}

void z80::emular_LD_A_NN()
{
	regs.A = readByteFromMemory(regs.PC + 1);
	regs.PC += 3;
	Tstates += 13;
}

void z80::emular_INC_RR(p)
{
	switch(p)
	{
		case 0: // BC
			regs.BC += 1;
			break;
		case 1: // DE
			regs.DE += 1;
			break;
		case 2: // HL
			regs.HL += 1;
			break;
		case 3: // SP
			regs.SP += 1;
			break;
	}
	
	regs.PC += 1;
	Tstates += 6;
}

void z80::emular_DEC_RR(p)
{
	switch(p)
	{
		case 0: // BC
			regs.BC -= 1;
			break;
		case 1: // DE
			regs.DE -= 1;
			break;
		case 2: // HL
			regs.HL -= 1;
			break;
		case 3: // SP
			regs.SP -= 1;
			break;
	}
	
	regs.PC += 1;
	Tstates += 6;
}

void  z80::emular_INC_R(p)
{
	switch(p)
	{
		case 0:
			regs.B++;
			break;
		case 1:
			regs.C++;
			break;
		case 2:
			regs.D++;
			break;
		case 3:
			regs.E++;
			break;
		case 4:
			regs.H++;
			break;
		case 5:
			regs.L++;
			break;
		case 6:
			byte tmp = readByteFromMemory(regs.HL); // Lee 1 byte es byte hay que incrementarlo 
			// En la misma posición de memoria HL hay que escribir tmp + 1
			addressBus->Write(regs.HL);
			dataBus->Write(tmp + 1);
			memoria->WR();
			break;
		case 7:
			regs.A++;
			break;
	}
	
	// Asignar todos los flags
	regs.PC += 1;
	Tstates += 4;
}

void z80::emular_DEC_R(p)
{
	switch(p)
	{
		case 0:
			regs.B--;
			break;
		case 1:
			regs.C--;
			break;
		case 2:
			regs.D--;
			break;
		case 3:
			regs.E--;
			break;
		case 4:
			regs.H--;
			break;
		case 5:
			regs.L--;
			break;
		case 6:
			byte tmp = readByteFromMemory(regs.HL); // Lee 1 byte es byte hay que incrementarlo 
			// En la misma posición de memoria HL hay que escribir tmp + 1
			addressBus->Write(regs.HL);
			dataBus->Write(tmp - 1);
			memoria->WR();
			break;
		case 7:
			regs.A--;
			break;
	}
	
	// Asignar todos los flags
	regs.PC += 1;
	Tstates += 4;
}

void z80::emular_LD_R_N(y)
{
	byte n = readByteFromMemory(regs.PC + 1);
	switch(y)
	{
		case 0:
			regs.B = n;
			break;
		case 1;
			regs.C = n;
			break;
		case 2:
			regs.D = n;
			break;
		case 3:
			regs.E = n;
			break;
		case 4: 
			regs.H = n;
			break;
		case 5:
			regs.L = n;
		case 6:
			addressBus->Write(regs.HL);
			dataBus->Write(n);
			memoria->WR();
			break;
		case 7:
			regs.A = n;
			break;
	}
	
	regs.PC += 2;
	Tstates + = 7;
}

void z80::emular_RLCA()
{
	// Toma el bit de signo (bit 7)
	byte bit_7 = (regs.A | 0x80) >> 7;
	
	// Desplazamiento de 1 bit a la izquierda
	regs.A = regs.A << 1;
	
	// Se copia el bit de signo en el bit 0
	regs.A = regs.A | bit_7;
	
	// Flags
	regs.HF = 0;
	regs.NF = 0;
	regs.CF = bit_7;
	
	regs.PC += 1;
	Tstates += 4;
}

void z80::emular_RRCA()
{
	// Toma el bit 0
	byte bit_0 = regs.A & 0x01;
	
	// Desplazamiento de 1 bit a la izquierda
	regs.A = regs.A >> 1;
	
	// Se copia el bit 0 en el bit 7
	regs.A = regs.A | (bit_0) ? 0x80 : 0x00;
	
	// Flags
	regs.HF = 0;
	regs.NF = 0;
	regs.CF = bit_0;
	
	regs.PC += 1;
	Tstates += 4;
}

void z80::emular_RLA()
{
	// Almacena el CF
	byte carryFlag = regs.CF;
	
	// Toma el bit 7
	byte bit_7 = get_bit(7, regs.A);
	
	// Desplazamiento a la izquierda 1 bit_0
	regs.A = regs.A << 1;
	
	regs.CF = bit_7;
	regs.A = regs.A | carryFlag;
	
	// Flags
	regs.HF = 0;
	regs.NF = 0;
	
	Tstates += 4;
	regs.Pc += 1;
}

/*
	A = 10101100 		bit de estado CF = 1
	Rota el que sale vuelve a entrar
	Desplazamiento no vuelve a entrar
	
		carryFlag = 1
		bit_0 = 0
		
		1010110 este 0 se ha perdido  
							0 1010110
		or con el CF = 1	1 0000000
							---------
							11010110 = A y CF = 0
*/

void z80::emular_RRA()
{
	// Almacena el CF
	byte carryFlag = regs.CF;
	
	// Toma el bit 7
	byte bit_0 = get_bit(0, regs.A);
	
	// Desplaza 1 bit a la derecha
	regs.A = regs.A >> 1;
	
	// Colocar el carryFlag en el bit 7
	if(carryFlag) {
		regs.A = regs.A | 0x80;
	}
	
	regs.CF = bit_0;
	
	// Flags
	regs.HF = 0;
	regs.NF = 0;
	
	Tstates += 4;
	regs.Pc += 1;
}

/*
	A = 1001 0001 -> 11 = 0001 0001 en BCD 
*/

void z80::emular_DDA()
{
	
}

void z80::emular_CPL() // Duda
{
	// Niega 0 pone 1 1 pone un 0
	// Complemento a 1 a 2
	regs.A = ~regs.A;
	regs.HF = 1;
	regs.NF = 1;
	
	regs.PC += 1;
	Tstates += 4;
}

void z80::emular_SCF() 
{
	regs.CF = 1;
	regs.HF = 0;
	regs.NF = 0;
	
	regs.PC += 1;
	Tstates += 4;
}

void z80::emular_CCF() 
{
	// Complemento de carry flag
	regs.HF = regs.CF;
	regs.CF = ~regs.CF; // negar carryFlag
	regs.NF = 0;
	
	regs.PC += 1;
	Tstates += 4;
}

void z80::emular_LD_R8_R8(byte rd, byte ro)
{
	/*
		El contenido de cualquier registro r' se carga en cualquier otro registro r.r, r' identifica cualquiera de los registros A, B, C, D, E, H o L, 
		ensamblados de la siguiente manera en el código de objeto:
	*/
	
	byte valor = getRegisterFromIndex(rd);
	
	switch(rd)
	{
		case 0: regs.B = valor; break;
		case 1: regs.C = valor; break;
		case 2: regs.D = valor; break;
		case 3: regs.E = valor; break;
		case 4: regs.H = valor; break;
		case 5: regs.L = valor; break;
		case 6: regs.A = valor; break;
	}
	
	regs.PC += 1;
	Tstates += 4;
}

void z80::INT()
{
	INTERRUPT = 1;
}

void z80::emular_HALT()
{
	// Para el procesador. Para los dispositivos que son más lentos para el procesador. 
	while(!INTERRUPT) 
		NOP();
		
	INTERRUPT = 0;
	Tstates += 4;
	regs.PC += 1;
}

void z80::emular_ALU_Operation(byte operation, byte reg) // en el switch emular_ALU_Operation(y, z)
{
	// ALU[y], r[z]
	switch(operation)
	{
		case 0: ADD(reg); break;
		case 1: ADC(reg); break;
		case 2: SUB(reg); break;
		case 3: SBC(reg); break;
		case 4: AND(reg); break;
		case 5: XOR(reg); break;
		case 6: OR(reg); break;
		case 7: CP(reg); break;
	}
}

// Suma el acumulador con otro registro de 8 bits
void z80::emular_ADD(byte ro)
{
	// Calcula la suma 8 bits y afecta los flags HF, PF y CF
	regs.A = ALU->add(regs.A, getRegisterFromIndex(ro));
	
	// SF
	// Número negativo 11100011. Negativo significa que el bit más significativo está puesto a 1
	// se llama complemento a2
	// el bit 7 te indica si el número es positivo negativo
	// empieza por cero es positivo en 1 negativo
	
	// Comprueba que está a 0 o a 1
	regs.SF = ((regs.A & 0x80) == 0x80) ?  1 : 0;
	
	// ZF
	regs.Z = (regs.A == 0) ? 1 : 0;
	
	// NF
	regs.N = 0;
	
	Tstates += 4;
	regs.PC += 1;
	
	/*
		1000 0101 valor del registro A
	&	1000 0000 0x80 
		---------
		Me da todo a 0 y lo que haya en el bit 7
		1000 0000 = 0x80
		
		0000 0101 valor del registro A
	&	1000 0000 0x80 
		---------
		0000 0000 = 0x80
		
		Si le hago un & a un registro de 8 bit con 80 y me da 80 
		es que el bit 7 está a 1 si no lo pongo a 0
	*/
}

// Suma con acarreo de acumulador con otro registro
void z80::emular_ADC(byte ro)
{
	// Calcula la suma 8 bits y afecta los flags HF, PF y CF
	regs.A = ALU->add(regs.A, getRegisterFromIndex(ro));
	
	// Suma acarreo si el carry flag es a 1
	regs.A = ALU->add(regs.A, regs.CF);
	
	// SF
	regs.SF = ((regs.A & 0x80) == 0x80) ? 1 : 0;
	
	// ZF
	regs.ZF = (regs.A == 0) ? 1 : 0;
	
	// NF
	regs.NF = 0;
	
	Tstates += 4;
	regs.PC += 1;
}

void z80::emular_SUB(byte ro)
{
	// El procesador no sabe restar, lo que hace es sumar el complemento a2 del otro número
	// 21 - 33  -> 21 + (-33 complemento a2)
	
	// Calcula la resta 8 bits y afecta los flags HF, PF y CF
	regs.A = ALU->add(regs.A, c2(getRegisterFromIndex(ro)));
	
	// SF
	regs.SF = ((regs.A & 0x80) == 0x80) ? 1 : 0;
	
	// ZF
	regs.ZF = (regs.A == 0) ? 1 : 0;
	
	// NF
	regs.NF = 1;
	
	Tstates += 4;
	regs.PC += 1;
}

void z80::emular_SBC(byte ro)
{
	regs.A = ALU->add(regs.A, c2(getRegisterFromIndex(ro)));
	
	// Resta CF
	regs.A = ALU->add(regs.A, c2(regs.CF));
	
	// SF
	regs.SF = ((regs.A & 0x80) == 0x80) ? 1 : 0;
	
	// ZF
	regs.ZF =(regs.A == 0) ? 1 : 0;
	
	// NF
	regs.NF = 1;
	
	Tstates += 4;
	regs.PC += 1;
	
	// función pendiente ALU->sub(regs.A, regs.CF)
	// la función llama a add con el complemento a2 por el segundo parametro
	
	/*
		CF 0000 0001  complemento a2 a negativo -> 1000 0001
		
		complemento a1 							1111 1110 -1 en complemento a1
		complemento a2 = complemento a1 + 1		0000 0001
												---------
												1111 1111 -> -1 en complemento a2
	*/
}

void z80::emular_AND(byte reg)
{
	regs.A = regs.A ^ getRegisterFromIndex(reg);
	
	// Resta CF
	regs.A = ALU->add(regs.A, c2(regs.CF));
	
	// SF
	regs.SF = ((regs.A & 0x80) == 0x80) ? 1 : 0;
	
	// ZF
	regs.ZF =(regs.A == 0) ? 1 : 0;
	
	// HF
	regs.NF = 0;
	
	// NF
	regs.NF = 0;
	
	// CF
	regs.CF = 1;
	
	Tstates += 4;
	regs.PC += 1;
}

void z80::emular_RET()
{
	// RET retorno de la función 
	// Lo que haya en la cima lo recupera. La pila está apuntada por el registro SP
	
	// SP pila  SP -> cima
	// Saca un valor de 16 bit y se lo asigna al registro PC
	// y la cima de la pila la decrementa en 2
	
	byte low = readByteFromMemory(regs.SP);
	byte high = readByteFromMemory(regs.SP + 1); 
	
	regs.PC = high * 256 + low; // el regs.PC += 1 salta aquí
	regs.SP -= 2; // Porque es un registo de 16 bit
	
	Tstates += 10;
}

void z80::emular_POP(byte reg)
{
	switch(reg) 
	{
		case 0: 
			regs.C = readByteFromMemory(regs.SP); break;
			regs.B = readByteFromMemory(regs.SP + 1); break;
		case 1: 
			regs.E = readByteFromMemory(regs.SP); break;
			regs.D = readByteFromMemory(regs.SP + 1); break;
		case 2: 
			regs.L = readByteFromMemory(regs.SP); break;
			regs.H = readByteFromMemory(regs.SP + 1); break;
		case 3: 
			regs.F = readByteFromMemory(regs.SP); break;
			regs.A = readByteFromMemory(regs.SP + 1); break;
	}
	
	// SP = FFFF PUSH HL -> SP = FFFD decrece hacia abajo decrece 2
	//			 PUSH BC -> SP	 FFFB
	//			 POP BC -> SP = FFFD
	//           POP HL -> SP = FFFF Apila suma y desapila resta
	
	// Apila con registros de 16 bits (2 bytes)
	// PUSH inserta 16 bits -> SP = SP - 2
	// POP extrae 16 bits -> SP = SP + 2
	regs.SP += 2; 
	
	Tstates += 10;
	regs.PC += 1;
}

void z80::emular_CP(byte reg)
{
	// Restar y afectar los flags
	// Para los if en ensamblador 
	// Comparar es restar y restar es sumar con el complemento a2
	byte temp = regs.A - getRegisterFromIndex(reg);
	
	// CF, VF, HF
	byte temp = ALU->add(regs.A, c2(getRegisterFromIndex(reg)));
	
	// SF
	regs.SF = ((regs.A & 0x80) == 0x80) ? 1 : 0;
	
	// ZF
	regs.ZF =(regs.A == 0) ? 1 : 0;
	
	// NF
	regs.NF = 1;
	
	Tstates += 4;
	regs.PC += 1;
}

void z80::emular_JP_HL()
{
	// Salto absoluto
	byte low = readByteFromMemory(regs.HL);
	byte high = readByteFromMemory(regs.HL + 1); 
	
	regs.PC = high * 256 + low; 
	
	Tstates += 4;
}

void z80::emular_EXX()
{
	// Intercambio de los banco de registro
	
	word temp;
	
	// BC
	temp = regs.BC;
	regs.BC = regs._BC;
	regs._BC = temp;
		
	// DE
	temp = regs.DE;
	regs.DE = regs._DE;
	regs._DE = temp;
		
	// HL
	temp = regs.HL;
	regs.HL = regs._HL;
	regs._HL = temp;
	
	Tstates += 4;
	regs.PC += 1;
}

void z80::emular_LD_SP_HL() 
{
	// SP <- HL
	// La pila la mete en HL
	regs.SP = regs.HL;
	
	Tstates += 6;
	regs.PC += 1;
}


// Salto absoluto  pero condicional
// cc condicional
void z80::emular_JP_CC_nn(byte y)
{
	byte dirLow = readByteFromMemory(regs.PC + 1);
	byte dirHigh = readByteFromMemory(regs.PC + 2);
	word dirSalto = dirHigh * 256 + dirLow;
	
	// Tipo de salto
	switch(y)
	{
		case 0: // JP NZ, nn
			(regs.ZF == 0) ? regs.PC = dirSalto : regs.PC += 3; break;
		case 1: // JP Z, nn
			(regs.ZF == 1) ? regs.PC = dirSalto : regs.PC += 3; break;
		case 2: // JP NC, nn
			(regs.CF == 0) ? regs.PC = dirSalto : regs.PC += 3; break;
		case 3: // JP C, nn
			(regs.CF == 1) ? regs.PC = dirSalto : regs.PC += 3; break;
		case 4: // JP PO, nn
			(regs.PF == 0) ? regs.PC = dirSalto : regs.PC += 3; break;
		case 5: // JP PE, nn
			(regs.PF == 1) ? regs.PC = dirSalto : regs.PC += 3; break;
		case 6: // JP NS, nn
			(regs.SF == 0) ? regs.PC = dirSalto : regs.PC += 3; break;
		case 7: // JP S, nn
			(regs.SF == 1) ? regs.PC = dirSalto : regs.PC += 3; break;
	}
	
	Tstates += 10;
}

// Salto incondicional
void z80::emular_JP_nn() 
{
	// Salto absoluto incondicional PC <- nn
	byte dirLow = readByteFromMemory(regs.PC + 1);
	byte dirHigh = readByteFromMemory(regs.PC + 2);
	// La monta 
	word dirSalto = dirHigh * 256 + dirLow;
	
	regs.PC = dirSalto;
	
	Tstates += 10;
}

void z80::emular_AfterCB()
{
	// Lee el siguiente código de operación
	opcode = readByteFromMemory(++regs.PC);
	descodificar(opcode);
	// En función de x
	switch(x)
	{
		case 0: // Rotaciones y desplazamientos
			switch(y)
			{
				case 0: RLC(z); break; // R rotación
				case 1: RCC(z); break;
				case 2: RL(z); break;
				case 3: RR(z); break;
				case 4: SLA(z); break; // S desplazamiento
				case 5: SRA(z); break;
				case 6: SLL(z); break;
				case 7: SRL(z); break;
			}
			
			break;
		case 1: BIT(byte index, byte regs); break;
		case 2: RES(byte index, byte regs); break;
		case 3: SET(byte index, byte regs); break;
		
	}
}

void z80::emular_RLC(byte z)
{
	// Coge el bit 7 y lo pasa al flag de acarreo 
	// Mira el registro de que se trata
	switch(z)
	{
		case 0: 
			// Registro B
			// Toma el bit 7
			byte bit_7 = get_bit(7, regs.B);
				
			// Desplaza a la izquierda 1 bit
			regs.B = regs.B << 1;
			
			// Se copia el bit 7 en el bit 0
			regs.B = regs.B | bit_7;
			
			// Se copia el bit 7 en el carry flag
			regs.CF= bit_7;
			
			break;
		
		case 1: 
			byte bit_7 = get_bit(7, regs.C);
				
			// Desplaza a la izquierda 1 bit
			regs.C = regs.C << 1;
			
			// Se copia el bit 7 en el bit 0
			regs.C = regs.C | bit_7;
			
			// Se copia el bit 7 en el carry flag
			regs.CF= bit_7;
			
			break;
		case 2: 
			byte bit_7 = get_bit(7, regs.D);
				
			// Desplaza a la izquierda 1 bit
			regs.D = regs.C << 1;
			
			// Se copia el bit 7 en el bit 0
			regs.D = regs.D | bit_7;
			
			// Se copia el bit 7 en el carry flag
			regs.CF= bit_7;
			
			break;
		case 3: 
			byte bit_7 = get_bit(7, regs.E);
				
			// Desplaza a la izquierda 1 bit
			regs.E = regs.E << 1;
			
			// Se copia el bit 7 en el bit 0
			regs.E = regs.E | bit_7;
			
			// Se copia el bit 7 en el carry flag
			regs.CF= bit_7;
			
			break;
		case 4: 
			byte bit_7 = get_bit(7, regs.H);
				
			// Desplaza a la izquierda 1 bit
			regs.H = regs.H << 1;
			
			// Se copia el bit 7 en el bit 0
			regs.H = regs.H | bit_7;
			
			// Se copia el bit 7 en el carry flag
			regs.CF= bit_7;
			
			break;
		case 5: 
			byte bit_7 = get_bit(7, regs.L);
				
			// Desplaza a la izquierda 1 bit
			regs.L = regs.L << 1;
			
			// Se copia el bit 7 en el bit 0
			regs.L = regs.L | bit_7;
			
			// Se copia el bit 7 en el carry flag
			regs.CF= bit_7;
			
			break;
		case 6: 
			byte bit_7 = get_bit(7, regs.A);
				
			// Desplaza a la izquierda 1 bit
			regs.A = regs.A << 1;
			
			// Se copia el bit 7 en el bit 0
			regs.A = regs.A | bit_7;
			
			// Se copia el bit 7 en el carry flag
			regs.CF= bit_7;
			
			break;
	}
}

void z80::emular_RRC(byte z)
{
	// Mira el registro
	switch(z)
	{
		case 0: // Registro B
				// Toma el bit 7
				byte bit_0 = get_bit(0. regs.B);
				
				// Desplaza a la izquierda 1 bit
				regs.B = regs.B >> 1;
				
				// Se copia el bit 7 en el bit 0
				regs.B = regs.B | bit_0;
				
				// Se copia el bit 7 en el carry flag
				regs.CF= bit_0;
				
				break;
		
		case 1: // Idem
		case 2: // Idem
	}
}

void z80::emular_RL(byte z)
{
	switch(z)
	{
		case 0: // RL B
			byte bit_7 = get_bit(7, regs.B);
			regs.B = regs.B << 1;
			regs.B = regs.B | regs.CF;
			r.CF = bit_7;
			break;
	}
}

void z80::emular_RR(byte z)
{
	switch(z)
	{
		case 0: // RR B
			byte bit_0 = get_bit(0, regs.B);
			regs.B = regs.B >> 1;
			if(regs.CF == 1)
				regs.B = regs.B | 0x80;
			r.CF = bit_0;
			break;
	}
}

void z80::emular_SLA(byte z)
{
	switch(z)
	{
		case 0: // Registro B -> SLA B
			byte bit_7 = get_bit(7, regs.B);
			regs.B = regs.B << 1;		
			regs.CF = bit_7;
			break;
	}
}

void z80::emular_SRA(byte z)
{
	switch(z)
	{
		case 0: // Registro B -> SRA B
			byte bit_0 = get_bit(0, regs.B);
			regs.B = regs.B >> 1;		
			regs.CF = bit_0;
			break;
	}
}

// Instrucciones no oficiales y por falta de tiempo no las vamos a implementar
void z80::emular_SLL(){}
void z80::emular_SRL(){}

void z80::emular_BIT(byte index, byte reg)
{
	(get_bit(index, getRegisterFromIndex(reg)) == 0) ? regs.ZF = 1 : regs.ZF = 0;
	regs.HF = 1;
	regs.NF = 0;
	
	Tstates += 8;
	regs.Pc += 2;
}

/*
	Devuelve el bit en posicion index (0..7)
*/

byte z80::get_bit(byte index, byte reg)
{
	byte pesos[] = {1,2,4,8,16,32,64,128};
	
	return (reg & pesos[index]) >> index;
}

void z80::emular_RES(byte index, byte reg)
{
	byte pesos[] = {1,2,4,8,16,32,64,128};
	switch(reg)
	{
		case 0: // Registro B
			// Máscara
			regs.B = regs.B & (255 - peso[index]); break;
		case 1: // Regsitro C // Table  "r"
	}
}

void z80::emular_SET(byte index, byte reg)
{
	byte pesos[] = {1,2,4,8,16,32,64,128};
	switch(reg)
	{
		case 0: // Registro B
			// Máscara
			regs.B = regs.B | (peso[index]); break;
		case 1: // Regsitro C // Table  "r"
	}
}

void z80::emular_OUT_n_A()
{
	// No implementada
	// Se usa en el bus de I/O
}

void z80::emular_IN_A_N() 
{
	// No implementada
	// Se usa en el bus de I/O
}

void z80::emular_EX_SP_HL() 
{
	bye low = readByteFromMemory(regs.SP);
	byte high = readByteFromMemory(regs.SP + 1);
	
	word temp = regs.HL;
	
	regs.HL = high * 256 + low;
	
	addressBus.write(regs.SP);
	dataBus.write(regs.L);
	memoria.WR();
	
	addressBus.write(regs.SP + 1);
	dataBus.write(regs.H);
	memoria.WR();
	
	Tstates += 19;
	regs.PC += 1;
}

void z80::emular_EX_DE_HL() 
{
	word temp = regs.HL;
	
	regs.HL = regs.DE;
	regs.DE = temp;
	
	Tstates += 4;
	reg.PC +=  1;
}

// Habilita y desabilita interrupciones
void z80::emular_DI() 
{
	interrupt_enable = 0;
	Tstates += 4;
	regs.PC += 1;
}

void z80::emular_EI() 
{
	interrupt_enable = 1;
	Tstates += 4;
	regs.PC += 1;
}

void z80::emular_CALL_CC_nn(byte y)
{
	// Lee la dirección de salto
	byte dirLow = readByteFromMemory(regs.PC + 1);
	byte dirHigh = readByteFromMemory(regs.PC + 2);
	word dirSalto = dirHigh * 256 * dirLow;
	
	// Mete en la pila la dirección de retorno
	// Mete en la pila el registro PC + 3
	
	addressBus.write(regs.SP - 1);
	dataBus.write((byte)(regs.PC + 3) & 0xFF00 >> 8));
	memoria.WR();
	
	addressBus.write(regs.SP - 2);
	dataBus.write((byte)(regs.PC + 3) & 0x00FF));
	memoria.WR();
	bool salta = false;
	
	switch(y)
	{
		case 0: // CALL NZ, nn
			(regs.ZF == 0) ? salta = true : salta = false; break;
		case 1: // CALL Z, NN	
			(regs.ZF == 1) ? salta = true : salta = false; break;
		case 2: // CALL NC, nn
			(regs.CF == 0) ? salta = true : salta = false; break;
		case 3: // CALL C, nn
			(regs.CF == 1) ? salta = true : salta = false; break;
		case 4: // CALL PO, nn
			(regs.PF == 0) ? salta = true : salta = false; break;
		case 5: // CALL PE, nn
			(regs.PF == 1) ? salta = true : salta = false; break;
		case 6: // CALL NS, nn
			(regs.SF == 0) ? salta = true : salta = false; break;
		case 7: // CALL S, nn	
			(regs.SF == 1) ? salta = true : salta = false; break;
	}
	
	if(salta)
	{
		regs.PC = dirSalto;
		Tstates += 17;
	} else {
		regs.PC += 3;
		Tstates += 10;
	}
}

void z80::emular_PUSH(byte p)
{
	switch(p) 
	{
		case 0: // PUSH BC
			addressBus.write(regs.SP - 1);
			dataBus.write(regs.B);
			memoria.WR();
			
			addressBus.write(regs.SP - 2);
			dataBus.write(regs.C);
			memoria.WR();
			break;
		case 1: // PUSH DE
			addressBus.write(regs.SP - 1);
			dataBus.write(regs.D);
			memoria.WR();
			
			addressBus.write(regs.SP - 2);
			dataBus.write(regs.E);
			memoria.WR();
			break;
		case 2: // PUSH HL
			addressBus.write(regs.SP - 1);
			dataBus.write(regs.H);
			memoria.WR();
			
			addressBus.write(regs.SP - 2);
			dataBus.write(regs.L);
			memoria.WR();
			break;
		case 3: // PUSH AF
			addressBus.write(regs.SP - 1);
			dataBus.write(regs.A);
			memoria.WR();
			
			addressBus.write(regs.SP - 2);
			dataBus.write(regs.F);
			memoria.WR();
			break;
	}
	
	Tstates += 11;
	regs.PC += 1;
}

void z80::emular_CALL_nn()
{
	// Lee la dirección de salto
	byte dirLow = readByteFromMemory(regs.PC + 1);
	byte dirHigh = readByteFromMemory(regs.PC + 2);
	word dirSalto = dirHigh * 256 * dirLow;
	
	// Mete en la pila la dirección de retorno
	// Mete en la pila el registro PC + 3
	
	addressBus.write(regs.SP - 1);
	dataBus.write((byte)(regs.PC + 3) & 0xFF00 >> 8));
	memoria.WR();
	
	addressBus.write(regs.SP - 2);
	dataBus.write((byte)(regs.PC + 3) & 0x00FF));
	memoria.WR();
	
	Tstates += 17;
	regs.PC = dirSalto;
}

void z80::emular_AfterDD()
{
	// Lee el siguiente código de operación
	opcode = readByteFromMemory(++regs.PC);
	descodificar(opcode);
	
	if(opcode == 0xDD || opcode == 0xED || opcode = 0xFD)
		NOP();
	else if(opcode == 0xCB)
		AfterDDCB();
	else {
		// ?
		replace_HL_IX = true;
	}
	
	// Tstates = ?
	regs.PC += 1;
}

void z80::emular_AfterED()
{
	if(x == 0 || x = 3)
		NOP();
	else if(x == 1)
	{
		switch(z)
		{
			case 0:
				if(y == 6)
					IN_C();
				else 
					IN_R8_C();
				break;
			case 1:
				if(y == 6)
					OUT_C_0();
				else 
					OUT_C_R8();
				break;
			case 2:
				(q == 0) ? SBC_HL_RR() : ADC_HL_RR(); break;
			case 3:
				(q == 0) ? LD_nn_RR() : LD_RR_nn(); break;
			case 4:
				NEG();
			case 5:
				(y == 0) ? RETN() : RETI(); break;
			case 6: 
				IM(y);
				break;
			case 7:
				switch(y)
				{
					case 0: LD_I_A(); break;
					case 1: LD_R_A(); break;
					case 2: LD_A_I(); break;
					case 3: LD_A_R(); break;
					case 4: RRD(); break;
					case 5: RLD(); break;
					case 6: 
					case 7: NOP(); break;
				}
		}
	}
	else if(x == 2)
	{
		// Instrucciones de bloque
		BLI(y,z);
	}
}

void z80::emular_AfterFD()
{
	// Lee el siguiente código de operación
	opcode = readByteFromMemory(++regs.PC);
	descodificar(opcode);
	
	if(opcode == 0xDD || opcode == 0xED || opcode = 0xFD)
		NOP();
	else if(opcode == 0xCB)
		AfterXXCB();
	else 
		// ?
		replace_HL_IX = true;
	
	
	// Tstates = ?
	regs.PC += 1;
}

// Mejoras memoria
void z80::emular_AfterXXCB()
{
	// ?
}

void z80::emular_ADC_HL_RR(byte p)
{
	switch(p)
	{
		case 0: // ADD HL, BC
			regs.HL = ALU->adc(regs.HL, regs.BC);
			break;
		case 1: // ADC HL, DE
			regs.HL = ALU->adc(regs.HL, regs.BC);
			break;
		case 2: // ADC HL, HL
			regs.HL = ALU->adc(regs.HL, regs.HL);
			break;
		case 3: // ADC HL, SP
			regs.HL = ALU->adc(regs.HL, regs.SP);
			break;
	}
	
	Tstates += 15;
	regs.PC += 2;
}











