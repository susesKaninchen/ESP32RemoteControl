#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

extern boolean reloadTFT;
char buff[200];

byte wifiState = 0;
AsyncWebServer server(80);
//WiFiServer server(PORT_WEBSERVER);
WiFiManager wm;

char* stateAsJson() {
  /*
   * TODO: Status wie Akku, Adressen Stati und Antwort vom Bot hinzufügen
    unsigned long timestamp = 0;
    char akku = 190;
    int userValue_1 = 0;
    int userValue_2 = 0;
    char string[25];
    int validate = validateNumber;*/
  sprintf(buff, "{\"leftStick\": %u, \"rightStick\": %u, \"left1\": %u, \"left2\": %u, \"right1\": %u, \"right2\": %u, \"switchTop\": %u, \"menueButton\": %u, \"leftStickX\": %u, \"leftStickY\": %u, \"rightStickX\": %u, \"rightStickY\": %u}", stateInput.leftStick, stateInput.rightStick, stateInput.left1, stateInput.left2, stateInput.right1, stateInput.right2, stateInput.switchTop, stateInput.menueButton, stateInput.leftStickX, stateInput.leftStickY, stateInput.rightStickX, stateInput.rightStickY);
  return buff;
}

void setWifiState(byte state) {
  wifiState = state;
  reloadTFT = true;
}

void configModeCallback (WiFiManager *myWiFiManager) {
  setWifiState(3);
}

void initWifi() {
  if (!configSet.webserverEnabled) {
    setWifiState(0);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    btStop();
    vTaskDelete(NULL);//  Kill this Task
  }
  setWifiState(2);
  WiFi.mode(WIFI_STA); // calls esp_wifi_set_mode(WIFI_MODE_STA); and esp_wifi_start();
  WiFi.enableSTA(true);
  // Http Zeug

  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect("Fernbedienung")) {
    setWifiState(0);
    configSet.webserverEnabled = false;
    //server.stop();
    //server = NULL;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    btStop();
    vTaskDelete(NULL);//  Kill this Task
  }
  setWifiState(1);
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

#ifdef DEBUG_CONSOLE
  Serial.println("IP:");
  Serial.println(WiFi.localIP());
  Serial.println();
#endif
  if (!SPIFFS.begin()) {
#ifdef DEBUG_CONSOLE
    Serial.println("SPIFFS Mount Failed");
#endif
  }


  // ---------- Webserver ---------
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html");
  });
  server.on("/state", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", stateAsJson());
  });
  server.begin();
}

void handleWifi() {
  String header;
#ifdef DEBUG_CONSOLE
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());
#endif
  //handleBLE();
  if (!configSet.webserverEnabled) {
    setWifiState(3);
    //server.stop();
    //server = NULL;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    btStop();
    vTaskDelete(NULL);//  Kill this Task
  }
  ArduinoOTA.handle();
  /*
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
              stateInput.menueButton = true;
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
              stateInput.menueButton = false;
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
            client.println(stateInput.menueButton);

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
    }*/
}
