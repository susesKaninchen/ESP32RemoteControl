#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

extern void updateInput();
extern void saveConfig();
extern void menueBT();
extern void menueTimeout();
extern void menueWebServer();
extern void menueRFrecive();
extern void menueRFpower();
extern void menueRFAdresse();

extern Input_State stateInput;
// ############################################## Menue/Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
const PROGMEM char *menueStrings[] = { "RFAdresse", "RF recive", "RF Send Lvl", "Web Server", "Bluetouch", "Timeout"};
const PROGMEM char *boolStrings[] = { "Aktiviert", "Deaktiviert"};
const PROGMEM char *powerLvlStrings[] = { "MIN", "LOW", "HIGH", "MAX"};
const PROGMEM int menueStringsLen = 6;

bool isLeft(){
	return stateInput.leftStickX < 512 - 400 || stateInput.rightStickX < 512 - 400;
}

bool isRight(){
	return stateInput.leftStickX > 512 + 400 || stateInput.rightStickX > 512 + 400;
}

bool isTop(){
	return stateInput.leftStickY > 512 + 400 || stateInput.rightStickY > 512 + 400;
}

bool isDown(){
	return stateInput.leftStickY < 512 - 400 || stateInput.rightStickY < 512 - 400 ;
}

void initTft(){
	tft.initR(INITR_BLACKTAB);
	tft.setRotation(1);
	tft.fillScreen(ST77XX_BLACK);
}

void addRange(byte &i, int maxInt, int minInt) {

  if (++i > maxInt) {
    i = minInt;
  }
}

void subRange(byte &i, int maxInt, int minInt) {
  if (--i < minInt) {
    i = maxInt;
  }
}

void drawOption(const char *menueString, uint32_t value) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);
  // Hauptpunkt
  tft.setCursor(4, tft.height() / 2 - 10);
  tft.setTextSize(3);
  tft.println(menueString);
  tft.println(value);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: ");
  Serial.println(menueString);
  Serial.println(value);
  Serial.println();
#endif
}

void drawOption(const char *menueString, const char *valueString) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);
  // Hauptpunkt
  tft.setCursor(4, tft.height() / 2 - 10);
  tft.setTextSize(3);
  tft.println(menueString);
  tft.println(valueString);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: ");
  Serial.println(menueString);
  Serial.println(valueString);
  Serial.println();
#endif
}

void menueBool(bool &value, const char *text) {
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

void menueLong(uint32_t &value, const char *text) {
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
      addRange(tempValue, len, 0);
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
      addRange(tempValue, len, 0);
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
      subRange(tempValue, len, 0);
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
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);
  int counterPoints = 0;
  int pointer = active;
  // Hauptpunkt
  tft.setCursor(4, tft.height() / 2 - 10);
  tft.setTextSize(2);
  tft.setTextColor(colorActive);
  tft.print(text[active]);

  tft.drawLine(2, tft.height() / 2 + 12, tft.width() - 3, tft.height() / 2 + 12, colorActive);
  tft.drawLine(2, tft.height() / 2 + 13, tft.width() - 3, tft.height() / 2 + 13, colorActive);
  // NebenPunkte
  // 10 ist die halbe Schriftgröße von der mitte, 5 ist der Abstand zum nächten Text und 12 ist die schriftgröße 2
  tft.setCursor(4, tft.height() / 2 - (10 + 5 + 12));
  tft.setTextSize(1);
  tft.setTextColor(color);
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
  tft.setTextColor(color);
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
  drawMenueV2(menueStrings, menueStringsLen, ST77XX_WHITE, menueCounter, ST77XX_BLUE);
  delay(DELAY_ENTER_MENUE);
  while (true) {
    if (hasChangedTFT) {
#ifdef DEBUG_CONSOLE
      Serial.println("Update Menue 1");
      Serial.println();
#endif
      drawMenueV2(menueStrings, menueStringsLen, ST77XX_WHITE, menueCounter, ST77XX_BLUE);
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
        menueRFAdresse();
      } else if (menueCounter == 1) {
        menueRFrecive();
      } else if (menueCounter == 2) {
        menueRFpower();
      } else if (menueCounter == 3) {
        menueWebServer();
      } else if (menueCounter == 4) {
        menueBT();
      } else if (menueCounter == 5) {
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