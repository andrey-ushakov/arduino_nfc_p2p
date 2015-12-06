#include "SPI.h"
#include "PN532_SPI.h"
#include "snep.h"
#include "NdefMessage.h"
#include <EEPROM.h>

// sID
int address_sid_flag = 0;
int address_sid = 1;
int sID;

#define QA_CHARACTER 0x00
#define QA_RESULT    0x01
#define QA_HEAL      0x02

PN532_SPI pn532spi(SPI, 10);
SNEP nfc(pn532spi);
uint8_t ndefBuf[128];
uint8_t header[] = {0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x00};

uint8_t bufferMsg[128];
int msgLength = 0;

uint8_t setCharNdef(uint8_t *buffer, const uint8_t *character, uint8_t length);
uint8_t setRequestNdef(uint8_t *buffer, uint8_t request);
uint8_t setActionNdef(uint8_t *buffer, uint8_t action);
void print_ndef(const uint8_t *buffer, uint8_t length);
void print_ndef_payload(const uint8_t *buffer, uint8_t length);

bool isMsgInBuffer = false;

void setup() {
  Serial.begin(9600);
  
  // Get or generate sID
  bool sid_flag = EEPROM.read(address_sid_flag);
  if (!sid_flag) {
    generateSid();
  }
  sID = EEPROM.read(address_sid);
}

void loop() {
  int16_t result;
  uint8_t length;
  int16_t length16;
  
  //Serial.println("\n\n============ Start ============");
  
  if(!isMsgInBuffer) {
    readFromSerial();
    //Serial.print("I received from serial : ");
    //printSerialMessage();
    if(msgLength > 0) {
      isMsgInBuffer = true;
    }
  }
  
  result = nfc.poll();
  
  if (result == 1) { // client
      //Serial.println("Client peer");
      
      if(isMsgInBuffer && msgLength > 0) {          // send message
        // send to server
        length = setCharNdef(ndefBuf, bufferMsg, msgLength*sizeof(uint8_t));
        nfc.put(ndefBuf, length); // send character data
        isMsgInBuffer = false;
        //Serial.println("Client : Msg was sent");
      } else {
        //Serial.println("Client : Nothing to send");
      }

  }
  
  else if (result == 2) { // server
      //Serial.println("Server peer");
      
      length16 = nfc.serve(ndefBuf, sizeof(ndefBuf)); // get peer character data
      if(length16 > 0) {
        //print_ndef(ndefBuf, length16);
        print_ndef_payload(ndefBuf, length16);
      } else {
        //Serial.println("Server : Nothing to receive");
      }
  }
  else{
      //Serial.println("timeout");
    }
  
  
  //Serial.print("disconnect: ");
  int discRes = nfc.disconnect(500);
  //Serial.println(discRes);
  delay(3000);

}


void readFromSerial() {
  msgLength = 0;
  while(Serial.available() > 0) {
    bufferMsg[msgLength] = Serial.read();
    msgLength++;
  }
}


void printSerialMessage() {
  for (int i = 0; i < msgLength; ++i) {
      Serial.print(char(bufferMsg[i]));
    }
  Serial.println("");
}


uint8_t setCharNdef(uint8_t *buffer, const uint8_t *character, uint8_t length) {
  buffer[0] = 0xD4;
  buffer[1] = sizeof(header);
  buffer[2] = length;
  
  header[buffer[1] - 1] = 0x43;
  
  memcpy(buffer + 3, header, buffer[1]);
  memcpy(buffer + 3 + buffer[1], character, buffer[2]);
  
  return 3 + buffer[1] + buffer[2];
}

void print_ndef(const uint8_t *buffer, uint8_t length) {
  NdefMessage message = NdefMessage(buffer, length);
  message.print();
}

void print_ndef_payload(const uint8_t *buffer, uint8_t length) {
  NdefMessage message = NdefMessage(buffer, length);
  NdefRecord record = message.getRecord(0);
  int len = record.getPayloadLength();
  for (int szPos=0; szPos < len; szPos++) {
    if (record._payload[szPos] <= 0x1F)
      Serial.print(".");
    else
      Serial.print((char)record._payload[szPos]);
  }
  Serial.println("");
}

void generateSid() {
  randomSeed(analogRead(0));
  EEPROM.write(address_sid, random(1,256));
  EEPROM.write(address_sid_flag, true);
  Serial.println("generate done");
}
