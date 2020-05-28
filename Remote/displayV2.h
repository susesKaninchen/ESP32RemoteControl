#include <TFT_eSPI.h>
#include <SPI.h>

extern void updateInput();
extern void saveConfig();
extern void menueBT();
extern void menueTimeout();
extern void menueWebServer();
extern void menueRFrecive();
extern void menueRfReciveAdresse();
extern void menueRFpower();
extern void menueRfSendAdresse();

extern Input_State stateInput;
extern Reciv_Package recivPackage;
extern long lastAction;
// ############################################## Menue/Display
TFT_eSPI tft = TFT_eSPI();
const PROGMEM char *menueStrings[] = {"RFAdresseS", "RFAdresseE", "RF recive", "RF Send Lvl", "Web Server", "Bluetouch", "Timeout"};
const PROGMEM char *boolStrings[] = { "Aktiviert", "Deaktiviert"};
const PROGMEM char *powerLvlStrings[] = { "MIN", "LOW", "HIGH", "MAX"};
const PROGMEM int menueStringsLen = 7;
long lastTemp = 0;
bool wlanVerbunden = false;

bool isLeft() {
  return stateInput.leftStickX < 512 - 400 || stateInput.rightStickX < 512 - 400;
}

bool isRight() {
  return stateInput.leftStickX > 512 + 400 || stateInput.rightStickX > 512 + 400;
}

bool isTop() {
  return stateInput.leftStickY > 512 + 400 || stateInput.rightStickY > 512 + 400;
}

bool isDown() {
  return stateInput.leftStickY < 512 - 400 || stateInput.rightStickY < 512 - 400 ;
}

uint16_t bgColor(uint16_t x, uint16_t y)
{
  return TFT_BLACK;
}

void initTft() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  //tft.setCallback(bgColor); // Switch on color callback for anti-aliased fonts
}

void addRange(byte &i, byte minInt, byte maxInt) {
  if (++i > maxInt) {
    i = minInt;
  }
}

void subRange(byte &i, byte minInt, byte maxInt) {
  --i;
  if (i < minInt || i > maxInt) {
    i = maxInt;
  }
}

void drawAkku(float akkuValue) {
  int value = akkuValue * 100;
  int temp = value / 100;
  tft.fillRect(3, 4, 24, 9, TFT_BLACK);
  tft.setCursor(3, 4);
  tft.setTextSize(1);
  uint16_t color;
  if (value > 420) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("+++");
  } else {
    if (value > 400) {
      color = TFT_GREEN;
    } else if (value > 380) {
      color = TFT_YELLOW;
    } else {
      color = TFT_RED;
    }
    tft.setTextColor(color, TFT_BLACK);
    tft.print(temp);
    tft.setCursor(12, 4);
    tft.print(value - (temp * 100));
    tft.drawPixel(9, 10, color);
  }
  tft.drawRect(1, 1 , 27, 13, TFT_GREEN);
  tft.drawRect(28, 4 , 2, 6, TFT_GREEN);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: Akku ");
  Serial.println(akkuValue);
  Serial.println();
#endif
}

/*
  4,2V - 100%
  4,1V - 90%
  4,0V - 75%
  3,9V - 55%
  3,8V - 35%
  3,7V - 20%
  3,6V - 0%
*/

void drawWifi(bool active) {
  uint16_t color;
  if (active) {
    if (wlanVerbunden) {
      color = TFT_GREEN;
    } else {
      color = TFT_YELLOW;
    }
  } else {
    color = TFT_RED;
  }
  tft.drawLine(33, 7, 37, 3, color);
  tft.drawLine(37, 3, 40, 3, color);
  tft.drawLine(40, 3, 44, 7, color);

  tft.drawLine(35, 8, 37, 6, color);
  tft.drawLine(37, 6, 40, 6, color);
  tft.drawLine(40, 6, 42, 8, color);

  tft.drawRect(38, 9 , 2, 2, color);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: Wlan ");
  Serial.println(active);
  Serial.println();
#endif
}

