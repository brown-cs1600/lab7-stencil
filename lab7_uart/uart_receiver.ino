# ifndef SENDER // RECEIVER ONLY

/*
 * When inPin goes from HIGH (idle) to LOW, read in 8 bits + parity
 * Store data in buffer if computed parity matches received
 *
 * NOTE: USB Serial will not work in this function because it is an ISR of higher priority
 */
void uartReceive() {
  // delay for 1/3 of the UART period just to get reads towards the middle of bits
  delayMicroseconds(UART_PERIOD_MICROS / 3);
  unsigned long lastClockTime = micros();

  int i = 0;
  byte B = 0;
  byte parity = 0;
  // Read in 8 bits, LSB first
  while (i < ____) { // TODO
    B = B ___ 1; // TODO
    uartDelay(lastClockTime);
    lastClockTime = micros();
    int inPinVal = digitalRead(inPin);
    if (inPinVal == HIGH) {
      B = B ___ (0x1 ___); // TODO
      parity = parity ___; // TODO
    }
    i += 1;
  }
  // Receive parity bit
  uartDelay(lastClockTime);
  lastClockTime = micros();
  int inPinVal = digitalRead(inPin);

  // compare computed and received parity
  // if match, put value at end of rBuf
  byte receivedParity = (inPinVal == HIGH) ? 1 : 0;
  if (receivedParity == parity) {
    bufAdd(B); 
  }

  // get past this last bit so as not to trigger an early interrupt
  uartDelay(lastClockTime);
}

inline void loopReceiver() {
  static uint32_t frame[3] = {0, 0, 0};

  // Atomic access for buffer length
  noInterrupts();
  int bl = bufLength();
  interrupts();

  if (bl >= 6) {
    byte indx = bufRemove();
    Serial.println(indx, HEX);
    // check that this is the start of a frame
    if (indx >= 3) return;
    // check to see that the next four bytes look like valid data
    // keep them in the buffer for now in case we encounter something invalid
    uint32_t dispVal = 0;
    for (int i = 0; i < 4; i++) {
      byte b = bufGet(i);
      Serial.println(b, HEX);
      if (b == 0xff) return; // end byte encountered too early
      dispVal |= ((uint32_t) b) << (i * 8);
    }

    // check for end message
    Serial.println(bufGet(4), HEX);
    if (bufGet(4) != 0xff) return; // did not see expected end byte

    // successfully processed the last 5 bytes; remove them
    for (int i = 0; i < 5; i++) {
      bufRemove();
    }

    // update and display frame
    if (dispVal != frame[indx] && dispVal != 0) {
      frame[indx] = dispVal;
      matrix.loadFrame(frame);
    }
  }
}

#endif // ifndef SENDER