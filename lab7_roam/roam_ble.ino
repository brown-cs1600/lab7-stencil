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

  // Scan for corresponding home device
  BLEDevice home = connectByName(specialName);

  Serial.println("Connecting to our home device!");

  Serial.print("Found ");
  Serial.print(home.address());
  savedAddress = home.address();
  Serial.print(" '");
  Serial.print(home.localName());
  Serial.println("'");

  Serial.print("Sending over the computed monster ");
  Serial.println(getMonsterName());
  sendMonster(home, getMonsterKey(), getMonsterName());
  addMonster(getMonsterName());
  home.disconnect();
}

/*
 * wrapper for BLE scanForName function
 * blocks until peripheral is found
 */
BLEDevice connectByName(const String name) {
  const char* nameCStr = name.c_str();
  BLE.scanForName(nameCStr);
  BLEDevice peripheral = BLE.available();

  while (!peripheral) {
    BLE.scanForName(nameCStr);
    peripheral = BLE.available();
  }
  
  BLE.stopScan();

  // IMPORTANT: need to connect and discoverAttributes if we'll be reading/writing characterstics
  peripheral.connect();
  peripheral.discoverAttributes();
  return peripheral;
}

/*
 * wrapper for BLE scanForUuid function 
 * to connect to devices with outbox service
 * ignores monsters that were already found
 * blocks until peripheral is found
 */
BLEDevice connectToOutboxUuid() {
  BLE.scanForUuid(UUID_SERVICE_OUTBOX);
  BLEDevice peripheral = BLE.available();

  // TODO: modify while guard to ignore monsters that were already found
  // (get peripheral name by running peripheral.localName)
  while (!peripheral) {
    BLE.scanForUuid(UUID_SERVICE_OUTBOX);
    peripheral = BLE.available();
  }
  BLE.stopScan();

  peripheral.connect();
  peripheral.discoverAttributes();
  return peripheral;
}

/*
 * wrapper for BLE scanForAddress function
 * blocks until peripheral is found
 */
BLEDevice connectByAddress(const String addr) {
  const char* addrCStr = addr.c_str();
  BLE.scanForAddress(addrCStr);
  BLEDevice peripheral = BLE.available();

  while (!peripheral) {
    BLE.scanForAddress(addrCStr);
    peripheral = BLE.available();
  }
  BLE.stopScan();
  
  peripheral.connect();
  peripheral.discoverAttributes();
  return peripheral;
}

/*
 * Search for remote home devices to connect to
 * If home devices have keys, send key and name to local home device
 */
void scanForMonsters() {
  BLEDevice remote = connectToOutboxUuid();
  Serial.print("Found monster ");
  String foundName = remote.localName();
  Serial.println(foundName);
  
  // TODO: read key characteristic

  remote.disconnect();

  BLEDevice local = connectByAddress(savedAddress);
  // TODO: connect to local home device and send them the new monster for displaying!
  // remember that sendMonster does all the hashing for us
  local.disconnect();

}

/*
 * Helper function to hash and send found monster key and name
 * Assumes devices is connected and attributes have been discovered
 */
void sendMonster(BLEDevice p, uint32_t key, String name) {
    BLECharacteristic c = p.characteristic(UUID_CHARACTERISTIC_INBOX_KEY);
    uint32_t keyHashed = encrypt(password, key);
    c.writeValue(key);
    c = p.characteristic(UUID_CHARACTERISTIC_INBOX_KEY_HASHED);
    c.writeValue(keyHashed);
    c = p.characteristic(UUID_CHARACTERISTIC_INBOX_NAME);
    String nameHashed = encrypt(password, name);
    c.writeValue(name.c_str(), name.length());
    c = p.characteristic(UUID_CHARACTERISTIC_INBOX_NAME_HASHED);
    c.writeValue(nameHashed.c_str(), nameHashed.length());
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