void drawBT(bool active) {
  uint16_t color;
  if (active) {
    color = TFT_GREEN;
  } else {
    color = TFT_RED;
  }
  tft.drawLine(47, 4, 52, 9, color);
  tft.drawLine(52, 9, 49, 12, color);
  tft.drawLine(49, 12, 49, 1, color);
  tft.drawLine(49, 1, 52, 4, color);
  tft.drawLine(52, 4, 47, 9, color);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: BT ");
  Serial.println(active);
  Serial.println();
#endif
}

void drawAdresse(char* addrS, char* addrE) {
  //tft.fillRect(57, 2, 101, 12, TFT_BLACK);
  tft.drawRect(56, 1 , 102, 13, TFT_GREEN);
  tft.setCursor(59, 4);
  tft.setTextColor(0x666666, TFT_BLACK);
  tft.setTextSize(1);
  tft.print("E:");
  tft.print(addrS);
  tft.print("S:");
  tft.print(addrE);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: ADR S");
  Serial.println(addrS);
  Serial.println("Update TFT: ADR E");
  Serial.println(addrE);
  Serial.println();
#endif
}

void drawBlackAndTopLine() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 15, tft.width() - 1, 15, TFT_BLUE);
}

void drawNoRecive() {
  tft.setCursor(0, 20);
  tft.setTextColor(0x666666, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextWrap(true);
  tft.println("Empfangen deaktiviert, deshalb keine Daten vom Gerät, aber ich laufe schon:");
  long temp = millis();
  //tft.setCursor(30, 90);
  //tft.setTextColor(TFT_BLACK);
  //tft.print(lastTemp);
  tft.setCursor(30, 90);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print(temp);
  lastTemp = temp;
}

void drawRecive() {
  tft.fillRect(0, 20, tft.width(), 32, TFT_BLACK);
  tft.setCursor(0, 20);
  tft.setTextColor(0x666666, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextWrap(true);
  tft.println("Letztes Paket erhalten: ");
  tft.println(recivPackage.timestamp);
  tft.print("Akkustatus des Bots: ");
  tft.println(recivPackage.akku);
  tft.print("Abschaltung der Fernbedienung in : ");
  tft.println((lastAction + configSet.timeout) - millis());
  tft.print("leftStickX: ");
  tft.println(stateInput.leftStickX);
  tft.print("leftStickY: ");
  tft.println(stateInput.leftStickY);
  tft.print("rightStickX: ");
  tft.println(stateInput.rightStickX);
  tft.print("rightStickY: ");
  tft.println(stateInput.rightStickY);
  //tft.print("menueButtonPresses: ");
  //tft.println(menueButtonPresses);
}

void drawOption(const char *menueString, uint32_t value) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextWrap(false);
  // Hauptpunkt
  tft.setCursor(4, tft.height() / 2 - 20);
  tft.setTextSize(2);
  tft.println(menueString);
  tft.print(" ");
  tft.println(value);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: ");
  Serial.println(menueString);
  Serial.println(value);
  Serial.println();
#endif
}

void drawOption(const char *menueString, const char *valueString) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextWrap(false);
  // Hauptpunkt
  tft.setCursor(4, tft.height() / 2 - 20);
  tft.setTextSize(2);
  tft.println(menueString);
  tft.print(" ");
  tft.println(valueString);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: ");
  Serial.println(menueString);
  Serial.println(valueString);
  Serial.println();
#endif
}

void drawOption(const char *menueString, const char *valueString, byte marker) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextWrap(false);
  // Hauptpunkt
  tft.setCursor(4, tft.height() / 2 - 20);
  tft.setTextSize(2);
  tft.println(menueString);
  tft.print(" ");
  tft.println(valueString);
  // TODO: Unterstreichung anpassen
  tft.drawLine(((int)marker + 1) * 6, 80, ((int) marker + 2) * 6, 80, TFT_BLUE);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: ");
  Serial.println(menueString);
  Serial.println(valueString);
  Serial.println();
#endif
}

void menueBool(bool &value, const char *text) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  bool hasChangedTFT = true;
  bool tempValue = value;
  drawOption(text, (tempValue ? boolStrings[0] : boolStrings[1]));
#ifdef DEBUG_CONSOLE
  Serial.print("Menue einstellen: ");
  Serial.print(*text);
  Serial.println();
