#include <EEPROM_Preferences.h>

EEPROM_Preferences::EEPROM_Preferences(const uint8_t deviceAddress, const uint16_t deviceSize, TwoWire *wire) {
  _deviceAddress = deviceAddress;
  _deviceSize = deviceSize;
  _wire = wire;
}

void EEPROM_Preferences::begin(int8_t writeProtectPin) {
  _wire->begin();

  if (writeProtectPin != -1) {
    _writeProtectPin = writeProtectPin;
    pinMode(_writeProtectPin, OUTPUT);
    disableWrite();
  }
}

bool EEPROM_Preferences::isConnected() {
  _wire->beginTransmission(_deviceAddress);
  return _wire->endTransmission() == 0;
}

char* EEPROM_Preferences::getString(const char* key, const char* defaultValue) {
  uint16_t address = _findAddress(DataType::TYPE_STRING, key);

  if (address == (uint16_t)-1) return (char*)defaultValue;

  static char buffer[DATA_SIZE + 1];

  _readEEPROM(address + TYPE_SIZE + KEY_SIZE, (byte*)buffer, DATA_SIZE);

  buffer[DATA_SIZE] = '\0';

  return buffer;
}

uint32_t EEPROM_Preferences::getUInt(const char* key, uint32_t defaultValue) {
  uint16_t address = _findAddress(DataType::TYPE_UINT, key);

  if (address == (uint16_t)-1) return defaultValue;

  byte buffer[DATA_SIZE];
  _readEEPROM(address + TYPE_SIZE + KEY_SIZE, buffer, DATA_SIZE);

  uint32_t value;
  memcpy(&value, buffer, sizeof(uint32_t));

  return value;
}

int32_t EEPROM_Preferences::getInt(const char* key, int32_t defaultValue) {
  uint16_t address = _findAddress(DataType::TYPE_INT, key);

  if (address == (uint16_t)-1) return defaultValue;

  byte buffer[DATA_SIZE];
  _readEEPROM(address + TYPE_SIZE + KEY_SIZE, buffer, DATA_SIZE);

  int32_t value;
  memcpy(&value, buffer, sizeof(int32_t));

  return value;
}

float EEPROM_Preferences::getFloat(const char* key, float defaultValue) {
  uint16_t address = _findAddress(DataType::TYPE_FLOAT, key);

  if (address == (uint16_t)-1) return defaultValue;

  byte buffer[DATA_SIZE];
  _readEEPROM(address + TYPE_SIZE + KEY_SIZE, buffer, DATA_SIZE);

  float value;
  memcpy(&value, buffer, sizeof(float));

  return value;
}

