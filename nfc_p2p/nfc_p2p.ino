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
  
  Serial.println("\n\n============ Start ============");
  
  
  
  
  
  result = nfc.poll();
  //Serial.print("Res : ");
  //Serial.println(result);
  
  if (result == 1) { // client
    while(true) {
      Serial.println("Client peer");
      readFromSerial();
      Serial.print("I received from serial : ");
      printSerialMessage();
      
      if(msgLength > 0) {          // try to send
        // send to server
        length = setCharNdef(ndefBuf, bufferMsg, sizeof(bufferMsg));
        nfc.put(ndefBuf, length); // send character data
        Serial.println("Msg was sended");
      } else {                      // try to receive
        Serial.println("Nothing to send");
        // TODO
        length = setRequestNdef(ndefBuf, QA_CHARACTER);
        length16 = nfc.get(ndefBuf, length, sizeof(ndefBuf)); // get peer character data
        if(length16 > 0) {
          print_ndef(ndefBuf, length16);
        } else {
          Serial.println("Nothing to receive");
        }
      }
      
      delay(3000);
    }
  }
  
  else if (result == 2) { // server
    while(true) {
      Serial.println("Server peer");
      readFromSerial();
      Serial.print("I received from serial : ");
      printSerialMessage();
      
      if(msgLength > 0) {          // try to send
        // TODO
        Serial.println("try to send");
        length = setCharNdef(ndefBuf, bufferMsg, sizeof(bufferMsg));
        nfc.serve(ndefBuf, length); // send character data
      
      } else {                      // try to receive
        Serial.println("Nothing to send");
        length16 = nfc.serve(ndefBuf, sizeof(ndefBuf)); // get peer character data
        if(length16 > 0) {
          print_ndef(ndefBuf, length16);
        } else {
          Serial.println("Nothing to receive");
        }
      }
      
      delay(3000);
    }
    
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


uint8_t setRequestNdef(uint8_t *buffer, uint8_t request) {
  buffer[0] = 0xD4;
  buffer[1] = sizeof(header);
  buffer[2] = 0x01;
  
  header[buffer[1] - 1] = 0x51;
  
  memcpy(buffer + 3, header, buffer[1]);
  buffer[3 + buffer[1]] = request;
  
  return 3 + buffer[1] + buffer[2];
}
