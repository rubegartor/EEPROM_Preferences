#include <EEPROM_Preferences.h>

EEPROM_Preferences::EEPROM_Preferences(const uint8_t deviceAddress, const uint32_t deviceSize, TwoWire *wire) {
  _deviceAddress = deviceAddress;
  _deviceSize = deviceSize;
  _wire = wire;
}

void EEPROM_Preferences::begin() {
  _wire->begin();
}

bool EEPROM_Preferences::isConnected() {
  _wire->beginTransmission(_deviceAddress);
  return _wire->endTransmission() == 0;
}

char* EEPROM_Preferences::getString(const char* key, const char* defaultValue) {
  int32_t address = _findAddress(DataType::TYPE_STRING, key);

  if (address == -1) {
    return (char*)defaultValue;
  }

  static char buffer[DATA_SIZE + 1];

  _readEEPROM(address + TYPE_SIZE + KEY_SIZE, (byte*)buffer, DATA_SIZE);

  buffer[DATA_SIZE] = '\0';

  return buffer;
}

int EEPROM_Preferences::getInt(const char* key, int defaultValue) {
  int32_t address = _findAddress(DataType::TYPE_INT, key);

  if (address == -1) {
    return defaultValue;
  }

  byte buffer[DATA_SIZE];
  _readEEPROM(address + TYPE_SIZE + KEY_SIZE, buffer, DATA_SIZE);

  int value;
  memcpy(&value, buffer, sizeof(int));

  return value;
}

float EEPROM_Preferences::getFloat(const char* key, float defaultValue) {
  int32_t address = _findAddress(DataType::TYPE_FLOAT, key);

  if (address == -1) {
    return defaultValue;
  }

  byte buffer[DATA_SIZE];
  _readEEPROM(address + TYPE_SIZE + KEY_SIZE, buffer, DATA_SIZE);

  float value;
  memcpy(&value, buffer, sizeof(float));

  return value;
}

bool EEPROM_Preferences::getBool(const char* key, bool defaultValue) {
  int32_t address = _findAddress(DataType::TYPE_BOOL, key);

  if (address == -1) {
    return defaultValue;
  }

  byte buffer[DATA_SIZE];
  _readEEPROM(address + TYPE_SIZE + KEY_SIZE, buffer, DATA_SIZE);

  bool value;
  memcpy(&value, buffer, sizeof(bool));

  return value;
}

void EEPROM_Preferences::writeString(const char *key, const char *value) {
  Record record;
  record.type = DataType::TYPE_STRING;
  strcpy(record.key, key);
  strcpy((char*)record.data, value);

  _writeRecord(record);
}

void EEPROM_Preferences::writeInt(const char *key, int value) {
  Record record;
  record.type = DataType::TYPE_INT;
  strcpy(record.key, key);
  memcpy(record.data, &value, sizeof(int));

  _writeRecord(record);
}

void EEPROM_Preferences::writeFloat(const char *key, float value) {
  Record record;
  record.type = DataType::TYPE_FLOAT;
  strcpy(record.key, key);
  memcpy(record.data, &value, sizeof(float));

  _writeRecord(record);
}

void EEPROM_Preferences::writeBool(const char *key, bool value) {
  Record record;
  record.type = DataType::TYPE_BOOL;
  strcpy(record.key, key);
  memcpy(record.data, &value, sizeof(bool));

  _writeRecord(record);
}

void EEPROM_Preferences::remove(const char* key) {
  int32_t address = _findAddress(DataType::TYPE_STRING, key);
  if (address == -1) {
    return;
  }

  byte buffer[RECORD_SIZE];
  memset(buffer, 0xFF, RECORD_SIZE);

  _writeEEPROM(address, buffer, RECORD_SIZE);
}

bool EEPROM_Preferences::freeEEPROM() {
  byte buffer[RECORD_SIZE];
  memset(buffer, 0xFF, RECORD_SIZE);

  for (uint32_t i = 0; i < _deviceSize; i += RECORD_SIZE) {
    _writeEEPROM(i, buffer, RECORD_SIZE);
  }

  return true;
}

void EEPROM_Preferences::_writeEEPROM(uint32_t address, byte* data, uint32_t length) {
  while (length > 0) {
    uint8_t pageOffset = address % 64;
    uint8_t pageRemaining = 64 - pageOffset;
    uint8_t bufferLimit = min((uint32_t)30, length);
    uint8_t bytesToWrite = min(pageRemaining, bufferLimit);

    Wire.beginTransmission(EEPROM_ADDRESS);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    for (uint32_t i = 0; i < bytesToWrite; i++) {
      Wire.write(data[i]);
    }
    Wire.endTransmission();

    while (!isConnected());    

    address += bytesToWrite;
    data += bytesToWrite;
    length -= bytesToWrite;
  }
}

void EEPROM_Preferences::_readEEPROM(uint32_t address, byte* buffer, uint32_t length) {
  Wire.beginTransmission(EEPROM_ADDRESS);
  Wire.write((address >> 8) & 0xFF);
  Wire.write(address & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_ADDRESS, length);
  for (uint32_t i = 0; i < length; i++) {
    if (Wire.available()) {
      buffer[i] = Wire.read();
    }
  }
}

byte EEPROM_Preferences::_readByte(uint32_t address) {
  byte rdata = 0xFF;
  _readEEPROM(address, &rdata, 1);

  return rdata;
}

int32_t EEPROM_Preferences::_nextAddress() {
  for (uint32_t i = 0; i < _deviceSize; i += RECORD_SIZE) {
    if (_readByte(i) == 0xFF) return i;
  }

  return -1;
}

int32_t EEPROM_Preferences::_findAddress(DataType type, const char* key) {
  for (uint32_t i = 0; i < _deviceSize; i += RECORD_SIZE) {
    if (_readByte(i) == 0xFF) break;
    if (_readByte(i + TYPE_SIZE) == 0xFF) break;

    byte memType = _readByte(i);

    char buffer[KEY_SIZE + 1];
    _readEEPROM(i + TYPE_SIZE, (byte*)buffer, KEY_SIZE);
    buffer[KEY_SIZE] = '\0';

    if (memType == type && strcmp(key, buffer) == 0) {
      return i;
    }
  }

  return -1;
}

void EEPROM_Preferences::_writeRecord(Record record) {
  int32_t address = _findAddress(record.type, record.key);
  if (address == -1) {
    address = _nextAddress();
    if (address == -1) {
      return;
    }
  }

  byte buffer[RECORD_SIZE];
  memset(buffer, 0xFF, sizeof(buffer));

  buffer[0] = record.type;
  memcpy(buffer + TYPE_SIZE, record.key, KEY_SIZE);
  memcpy(buffer + TYPE_SIZE + KEY_SIZE, record.data, DATA_SIZE);
  
  _writeEEPROM(address, buffer, sizeof(buffer));
}