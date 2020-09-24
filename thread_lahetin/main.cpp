#include "mbed.h"
#include <string>

EventQueue queue;
void handler(string printattavaViesti,int msgType);
Event<void(string,int)> event(&queue, handler);	

void EventThreadFunction(void);
void printMsg(string msg);
void printData(string data);
extern void tokaThreadFunction();
extern void ekaThreadFunction();


DigitalOut myled(LED1);
Serial pc(USBTX, USBRX);

Thread ekaLahetin(osPriorityNormal);
Thread tokaLahetin(osPriorityNormal);

char testiviesti[] = "pelaa";
int8_t buffer[3] = {-1,3,2};
int8_t * ptr = &buffer[0]; 

int main()
{
	pc.printf("________________RESET____________________\n\r");
	Thread EventThread(osPriorityAboveNormal);
	EventThread.start(EventThreadFunction);
    ekaLahetin.start(ekaThreadFunction);
    tokaLahetin.start(tokaThreadFunction);
    while(1)
    {

    }

}

void EventThreadFunction(void)
{
    queue.dispatch_forever();
}

void handler(string viesti,int type) {

	if(type == 1)
    {
		for(std::string::size_type i = 0; i<viesti.size();i++)
		{
		   pc.printf("%i ",(int8_t) viesti[i]);
		}
		pc.printf("\r\n");  // print new line
	}
	else
    {
		pc.printf("%s\r\n",viesti.c_str());
	}	

}
void printMsg(string msg)
{
	event.post(msg,0);  // msgType = 0 eli oikea C++ character string
}
void printData(string msg)
{
	event.post(msg,1);  // msgType = 1 eli int8_t dataa
}

