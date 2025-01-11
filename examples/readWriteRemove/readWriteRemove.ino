#include <EEPROM_Preferences.h>

EEPROM_Preferences preferences(0x50, EEPROM_24LC256);

void setup() {
  Serial.begin(115200);

  preferences.begin();

  preferences.writeBool("bool", true);
  Serial.println(preferences.getBool("bool", false)); // Output: 1 (1 == true / 0 == false)

  preferences.writeString("welcome", "Hello World");
  Serial.println(preferences.getString("welcome", "")); // Output: Hello World

  preferences.writeInt("int", -123);
  Serial.println(preferences.getInt("int", -1)); // Output: -123

  preferences.writeUInt("uint", 123);
  Serial.println(preferences.getUInt("uint", 0)); // Output: 123

  preferences.writeFloat("float", 3.14159);
  Serial.println(preferences.getFloat("float", -1.0), 5); // Output: 3.14159

  // Example of a key that does not exist
  Serial.println(preferences.getString("str", "str key not found")); // Output: str key not found

  // Remove register by key and check if it was removed
  if (preferences.removeString("welcome") == EEPROM_Preferences::StatusCode::OK) {
    Serial.println("Key removed");
  }
}

void loop() {
}