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
//#include "BLE.h"

// ############################################## Varriablen ##############################################
// ############################################## System/FreeRTOS

TaskHandle_t Task1, Task2;

// ############################################## Inputs
Input_State stateInput;
bool &menueButtonPresses = stateInput.menueButton;

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
    checkTimeout();
#ifdef DEBUG_CONSOLE
    Serial.println("Loop: Timeout");
#endif
    if (menueButtonPresses) {
      handleMenue();
      reloadTFT = true;
    } else {
      RFsend();
    }
#ifdef DEBUG_CONSOLE
    Serial.println("Loop: Bildschirm");
#endif
    // Hauptbildschirm Malen
    if (reloadTFT) {
      drawBlackAndTopLine();
      drawAkku(batteriespannung);
      drawWifi(configSet.webserverEnabled);
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
    delay(DELAY_LOOP);        // Delay to wait for packages;
    // Deine Display (15:x)
    if (configSet.recive) {
      RFrecive();
      drawRecive();
    }
  }
}

void loopCPU2( void * parameter )
{
  wlanVerbunden = false;
  if (!configSet.webserverEnabled) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    btStop();
    vTaskDelete(NULL);//  Kill this Task
  }
  WiFi.mode(WIFI_MODE_STA); // calls esp_wifi_set_mode(WIFI_MODE_STA); and esp_wifi_start();
  WiFi.enableSTA(true);
  // Http Zeug
  WiFiServer server(PORT_WEBSERVER);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(DELAY_WIFI_CONNECTION);
#ifdef DEBUG_CONSOLE
    Serial.println(".");
#endif
    if (!configSet.webserverEnabled) {
      server.stop();
      server = NULL;
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      btStop();
      vTaskDelete(NULL);//  Kill this Task
    }
  }
  wlanVerbunden = true;
  reloadTFT = true;
  // OTA
  ArduinoOTA.setHostname("RemoteControlESP32");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
  });

  ArduinoOTA.begin();


  server.begin();
#ifdef DEBUG_CONSOLE
  Serial.println("IP:");
  Serial.println(WiFi.localIP());
  Serial.println();
#endif
  String header;
  for (;;) {
#ifdef DEBUG_CONSOLE
    Serial.print("WiFi Status: ");
    Serial.println(WiFi.status());
#endif
    //handleBLE();
    if (!configSet.webserverEnabled) {
      server.stop();
      server = NULL;
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      btStop();
      vTaskDelete(NULL);//  Kill this Task
    }
    ArduinoOTA.handle();
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client) {                             // If a new client connects,
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected()) {            // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          header += c;
          if (c == '\n') {                    // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              // turns the GPIOs on and off

              if (header.indexOf("GET /menue") >= 0) {
#ifdef DEBUG_CONSOLE
                Serial.println("menueButtonPresses on");
#endif
                menueButtonPresses = true;
              } else if (header.indexOf("GET /up") >= 0) {
#ifdef DEBUG_CONSOLE
                Serial.println("stateInput.rightStickY on");
#endif
                stateInput.rightStickY = 1100;
              } else if (header.indexOf("GET /down") >= 0) {
#ifdef DEBUG_CONSOLE
                Serial.println("stateInput.rightStickY on");
#endif
                stateInput.rightStickY = 100;
              } else if (header.indexOf("GET /right") >= 0) {
#ifdef DEBUG_CONSOLE
                Serial.println("stateInput.rightStickX on");
#endif
                stateInput.rightStickX = 1100;
              } else if (header.indexOf("GET /left") >= 0) {
#ifdef DEBUG_CONSOLE
                Serial.println("stateInput.rightStickX on");
#endif
                stateInput.rightStickX = 100;
              } else {
                stateInput.rightStickX = 512;
                stateInput.rightStickY = 512;
                menueButtonPresses = false;
              }
              lastAction = millis();
              newWebInput = true;



              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head>");
              client.println("<title>ESP32 Fernbedinung</title>");
              client.println("</head>");

              // Web Page Heading
              client.println("<body><h1>Fernbedinung Web Server</h1>");
              client.println("<h2>Config</h2>");
              client.print("addrRfSend: ");
              client.println((int)configSet.addrRfSend);
              client.print("<br>addrRfRecive: ");
              client.println((int)configSet.addrRfRecive);
              client.print("<br>rfStaerke: ");
              client.println(configSet.rfStaerke);
              client.print("<br>webserverEnabled: ");
              client.println(configSet.webserverEnabled);
              client.print("<br>btEnabled: ");
              client.println(configSet.btEnabled);
              client.print("<br>recive: ");
              client.println(configSet.recive);
              client.print("<br>timeout: ");
              client.println(configSet.timeout);
              client.print("<br>validate: ");
              client.println(configSet.validate);

              client.println("<h2>Button State</h2>");
              client.print("leftStick: ");
              client.println(stateInput.leftStick);
              client.print("<br>rightStick: ");
              client.println(stateInput.rightStick);
              client.print("<br>left1: ");
              client.println(stateInput.left1);
              client.print("<br>left2: ");
              client.println(stateInput.left2);
              client.print("<br>switchTop: ");
              client.println(stateInput.switchTop);
              client.print("<br>leftStickX: ");
              client.println(stateInput.leftStickX);
              client.print("<br>leftStickY: ");
              client.println(stateInput.leftStickY);
              client.print("<br>rightStickX: ");
              client.println(stateInput.rightStickX);
              client.print("<br>rightStickY: ");
              client.println(stateInput.rightStickY);
              client.print("<br>menueButtonPresses: ");
              client.println(menueButtonPresses);

              client.println("</body></html>");
              client.println();
              break;
            } else { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        }
      }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
    }
  }
}

void loop() {
  // Dieser Task wird nicht benötigt und ist nicht konfiguriert, deswegen wird er zerstört
  vTaskDelete(NULL);//  Kill this Task
  delay(100);
}
