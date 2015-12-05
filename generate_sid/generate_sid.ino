#include <EEPROM.h>

int address_sid_flag = 0;
int address_sid = 1;
int sID;

void setup() {
  Serial.begin(9600);
  bool sid_flag = EEPROM.read(address_sid_flag);
  if (!sid_flag) {
    generateSid();
  }
  sID = EEPROM.read(address_sid);
}

void generateSid() {
  randomSeed(analogRead(0));
  EEPROM.write(address_sid, random(1,256););
  EEPROM.write(address_sid_flag, true);
  Serial.println("generate done");
}

void loop() {
  Serial.println(sID);
  delay(500);
}
