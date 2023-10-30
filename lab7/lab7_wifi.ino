/* UDP setup for NTP (world clock) */
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

char httpGETbuf[200]; // to form HTTP GET request
unsigned long reqTime; // time of NTP request
unsigned long secsSince1900; // NTP response

void sendHTTPReq() {
  // LAB STEP 4e: change the second argument to be (the current time since Jan 1, 1990 in seconds) / 3
  sprintf(httpGETbuf, "GET /integers/?num=1&min=1&max=3&col=1&base=10&format=plain&rnd=id.%lu HTTP/1.1", millis());
  client.println(httpGETbuf);
  client.println("Host: www.random.org");
  client.println();
}

/*
 * Print response to HTTP request
 * return true if response was received
 */
bool readWebpage() {
    // Check for bytes to read
  int len = client.available();
  if (len == 0){
    return false;
  }

  while (client.available()) {
    Serial.print((char) client.read());
  }
  Serial.println();
  return true;

  /*
   * LAB STEP 3b: change above code to only print RED, GREEN, or BLUE as it is read
   * LAB STEP 3c: change above code to send 'r', 'g', or 'b' via Serial1
   * LAB STEP 4e: change above code to put 'r', 'b', or 'g' in sBuf for sending on UART
   */
}

/*
 * Send SSL connection request to server
 * Return true if successful
 */
bool connectToWebpage() {
  if (client.connectSSL("www.random.org", 443)) {
    sendHTTPReq();
    return true;
  } else {
    Serial.println("Failed to fetch webpage");
    return false;
  }
}

/*
 * Connect to WiFi network, get NTP time, and connect to random.org
 */
void setupWiFi() {
  char ssid[] = "Brown-Guest";  // network SSID (name)
  char pass[] = ""; // for networks that require a password
  int status = WL_IDLE_STATUS;  // the WiFi radio's status

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid); // WiFi.begin(ssid, pass) for password
    delay(5000);
  }
  Serial.println("Connected!");

  connectToNTP();

  if (connectToWebpage()) {
    Serial.println("fetched webpage");
  } else {
    Serial.println("ERROR: failed to fetch webpage");
    Serial.println("Are SSL certificates installed?");
    while(true); // 
  }
  Serial.println();
}

/*
 * Use UDP to connect to NTP server
 * store seconds since Jan 1 1900 in secsSince1900
 * micros time of this request is in reqTime
 */
void connectToNTP() {
  Serial.println("\nStarting connection to NTP server...");
  Udp.begin(localPort);

  Serial.println("Requesting NTP time");
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  reqTime = millis();
  // wait to see if a reply is available; resend request if needed
  while(! Udp.parsePacket() ) {
    delay(2000);
    sendNTPpacket(timeServer);
    reqTime = millis();
  }
  if ( Udp.parsePacket() ) {
    Serial.println("packet received");
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    secsSince1900 = highWord << 16 | lowWord;
    // compute the time since we made the request
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900 + (millis() - reqTime) / 1000);
  }
}

/*
 * send an NTP request to the time server at the given address
 */
unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;


  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
