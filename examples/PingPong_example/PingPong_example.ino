// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       PingPong_example.ino
    Created:	10/12/2018 08.35.50
    Author:     RAHR\michael
*/

// Define User Types below here or use a .h file
//
#include "NRF51_Radio_library.h"
#include <Adafruit_Microbit.h>

Adafruit_Microbit_Matrix microbit;			//We only use this library to get easy access to the MATRIX LED, to sad you need to set the soft device to S110, even though we do not use it
											//Should be easy to fix, by removing the ble class of the in the library
NRF51_Radio MicrobitRadio = NRF51_Radio();	//Let's get a new instance of the Radio
enum STATENAME {
	START,
	SEND,
	RECV,
	RECVACT,
	SENDACK,
	WAITTOSEND
};


// Define Function Prototypes that use User Types below here or use a .h file
//
FrameBuffer				*myDataSendData;	//FrameBuffer for sending data to another device
FrameBuffer* myData;
int						 current_state;		//Var to hold the current state
static long currentMillis;					//Var to store the time gone since last time
const long interval = 5000;					//Wait time before sending
const long start_time = 10000;				//Wait time doing startup, we use this time to see if any other device is already running
const long send_interval = 200;				//In state send, after start we send out at 1 sec interval, ontil we start receiving something
const long wait_to_send = 100;
bool got_data = false;

// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts
void setup()
{
	Serial.begin(115200);
	Serial.println("Starting the PingPong example using Group 10, frekvens band 50");
	microbit.begin();
	microbit.show(microbit.HEART);
	MicrobitRadio.enable();
	MicrobitRadio.setGroup(10);
	MicrobitRadio.setFrequencyBand(50);
	Serial.println("Radio running");

		current_state = STATENAME::START;
		
	myDataSendData = new FrameBuffer();
	currentMillis = millis();
	

}

// Add the main program code into the continuous loop() function
void loop()
{
	
	if ((MicrobitRadio.dataReady() > 0)	&&(got_data!=true))					//Check if there are any data ready for us
	{
		myData = MicrobitRadio.recv();			//If so then let's get it
		got_data = true;
	}
	
	
	switch (current_state)
	{
	case STATENAME::START: {
	
		if (got_data == true) 
		{ 
			current_state = STATENAME::RECV;					//If we got data then we are not the first one, we change state to recv.
			got_data == false;									//reset the data flag;
		}
			
		if (millis() - currentMillis >= start_time) {
			current_state = STATENAME::SEND;					//We did not find any signal, so now we assume that we are the first one running
			currentMillis = millis();													//so we start to send, and then wait for a ack.
			Serial.println("State go to SEND");
		}
		
	
	}; break;

	case STATENAME::RECV: {
	
		
		if (got_data == true)
		{
			
			if (myData->group == 2) {
				Serial.println("Got Data");
				current_state = STATENAME::SENDACK;					//If we got data then we are not the first one, we change state to recv.
			}

			delete myData;
			got_data = false;
		}
	
	}; break;
	
	case STATENAME::SENDACK: {
		Serial.println("Send ACK");
		microbit.print("A");
		MicrobitRadio.send(myDataSendData);
		myDataSendData->length = 3;
		myDataSendData->group = 1;						//(1=ACK 2=SEND)
		myDataSendData->version = 10;
		myDataSendData->protocol = 10;
		MicrobitRadio.send(myDataSendData);
		current_state = STATENAME::WAITTOSEND;
		currentMillis = millis();

	} break;

	case STATENAME::RECVACT: {
		current_state = STATENAME::RECV;
	}; break;

	case STATENAME::WAITTOSEND: {
		if (millis() - currentMillis >= wait_to_send) {
			Serial.println("State go to SEND");
			current_state = STATENAME::SEND;
		}
	
	}; break;

	case STATENAME::SEND: {
		
		if (got_data == true)
		{
			if (myData->group == 1) {
				current_state = STATENAME::RECV;					//If we got data then we are not the first one, we change state to recv.
				Serial.println("Got ACK");
			}
			delete myData;
			got_data = false;									//reset the data flag;
		}
		if (millis() - currentMillis >= send_interval) {
			Serial.println("State = SEND");
			current_state = STATENAME::SEND;					//We did not find any signal, so now we assume that we are the first one running
			currentMillis = millis();							//so we start to send, and then wait for a ack.
			microbit.print("S");
			myDataSendData->length = 3;
			myDataSendData->group = 2;						//(1=ACK 2=SEND)
			myDataSendData->version = 10;
			myDataSendData->protocol = 10;
			MicrobitRadio.send(myDataSendData);
		}
	}; break;

	
	default:
		break;
	}
}
