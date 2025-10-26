/* Initialize BLE services and characterstics */
BLEService inboxService(UUID_SERVICE_INBOX);
BLEUnsignedIntCharacteristic inboxKey(UUID_CHARACTERISTIC_INBOX_KEY, BLEWriteWithoutResponse);
BLEUnsignedIntCharacteristic inboxKeyHashed(UUID_CHARACTERISTIC_INBOX_KEY_HASHED, BLEWriteWithoutResponse);
BLECharacteristic inboxName(UUID_CHARACTERISTIC_INBOX_NAME, BLEWriteWithoutResponse, 256, 0);
BLECharacteristic inboxNameHashed(UUID_CHARACTERISTIC_INBOX_NAME_HASHED, BLEWriteWithoutResponse, 256, 0);

BLEService outboxService(UUID_SERVICE_OUTBOX);
BLEUnsignedIntCharacteristic outboxKey(UUID_CHARACTERISTIC_OUTBOX_KEY, 0); // TODO: characteristic

/*
 * Initialize home device with special name
 * Advertise inbox service
 */
void homeInit() {
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

  // set the local name peripheral advertises
  // (need to do some converting from Arduino strings to const c strings)
  BLE.setLocalName(specialName.c_str());

  // add the characteristics to the service
  inboxService.addCharacteristic(inboxKey);
  inboxService.addCharacteristic(inboxKeyHashed);
  inboxService.addCharacteristic(inboxName);
  inboxService.addCharacteristic(inboxNameHashed);

  // add service
  BLE.addService(inboxService);

  // set the UUID for the service this peripheral advertises
  BLE.setAdvertisedService(inboxService);

  // start advertising
  BLE.advertise();

  Serial.println(("Bluetooth device active, waiting for connections..."));
}

/*
 * Connect to a BLE central device and check if inbox was written
 */
void checkInbox() {
  static bool initialized = false;

  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // wait for central to do its thing
    while (central.connected());

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());

    // Characteristics were written
    if (inboxKey.written() && inboxKeyHashed.written() && inboxName.written() && inboxNameHashed.written()) {
      // Check that received and computed hashes match
      String name = getBLEStringValue(inboxName);
      String nameHashedOurs = encrypt(password, getBLEStringValue(inboxName));
      String nameHashed = getBLEStringValue(inboxNameHashed);
      uint32_t key = inboxKey.value(); 
      uint32_t keyHashedOurs = encrypt(password, key);
      uint32_t keyHashed = inboxKeyHashed.value();
      if (nameHashedOurs == nameHashed && keyHashedOurs == keyHashed) {
        // If this is the first connection, change the monster's name and advertise outbox service
        if (!initialized) {
          Serial.println("initializing!");
          BLE.disconnect();
          const String nameCopy = arduino::String(name);
          // TODO: uncomment below for step 3C
          //initOutbox(nameCopy, key);

          BLE.advertise();
          initialized = true;
        }

        // A new monster was found. Display it on the menu!
        addMonster(name, key);
        printMonsterOptions();
      }
    }
  }
}

/*
 * Initializes the outbox according to the second sequence diagram
 */
void initOutbox(const String monsterName, uint32_t key) {
  // TODO: set name as monsterName (remember to convert to c_str)

  BLE.setAdvertisedService(outboxService);

  // TODO: add characteristic KEY to OUTBOX service

  // TODO: set value of KEY characteristic
  // outboxKey.writeValue(...)

  BLE.addService(outboxService);
}

/*
 * Helper function to read in a characteristic value character-by-character
 * into an Arduino String
 */
String getBLEStringValue(BLECharacteristic c) {
  int charSize = c.valueLength();
  const uint8_t* val = c.value();
  String ret = "";
  for (int i = 0; i < charSize; i++) {
    ret += (char) val[i];
  }
  return ret;
}
