extern Config configSet;
extern Input_State stateInput;

// ############################################## Shiftregister
const PROGMEM byte dataArray[] = { 0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001};
byte lastShiftState = 0;

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
  Serial.print(configSet.timeout);
  Serial.print("<");
  Serial.print(millis());
  Serial.print(" -> ");
  Serial.println((lastAction + configSet.timeout < millis()));
  Serial.println();
#endif
  if (lastAction + configSet.timeout < millis()) {
    enablePower(false);
  }
}

// ############################################## Shiftregister
byte readShiftregister() {
  byte tempState = 0;
  for (int j = 0; j < 8; j++) {
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(PIN_SR_ST_CP, LOW);
    //move 'em out
    shiftOut(PIN_SR_DS, PIN_SR_SH_CP, LSBFIRST , (dataArray[j]));
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(PIN_SR_ST_CP, HIGH);
    tempState += (digitalRead(PIN_SR_INPUT) * dataArray[j]);
  }
  if (lastShiftState != tempState) {
    lastAction = millis();
  }
#ifdef DEBUG_CONSOLE
  Serial.print("Input: ");
  Serial.println(tempState);
  Serial.print("Input changed: ");
  Serial.println((lastShiftState != tempState));
  Serial.println();
#endif
  return tempState;
}

void updateInput() {
  if (onlyOnline) {
    while (!newWebInput) {
      delay(10);
    }
    newWebInput = false;
  } else {
    byte digitalInputs = readShiftregister();
    stateInput.leftStick = (digitalInputs & dataArray[0] == dataArray[0]);
    stateInput.rightStick = (digitalInputs & dataArray[1] == dataArray[1]);
    stateInput.menueButton = (digitalInputs & dataArray[2] == dataArray[2]);
    stateInput.left1 = (digitalInputs & dataArray[3] == dataArray[3]);
    stateInput.left2 = (digitalInputs & dataArray[4] == dataArray[4]);
    stateInput.right1 = (digitalInputs & dataArray[5] == dataArray[5]);
    stateInput.right2 = (digitalInputs & dataArray[6] == dataArray[6]);
    stateInput.switchTop = (digitalInputs & dataArray[7] == dataArray[7]);
    int tempAnalog = analogRead(PIN_STICK_LX);
    if (tempAnalog != stateInput.leftStickX) {
      stateInput.leftStickX = tempAnalog;
      lastAction = millis();
    }
    tempAnalog = analogRead(PIN_STICK_LY);
    if (tempAnalog != stateInput.leftStickY) {
      stateInput.leftStickY = tempAnalog;
      lastAction = millis();
    }
    tempAnalog = analogRead(PIN_STICK_RX);
    if (tempAnalog != stateInput.rightStickX) {
      stateInput.rightStickX = tempAnalog;
      lastAction = millis();
    }
    tempAnalog = analogRead(PIN_STICK_RY);
    if (tempAnalog != stateInput.rightStickY) {
      stateInput.rightStickY = tempAnalog;
      lastAction = millis();
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
}

void initPins() {
  // Pins
  pinMode(PIN_MOSFET, OUTPUT);
  pinMode(PIN_SR_INPUT, INPUT);
  enablePower(true);
  // Analog
  analogSetSamples(5);
  analogReadResolution(10);
}