#endif
  delay(DELAY_ENTER_MENUE);
  while (true) {
    if (hasChangedTFT) {
      drawOption(text, (tempValue ? boolStrings[0] : boolStrings[1]));
      hasChangedTFT = false;
#ifdef DEBUG_CONSOLE
      Serial.println("TFT Update");
      Serial.println();
#endif
    }
    // Handle Inputs
    updateInput();
    // Analog nach Rechts = In Punkt gehen
    if (isRight()) {
#ifdef DEBUG_CONSOLE
      Serial.println("Stick nach rechts mache nichts");
      Serial.println();
#endif
      // Nichts
      continue;
    }
    // Analog nach Links = Zurück
    if (isLeft()) {
#ifdef DEBUG_CONSOLE
      Serial.println("Stick nach links, ändere und speichere es im EEPROM und gehe zurück");
      Serial.println();
#endif
      if (value != tempValue) {
        value = tempValue;
        saveConfig();
      }
      break;
    }
    // Analog nach Oben
    if (isTop() || isDown()) {
#ifdef DEBUG_CONSOLE
      Serial.println("Stick nach Oben/unten, ändere value");
      Serial.println();
#endif
      hasChangedTFT = true;
      tempValue = !tempValue;
      delay(DELAY_UPDATE_MENUE);
      continue;
    }
  }
}

void menueString(char *value, const char *text) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  bool hasChangedTFT = true;
  byte positionCounter = 0;
  drawOption(text, value);
#ifdef DEBUG_CONSOLE
  Serial.print("Menue einstellen: ");
  Serial.print(*text);
  Serial.println();
#endif
  delay(DELAY_ENTER_MENUE);
  while (true) {
    if (hasChangedTFT) {
      drawOption(text, value, positionCounter);
      hasChangedTFT = false;
#ifdef DEBUG_CONSOLE
      Serial.println("TFT Update");
      Serial.println();
#endif
    }
    // Handle Inputs
    updateInput();
    // Analog nach Rechts = In Punkt gehen
    if (isRight()) {
      addRange(positionCounter, 0, strlen(value) - 1);
#ifdef DEBUG_CONSOLE
      Serial.println("Position ++");
      Serial.println();
#endif
      continue;
    }
    // Analog nach Links = Zurück
    if (isLeft()) {
      positionCounter--;
      if (positionCounter < 0) {
#ifdef DEBUG_CONSOLE
        Serial.println("Stick nach links, ändere Power und speichere es im EEPROM und gehe zurück");
        Serial.println();
#endif
        saveConfig();
      }
      break;
    }
    // Analog nach Oben
    if (isTop()) {
      value[positionCounter]++;
      if (value[positionCounter] > '~') {
        value[positionCounter] == ' ';
      }
      hasChangedTFT = true;
#ifdef DEBUG_CONSOLE
      Serial.print("Stick nach oben, neue Value: ");
      Serial.print(value);
      Serial.println();
#endif
      delay(DELAY_UPDATE_MENUE);
      continue;
    }
    // Analog nach Unten
    if (isDown()) {
      value[positionCounter]--;
      if (value[positionCounter] < ' ') {
        value[positionCounter] == '~';
      }
      hasChangedTFT = true;
#ifdef DEBUG_CONSOLE
      Serial.print("Stick nach unten, neue Value: ");
      Serial.print(value);
      Serial.println();
#endif
      delay(DELAY_UPDATE_MENUE);
      continue;
    }
  }
}

void menueLong(uint32_t &value, const char *text) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  bool hasChangedTFT = true;
  uint32_t tempValue = value;
  drawOption(text, tempValue);
#ifdef DEBUG_CONSOLE
  Serial.print("Menue einstellen: ");
  Serial.print(*text);
  Serial.println();