bool EEPROM_Preferences::getBool(const char* key, bool defaultValue) {
  uint16_t address = _findAddress(DataType::TYPE_BOOL, key);

  if (address == (uint16_t)-1) return defaultValue;

  byte buffer[DATA_SIZE];
  _readEEPROM(address + TYPE_SIZE + KEY_SIZE, buffer, DATA_SIZE);

  bool value;
  memcpy(&value, buffer, sizeof(bool));

  return value;
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::writeString(const char *key, const char *value) {
  if (strlen(key) > KEY_SIZE) return StatusCode::KEY_LENGTH_OVERFLOW;
  if (strlen(value) > DATA_SIZE) return StatusCode::DATA_LENGTH_OVERFLOW;

  Record record;
  record.type = DataType::TYPE_STRING;
  strcpy(record.key, key);
  strcpy((char*)record.data, value);

  return _writeRecord(record);
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::writeUInt(const char *key, uint32_t value) {
  if (strlen(key) > KEY_SIZE) return StatusCode::KEY_LENGTH_OVERFLOW;
  if (value > UINT32_MAX) return StatusCode::DATA_LENGTH_OVERFLOW;

  Record record;
  record.type = DataType::TYPE_UINT;
  strcpy(record.key, key);
  memcpy(record.data, &value, sizeof(uint32_t));

  return _writeRecord(record);
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::writeInt(const char *key, int32_t value) {
  if (strlen(key) > KEY_SIZE) return StatusCode::KEY_LENGTH_OVERFLOW;
  if (value > INT32_MAX || value < INT32_MIN) return StatusCode::DATA_LENGTH_OVERFLOW;

  Record record;
  record.type = DataType::TYPE_INT;
  strcpy(record.key, key);
  memcpy(record.data, &value, sizeof(int32_t));

  return _writeRecord(record);
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::writeFloat(const char *key, float value) {
  if (strlen(key) > KEY_SIZE) return StatusCode::KEY_LENGTH_OVERFLOW;
  if (value > EEPROM_FLT_MAX || value < EEPROM_FLT_MIN) return StatusCode::DATA_LENGTH_OVERFLOW;

  Record record;
  record.type = DataType::TYPE_FLOAT;
  strcpy(record.key, key);
  memcpy(record.data, &value, sizeof(float));

  return _writeRecord(record);
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::writeBool(const char *key, bool value) {
  if (strlen(key) > KEY_SIZE) return StatusCode::KEY_LENGTH_OVERFLOW;

  Record record;
  record.type = DataType::TYPE_BOOL;
  strcpy(record.key, key);
  memcpy(record.data, &value, sizeof(bool));

  return _writeRecord(record);
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::removeUInt(const char* key) {
  return _remove(key, DataType::TYPE_UINT);
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::removeInt(const char* key) {
  return _remove(key, DataType::TYPE_INT);
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::removeString(const char* key) {
  return _remove(key, DataType::TYPE_STRING);
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::removeFloat(const char* key) {
  return _remove(key, DataType::TYPE_FLOAT);
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::removeBool(const char* key) {
  return _remove(key, DataType::TYPE_BOOL);
}

void EEPROM_Preferences::freeEEPROM() {
  const uint16_t blockSize = RECORD_SIZE * 2;
  byte buffer[blockSize];
  memset(buffer, 0xFF, blockSize);

  for (uint16_t i = 0; i < _deviceSize; i += blockSize) {
    _writeEEPROM(i, buffer, blockSize);
  }

  _writeCache(RECORD_SIZE);
}

void EEPROM_Preferences::enableWrite() {
  if (_writeProtectPin == -1) return;
  digitalWrite(_writeProtectPin, LOW);
}

void EEPROM_Preferences::disableWrite() {
  if (_writeProtectPin == -1) return;
  digitalWrite(_writeProtectPin, HIGH);
}

void EEPROM_Preferences::_writeEEPROM(uint16_t address, byte* data, uint16_t length) {
  while (length > 0) {
    uint8_t pageOffset = address % 64;
    uint8_t pageRemaining = 64 - pageOffset;
    uint8_t bufferLimit = min((uint16_t)30, length);
    uint8_t bytesToWrite = min(pageRemaining, bufferLimit);

    Wire.beginTransmission(_deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    for (uint8_t i = 0; i < bytesToWrite; i++) {
      Wire.write(data[i]);
    }
    Wire.endTransmission();

    while (!isConnected());

    address += bytesToWrite;
    data += bytesToWrite;
    length -= bytesToWrite;
  }
}

void EEPROM_Preferences::_readEEPROM(uint16_t address, byte* buffer, uint16_t length) {
  Wire.beginTransmission(_deviceAddress);
  Wire.write((address >> 8) & 0xFF);
  Wire.write(address & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(_deviceAddress, length);
  for (uint16_t i = 0; i < length; i++) {
    if (Wire.available()) buffer[i] = Wire.read();
  }
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::_remove(const char* key, DataType type) {
  if (_writeProtectPin != -1 && digitalRead(_writeProtectPin) == HIGH) return StatusCode::WRITE_PROTECTED;

  uint16_t address = _findAddress(type, key);
  if (address == (uint16_t)-1) return StatusCode::KEY_NOT_FOUND;

  uint16_t lastUsedAddress = _readCache() - RECORD_SIZE;

  byte emptyBuffer[RECORD_SIZE];
  memset(emptyBuffer, 0xFF, RECORD_SIZE);

  // If the address of the record to be deleted is the last used address, just clear the record and update the cache
  // or
  // If the address of the record to be deleted is the last address in memory, just clear the record and update the cache
  if (address == lastUsedAddress || address == _deviceSize - RECORD_SIZE) {
    _writeEEPROM(address, emptyBuffer, RECORD_SIZE);
    _writeCache(address);

    return StatusCode::OK;
  }

  // Copy the last record to the address of the deleted record
  byte lastRecordBuffer[RECORD_SIZE];
  _readEEPROM(lastUsedAddress, lastRecordBuffer, RECORD_SIZE);

  // Clear the record at the current address
  _writeEEPROM(address, emptyBuffer, RECORD_SIZE);

  // Write the last record to the address of the deleted record
  _writeEEPROM(address, lastRecordBuffer, RECORD_SIZE);

  // Clear the last record
  _writeEEPROM(lastUsedAddress, emptyBuffer, RECORD_SIZE);

  // Update the last used address
  _writeCache(_lastAddress - RECORD_SIZE);

  return StatusCode::OK;
}

uint16_t EEPROM_Preferences::_findAddress(DataType type, const char* key) {
  if (strlen(key) > KEY_SIZE) return (uint16_t)-1;

  char buffer[KEY_SIZE + 1];
  for (uint16_t i = RECORD_SIZE; i < _deviceSize; i += RECORD_SIZE) {
    byte memType = 0xFF;
    _readEEPROM(i, &memType, 1);

    if (memType == 0xFF) break;
    if (memType != type) continue;

    _readEEPROM(i + TYPE_SIZE, (byte*)buffer, KEY_SIZE);
    buffer[KEY_SIZE] = '\0';

    if (strcmp(key, buffer) == 0) return i;
  }

  return (uint16_t)-1;
}

EEPROM_Preferences::StatusCode EEPROM_Preferences::_writeRecord(Record record) {
  if (_writeProtectPin != -1 && digitalRead(_writeProtectPin) == HIGH) return StatusCode::WRITE_PROTECTED;

  uint16_t address = _findAddress(record.type, record.key);
  if (address == (uint16_t)-1) address = _readCache();

  if (address >= _deviceSize) return StatusCode::OUT_OF_MEMORY;

  byte buffer[RECORD_SIZE];
  memset(buffer, 0xFF, RECORD_SIZE);

  buffer[0] = record.type;
  strncpy((char*)(buffer + 1), record.key, KEY_SIZE + 1);

  switch (record.type) {
    case DataType::TYPE_STRING:
      strncpy((char*)(buffer + KEY_SIZE + 1), (char*)record.data, DATA_SIZE);
      break;
    case DataType::TYPE_INT:
      memcpy((char*)(buffer + KEY_SIZE + 1), record.data, sizeof(int32_t));
      break;
    case DataType::TYPE_UINT:
      memcpy((char*)(buffer + KEY_SIZE + 1), record.data, sizeof(uint32_t));
      break;
    case DataType::TYPE_BOOL:
      memcpy((char*)(buffer + KEY_SIZE + 1), record.data, sizeof(bool));
      break;
    case DataType::TYPE_FLOAT:
      memcpy((char*)(buffer + KEY_SIZE + 1), record.data, sizeof(float));
      break;
  }

  _writeEEPROM(address, buffer, RECORD_SIZE);
  _writeCache(address + RECORD_SIZE);

  return StatusCode::OK;
}

void EEPROM_Preferences::_writeCache(uint16_t value) {
  byte cacheBuffer[RECORD_SIZE];
  memset(cacheBuffer, 0xFF, RECORD_SIZE);

  memcpy(cacheBuffer, &value, sizeof(value));

  _writeEEPROM(0, cacheBuffer, RECORD_SIZE);

  _lastAddress = value;
}

uint16_t EEPROM_Preferences::_readCache() {
  if (_lastAddress != 0) return _lastAddress;

  byte cacheBuffer[RECORD_SIZE];

  _readEEPROM(0, cacheBuffer, RECORD_SIZE);

  uint16_t value;
  memcpy(&value, cacheBuffer, sizeof(value));

  _lastAddress = value;

  return _lastAddress;
}

const char* EEPROM_Preferences::dumpRecord(uint16_t address) {
  if (address % RECORD_SIZE != 0) return "Invalid address";

  static char output[96];
  byte buffer[RECORD_SIZE];
  _readEEPROM(address, buffer, RECORD_SIZE);

  for (uint8_t j = 0; j < RECORD_SIZE; j++) {
    snprintf(output + j * 3, sizeof(output) - j * 3, "%02X ", buffer[j]);
  }

  return output;
}