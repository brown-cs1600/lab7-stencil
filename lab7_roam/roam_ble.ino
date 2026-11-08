/* Stores the address of our local home device to connect to later */
static String savedAddress = "";

/*
 * Compute monster key and connect to home device to initialize it
 */
void roamInit() {
  if (!BLE.begin()) {
    Serial.println("BLE failed to start. Attempting a reset...");
    Serial.println("Please be patient while you wait for the device to reconnect");
    Serial.println("Don't unplug or reset your device! Just close and open the serial monitor again if nothing appears,");
    Serial.println("and send any character to begin the sketch");
    delay(1000);
    static const char RESET[] = "AT+RESET\n";
    Serial2.write(RESET, sizeof(RESET) - 1);
    delay(2000);
    NVIC_SystemReset();
  }
  BLE.setTimeout(1000); // ATT operation timeout window (to avoid certain functions hanging)
  randomSeed(hashBLEAddr());

  // Scan for corresponding home device
  BLEDevice home = connectByScan(connectScanName, specialName);

  Serial.println("Connecting to our home device!");

  Serial.print("Found ");
  Serial.print(home.address());
  savedAddress = home.address();
  Serial.print(" '");
  Serial.print(home.localName());
  Serial.println("'");

  Serial.print("Sending over the computed monster ");
  Serial.println(getMonsterName());
  while(!sendMonster(home, getMonsterKey(), getMonsterName())) {
    delay(1000);
  sendMonster(home, getMonsterKey(), getMonsterName());
  }
  home.disconnect();
  Serial.println("Success! Monster added.");
  addMonster(getMonsterName());
}

/*
 * Wrapper for the connectScan functions, to reduce network traffic by only spending a proportion of time scanning
 * Blocking (only returns when device has successfully connected), hence the need to not hog the network
 * Takes in a scan function (connectScanName, connectScanAddress, connectScanUuid)
 * and the corresponding identifier (name, address, uuid) to scan for
 */
BLEDevice connectByScan(BLEDevice (*scanFun) (const String, unsigned long), const String iden_val) {
  const int PERCENT = 10; // percent of time to be scanning vs. idle
  while(true) {
    BLEDevice d = scanFun(iden_val, PERCENT * 100);
    if (d) return d;
    delay((100 - PERCENT) * random(90, 110));
  }
}
/*
 * Attempts to connect to and discover attributes of given peripheral
 * Performs exponential backoff if doesn't succeed, to reduce network congestion
 * Retries up to the given maximum
 */
bool attemptConnectAndDiscover(BLEDevice p, int max_retries) {
  bool success = p.connect() && p.discoverAttributes();
  int retries = 0;
  while(!success && retries < max_retries) {
    int delayCap = 300 * (1 << min(retries, 4));
    p.disconnect(); // documentation recommends disconnect before attribute rediscovery
    delay(random(delayCap / 2, delayCap));
    success = p.connect() && p.discoverAttributes();
    retries++;
  }
  return success;
}

/*
 * wrapper for BLE scanForName, connect, and discover functions
 * returns empty BLEDevice if timeout is exceeded
 */
BLEDevice connectScanName(const String name, unsigned long timeout) {
  const char* nameCStr = name.c_str();
  BLE.scanForName(nameCStr);
  unsigned long startTime = millis();
  while(millis() - startTime < timeout) {
    delay(10);
    BLEDevice peripheral = BLE.available();

    if (!peripheral) continue;

    BLE.stopScan();

    // IMPORTANT: need to connect and discoverAttributes if we'll be reading/writing characterstics
    // We keep retrying to connect if connection fails
    if (attemptConnectAndDiscover(peripheral, 20)) {
      return peripheral;
    } else {
      BLE.scanForName(nameCStr);
    }
  }
  BLE.stopScan();
  return BLEDevice(); // empty device
}

/*
 * wrapper for BLE scanForUuid, connect, and discover functions
 * IGNORES MONSTER THAT HAS ALREADY BEEN FOUND
 * returns empty BLEDevice if timeout is exceeded
 */
BLEDevice connectScanUuid(const String uuid, unsigned long timeout) {
  BLE.scanForUuid(uuid);
  unsigned long startTime = millis();
  // TODO: return peripheral after connecting to it
  // Use the same number of retries as connectByAddress
  // IMPORTANT: do not try to connect to monsters you've already found!
  // (check this by calling peripheral.localName on an available peripheral)
  while(millis() - startTime < timeout) {
    delay(10);

  }
  BLE.stopScan();
  return BLEDevice(); // empty device
}

/*
 * wrapper for BLE scanForAddress, connect, and discover functions
 * returns empty BLEDevice if timeout is exceeded
 */
BLEDevice connectScanAddress(const String addr, unsigned long timeout) {
  const char* addrCStr = addr.c_str();
  BLE.scanForAddress(addrCStr);
  unsigned long startTime = millis();
  while(millis() - startTime < timeout) {
    delay(10);
    BLEDevice peripheral = BLE.available();

    if (!peripheral) continue;

    BLE.stopScan();

    // attempt to connect to peripheral; with retries
    if (attemptConnectAndDiscover(peripheral, 6)) {
      return peripheral;
    } else {
      // try the whole thing again
      BLE.scanForAddress(addrCStr);
    }
  }
  BLE.stopScan();
  return BLEDevice(); // empty device
}

/*
 * Search for remote home devices to connect to
 * If home devices have keys, send key and name to local home device
 */
void scanForMonsters() {
  BLEDevice remote = connectByScan(connectScanUuid, UUID_SERVICE_OUTBOX);
  Serial.print("Found monster ");
  String foundName = remote.localName();
  Serial.println(foundName);
  
  // TODO: read key characteristic
  // IMPORTANT: if characteristic is not found, disconnect remote before returning

  remote.disconnect(); // done interacting with remote. Let them go!

  BLEDevice local = connectByScan(connectScanAddress, savedAddress);
  // TODO: connect to local home device and send them the new monster for displaying!
  // remember that sendMonster does all the hashing for us
  // IMPORTANT: check if sendMonster succeeded and retry up to three times if it failed
  local.disconnect();
  // TODO: if sending succeeded, add the monster to the list of found monsters
}

/*
 * Helper function to hash and send found monster key and name
 * Assumes device is connected and attributes have been discovered
 * returns false if writing to any of the characteristics failed
 */
bool sendMonster(BLEDevice p, uint32_t key, String name) {
    uint32_t keyHashed = encrypt(password, key);
    String nameHashed = encrypt(password, name);

    BLECharacteristic c = p.characteristic(UUID_CHARACTERISTIC_INBOX_KEY);
    if (!c || !c.writeValue(key)) return false;
    
    c = p.characteristic(UUID_CHARACTERISTIC_INBOX_KEY_HASHED);
    if (!c || !c.writeValue(keyHashed)) return false;

    c = p.characteristic(UUID_CHARACTERISTIC_INBOX_NAME);
    if (!c || !c.writeValue(name.c_str(), name.length())) return false;
    
    c = p.characteristic(UUID_CHARACTERISTIC_INBOX_NAME_HASHED);
    return c && c.writeValue(nameHashed.c_str(), nameHashed.length());
}

/*
 * Helper function to read in a characteristic value byte-by-byte
 * into a uint32_t
 */
uint32_t getBLEIntValue(BLECharacteristic c) {
  c.read();
  int charSize = c.valueSize();
  const uint8_t* val = c.value();
  uint32_t ret;
  for (int i = 0; i < charSize; i++) {
    ret <<= 8;
    ret += val[charSize - 1 - i];
  }
  return ret;
}