#endif
  delay(DELAY_ENTER_MENUE);
  while (true) {
    if (hasChangedTFT) {
      drawOption(text, tempValue);
      hasChangedTFT = false;
#ifdef DEBUG_CONSOLE
      Serial.println("TFT Update");
      Serial.println();
#endif
    }
    // Handle Inputs
    updateInput();
    // Analog nach Rechts = In Punkt gehen
    if (isRight()) {
#ifdef DEBUG_CONSOLE
      Serial.println("Stick nach rechts addire Value");
      Serial.println();
#endif
      tempValue++;
      continue;
    }
    // Analog nach Links = Zurück
    if (isLeft()) {
#ifdef DEBUG_CONSOLE
      Serial.println("Stick nach links, ändere Power und speichere es im EEPROM und gehe zurück");
      Serial.println();
#endif
      if (tempValue != value) {
        value = tempValue;
        saveConfig();
      }
      break;
    }
    // Analog nach Oben
    if (isTop()) {
      hasChangedTFT = true;
      tempValue++;
#ifdef DEBUG_CONSOLE
      Serial.print("Stick nach oben, neue Value: ");
      Serial.print(tempValue);
      Serial.println();
#endif
      delay(DELAY_UPDATE_MENUE);
      continue;
    }
    // Analog nach Unten
    if (isDown()) {
      hasChangedTFT = true;
      tempValue--;
#ifdef DEBUG_CONSOLE
      Serial.print("Stick nach unten, neue Value: ");
      Serial.print(tempValue);
      Serial.println();
#endif
      delay(DELAY_UPDATE_MENUE);
      continue;
    }
  }
}

void menueArray(byte &value, const char *text, const char *values[], int len) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  bool hasChangedTFT = true;
  byte tempValue = value;
  drawOption(text, values[tempValue]);
#ifdef DEBUG_CONSOLE
  Serial.print("Menue einstellen: ");
  Serial.print(*text);
  Serial.println();
#endif
  delay(DELAY_ENTER_MENUE);
  while (true) {
    if (hasChangedTFT) {
      drawOption(text, values[tempValue]);
      hasChangedTFT = false;
#ifdef DEBUG_CONSOLE
      Serial.println("TFT Update");
      Serial.println();
#endif
    }
    // Handle Inputs
    updateInput();
    // Analog nach Rechts = In Punkt gehen
    if (isRight()) {
#ifdef DEBUG_CONSOLE
      Serial.println("Stick nach rechts addire Value");
      Serial.println();
#endif
      addRange(tempValue, 0, len);
      continue;
    }
    // Analog nach Links = Zurück
    if (isLeft() ) {
#ifdef DEBUG_CONSOLE
      Serial.println("Stick nach links, ändere Power und speichere es im EEPROM und gehe zurück");
      Serial.println();
#endif
      if (tempValue != value) {
        value = tempValue;
        saveConfig();
      }
      break;
    }
    // Analog nach Oben
    if (isTop()) {
      hasChangedTFT = true;
      addRange(tempValue, 0, len - 1);
#ifdef DEBUG_CONSOLE
      Serial.print("Stick nach oben, neue Value: ");
      Serial.print(values[tempValue]);
      Serial.println();
#endif
      delay(DELAY_UPDATE_MENUE);
      continue;
    }
    // Analog nach Unten
    if (isDown()) {
      hasChangedTFT = true;
      subRange(tempValue, 0, len - 1);
#ifdef DEBUG_CONSOLE
      Serial.print("Stick nach unten, neue Value: ");
      Serial.print(values[tempValue]);
      Serial.println();
#endif
      delay(DELAY_UPDATE_MENUE);
      continue;
    }
  }
}

