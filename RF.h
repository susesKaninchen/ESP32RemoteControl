#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

extern Config configSet;
extern Input_State stateInput;
RF24 radio(RADIO_CE, RADIO_CS);

// ############################################## Antenne

void RFsend() {
#ifdef DEBUG_CONSOLE
  Serial.println();
  Serial.println("Sende Paket->");
  Serial.println();
#endif
  radio.write(&stateInput, sizeof(stateInput));
}

void RFchangeRecive() {
#ifdef DEBUG_CONSOLE
  Serial.print("Antenne empfangen: ");
  Serial.println(configSet.recive);
  Serial.println();
#endif
  if (configSet.recive) {
    radio.startListening();
  } else {
    radio.stopListening();
  }
}

void RFchangeAddresse() {
#ifdef DEBUG_CONSOLE
  Serial.print("Antenne wechsle Adresse: ");
  Serial.println((int)configSet.addrRF);
  Serial.println();
#endif
  radio.stopListening();
  radio.openWritingPipe(configSet.addrRF);
}

void RFchangeLevel() {
#ifdef DEBUG_CONSOLE
  Serial.println("Antenne Stelle stärke ein: (0 = min, [1 = LOW], 2 = MED, 3 = HIGH), <3 = LOW");
  Serial.println(configSet.rfStaerke);
  Serial.println();
#endif
  if (configSet.rfStaerke == 0) {
    radio.setPALevel(RF24_PA_MIN);       // Sendestärke auf LOW setzen, umw eniger störungen in Räumen zu bekommen
  } else if (configSet.rfStaerke == 1) {
    radio.setPALevel(RF24_PA_LOW);
  } else if (configSet.rfStaerke == 2) {
    radio.setPALevel(RF24_PA_HIGH);
  } else if (configSet.rfStaerke == 3) {
    radio.setPALevel(RF24_PA_MAX);
  } else {
    radio.setPALevel(RF24_PA_LOW);
  }
}

void RFinit() {
#ifdef DEBUG_CONSOLE
  Serial.println("Starte Antenne: ");
  Serial.println();
#endif
  radio.begin();                       // Initialisirung des Senders
  RFchangeLevel();
  RFchangeAddresse();
}