#include <EEPROM_Preferences.h>

EEPROM_Preferences preferences(0x50, EEPROM_24LC256);

#define EEPROM_RECORD_SIZE 32 // EEPROM_Preferences.h:65

void setup() {
  Serial.begin(115200);
  preferences.begin();

  for (uint16_t i = 0; i < EEPROM_24LC256; i += EEPROM_RECORD_SIZE) {
    char buffer[128];
    sprintf(buffer, "Address: %d - %s", i, preferences.dumpRecord(i));

    Serial.println(buffer);
  }
}

void loop() {

}