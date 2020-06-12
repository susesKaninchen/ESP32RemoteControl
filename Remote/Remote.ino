#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "PinDefines.h"
#include "config.h"
#include "structs.h"
#include "EepromConfig.h"
#include "RF.h"
#include "displayV2.h"
#include "PinInput.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "WLANhandle.h"
//#include "BLE.h"

// ############################################## Varriablen ##############################################
// ############################################## System/FreeRTOS

TaskHandle_t Task1, Task2;

// ############################################## Inputs
Input_State stateInput;

Reciv_Package recivPackage;

// ############################################## Config
Config configSet;

bool reloadTFT = true;

// ############################################## Funktionen ##############################################
// ############################################## Menue/Display

void menueBT() {
  menueBool(configSet.btEnabled, menueStrings[5]);
}
void menueTimeout() {
  menueInt(configSet.timeout, menueStrings[6]);
}

void menueWebServer() {
  bool tempBool = configSet.webserverEnabled;
  menueBool(configSet.webserverEnabled, menueStrings[4]);
  if (configSet.webserverEnabled && tempBool != configSet.webserverEnabled) {
    xTaskCreatePinnedToCore(loopCPU2, "loopCPU2", 10000, NULL, 5, &Task2, 1);
  }
}

void menueRFpower() {
  menueArray(configSet.rfStaerke, menueStrings[3], powerLvlStrings, 4);
  RFchangeLevel();
}

void menueRfSendAdresse() {
  menueString(configSet.addrRfSend, menueStrings[0]);
  RFchangeAddresseSend();
}

void menueRfReciveAdresse() {
  menueString(configSet.addrRfRecive, menueStrings[1]);
  RFchangeAddresseReciv();
}

void menueRFrecive() {
  menueBool(configSet.recive, menueStrings[2]);
  saveConfig();
  //RFchangeRecive();
}


// ############################################## Programm ##############################################

void setup() {
#ifdef DEBUG_CONSOLE
  Serial.begin(115200);
  Serial.println("Starte Programm und deffiniere Pins");
  Serial.println();
#endif
  initPins();
  initEEPROM();
  loadConfig();
  RFinit();
  initTft();
  //
  //initBLE();

  //                      loop Function, Name, Stack size in words, Inputparameter, Prioretx (-1 best 0 lowest), task Handle, Core (0,1)
  xTaskCreatePinnedToCore(loopCPU1, "loopCPU1", 10000, NULL, -1, &Task1, 0);
  delay(DELAY_TASK);  // needed to start-up task1
  xTaskCreatePinnedToCore(loopCPU2, "loopCPU2", 10000, NULL, 5, &Task2, 1);
}

void loopCPU1( void * parameter )
{

  for (;;) {
#ifdef DEBUG_CONSOLE
    Serial.println("Loop ..");
    Serial.println();
#endif
    updateInput();
#ifdef DEBUG_CONSOLE
    Serial.println("Loop: Input");
#endif
    RFsend();
#ifdef DEBUG_CONSOLE
    Serial.println("Loop: Send Package");
#endif
    checkTimeout();
#ifdef DEBUG_CONSOLE
    Serial.println("Loop: Timeout");
#endif
    if (stateInput.menueButton) {
      handleMenue();
      reloadTFT = true;
    }
#ifdef DEBUG_CONSOLE
    Serial.println("Loop: Bildschirm");
#endif
    // Hauptbildschirm Malen
    if (reloadTFT) {
      drawBlackAndTopLine();
      drawAkku(batteriespannung);
      drawWifi(wifiState);
      drawBT(configSet.btEnabled);
      drawAdresse(configSet.addrRfSend, configSet.addrRfRecive);
      if (!configSet.recive) {
        drawNoRecive();
      }
      reloadTFT = false;
    }
    if (readAkku()) {
      drawAkku(batteriespannung);
    }
    if (configSet.recive) {
      RFrecive();
#ifdef DEBUG_CONSOLE
      Serial.println("Loop: CheckRecive and Draw");
#endif
      drawRecive();
    }
    delay(DELAY_LOOP);        // Delay to wait for packages;
  }
}

void loopCPU2( void * parameter )
{
  initWifi();
  for (;;) {
    handleWifi();
  }
}

void loop() {
  // Dieser Task wird nicht benötigt und ist nicht konfiguriert, deswegen wird er zerstört
  vTaskDelete(NULL);//  Kill this Task
  delay(100);
}
