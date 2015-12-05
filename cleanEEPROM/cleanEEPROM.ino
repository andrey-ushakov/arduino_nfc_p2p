#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:
  
  Serial.println(random(1000,10000));
}
