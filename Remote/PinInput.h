extern Config configSet;
extern Input_State stateInput;

// ############################################## Shiftregister
const PROGMEM byte dataArray[] = { 1, 2, 4, 8, 16, 32, 64, 128, 0};
const byte dataArrayLen = 31;
unsigned long lastShiftState = 0;

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

// ############################################## Shiftregister
unsigned long readShiftregister() {
  // Momentan nur 4 Register
  unsigned long tempState = 0;
  // Shift first HIGH BIT
  digitalWrite(PIN_SR_ST_CP, LOW);      // Start Writingmode
  digitalWrite(PIN_SR_SH_CP, LOW);      // clock to low
  digitalWrite(PIN_SR_DS, HIGH);        // Bite HIGH
  digitalWrite(PIN_SR_SH_CP, HIGH);     // Trigger Clock to store Bite
  digitalWrite(PIN_SR_SH_CP, LOW);      // Reset Clock
  digitalWrite(PIN_SR_ST_CP, HIGH);     // Ausgeben
  digitalWrite(PIN_SR_DS, LOW);         // Bite to LOW
  vTaskDelay(1);                        // wait to discharge Pins AND Reset WDT
  tempState += digitalRead(PIN_SR_INPUT);
  //Shift alle LOW BITs
  for (int j = 0; j < dataArrayLen; j++) {
    tempState = tempState << 1;         // Nächste stelle
    digitalWrite(PIN_SR_ST_CP, LOW);    // Start Writingmode
    digitalWrite(PIN_SR_SH_CP, HIGH);   // Trigger Clock to store Bite
    digitalWrite(PIN_SR_SH_CP, LOW);    // Reset Clock
    digitalWrite(PIN_SR_ST_CP, HIGH);   // Ausgeben
    vTaskDelay(1);                      // wait to discharge Pins AND Reset WDT
    tempState += digitalRead(PIN_SR_INPUT);
  }
  // Shift last Bit out
  digitalWrite(PIN_SR_ST_CP, LOW);    // Start Writingmode
  digitalWrite(PIN_SR_SH_CP, HIGH);   // Trigger Clock to store Bite
  digitalWrite(PIN_SR_SH_CP, LOW);    // Reset Clock
  digitalWrite(PIN_SR_ST_CP, HIGH);   // Ausgeben
  // Vergleiche Eingaben
  Serial.println(tempState);
  if (lastShiftState != tempState) {
    lastAction = millis();
    lastShiftState = tempState;
  }
  vTaskDelay(1);                      // wait to Reset WDT
#ifdef DEBUG_CONSOLE
  Serial.print("Input: ");
  Serial.println(tempState);
  Serial.print("Input changed: ");
  Serial.println((lastShiftState != tempState));
  Serial.println();
#endif
  return tempState;
}

void updateInputPins(unsigned long shiftStates) {
  stateInput.buttonStates = shiftStates;
  stateInput.leftStick = ((shiftStates & (1 << 31)) == (1 << 31));
  stateInput.rightStick = ((shiftStates & (1 << 30)) == (1 << 30));
  stateInput.menueButton = ((shiftStates & (1 << 29)) == (1 << 29));
  stateInput.left1 = ((shiftStates & (1 << 28)) == (1 << 28));
  stateInput.left2 = ((shiftStates & (1 << 27)) == (1 << 27));
  stateInput.right1 = ((shiftStates & (1 << 26)) == (1 << 26));
  stateInput.right2 = ((shiftStates & (1 << 25)) == (1 << 25));
  stateInput.switchTop = ((shiftStates & (1 << 24)) == (1 << 24));
}

void updateInput() {
  if (onlyOnline) {
    while (!newWebInput) {
      delay(10);
    }
    newWebInput = false;
  } else {
    updateInputPins(readShiftregister());
    int tempAnalog = analogRead(PIN_STICK_LX);
    if (tempAnalog != stateInput.leftStickX) {
      if (abs(tempAnalog - (int) stateInput.leftStickX) > MIN_ANALOG_DIFF ) {
        lastAction = millis();
      }
      stateInput.leftStickX = tempAnalog;
    }
    tempAnalog = analogRead(PIN_STICK_LY);
    if (tempAnalog != stateInput.leftStickY) {
      if (abs(tempAnalog - (int) stateInput.leftStickY) > MIN_ANALOG_DIFF ) {
        lastAction = millis();
      }
      stateInput.leftStickY = tempAnalog;
    }
    tempAnalog = analogRead(PIN_STICK_RX);
    if (tempAnalog != stateInput.rightStickX) {
      if (abs(tempAnalog - (int) stateInput.rightStickX) > MIN_ANALOG_DIFF ) {
        lastAction = millis();
      }
      stateInput.rightStickX = tempAnalog;
    }
    tempAnalog = analogRead(PIN_STICK_RY);
    if (tempAnalog != stateInput.rightStickY) {
      if (abs(tempAnalog - (int) stateInput.rightStickY) > MIN_ANALOG_DIFF ) {
        lastAction = millis();
      }
      stateInput.rightStickY = tempAnalog;
    }
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
  pinMode(PIN_SR_ST_CP, OUTPUT);
  pinMode(PIN_SR_SH_CP, OUTPUT);
  pinMode(PIN_SR_DS, OUTPUT);
  pinMode(PIN_MOSFET, OUTPUT);
  pinMode(PIN_SR_INPUT, INPUT);
  enablePower(true);
  // Analog
  //analogSetSamples(5);
  //analogSetCycles(200);
  //analogSetClockDiv(200);
  analogReadResolution(10);
}
