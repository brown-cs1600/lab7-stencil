AES128 aes128;

/*
 * Hashes String value with password using AES128
 * Non-invertible (because null character might terminate result early)
 * password must be 16 characters long
 */
String encrypt(String password, String value) {
  if (password.length() != 16) {
    Serial.println("Password must be 16 bytes long");
    return "";
  }
  aes128.setKey((const uint8_t*) password.c_str(), 16);

  int bytes = 0;
  uint8_t valBuf[16];
  uint8_t resBuf[16];
  String result = "";
  while (bytes < value.length()) {
    int toCopy = value.length() - bytes < 16 ? value.length() - bytes : 16;
    memset(valBuf, 0, 16);
    memcpy(valBuf, value.c_str() + bytes, toCopy);
    aes128.encryptBlock(resBuf, valBuf);
    for (int i = 0; i < 16; i++) {
      if (resBuf[i] != 0) {
        result += (char) resBuf[i];
      }
    }
    bytes += 16;
  }
  return result;
}

/*
 * Hashes uint32_t value with password using AES128
 * password must be 16 characters long
 */
uint32_t encrypt(String password, uint32_t value) {
  if (password.length() != 16) {
    Serial.println("Password must be 16 bytes long");
    return 0;
  }
  aes128.setKey((const uint8_t*) password.c_str(), 16);

  int bytes = 0;
  uint8_t valBuf[16];
  uint8_t resBuf[16];
  memset(valBuf, 0, 16);
  for (int b = 0; b < 4; b++) {
    valBuf[b] = (value >> (b * 8)) & 0xff;
  }
  aes128.encryptBlock(resBuf, valBuf);
  uint32_t result = 0;
  for (int b = 0; b < 16; b++) {
    result |= resBuf[15 - b];
    result <<= 8;
  }
  return result;
}
