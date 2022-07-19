#include <Arduino.h>
#include "NRF51_Radio_library.h"

NRF51_Radio MicrobitRadio = NRF51_Radio();

FrameBuffer *myDataSendData;
uint8_t msg[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; ; //max 32 
uint8_t selectedDir = 0;

void SendMessage();

void setup() {
	pinMode(5,INPUT_PULLUP); //btnA
    pinMode(11,INPUT_PULLUP); //btnB

	Serial.begin(9600);
	Serial.println("MicroBit V1:Controller");
	MicrobitRadio.enable();
}

void loop() {
			if (digitalRead(5)==LOW) {
                
                selectedDir++;
				
				Serial.print("Btn A: ");
                Serial.println(selectedDir);

    			msg[0] = selectedDir;
				SendMessage();
                delay(200); //debounce delay;
            } 
        
         if (digitalRead(11)==LOW) {
                selectedDir=0;
                Serial.println("Btn B: Stop");

				msg[0] = selectedDir;
				SendMessage();

                delay(200); //debounce delay 
            } 
}

void SendMessage()
{
	myDataSendData = new FrameBuffer();
	myDataSendData->length = 13;
	myDataSendData->group = 10;
	myDataSendData->version = 12;
	myDataSendData->protocol = 14;
	memcpy(myDataSendData->payload, &msg, sizeof(uint8_t) *10);

	Serial.print("Sending: ");
	Serial.println(myDataSendData->payload[0]);
	
	MicrobitRadio.send(myDataSendData);
}