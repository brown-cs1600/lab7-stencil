/*
 * CSCI 1600 lab 7 starter code
 * Author: Milda Zizyte
 */

#include "lab7.h"

// IMPORTANT: make sure this is present for one device and absent (commented out) for the other!!
#define SENDER

// Only the receiver displays the monster
#ifndef SENDER
#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;
#endif

void setup() {
  Serial.begin(9600);

  #ifdef SENDER
  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, HIGH); // UART idle-high signal

  # else // RECEIVER
  matrix.begin();
  pinMode(inPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(inPin), uartReceive, FALLING);
  # endif
}

void loop() {
  # ifdef SENDER
  loopSender();
  # else
  loopReceiver();
  # endif
}