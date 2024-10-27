const String monsters[8] = {"Trevin", "Jefferjohn", "Tomothy", "Briandon", "Sleven", "Marcob", "Granthony", "Monstaff"};

/* Helper function to get index out of BLE MAC address */
uint32_t hashBLEAddr() {
  String BLEaddr = BLE.address();
  uint32_t hash = 0;
  for (int i = 0; i < BLEaddr.length(); i++) {
    hash = (hash << 5) - hash + (uint32_t) BLEaddr[i];
  }
  return hash;
}

/* Helper function to get monster name based on BLE MAC address */
String getMonsterName() {
  return monsters[(hashBLEAddr() % 19) / 3];
}

/* Helper function to compute monster decryption key */
uint32_t getMonsterKey() {
  uint32_t hash = (hashBLEAddr() % 19) / 3;
  uint32_t mask1 = 0x01010101;
  uint32_t mask2 = 0xf0f0f0f0;
  return (mask1 * (hash + 6)) + (mask2 - ((mask1 * (8 - hash)) << 4));
}

static bool monsterFound[8] = {false, false, false, false, false, false, false, false};

/* Marks corresponding monster as found */
void addMonster(String name) {
  for (int i = 0; i < 8; i++) {
    if (monsters[i] == name) {
      monsterFound[i] = true;
      break;
    }
  }
}

/* Checks if corresponding monster was found */
bool hasMonster(String name) {
  for (int i = 0; i < 8; i++) {
    if (monsters[i] == name) return monsterFound[i];
  }
  return false;
}