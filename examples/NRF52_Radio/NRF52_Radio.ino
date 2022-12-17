#include "NRF52_Radio_library.h"
NRF52_Radio MicrobitRadio = NRF52_Radio();

FrameBuffer        *myDataSendData;
const long interval = 1000; //1000 ms
const int MSGSIZE =2; //max 32
uint8_t msg[MSGSIZE]; //since global, it will init to all zero's.
static long currentMillis = 0;
const int frequency = 7; //2.407GHz Radio
const uint8_t group = 1;


void setup() {
  Serial.begin(115200);
 
  myDataSendData = new FrameBuffer();
  myDataSendData->length = 5; //3 (fixed) + MSGSIZE
  myDataSendData->version = 12;
  myDataSendData->group = group;
  myDataSendData->protocol = 14;

  
  msg[0] = 0x00;
  msg[1] = 0xFF;
  
  memcpy(myDataSendData->payload, &msg, sizeof(uint8_t) * MSGSIZE); 
  
  Serial.println("Micro:Bit V2");
  Serial.print("Frequency: ");
  Serial.print(frequency);
  Serial.print(" Length: ");
  Serial.print(myDataSendData->length);
  Serial.print(" Version: ");
  Serial.print(myDataSendData->version);
  Serial.print(" Group: ");
  Serial.print(myDataSendData->group);
  Serial.print(" Protocol ");
  Serial.println(myDataSendData->protocol);
  
  MicrobitRadio.enable();

  MicrobitRadio.setFrequencyBand(frequency);
  MicrobitRadio.setGroup(group);
  
  Serial.println("");
  Serial.println("READY");
  Serial.println("");
}

void loop() {
  //RADIO RX (RECEIVING) as soon as AVAILABLE
  FrameBuffer* myData = MicrobitRadio.recv();
  if (myData != NULL) {
     Serial.print("Micro:Bit V2 Receving: ");
     Serial.print(myData->payload[0]);
     Serial.print(" ");
     Serial.println(myData->payload[1]);

     delete myData;
  }
  
  //RADIO TX (SENDING) every INTERVAL
  if (millis() - currentMillis >= interval){
     Serial.println("Micro:Bit V2 sending: 0x00,0xFF");

     MicrobitRadio.send(myDataSendData);
     currentMillis = millis();
  }
}
