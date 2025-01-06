# EEPROM_Preferences (WIP)

Arduino Library for I2C EEPROM - 24LC512/256/64/32/16/08/04/02/01.


## Description

This library allows to interact with external I2C EEPROM in the style of the ESP32 Preferences.h library, avoiding the user having to manage the memory addresses.

## Example

```cpp
#include <EEPROM_Preferences.h>

EEPROM_Preferences preferences(0x50, EEPROM_24LC256);

void setup() {
  Serial.begin(115200);

  preferences.begin();

  preferences.writeBool("bool", true);
  Serial.println(preferences.getBool("bool", false));

  preferences.writeString("welcome", "Hello word");
  Serial.println(preferences.getString("welcome", ""));
}

void loop() {
}
```

## Interface

### Constructor

```cpp
EEPROM_Preferences(const uint8_t deviceAddress, const uint32_t deviceSize, TwoWire *wire = &Wire);
```

### Read methods


* Obtains a string through a key, if the key is not found in memory, it will return the default value indicated in the second parameter
```cpp
char* getString(const char* key, const char* defaultValue);
```

* Obtains an int through a key, if the key is not found in memory, it will return the default value indicated in the second parameter
```cpp
int getInt(const char* key, int defaultValue);
```

* Obtains a float through a key, if the key is not found in memory, it will return the default value indicated in the second parameter
```cpp
float getFloat(const char* key, float defaultValue);
```

* Obtains a bool through a key, if the key is not found in memory, it will return the default value indicated in the second parameter
```cpp
bool getBool(const char* key, bool defaultValue);
```

### Write methods

* Writes a string to memory using the specified key
```cpp
void writeString(const char* key, const char* value);
```

* Writes an int to memory using the specified key
```cpp
void writeInt(const char* key, int value);
```

* Writes a float to memory using the specified key
```cpp
void writeFloat(const char* key, float value);
```

* Writes a bool to memory using the specified key
```cpp
void writeBool(const char* key, bool value);
```

### Others

* Removes record by value from memory
```cpp
void remove(const char* key);
```

* Completely clears the memory (it takes some time to perform this operation)
```cpp
bool freeEEPROM();
```

## Tested platforms

- [x]  Arduino AVR (Uno, Nano, Mega, etc.)
- [x]  Espressif ESP32
- [X]  RP2040
- [x]  Teensy 4.1


## To-Do

- [ ]  Add support to all 24LCXXX memories (only tested with 24LC256)
- [ ]  Improve memory space, currently all records occupy 32 bytes
- [ ]  Extend get/set methods to use all available types
- [ ]  Add posibility to manage writeProtection
- [ ]  Input validation
- [ ]  Implement key<>address cache


## Credits

Inspired by RobTillaart's I2C_EEPROM library (https://github.com/RobTillaart/I2C_EEPROM) and ESP32 Preferences library
