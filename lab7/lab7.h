#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

/* HTTP helper function definitions */
void setupWiFi();
bool connectToWebpage();
bool readWebpage();
void sendHTTPReq();

/* UART helper function definitions */
void uartSend(byte B);
void uartReceive();

// WiFi client
WiFiSSLClient client;

/* send and receive buffers for UART communication */
const int rsBufSize = 80;
byte sBuf[rsBufSize];
int sBufStart = 0;
int sBufEnd = 0;
byte rBuf[rsBufSize];
int rBufStart = 0;
int rBufEnd = 0;

const int UART_PERIOD_MICROS = 104;

const int inPin = 5;
const int outPin = 3;

const int rPin = 0;
const int gPin = 1;
const int bPin = 2;