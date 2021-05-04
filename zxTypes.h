/*
	Emulación procesador Zilog z80A
	
	Interno:
		// Parte alta
		// Parte baja
		- Banco de registros
			BC (16 bits) - B (8 bits) C (8 bits)
			DE (16 bits) - D (8 bits) E (8 bits)
			HL (16 bits) - H (8 bits) L (8 bits)
			IX (16 bits puro) - Indexar array
			IY (16 bits no se pueden romper en dos de 8 bit) - Indexar array 
			SP (16 bits) Registro de pila: cuando se llama a una función o subrutina el registo de pila almacena en memoria la dirección de retorno
											// donde tiene que volver esa función cuando termine.
			AF - A (8 bits) F (8 bits) Nunca se puede trabajar como 16 bits, son los registros más importante de la máquina.
										// A es el registro ACUMULADOR, todas las operaciones aritméticas y lógicas tienen como resultados el registro A.
										// Acumula todos los resultados parciales de todas las operaciones.
										
										// F Flags (Estado): determina si salta, no salta, paridad par o impar. EN QUE ESTADO ESTÁ LA MÁQUINA.
										// Está codificado a nivel de bits.
			
			REGISTROS ALTERNATIVOS
			B'C' Son registros para copia. Si los registros anteriores se quedaban corto te daba la posibilidad de guardar. Lo vuelcas en los primas y cuando quieras recuperar haces otro intercambio en los originales.
			D' E' - Banco alternativos. No se puede ir de uno en uno.
			H' L'
			A' F'
			
			REGISTROS ESPECIALES
			IR - I(8 bits) - Donde se descodifica cada instrucción que haya en la memoria.
			   - R(8 bits) - Lo utiliza el hardware, lo utiliza para el refresco de la memoria RAM. Ese registro se van incrementando con unos ciclos de reloj y se va refrescando la memoria RAM para que no se pierda su contenido
	
	Externo:
		- Bus de direcciones 16 bits (65336) o 65535 (64k)
		- Spectrum 48k -> 64k
							ROM - 16 Kbytes
							RAM - 48 Kbytes
							---------------
							TOTAL - 64 Kbytes
		- Bus de datos 8 bit
		
		// BUS DE DIRECCIONES
		// BUS DE DATOS
		// BUS DE CONTROL
*/

/*
	Define todos los tipos de datos usados en el emulador
*/

// Tipo byte (8 bits)
typedef unsigned char byte;

// Tipo word (16 bits)
typedef unsigned short word;

struct RegisterBank {
	//Registro AF (16 bits)
	union {
		word AF;
		struct {
			// Estructura se guardar el valor de memoria mayor. Solo se puede almacenar en un momento dado una de ella.
			union {
				// Registro F
				// byte x; 8 bit
				// word y; 16 bit
				// Si escribo en y estoy escribiendo en una parte de y. Si se escribe en x se escribe en 8 bit de y 
				byte F; // Registro F (estado)
				struct {
					byte CF : 1; // Tamaño
					byte NF : 1;
					byte PF : 1;
					byte XF : 1;
					byte HF : 1;
					byte YF : 1;
					byte ZF : 1;
					byte SF : 1;
				};
			};
			
			// Registro acumulador
			byte A;
			//r.A = 56
			//r.CF = 1
			//r.CF = 0;
		};
	};
	
	// Registo BC
	union BC {
		//byte x; // 8 bit estoy en una parte de y
		//word y; //reservar para el mayor 16 bit
		
		word BC;
		struct {
			byte C;
			byte B;
		}
	}

	// Registro DE
	union
	{
		word DE;
		struct
		{
			byte E;
			byte D;
		}
	}

	// Registro HL
	union 
	{
		// Registro HL de 16 bits
		word HL;
		struct
		{
			byte L;
			byte H;
		}
	}
	
	// Registro IX
	word IX
	
	// Registro IY
	word IY
	
	// Alternativos
	word _AF;
	word _BC;
	word _DE;
	word _HL;
	
	word PC;
	word SP; // Registro de pila
};

















