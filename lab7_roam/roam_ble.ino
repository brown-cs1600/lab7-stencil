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
  const char* specialNameCStr = specialName.c_str();
  BLE.scanForName(specialNameCStr);
  BLEDevice peripheral = BLE.available();

  while (!peripheral) {
    BLE.scanForName(specialNameCStr);
    peripheral = BLE.available();
  }

  Serial.println("Connected to our home device!");

  Serial.print("Found ");
  Serial.print(peripheral.address());
  savedAddress = peripheral.address();
  Serial.print(" '");
  Serial.print(peripheral.localName());
  Serial.println("'");

  BLE.stopScan();

  // IMPORTANT: need to connect and discoverAttributes before reading/writing characterstics
  peripheral.connect();
  peripheral.discoverAttributes();
  Serial.print("Sending over the computed monster ");
  Serial.println(getMonsterName());
  sendMonster(peripheral, getMonsterKey(), getMonsterName());
  addMonster(getMonsterName());
  peripheral.disconnect();
}

/*
 * Search for remote home devices to connect to
 * If home devices have keys, send key and name to local home device
 */
void scanForMonsters() {
  // TODO: implement me based on the sequence diagram!
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
