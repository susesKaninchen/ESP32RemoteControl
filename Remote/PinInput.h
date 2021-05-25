extern Config configSet;
extern Input_State stateInput;

// ############################################## Power MOSFET
long lastAction = millis();
float batteriespannung = 0;

// Enable Mosfet
void enablePower(bool enable) {
#ifdef DEBUG_CONSOLE
  Serial.print("Schlate Strom: ");
  Serial.println(enable);
  Serial.println();
#endif
  if (enable) {
    digitalWrite(PIN_MOSFET, HIGH);       // Enable Power
  } else {
    digitalWrite(PIN_MOSFET, LOW);       // Disable Power
    esp_deep_sleep_start();
  }
}

// Read Batterie
bool readAkku() {
  float r1 = 100000.0;
  float r2 = 10000.0;
  int messwehrt = analogRead(PIN_AKKU_VOLTAGE);
  float vout = (messwehrt * 3.3) / 1024.0;
  float newBatSpan = vout / (r2 / (r1 + r2));
  bool batteriespannungChanged = (newBatSpan != batteriespannung);
  batteriespannung = newBatSpan;
#ifdef DEBUG_CONSOLE
  Serial.print("Lese Akku: ");
  Serial.println(batteriespannung);
  Serial.println();
#endif
  return batteriespannungChanged;
}

void checkTimeout() {
#ifdef DEBUG_CONSOLE
  Serial.println("Teste ob Timeout (Abschalten wegen inaktivität): ");
  Serial.print(lastAction);
  Serial.print("+");
  Serial.print(configSet.timeout * 1000);
  Serial.print("<");
  Serial.print(millis());
  Serial.print(" -> ");
  Serial.println((lastAction + (configSet.timeout * 1000) < millis()));
  Serial.println();
#endif
  if (lastAction + (configSet.timeout * 1000) < millis()) {
    enablePower(false);
  }
}

void readAttiny(){
  
}

void updateInput() {
  if (onlyOnline) {
    while (!newWebInput) {
      delay(10);
    }
    newWebInput = false;
  } else {
    readAttiny();
    //TODO: Alle Inpus definnieren.
    /*int tempAnalog = analogRead(PIN_STICK_LX);
    if (tempAnalog != stateInput.leftStickX) {
      if (abs(tempAnalog - (int) stateInput.leftStickX) > MIN_ANALOG_DIFF ) {
        lastAction = millis();
      }
      stateInput.leftStickX = tempAnalog;
    }*/
  }
#ifdef DEBUG_CONSOLE
  Serial.println("Inputs:");
  Serial.print("leftStick: ");
  Serial.println(stateInput.leftStick);
  Serial.print("rightStick: ");
  Serial.println(stateInput.rightStick);
  Serial.print("left1: ");
  Serial.println(stateInput.left1);
  Serial.print("left2: ");
  Serial.println(stateInput.left2);
  Serial.print("switchTop: ");
  Serial.println(stateInput.switchTop);
  Serial.print("leftStickX: ");
  Serial.println(stateInput.leftStickX);
  Serial.print("leftStickY: ");
  Serial.println(stateInput.leftStickY);
  Serial.print("rightStickX: ");
  Serial.println(stateInput.rightStickX);
  Serial.print("rightStickY: ");
  Serial.println(stateInput.rightStickY);
  Serial.print("stateInput.menueButton: ");
  Serial.println(stateInput.menueButton);
  Serial.println();
#endif
  vTaskDelay(1);
  //delay(1); //WDT Reset
}

void initPins() {
  // Pins
  pinMode(PIN_MOSFET, OUTPUT);
  enablePower(true);
  // Analog
  //analogSetSamples(5);
  //analogSetCycles(200);
  //analogSetClockDiv(200);
  analogReadResolution(10);
}
