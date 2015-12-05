#include "SPI.h"
#include "PN532_SPI.h"
#include "snep.h"
#include "NdefMessage.h"

#define QA_CHARACTER 0x00
#define QA_RESULT    0x01
#define QA_HEAL      0x02

PN532_SPI pn532spi(SPI, 10);
SNEP nfc(pn532spi);
uint8_t ndefBuf[128];
uint8_t header[] = {0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x00};

uint8_t bufferMsg[16];
int msgLength = 0;

uint8_t setCharNdef(uint8_t *buffer, const uint8_t *character, uint8_t length);
uint8_t setRequestNdef(uint8_t *buffer, uint8_t request);
uint8_t setActionNdef(uint8_t *buffer, uint8_t action);
void print_ndef(const uint8_t *buffer, uint8_t length);



void setup() {
  Serial.begin(115200);
}

void loop() {
  int16_t result;
  uint8_t length;
  int16_t length16;
  
  Serial.println("Start...");
  
  result = nfc.poll();
  Serial.print("Res : ");
  Serial.println(result);
  
  
  if (result == 1) { // client
    Serial.println("Client peer");
    readFromSerial();
    
    if(msgLength > 0) {
      // say what you got:
      Serial.print("I received from serial : ");
      printSerialMessage();
      
      // send to server
      length = setCharNdef(ndefBuf, bufferMsg, sizeof(bufferMsg));
      nfc.put(ndefBuf, length); // send character data
    } else {
      Serial.println("Nothing to send");
    }
  }
  
  else if (result == 2) { // server
    Serial.println("Server peer");
    length16 = nfc.serve(ndefBuf, sizeof(ndefBuf)); // get peer character data
    print_ndef(ndefBuf, length16);
  }
  
  
  
  ///////
  /*readFromSerial();
    
  if(msgLength > 0) {
    // say what you got:
    Serial.print("I received from serial : ");
    printSerialMessage();

  } else {
    Serial.println("Nothing");
  }*/
  
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