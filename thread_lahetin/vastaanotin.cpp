#include "mbed.h"
#include <iostream>
#include <string>
#include "main.h"
	

    #include "ask_transmitter.h"
    ask_transmitter_t lahetin2;
	char viesti2[]="2: ACKviesti";
	uint8_t receiver_transmitter_address = 0x02;
	uint8_t receiver_target_receiver_address = 0x01;
	
	#include "ask_receiver.h"
	ask_receiver_t vastaanotin2;
	#define BUFFER_SIZE 60
	uint8_t buffer2[BUFFER_SIZE];
	uint8_t receiver_receiver_address = 0x02;
	uint8_t receiver_received_receiver_address = 0;
	uint8_t receiver_received_transmitter_address = 0;
	


#define PACKET_DATA_SIZE 50
#define HEADER_SIZE 2
#define PACKET_SIZE PACKET_DATA_SIZE+HEADER_SIZE



/*********************************************************************
* Toka thread eli vastaanotin joka kokooaa vastaanotetut paketit
*********************************************************************/

void tokaThreadFunction()
{
	while(!lahetin2.init(1000,D6,receiver_transmitter_address))
	{
		pc.printf("2: trasmitter2 initialization failed\r\n");
	}
	pc.printf("2: lahtettimen lahetin alustettu\r\n");
	
	while(!vastaanotin2.init(1000,D4,receiver_receiver_address))
	{
		pc.printf("2: receiver2 initialization failed\r\n");
	}
	pc.printf("2: vastaanottimen vastaanotin2 alustettu\r\n");
	
	    
    while(true)
	{
       
		int koko = 0;
		while(koko == 0)  // eli odotetaan kunnes viesti saadaan 
		{
			koko = vastaanotin2.recv(&buffer2,BUFFER_SIZE);
		}

        /*
		* Ja tähän funktio, joka liimaa paketit peräkkäin
		* ja kun kaikki paketit on saatu tulostaa paketit
		* sarjaväylälle. Ja tuo tulostus sitten nauhoitetaan
		* puttyllä.
		*/
		printData(string((char *)&buffer2,koko)); // make a string by giving pointer to data and size of data	
		                                          // and then deliver that string to printData function
							
		while(!lahetin2.send(receiver_target_receiver_address,&viesti2, sizeof(viesti2)))
		{
			pc.printf("2: trasmitter sending failed\r\n");
		}
		

		
	}
}


