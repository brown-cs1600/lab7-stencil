#include <AES.h>
#include <ArduinoBLE.h>
#include <Crypto.h>

/* TODO */
// Must match lab7_roam.h exactly!!
const String specialName = "";
const String password = "Custom password! (must be 16 characters)";
//             start -> |1234567890123456| <- end

#define UUID_SERVICE_INBOX "8c0b243e-70bc-4e94-ba10-0b2b6bc54ef6"
#define UUID_CHARACTERISTIC_INBOX_KEY "1bade720-66e6-48a0-9fce-5f282eb14e99"
#define UUID_CHARACTERISTIC_INBOX_KEY_HASHED "b95fefde-c850-41a9-b281-1bda0433ddc7"
#define UUID_CHARACTERISTIC_INBOX_NAME "c596f5d0-ff82-4115-b564-753d7e1b35a3"
#define UUID_CHARACTERISTIC_INBOX_NAME_HASHED "46060ad3-2216-4165-936d-4d556a04427f"

#define UUID_SERVICE_OUTBOX "f7f98528-bdcd-4aeb-8b23-27fe1ef11bce"
#define UUID_CHARACTERISTIC_OUTBOX_KEY "b05233b4-de3e-440d-96e0-b9bcb0a4cfc8"

/* BLE functions */
void roamInit();
void scanForMonsters();
void sendMonster(BLEDevice p, int32_t key, String name);
uint32_t getBLEIntValue(BLECharacteristic c);

/* AES128 utilities */
String encrypt(String key, String value);
uint32_t encrypt(String key, int32_t value);

/* monster utilities */
String getMonsterName();
uint32_t getMonsterKey();
void addMonster(String name);
bool hasMonster(String name);