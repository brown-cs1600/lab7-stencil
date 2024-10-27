#include "lab7_roam.h"

void setup() {
  Serial.begin(9600);
  while(!Serial);

  // Compute monster name and key and send to home device for initialization
  roamInit();
}

void loop() {
  delay(1000);
  // Collect keys from remote home devices
  scanForMonsters();
}
