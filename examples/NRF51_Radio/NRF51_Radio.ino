/*
 Name:		NRF_Radio.ino
 Created:	11/30/2018 2:40:19 AM
 Author:	michael
*/

#include "NRF51_Radio_library.h"


NRF51_Radio MicrobitRadio = NRF51_Radio();

FrameBuffer				*myDataSendData;
const int COL1 = 3;     // Column #1 control
const int LED = 26;     // 'row 1' led
const long interval = 5000;

// define prototypes for any methods that use 'user types' created in .ino code or use cpp/h files
//
void test(NRF51_Radio _lib1);


// methods below here
//

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Serial.println("Starting the Radio Library");
	pinMode(COL1, OUTPUT);
	digitalWrite(COL1, LOW);
	pinMode(LED, OUTPUT);


	myDataSendData = new FrameBuffer();
	MicrobitRadio.hello("Test");
	MicrobitRadio.enable();
}

// the loop function runs over and over again until power down or reset
void loop() {
	digitalWrite(LED, HIGH);
	
	static long currentMillis;


	FrameBuffer* myData = MicrobitRadio.recv();
	if (myData != NULL) {
		Serial.print(myData->length);
		Serial.print("    ");
		Serial.print(myData->version);
		Serial.print("    ");
		Serial.print(myData->group);
		Serial.print("    ");
		Serial.print(myData->protocol);
		Serial.print("    ");
		Serial.print(myData->payload[10]);
		Serial.print("    ");
		Serial.println(MicrobitRadio.dataReady());
		delete myData;

		myDataSendData->length = 3;
		myDataSendData->group = 10;
		myDataSendData->version = 12;
		myDataSendData->protocol = 14;


		if (millis() - currentMillis >= interval)
		{

			Serial.println(currentMillis);
			MicrobitRadio.send(myDataSendData);
			currentMillis = millis();
		}
	}

	delay(10);
	digitalWrite(LED, LOW);
	delay(10);
}

//This method demonstrates 1) using a library 2) using user types as function params
void test(NRF51_Radio _lib1)
{
	MicrobitRadio.hello("Hello Visual Micro");
}