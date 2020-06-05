// ############################################## Includes ##############################################
#include <pgmspace.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <FS.h>
#include <JPEGDecoder.h>
#include "JPEG_functions.h"
#include "images.h"

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

// ############################################## TFT Values ##############################################
TFT_eSPI tft = TFT_eSPI();

const PROGMEM char *menueStrings[] = {"RFAdresseS", "RFAdresseE", "RF recive", "RF Send Lvl", "Web Server", "Bluetouch", "Timeout"};
const PROGMEM char *boolStrings[] = { "Aktiviert", "Deaktiviert"};
const PROGMEM char *powerLvlStrings[] = { "MIN", "LOW", "HIGH", "MAX"};
const PROGMEM int menueStringsLen = 7;
long lastTemp = 0;
bool wlanVerbunden = false;

// ############################################## Hilfsfunktionen
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

// ############################################## TFT Steuerrung
void initTft() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextDatum(0);
}

// ############################################## TFT ICONS
void drawAkku(float akkuValue) {
  int value = akkuValue * 100;
  int temp = value / 100;
  tft.fillRect(3, 4, 24, 9, TFT_BLACK);
  tft.setCursor(3, 4, 1);
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
    tft.setCursor(12, 4, 1);
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

void drawWifi(byte state) {//0 red, 1 green, 2 yellow, 3 blue
  uint16_t color;
  if (state == 0) {
    color = TFT_RED;
  } else if (state == 1) {
    color = TFT_GREEN;
  } else if (state == 2) {
    color = TFT_YELLOW;
  } else if (state == 3) {
    color = TFT_BLUE;
  } else {
    color = TFT_BROWN;
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
  Serial.println(state);
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
  tft.setCursor(59, 4, 1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.print("E:");
  tft.print(addrS);
  tft.print(" |S:");
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
  drawArrayJpeg(windows, sizeof(windows), 0, 16);
  //drawJpeg("/Windows.jpg", 0, 16);
  //createArray("/Windows.jpg");
  //delay(10000);
}

// ############################################## TFT <------------------------------------------------------ USERCODE
void drawNoRecive() {
  drawArrayJpeg(backGround, sizeof(backGround), 0, 15);
  drawArrayJpeg(robot, sizeof(robot), (160 - 64) / 2, 128 - 65);
  tft.setTextDatum(1);
  tft.setTextColor(TFT_BLACK);
  tft.setTextWrap(true);
  tft.drawString("Empfangen deaktiviert", tft.width() / 2, 25, 2);
  tft.setTextDatum(0);
}

void drawRecive() {
  //tft.fillRect(0, 20, tft.width(), 32, TFT_BLACK);
  tft.setCursor(0, 20, 2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextWrap(true);
  tft.println("Letztes Paket erhalten: ");
  tft.println(recivPackage.timestamp);
  tft.print("Akkustatus des Bots: ");
  tft.println((int)recivPackage.akku);
  tft.println(recivPackage.string);
  tft.print("Abschaltung der Fernbedienung in : ");
  tft.println((lastAction + (configSet.timeout * 1000)) - millis());
  /*tft.print("leftStickX: ");
    tft.print(stateInput.leftStickX);
    tft.println("..");
    tft.print("leftStickY: ");
    tft.print(stateInput.leftStickY);
    tft.println("..");
    tft.print("rightStickX: ");
    tft.print(stateInput.rightStickX);
    tft.println("..");
    tft.print("rightStickY: ");
    tft.print(stateInput.rightStickY);
    tft.println("..");*/
  //tft.print("menueButtonPresses: ");
  //tft.println(menueButtonPresses);
}

// ############################################## TFT Menue
void drawOption(const char *menueString, const char *valueString, uint16_t color) {
  drawArrayJpeg(backGround, sizeof(backGround), 0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextWrap(false);
  tft.setTextDatum(1);
  tft.drawString(menueString, tft.width() / 2, tft.height() / 4, 4);
  tft.print(" ");
  tft.setTextColor(color);
  tft.drawString(valueString, tft.width() / 2, tft.height() / 2, 4);
  tft.setTextDatum(0);
#ifdef DEBUG_CONSOLE
  Serial.println("Update TFT: ");
  Serial.println(menueString);
  Serial.println(valueString);
  Serial.println();
#endif
}

void drawOptionColor(const char *menueString, uint32_t value, uint16_t color) {
  char  buf[11];
  sprintf(buf, "%u", value);
  drawOption(menueString, buf, color);
}

void drawOption(const char *menueString, uint32_t value) {
  char  buf[11];
  sprintf(buf, "%u", value);
  uint16_t color;
  if (value == 0) {
    color = TFT_GREEN;
  } else if (value == 1) {
    color = TFT_RED;
  }
  drawOption(menueString, buf, color);
}

void drawOption(const char *menueString, const char *valueString) {
  drawOption(menueString, valueString, (uint16_t) TFT_BLACK);
}

void drawOption(const char *menueString, const char *valueString, byte marker) {
  drawOption(menueString, valueString, (uint16_t) TFT_BLACK);
  int y = tft.width() / 2 + ((int)marker + 1) * 14 - 48;
  int x = tft.height() / 2 + 20;
  tft.drawLine(y, x, y + 5, x + 5, TFT_BLUE);
  tft.drawLine(y + 5, x + 5, y + 10, x, TFT_BLUE);

  tft.drawLine(y, x - 22, y + 5, x - 22 - 5, TFT_BLUE);
  tft.drawLine(y + 5, x - 22 - 5, y + 10, x - 22, TFT_BLUE);
}


void menueBool(bool &value, const char *text) {
  bool hasChangedTFT = true;
  bool tempValue = value;
  drawOption(text, (tempValue ? boolStrings[0] : boolStrings[1]), (uint16_t) (tempValue ? (uint16_t) TFT_GREEN : (uint16_t) TFT_RED));
#ifdef DEBUG_CONSOLE
  Serial.print("Menue einstellen: ");
  Serial.print(*text);
  Serial.println();
#endif
  delay(DELAY_ENTER_MENUE);
  while (true) {
    if (hasChangedTFT) {
      drawOption(text, (tempValue ? boolStrings[0] : boolStrings[1]), (uint16_t) (tempValue ? (uint16_t) TFT_GREEN : (uint16_t) TFT_RED));
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
      delay(1);
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
  bool hasChangedTFT = true;
  byte positionCounter = 0;
  drawOption(text, value, positionCounter);
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
      delay(DELAY_UPDATE_MENUE);
      hasChangedTFT = true;
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

void menueInt(uint16_t &value, const char *text) {
  bool hasChangedTFT = true;
  uint16_t tempValue = value;
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
      hasChangedTFT = true;
      tempValue++;
      delay(DELAY_UPDATE_MENUE);
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
      delay(DELAY_UPDATE_MENUE_VALUE);
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
      delay(DELAY_UPDATE_MENUE_VALUE);
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
      addRange(tempValue, 0, len);
      hasChangedTFT = true;
      delay(DELAY_UPDATE_MENUE);
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

void drawMenueV2(const char *text[], int len, int active) {
  //tft.fillScreen(TFT_BLACK);
  drawArrayJpeg(backGround, sizeof(backGround), 0, 0);
  tft.setTextWrap(false);
  int counterPoints = 0;
  int pointer = active;
  tft.setTextDatum(1);
  // Hauptpunkt
  drawArrayJpeg(buttonMenue, sizeof(buttonMenue), 0, tft.height() / 2 - 16);
  tft.setTextColor(TFT_BLACK);
  tft.drawString(text[active], tft.width() / 2, tft.height() / 2 - 12, 4);
  // 2
  drawArrayJpeg(buttonMenueM, sizeof(buttonMenueM), 32, tft.height() / 2 - (40));
  tft.setTextColor(TFT_WHITE);
  if (counterPoints >= len) {
    return;
  }
  if (active - 1 < 0) {
    pointer = len + active - 1;
  } else {
    pointer = active - 1;
  }
  tft.drawString(text[pointer], tft.width() / 2, tft.height() / 2 - (39), 2);
  counterPoints++;
  // Pinkt4
  drawArrayJpeg(buttonMenueM, sizeof(buttonMenueM), 32, tft.height() / 2 + (16));
  if (counterPoints >= len) {
    return;
  }
  if (active + 1 >= len) {
    pointer = active + 1 - len;
  } else {
    pointer = active + 1;
  }
  tft.drawString(text[pointer], tft.width() / 2, tft.height() / 2 + (17), 2);
  counterPoints++;
  // Punkte 1
  drawArrayJpeg(buttonMenueK, sizeof(buttonMenueK), 40, tft.height() / 2 - (60));
  tft.setTextColor(TFT_LIGHTGREY);
  if (counterPoints >= len) {
    return;
  }
  if (active - 2 < 0) {
    pointer = len + active - 2;
  } else {
    pointer = active - 2;
  }
  tft.drawString(text[pointer], tft.width() / 2, tft.height() / 2 - (57), 1);
  counterPoints++;
  // Pinkt5
  drawArrayJpeg(buttonMenueK, sizeof(buttonMenueK), 40, tft.height() / 2 + (42));
  if (counterPoints >= len) {
    return;
  }
  if (active + 2 >= len) {
    pointer = active + 2 - len;
  } else {
    pointer = active + 2;
  }
  tft.drawString(text[pointer], tft.width() / 2, tft.height() / 2 + (45), 1);
  counterPoints++;
  tft.setTextDatum(0);
}

void handleMenue() {
#ifdef DEBUG_CONSOLE
  Serial.println("Gehe ins erste Menue");
  Serial.println();
#endif
  byte menueCounter = 0;
  bool hasChangedTFT = true;
  drawMenueV2(menueStrings, menueStringsLen, menueCounter);
  delay(DELAY_ENTER_MENUE);
  while (true) {
    if (hasChangedTFT) {
#ifdef DEBUG_CONSOLE
      Serial.println("Update Menue 1");
      Serial.println();
#endif
      drawMenueV2(menueStrings, menueStringsLen, menueCounter);
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
