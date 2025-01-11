# EEPROM_Preferences (WIP)

Arduino Library for I2C EEPROM - 24LC256/64/32/16/08/04/02/01.


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
EEPROM_Preferences(const uint8_t deviceAddress, const uint16_t deviceSize, TwoWire *wire = &Wire);
```

### Initialization

* Initializes the eeprom and configures the write protection pin if necessary
```cpp
void begin(int8_t writeProtectPin = -1);
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

* Obtains an uint through a key, if the key is not found in memory, it will return the default value indicated in the second parameter
```cpp
int getUInt(const char* key, int defaultValue);
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
StatusCode writeString(const char* key, const char* value);
```

* Writes an int to memory using the specified key
```cpp
StatusCode writeInt(const char* key, int32_t value);
```

* Writes an uint to memory using the specified key
```cpp
StatusCode writeUInt(const char* key, uint32_t value);
```

* Writes a float to memory using the specified key
```cpp
StatusCode writeFloat(const char* key, float value);
```

* Writes a bool to memory using the specified key
```cpp
StatusCode writeBool(const char* key, bool value);
```

### Others

* Removes string record by value from memory and reallocate memory
```cpp
StatusCode removeString(const char* key);
```

* Removes int record by value from memory and reallocate memory
```cpp
StatusCode removeInt(const char* key);
```

* Removes uint record by value from memory and reallocate memory
```cpp
StatusCode removeUInt(const char* key);
```

* Removes bool record by value from memory and reallocate memory
```cpp
StatusCode removeBool(const char* key);
```

* Removes float record by value from memory and reallocate memory
```cpp
StatusCode removeFloat(const char* key);
```

* Enable write protection mode on the memory
```cpp
void enableWrite();
```

* Disable write protection mode on the memory
```cpp
void disableWrite();
```

* Completely clears the memory (it takes some time to perform this operation)
```cpp
bool freeEEPROM();
```

* Dumps register data from EEPROM, address should be multiple of 32
```cpp
const char* dumpRecord(uint16_t address);
```

### StatusCode

| Code | Text                                 |
|------|--------------------------------------|
| 0    | No errors found                      |
| 1    | Memory is full                       |
| 2    | Key not found (only on remove)       |
| 3    | Key length overflow (only on write)  |
| 4    | Data length overflow (only on write) |
| 5    | Memory is write protected            |

## To-Do

- [ ]  Add support to all 24LCXXX memories (only tested with 24LC256)


## Credits

Inspired by RobTillaart's I2C_EEPROM library (https://github.com/RobTillaart/I2C_EEPROM) and ESP32 Preferences library
