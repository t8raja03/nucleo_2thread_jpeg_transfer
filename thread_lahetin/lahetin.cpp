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
int8_t message[PACKET_SIZE];		// message[] = paketti
uint16_t offset = 0;

int viestin_koko = sizeof(table);	// table-taulukon (pakattu_kuva.h) koko
									// byteinä, koska taulukon alkiot on
									// int8_t eli 8 bittisiä, kertoo myös alkioiden
									// määrän


void kasaaPaketti(int ptr)
{	
	/********************************************************
	 * Headerin (2 byteä) rakenne:
	 * 
	 * 			message[0]					  message[1]
	 * 0   0   0   0   0   0   0   0   |    0 0 0 0 0 0 0 0 
	 * 	   |   |-------------------|	    |-------------|
	 * 	   |		    |				         * datan koko (int8_t, <128)
	 * 	   |			 \
	 * 	   |			  * paketin järjestysnumero (6 byteä, <64)
	 *     | 			    (tiedetään, että paketteja on tässä tapauksessa vähemmän)
	 * 	    \
	 * 		 * viimeisen paketin lippu, 1 viimeisessä paketissa
	*/

	// Otetaan paketin alkukohta erilliseen muuttujaan muistiin
	uint8_t paketti_startptr = ptr;

	// Headerin ensimmäiseen tavuun paketin järjestysnumero
	message[0] = 0x00 + ( ptr / PACKET_DATA_SIZE );

	// Kasataan paketin dataosio
	for(int i = 0;i<PACKET_DATA_SIZE;i++)
	{
		if ( ptr > (int)sizeof(table)) {
			message[HEADER_SIZE+i] = 0x00;
			// Viimeinenkin lähetetty paketti on 50 byteä,
			// tässä laitetaan ylimääräiset nollaksi.
			// ( ptr > table-taulukon alkioiden määrä)
			}
		else {
			message[HEADER_SIZE+i] = table[ptr++];
		}
	}

	// Paketin toinen byte on datapaketin koko
	message[1] = ptr - paketti_startptr;

	// Jos ptr > table-taulun (pakattu_kuva.h) alkioiden määrä
	// laitetaan lippu merkiksi viimeisestä paketista
	if ( ptr > (int)sizeof(table) ) {
		message[0] += (1 << 6);
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
	pc.printf("1: viestin koko=%i B\n\r", viestin_koko);

	// Lähetettyjen 
	int pakettien_maara = 0;
	    
    while(true)
	{
        
		kasaaPaketti(offset);
		int paketin_koko = sizeof(message); // Lähetettävän paketin koko (byteä)
					
		while(!lahetin1.send(transmitter_target_receiver_address,&message, sizeof(message)))
		{
			pc.printf("1: trasmitter sending failed\r\n");
		}

		pc.printf("1: Lahetetty paketti %i B\n\r", paketin_koko); // helpottamaan seuraamista
		
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
			if(offset > (sizeof(table)))
		    {	
				// kun offset > table-taulun alkioiden määrä, eli
				// viimeinen paketti on kasattu,
				// tulostetaan sarjaportille tieto helpottamaan seuraamista

				pc.printf("1: __________Offset reset, lahetettyja paketteja %i ________\n\r", pakettien_maara);
				offset = pakettien_maara = 0;
				wait_us(10000*1000);
		    }

			// Tämä tulostaa vastaanottimen takaisin lähettämän kuittausviestin
			// sarjaportille
			pc.printf("1: vastaanotettu: ");
			//printData(string((char *)&buffer1,koko));  // this prints data as integers
			printMsg(string((char *)&buffer1,koko));    // and this prints it as characters
		}
	}
}


