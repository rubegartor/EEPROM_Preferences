#include <EEPROM_Preferences.h>

EEPROM_Preferences preferences(0x50, EEPROM_24LC256);

#define WP_PIN 2

void setup() {
  Serial.begin(115200);
  preferences.begin(WP_PIN); // By default if WP_PIN is defined the EEPROM is write protected

  EEPROM_Preferences::StatusCode statusPre = preferences.writeBool("bool", true);

  if (statusPre == EEPROM_Preferences::StatusCode::WRITE_PROTECTED) {
    Serial.println("EEPROM is write protected");
  }

  preferences.enableWrite();

  EEPROM_Preferences::StatusCode statusPost = preferences.writeBool("bool", true);

  if (statusPost == EEPROM_Preferences::StatusCode::WRITE_PROTECTED) {
    Serial.println("EEPROM is write protected");
  }

  if (statusPre == EEPROM_Preferences::StatusCode::OK) {
    Serial.println("EEPROM write success");
  }
}

void loop() {

}