#include "mbed.h"
#include <string>
#include "pakattu_kuva.h"
#include "main.h"


    #include "ask_transmitter.h"
	ask_transmitter_t lahetin1;
	char viesti1[]="eka lahettimen viesti";
	uint8_t transmitter_address = 0x01;
	uint8_t transmitter_target_receiver_address = 0x02;

	#include "ask_receiver.h"
    ask_receiver_t vastaanotin1;
	#define BUFFER_SIZE 60
	char buffer1[BUFFER_SIZE];
	uint8_t transmitter_receiver_address = 0x01;
	uint8_t transmitter_received_receiver_address = 0;
	uint8_t transmitter_received_transmitter_address = 0;
	

#define PACKET_DATA_SIZE 50
#define HEADER_SIZE 2
#define PACKET_SIZE PACKET_DATA_SIZE+HEADER_SIZE


Timer kuittauskello1;
int8_t message[PACKET_SIZE];
uint16_t offset = 0;


void kasaaPaketti(int ptr)
{
	message[0] = 0xff;
	message[1] = 0x5a;
	for(int i = 0;i<PACKET_DATA_SIZE;i++)
    {
		message[HEADER_SIZE+i] = table[ptr++];
	}
}


/*********************************************************************
* Eka thread eli lähetin joka lähettää paketteja
*********************************************************************/
void ekaThreadFunction()
{
	while(!lahetin1.init(1000,D7,transmitter_address))
	{
		pc.printf("1: trasmitter1 initialization failed\r\n");
	}
	pc.printf("1: lahettimen lahetin1 alustettu\r\n");
	
    while(!vastaanotin1.init(1000,D5,transmitter_receiver_address))
	{
		pc.printf("1: receiver1 initialization failed\r\n");
	}
	pc.printf("1: lahettimen vastaanotin1 alustettu\r\n");

	// Tulostetaan viestin koko sarjaportille
	int viestin_koko = sizeof(table)*8;
	pc.printf("1: viestin koko=%i bittia\n\r", viestin_koko);

	int pakettien_maara = 0;
	    
    while(true)
	{
        
		kasaaPaketti(offset);
		int paketin_koko = sizeof(message)*8;
					
		while(!lahetin1.send(transmitter_target_receiver_address,&message, sizeof(message)))
		{
			pc.printf("1: trasmitter sending failed\r\n");
		}

		pc.printf("1: Lahetetty paketti %i bittia\n\r", paketin_koko);
		
		kuittauskello1.reset();
		kuittauskello1.start();
		int threshold1 = 5;
		int koko = 0;
		while(koko == 0&&kuittauskello1.read()<threshold1)
		{
			koko = vastaanotin1.recv(&buffer1,BUFFER_SIZE);
		}
		
		if(koko == 0)  // tarkoittaa, että kuittauskello laukesi
		{
			//char msg[] = "kuittauskello laukesi";
			//printMsg(msg);
						
						
		    string msg("1: kuittauskello laukesi");
			printMsg(msg);

			
			
		}
        else           // saatiin kuittaus vastaanottajalta
	    {
			string msg("1: kuittaus vastaanotettu");
			pakettien_maara++;
			printMsg(msg);
            offset = offset + PACKET_DATA_SIZE;
			if(offset > (sizeof(table) - 50))
		    {	
				pc.printf("1: __________Offset reset, lahetettyja paketteja %i ________\n\r", pakettien_maara);
				offset = pakettien_maara = 0;
		    }
			pc.printf("1: vastaanotettu: ");
			//printData(string((char *)&buffer1,koko));  // this prints data as integers
			printMsg(string((char *)&buffer1,koko));    // and this prints it as characters
		}
	}
}


