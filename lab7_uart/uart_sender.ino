#ifdef SENDER // SENDER ONLY

/*
 * Send start bit, byte B, and parity on outPin
 *
 * NOTE: no guarantees that USB Serial will work in this function,
 * since it is very timing-sensistive (each bit is ~100us)
 */
void uartSend(byte B) {
  // send start bit
  unsigned long lastClockTime = micros(); // record when pin was flipped
  digitalWrite(outPin, LOW);
  int i = 0;
  byte parity = 0;
  // Send 8 bits, LSB first
  while (i < ___) { // TODO
    if ((B ___ ) == ___ ) { // TODOD
      uartDelay(lastClockTime);
      lastClockTime = micros();
      digitalWrite(outPin, HIGH_BITFLIP);
      parity = parity ____; // TODO
    } else {
      uartDelay(lastClockTime);
      lastClockTime = micros();
      digitalWrite(outPin, LOW);
    }
    B = B ___ 1; // TODO
    i += 1;
  }
  // write parity bit;
  if (parity == 1) {
    uartDelay(lastClockTime);
    lastClockTime = micros();
    digitalWrite(outPin, HIGH);
  } else {
    uartDelay(lastClockTime);
    lastClockTime = micros();
    digitalWrite(outPin, LOW);
  }
  uartDelay(lastClockTime);
  // back to idle
  digitalWrite(outPin, HIGH);
}

const uint32_t monster[3] = {0x1fcf07bd, 0xd1051dcf, 0x07bfd001};

inline void loopSender() {
  // Add each index, integer, and end byte (0xff) to buffer
  for (int i = 0; i < 3; i++) {
    bufAdd(i);
    uint32_t toSend = monster[i];
    for (int j = 0; j < 4; j++) {
      bufAdd(toSend & 0xff);
      toSend >>= 8;
    }
    bufAdd(0xff);
  }

  // send everything in buffer while emptying
  while (!bufIsEmpty()) {
    Serial.println(bufGet(0), HEX);
    uartSend(bufRemove());
    delayMicroseconds(200);
  }
  // do it all again after 1 second
  delay(1000);
}

#endif