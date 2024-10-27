#include "lab7_home.h"

void setup() {
  Serial.begin(9600);
  while(!Serial);

  matrix.begin();

  // Connect to roam and get initial key
  homeInit();
}

void loop() {
  // Gather more keys
  checkInbox();
  
  // Display monster
  while(Serial.available() > 0) {
    char input = Serial.read();
    if (input >= '0' && input <= '7') {
      displayMonster(input - '0');
    }
  }
}
