#include <Arduino.h>
#include "NRF52_Radio_library.h"
NRF52_Radio MicrobitRadio = NRF52_Radio();

uint8_t receivedDir = 0;

void Act();

void setup() {
  Serial.begin(9600);
  Serial.println("MicroBit V2:Mecanum-Car");
  MicrobitRadio.enable();
}

void loop() {
	FrameBuffer* myData = MicrobitRadio.recv();
	if (myData != NULL) {
		Serial.print(myData->length);
		Serial.print("    ");
		Serial.print(myData->version);
		Serial.print("    ");
		Serial.print(myData->group);
		Serial.print("    ");
		Serial.print(myData->protocol);
		Serial.print("     data: ");
		
		Serial.println(myData->payload[0]);
		receivedDir = myData->payload[0];
		
		delete myData;
		
		Act();
  }
}

void Act() {
}