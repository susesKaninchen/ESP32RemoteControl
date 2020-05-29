#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

extern Config configSet;
extern Input_State stateInput;
extern Reciv_Package recivPackage;
extern int validateNumber;
RF24 radio(RADIO_CE, RADIO_CS);
int missingPackages = 0;

// ############################################## Antenne

void RFsend() {
#ifdef DEBUG_CONSOLE 
  Serial.println();
  Serial.println("Sende Paket->");
  Serial.println();
#endif
  radio.stopListening();
  radio.write(&stateInput, sizeof(stateInput));
  if (configSet.recive) {
    radio.startListening();
  }
}

void RFrecive() {
  if (radio.available()) {
    radio.read( &recivPackage, sizeof(Reciv_Package) );
    #ifdef DEBUG_CONSOLE
    Serial.println("Paket empfangen: ");
    Serial.println(recivPackage.timestamp);
    Serial.println((int)recivPackage.akku);
    Serial.println(recivPackage.validate);
    #endif
    if (recivPackage.validate == validateNumber) {
      missingPackages = 0;
    } else {
      missingPackages++;
    }
  } else {
    missingPackages++;
  }

}

/*
// Unnötig
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
}*/

void RFchangeAddresseSend() {
#ifdef DEBUG_CONSOLE
  Serial.print("Antenne wechsle sende Adresse: ");
  Serial.println(configSet.addrRfSend);
  Serial.println();
#endif
  radio.stopListening();
  byte adressTemp[6];
  for (int dd = 0; dd < 6; dd++) {
    adressTemp[dd] = (byte) configSet.addrRfSend[dd];
  }
  radio.openWritingPipe(adressTemp);
}

void RFchangeAddresseReciv() {
#ifdef DEBUG_CONSOLE
  Serial.print("Antenne wechsle empfangs Adresse: ");
  Serial.println(configSet.addrRfRecive);
  Serial.println();
#endif
  radio.stopListening();
  byte adressTemp[6];
  for (int dd = 0; dd < 6; dd++) {
    adressTemp[dd] = (byte) configSet.addrRfRecive[dd];
  }
  radio.openReadingPipe(1,adressTemp);
  radio.startListening();
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
  #ifdef DEBUG_CONSOLE
  Serial.print("Antenne angeschlossen: ");
  Serial.println(radio.isChipConnected());
  #endif
  RFchangeLevel();
  RFchangeAddresseSend();
  if (configSet.recive) {
    RFchangeAddresseReciv();
  }
}