void drawMenueV2(const char *text[], int len, uint16_t color, int active, uint16_t colorActive) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextWrap(false);
  int counterPoints = 0;
  int pointer = active;
  // Hauptpunkt
  tft.setCursor(4, tft.height() / 2 - 10);
  tft.setTextSize(2);
  tft.setTextColor(colorActive);
  tft.print(text[active]);

  tft.drawLine(2, tft.height() / 2 + 10, tft.width() - 3, tft.height() / 2 + 10, colorActive);
  tft.drawLine(2, tft.height() / 2 + 11, tft.width() - 3, tft.height() / 2 + 11, colorActive);
  // NebenPunkte
  // 10 ist die halbe Schriftgröße von der mitte, 5 ist der Abstand zum nächten Text und 12 ist die schriftgröße 2
  tft.setCursor(4, tft.height() / 2 - (10 + 5 + 12));
  tft.setTextSize(1);
  tft.setTextColor(color, TFT_BLACK);
  if (counterPoints >= len) {
    return;
  }
  if (active - 1 < 0) {
    pointer = len + active - 1;
  } else {
    pointer = active - 1;
  }
  tft.print(text[pointer]);
  counterPoints++;
  // Pinkt3
  tft.setCursor(4, tft.height() / 2 + (13 + 3));
  if (counterPoints >= len) {
    return;
  }
  if (active + 1 >= len) {
    pointer = active + 1 - len;
  } else {
    pointer = active + 1;
  }
  tft.print(text[pointer]);
  counterPoints++;
  // NebenPunkte 2
  // 10 ist die halbe Schriftgröße von der mitte, 5 ist der Abstand zum nächten Text und 12 ist die schriftgröße 2 un dann mit 1
  tft.setCursor(4, tft.height() / 2 - (10 + 5 + 12 + 5 + 10));
  tft.setTextSize(1);
  tft.setTextColor(color, TFT_BLACK);
  if (counterPoints >= len) {
    return;
  }
  if (active - 2 < 0) {
    pointer = len + active - 2;
  } else {
    pointer = active - 2;
  }
  tft.print(text[pointer]);
  counterPoints++;
  // Pinkt3
  tft.setCursor(4, tft.height() / 2 + (10 + 5 + 12 + 3 + 10));
  if (counterPoints >= len) {
    return;
  }
  if (active + 2 >= len) {
    pointer = active + 2 - len;
  } else {
    pointer = active + 2;
  }
  tft.print(text[pointer]);
  counterPoints++;
}

void handleMenue() {
#ifdef DEBUG_CONSOLE
  Serial.println("Gehe ins erste Menue");
  Serial.println();
#endif
  byte menueCounter = 0;
  bool hasChangedTFT = true;
  drawMenueV2(menueStrings, menueStringsLen, TFT_WHITE, menueCounter, TFT_BLUE);
  delay(DELAY_ENTER_MENUE);
  while (true) {
    if (hasChangedTFT) {
#ifdef DEBUG_CONSOLE
      Serial.println("Update Menue 1");
      Serial.println();
#endif
      drawMenueV2(menueStrings, menueStringsLen, TFT_WHITE, menueCounter, TFT_BLUE);
      hasChangedTFT = false;
      delay(DELAY_UPDATE_MENUE);
    }
    // Handle Inputs
    updateInput();
    // Analog nach Rechts = In Punkt gehen
    if (isRight()) {
#ifdef DEBUG_CONSOLE
      Serial.print("Gehe auf Menuepunkt: ");
      Serial.println(menueCounter);
      Serial.print(" -> ");
      Serial.println(menueStrings[menueCounter]);
      Serial.println();
#endif
      // TODO: Punkt auswählen
      if (menueCounter == 0) {
        menueRfSendAdresse();
      } else if (menueCounter == 1) {
        menueRfReciveAdresse();
      } else if (menueCounter == 2) {
        menueRFrecive();
      } else if (menueCounter == 3) {
        menueRFpower();
      } else if (menueCounter == 4) {
        menueWebServer();
      } else if (menueCounter == 5) {
        menueBT();
      } else if (menueCounter == 6) {
        menueTimeout();
      }
      hasChangedTFT = true;
      continue;
    }
    // Analog nach Links = Zurück
    if (isLeft()) {
#ifdef DEBUG_CONSOLE
      Serial.println("Verlasse menue");
      Serial.println();
#endif
      break;
    }
    // Analog nach Oben = Punkt Oberhalb aktiv
    if (isTop()) {
      hasChangedTFT = true;
      subRange(menueCounter, 0, menueStringsLen - 1);
#ifdef DEBUG_CONSOLE
      Serial.print("Wechsele Menuepunkt auf: ");
      Serial.print(menueCounter);
      Serial.print(" -> ");
      Serial.println(menueStrings[menueCounter]);
      Serial.println();
#endif
      continue;
    }
    // Analog nach Unten = Punkt unterhalb aktiv
    if (isDown()) {
      hasChangedTFT = true;
      addRange(menueCounter, 0, menueStringsLen - 1);
#ifdef DEBUG_CONSOLE
      Serial.print("Wechsele Menuepunkt auf: ");
      Serial.print(menueCounter);
      Serial.print(" -> ");
      Serial.println(menueStrings[menueCounter]);
      Serial.println();
#endif
      continue;
    }
  }
}
