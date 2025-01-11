#ifndef EEPROM_PREFERENCES_H
#define EEPROM_PREFERENCES_H

#include <Arduino.h>
#include <Wire.h>

#define EEPROM_FLT_MAX 3.4E+38
#define EEPROM_FLT_MIN -3.4E+38

enum EEPROM_24LCXX {
  //EEPROM_24LC01 = 128,
  //EEPROM_24LC02 = 256,
  //EEPROM_24LC04 = 512,
  //EEPROM_24LC08 = 1024,
  //EEPROM_24LC16 = 2048,
  //EEPROM_24LC32 = 4096,
  //EEPROM_24LC64 = 8192,
  //EEPROM_24LC128 = 16384,
  EEPROM_24LC256 = 32768
};

class EEPROM_Preferences {
  public:
    EEPROM_Preferences(const uint8_t deviceAddress, const uint16_t deviceSize, TwoWire *wire = &Wire);

    enum StatusCode {
      OK = 0,
      OUT_OF_MEMORY = 1,
      KEY_NOT_FOUND = 2,
      KEY_LENGTH_OVERFLOW = 3,
      DATA_LENGTH_OVERFLOW = 4,
      WRITE_PROTECTED = 5,
    };

    void begin(int8_t writeProtectPin = -1);
    bool isConnected();

    uint32_t getUInt(const char* key, uint32_t defaultValue);
    int32_t getInt(const char* key, int32_t defaultValue);
    char* getString(const char* key, const char* defaultValue);
    float getFloat(const char* key, float defaultValue);
    bool getBool(const char* key, bool defaultValue);

    StatusCode writeUInt(const char* key, uint32_t value);
    StatusCode writeInt(const char* key, int32_t value);
    StatusCode writeString(const char* key, const char* value);
    StatusCode writeFloat(const char* key, float value);
    StatusCode writeBool(const char* key, bool value);

    StatusCode removeUInt(const char* key);
    StatusCode removeInt(const char* key);
    StatusCode removeString(const char* key);
    StatusCode removeFloat(const char* key);
    StatusCode removeBool(const char* key);

    void freeEEPROM();
    void enableWrite();
    void disableWrite();

    const char* dumpRecord(uint16_t address);
  private:
    static const uint8_t DATA_SIZE = 21;
    static const uint8_t TYPE_SIZE = 1;
    static const uint8_t KEY_SIZE = 10;
    static const uint8_t RECORD_SIZE = TYPE_SIZE + KEY_SIZE + DATA_SIZE;

    enum DataType {
      TYPE_INT = 0x01,
      TYPE_UINT = 0x02,
      TYPE_BOOL = 0x03,
      TYPE_FLOAT = 0x04,
      TYPE_STRING = 0x05
    };

    struct Record {
      DataType type;
      char key[KEY_SIZE + 1];
      byte data[DATA_SIZE + 1];
    };

    TwoWire* _wire;
    int8_t _writeProtectPin = -1;
    uint8_t _deviceAddress;
    uint16_t _deviceSize;
    uint16_t _lastAddress = 0;

    void _writeEEPROM(uint16_t address, byte* data, uint16_t length);
    void _readEEPROM(uint16_t address, byte* buffer, uint16_t length);
    void _writeCache(uint16_t value);
    uint16_t _findAddress(DataType type, const char* key);
    uint16_t _readCache();
    StatusCode _remove(const char* key, DataType type);
    StatusCode _writeRecord(Record record);
};

#endif