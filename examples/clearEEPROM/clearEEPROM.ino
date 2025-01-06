#include <EEPROM_Preferences.h>

EEPROM_Preferences preferences(0x50, EEPROM_24LC256);

void setup() {
  Serial.begin(115200);

  preferences.begin();
  preferences.freeEEPROM();

  Serial.println("EEPROM cleared");
}

void loop() {
}