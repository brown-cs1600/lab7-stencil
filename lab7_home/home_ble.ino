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
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
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

  Serial.println(("Bluetooth® device active, waiting for connections..."));
}

/*
 * Connect to a BLE central device and check if inbox was written
 */
void checkInbox() {
  static bool initialized = false;

  // listen for Bluetooth® Low Energy peripherals to connect:
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

          /* TODO: second sequence diagram */

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