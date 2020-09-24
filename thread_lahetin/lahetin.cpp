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
		pc.printf("trasmitter1 initialization failed\r\n");
	}
	pc.printf("lahtettimen lahetin alustettu\r\n");
	
    while(!vastaanotin1.init(1000,D5,transmitter_receiver_address))
	{
		pc.printf("receiver1 initialization failed\r\n");
	}
	pc.printf("vastaanottimen vastaanotin1 alustettu\r\n");
	
	    
    while(true)
	{
        
		kasaaPaketti(offset);
					
		while(!lahetin1.send(transmitter_target_receiver_address,&message, sizeof(message)))
		{
			pc.printf("trasmitter sending failed\r\n");
		}
		
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
						
						
		    string msg("kuittauskello laukesi");
			printMsg(msg);

			
			
		}
        else           // saatiin kuittaus vastaanottajalta
	    {
            offset = offset + 50;
			if(offset > 200)
		    {
				offset = 0;
		    }
			//printData(string((char *)&buffer1,koko));  // this prints data as integers
			printMsg(string((char *)&buffer1,koko));    // and this prints it as characters
		}
	}
}


