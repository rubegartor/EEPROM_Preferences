#ifndef EEPROM_PREFERENCES_H
#define EEPROM_PREFERENCES_H

#include <Arduino.h>
#include <Wire.h>

#define EEPROM_ADDRESS 0x50
#define DATA_SIZE 21
#define TYPE_SIZE 1
#define KEY_SIZE 10
#define RECORD_SIZE (TYPE_SIZE + KEY_SIZE + DATA_SIZE)

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
    EEPROM_Preferences(const uint8_t deviceAddress, const uint32_t deviceSize, TwoWire *wire = &Wire);

    void begin();
    bool isConnected();

    char* getString(const char* key, const char* defaultValue);
    int getInt(const char* key, int defaultValue);
    float getFloat(const char* key, float defaultValue);
    bool getBool(const char* key, bool defaultValue);

    void writeString(const char* key, const char* value);
    void writeInt(const char* key, int value);
    void writeFloat(const char* key, float value);
    void writeBool(const char* key, bool value);

    void remove(const char* key);

    bool freeEEPROM();
  private:
    enum DataType {
      TYPE_INT = 0x01,
      TYPE_FLOAT = 0x02,
      TYPE_STRING = 0x03,
      TYPE_BOOL = 0x04
    };

    struct Record {
      DataType type;
      char key[KEY_SIZE + 1];
      byte data[DATA_SIZE + 1];
    };

    TwoWire* _wire;
    uint8_t _deviceAddress;
    uint32_t _deviceSize;

    void _writeEEPROM(uint32_t address, byte* data, uint32_t length);
    void _readEEPROM(uint32_t address, byte* buffer, uint32_t length);
    byte _readByte(uint32_t address);
    int32_t _nextAddress();
    int32_t _findAddress(DataType type, const char* key);
    void _writeRecord(Record record);
};

#endif