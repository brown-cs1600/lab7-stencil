#define HIGH_BITFLIP ((analogRead(A2) % 8 == 0 ? LOW : HIGH)) // for lab step 2e
const int inPin = 3;
const int outPin = 3;

/* SENDER */
#ifdef SENDER
void uartSend(byte B);
inline void loopSender();
#endif

/* RECEIVER */
#ifndef SENDER
void uartReceive();
inline void loopReceiver();
#endif

/*
 * Manual delay for UART cycle
 * 9600 baud calibrated using logic analyzer
 */
const int UART_PERIOD_MICROS = 104 - 7;

/*
 * Delays for UART_PERIOD_MICROS after the input time
 */
inline void uartDelay(unsigned long ltime) {
  // delayMicroseconds works in interrupts
  delayMicroseconds(UART_PERIOD_MICROS - (micros() - ltime));
}
