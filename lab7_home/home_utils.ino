/* Encrypted monsters */
const uint32_t monsters[8][3] = {
  {0x74765678, 0x97D84886, 0xD8698588},
  {0x87879984, 0x76AEA816, 0xAE847799},
  {0x98989898, 0xD8A79A58, 0xA798D898},
  {0xB8680BAC, 0xB92CA3F9, 0xE8B388B5},
  {0xC81FC5C4, 0xAAF7BE6D, 0x5BED4D90},
  {0xC04C34B0, 0x3C37B03C, 0x34C04BCB},
  {0xDCDCA3D1, 0x3CA3DB3C, 0x83D33CA3},
  {0xDACB3B7C, 0x21322A5C, 0x1E4EF452}};

const String monsterNames[8] = {"Jefferjohn", "Trevin", "Tomothy", "Briandon", "Sleven", "Marcob", "Granthony", "Monstaff"};
static uint32_t monsterKeys[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/* Stores key of corresponding monster */
void addMonster(String name, uint32_t key) {
  for (int i = 0; i < 8; i++) {
    if (monsterNames[i] == name && monsterKeys[i] == 0) {
      Serial.print("You found ");
      Serial.print(name);
      Serial.println("!");

      monsterKeys[i] = key;
      break;
    }
  }
}

/* Checks if key for corresponding monster was found */
bool hasMonster(String name) {
  for (int i = 0; i < 8; i++) {
    if (monsterNames[i] == name) return monsterKeys[i] != 0;
  }
  return false;
}

/* Prints a menu of found monsters */
void printMonsterOptions() {
  Serial.println("\nFOUND MONSTERS:");
  int j = 0;
  for (int i = 0; i < 8; i++) {
    if (monsterKeys[i] != 0) {
      Serial.print(j);
      Serial.print(": ");
      Serial.println(monsterNames[i]);
      j++;
    }
  }
}

/* Displays the found monster at given index */
void displayMonster(int index) {
  for (int i = 0; i < 8; i++) {
    if (monsterKeys[i] != 0) {
      if (index == 0) {
        Serial.print("Displaying ");
        Serial.println(monsterNames[i]);
        uint32_t key = monsterKeys[i];
        // encryptid will be an array of length 3
        const uint32_t* encryptid = monsters[i];
        uint32_t decryptid[3];

        /* TODO: compute the decryptid based on the encryptid and the key */

        matrix.loadFrame(decryptid);
        break;
      } else {
        index--;
      }
    }
  }
}