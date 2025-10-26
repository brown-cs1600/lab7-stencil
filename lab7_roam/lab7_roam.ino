#include "lab7_roam.h"

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Send any character to begin");
  while (!Serial.available()) { delay(100); }
  Serial.println("Roam code started!");

  // Compute monster name and key and send to home device for initialization
  roamInit();
}

void loop() {
  delay(5000);
  // Collect keys from remote home devices
  scanForMonsters();
}
