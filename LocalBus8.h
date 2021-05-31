class LocalBus8
{
	private:
		byte valor; //1 byte. Un bus de 8 líneas
	
	public:
		LocalBus8(); 
		~LocalBus8();
		byte Read(); 
		void Write(byte data); 
};