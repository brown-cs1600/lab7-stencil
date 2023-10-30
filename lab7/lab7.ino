/*
 * CSCI 1600 lab 7 starter code
 * Author: Milda Zizyte
 */

#include "lab7.h"

/*
 * LAB STEP 4c/g
 * Change this to false for receiver!!
 */
const bool SENDER = true;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);

  /*
   * Uncomment for LAB STEP 4a
   * Initialize software UART pins and behaviors
   */
  /*
  pinMode(inPin, INPUT);
  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, HIGH);
  attachInterrupt(inPin, uartReceive, CHANGE);

  // initialize pins
  const int ledPins[3] = {rPin, gPin, bPin};
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }*/
  
  if (SENDER) { setupWiFi(); }
}

void loop() {
  if (SENDER) {
    // Check for response to HTTP request
    if (readWebpage()) {
      Serial.println("Something was received using HTTP!");
      /*
       * LAB STEP 4e:
       * Check if sBuf is not empty
       * Send bytes via UART
       */
      
      // Send a new request
      delay(2000); // remove me!
      sendHTTPReq();
    }

    // Connection ended / no more bytes to read
    if (!client.connected()) {
      delay(500);
      // Try to reconnect
      connectToWebpage();
    }
  } else { // not SENDER
    /*
     * LAB STEP 4f:
     * Check if rBuf is not empty
     * Print to serial monitor (for debugging) and/or light up corresponding LED
     */
  }
}
