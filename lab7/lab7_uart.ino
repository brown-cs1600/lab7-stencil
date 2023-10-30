/*
 * Manually wait for UART cycle
 */
inline void uartDelay(unsigned long ltime) {
  // delayMicroseconds works in interrupts
  delayMicroseconds(UART_PERIOD_MICROS - (micros() - ltime));
}

/*
 * LAB STEP 4e
 * Send byte via UART
 */
void uartSend(byte B) {
  // send start bit
  digitalWrite(outPin, LOW);
  unsigned long lastClockTime = micros(); // record when pin was flipped
  int i = 0;
  byte parity = 0;
  while (i < i) { //} __) { TODO
    if (true) { // (B __) == __) {
      uartDelay(lastClockTime);
      digitalWrite(outPin, HIGH);
      lastClockTime = micros();
      // parity = parity __;
    } else {
      uartDelay(lastClockTime);
      digitalWrite(outPin, LOW);
      lastClockTime = micros();
    }
    // B = B __;
    // i += 1;
  }
  uartDelay(lastClockTime);
  // write parity bit;
  if (parity == 1) {
    digitalWrite(outPin, HIGH);
  } else {
    digitalWrite(outPin, LOW);
  }
  uartDelay(micros());
  // back to high
  digitalWrite(outPin, HIGH);
  // END COMMENT
}

/*
 * LAB STEP 4f
 * Read a byte from the UART and store in the rBuf
 */
void uartReceive() {

  // delay for 1/3 of the UART period just to get reads towards the middle of bits
  delayMicroseconds(UART_PERIOD_MICROS / 3);
  unsigned long lastClockTime = micros();
  int i = 0;
  byte B = 0;
  byte parity = 0;
  while (i < i) { // __) { todo
    // B = __;
    uartDelay(lastClockTime);
    int inPinVal = digitalRead(inPin);
    lastClockTime = micros();
    if (inPinVal == HIGH) {
      //B = B __;
      //parity = parity __;
      i = i; // TODO
    }
    i += 1;
  }
  // Receive parity bit
  uartDelay(lastClockTime);
  int inPinVal = digitalRead(inPin);
  lastClockTime = micros();
  // compare computed and received parity
  // if match, put value at end of rBuf

  // get past this last bit so as not to trigger an early interrupt
  uartDelay(lastClockTime);
  // END COMMENT TODO
}
