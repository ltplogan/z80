/*
	Implementación de un bus de direcciones de 16 bits
*/

class LocalBus16
{
	private:
		// Almacena el valor contenido en el bus
		// 16 líneas que van de 0 a 65535 tope de la memoria
		word valor; // Valor del bus 
		// Este atributo representa el valor dentro del bus de direcciones
		// El procesador va a escribir ahí
	
	public:
		LocalBus16(); // Pone el valor a 0
		~LocalBus16();
		word Read(); // Lee del bus en esa posición
		void Write(word data); // Escribe un valor de 16 bits
};

