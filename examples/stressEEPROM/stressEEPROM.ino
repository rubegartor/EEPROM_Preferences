#include <EEPROM_Preferences.h>

EEPROM_Preferences preferences(0x50, EEPROM_24LC256);

#define EEPROM_RECORD_SIZE 32 // EEPROM_Preferences.h:65
#define EEPROM_KEY_SIZE 10 // EEPROM_Preferences.h:64

void setup() {
  Serial.begin(115200);
  preferences.begin();

  preferences.freeEEPROM();

  for (uint16_t i = EEPROM_RECORD_SIZE; i < EEPROM_24LC256; i += EEPROM_RECORD_SIZE) {
    char key[EEPROM_KEY_SIZE];
    snprintf(key, EEPROM_KEY_SIZE, "%d", i);

    preferences.writeString(key, "Hello, World!");

    Serial.print("Writed: ");
    Serial.println(key);
  }

  Serial.println("EEPROM filled");

  for (uint16_t i = EEPROM_RECORD_SIZE; i < EEPROM_24LC256; i += EEPROM_RECORD_SIZE) {
    char key[EEPROM_KEY_SIZE];
    snprintf(key, EEPROM_KEY_SIZE, "%d", i);

    Serial.print("Address: ");
    Serial.print(i);
    Serial.print(" Data: ");
    Serial.println(preferences.getString(key, "Not found"));
  }
}

void loop() {

}