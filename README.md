# NRF51_Radio_library

This library is based on the Driver from Landcaster University microbit-dal Radio implementation.
To use this library you should have the Microbit Board from sanddeepmistry install, you can get that from here
https://sandeepmistry.github.io/arduino-nRF5/package_nRF5_boards_index.json


This library will for now work in 1Mbit simple mode, it is fully function with the runtime also used in the Python and the Java runtime for the microbit.

You have the following function to play with.

* int setTransmitPower(int power); power 0-7
* int setFrequencyBand(int band); band 0-100
* int getRSSI();
* int enable();
* int disable();
* int setGroup(uint8_t group); group 0-255
* int dataReady();
* FrameBuffer* recv();
* int send(FrameBuffer *buffer); frameBuffer struct

```javascript
struct FrameBuffer
{
    uint8_t         length;                             // The length of the remaining bytes in the                  packet. includes protocol/version/group fields, excluding the length field itself.
    uint8_t         version;                            // Protocol version code.
    uint8_t         group;                              // ID of the group to which this packet belongs.
    uint8_t         protocol;                           // Inner protocol number c.f. those issued by IANA for IP protocols

    uint8_t         payload[MICROBIT_RADIO_MAX_PACKET_SIZE];    // User / higher layer protocol data
    FrameBuffer     *next;                              // Linkage, to allow this and other protocols to queue packets pending processing.
    int             rssi;                               // Received signal strength of this frame.
};

```



Example of the use can be found in the example library.


But in general you need to do the following.
```
#include "NRF51_Radio_library.h"

NRF51_Radio MicrobitRadio = NRF51_Radio();
```

In the setup
You can just enable the Radio, the it is ready
MicrobitRadio.enable();


 And then in the loop.
```
static long currentMillis;

//Check if there is any data in the buffer
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
		delete myData;  //Remember to delete it



//Fill in some data in the datastruct
		myDataSendData->length = 3;
		myDataSendData->group = 10;
		myDataSendData->version = 12;
		myDataSendData->protocol = 14;

//send the data each 5 sec
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
```
