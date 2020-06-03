extern Config configSet;
extern const int validateConfig;

#include "EEPROM.h"
void initEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
}

void saveConfig() {
  EEPROM.writeBytes(0, &configSet, sizeof(configSet));
  EEPROM.commit();
#ifdef DEBUG_CONSOLE
  Serial.println("+++++++++++++++++++++++++++++++++++ Config +++++++++++++++++++++++++++++++++++ ");
  Serial.println("Save Config auf EEPROM:");
  Serial.print("addrRF: ");
  Serial.println(configSet.addrRfSend);
  Serial.print("addrRfRecive: ");
  Serial.println(configSet.addrRfRecive);
  Serial.print("rfStaerke: ");
  Serial.println(configSet.rfStaerke);
  Serial.print("webserverEnabled: ");
  Serial.println(configSet.webserverEnabled);
  Serial.print("btEnabled: ");
  Serial.println(configSet.btEnabled);
  Serial.print("recive: ");
  Serial.println(configSet.recive);
  Serial.print("timeout: ");
  Serial.println(configSet.timeout);
  Serial.print("validate: ");
  Serial.println(configSet.validate);
  Serial.println();
#endif
}

void loadConfig() {
#ifdef DEBUG_CONSOLE
  Serial.println("Lade Config aus EEPROM: ");
  Serial.println();
#endif
  Config temp;
  EEPROM.readBytes(0, &temp, sizeof(temp));
#ifdef DEBUG_CONSOLE
  Serial.println("+++++++++++++++++++++++++++++++++++ Config +++++++++++++++++++++++++++++++++++ ");
  Serial.print("addrRF: ");
  Serial.println(configSet.addrRfSend);
  Serial.print("addrRfRecive: ");
  Serial.println(configSet.addrRfRecive);
  Serial.print("rfStaerke: ");
  Serial.println((int)temp.rfStaerke);
  Serial.print("webserverEnabled: ");
  Serial.println(temp.webserverEnabled);
  Serial.print("btEnabled: ");
  Serial.println(temp.btEnabled);
  Serial.print("recive: ");
  Serial.println(temp.recive);
  Serial.print("timeout: ");
  Serial.println(temp.timeout);
  Serial.print("validate: ");
  Serial.println(temp.validate);
  Serial.println();
#endif
  if (temp.validate != validateConfig) {
    saveConfig();
  } else {
    configSet = temp;
  }
#ifdef DEBUG_CONSOLE
  Serial.println("+++++++++++++++++++++++++++++++++++ Config +++++++++++++++++++++++++++++++++++ ");
  Serial.print("addrRF: ");
  Serial.println(configSet.addrRfSend);
  Serial.print("addrRfRecive: ");
  Serial.println(configSet.addrRfRecive);
  Serial.print("rfStaerke: ");
  Serial.println((int)configSet.rfStaerke);
  Serial.print("webserverEnabled: ");
  Serial.println(configSet.webserverEnabled);
  Serial.print("btEnabled: ");
  Serial.println(configSet.btEnabled);
  Serial.print("recive: ");
  Serial.println(configSet.recive);
  Serial.print("timeout: ");
  Serial.println(configSet.timeout);
  Serial.print("validate: ");
  Serial.println(configSet.validate);
  Serial.println();
#endif
}
