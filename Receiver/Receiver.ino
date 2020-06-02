#define DEBUG_CONSOLE        // Deaktiviert die Console WIRD EMPFOHLEN ZU DEAKTIVIEREN
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Structs and min Values
int missingPackages = 0;
int validateNumber = 56985;
char addrRfSend[6] = "00001";
char addrRfRecive[6] = "00000";
typedef struct
{
  bool leftStick = 0;
  bool rightStick = 0;
  bool left1 = 0;
  bool left2 = 0;
  bool right1 = 0;
  bool right2 = 0;
  bool switchTop = 0;
  bool menueButton = 0;
  unsigned int leftStickX = 512;
  unsigned int leftStickY = 512;
  unsigned int rightStickX = 512;
  unsigned int rightStickY = 512;
  int validate = validateNumber;
}
Input_State;

typedef struct
{
  unsigned long timestamp = 0;
  char akku = 190;
  int validate = validateNumber;
  // Alles was ihr so empfangen wollt, muss aber auch bei Fernbedienung angepasst werden
}
Send_Package;

#define RADIO_CE 5
#define RADIO_CS 4
RF24 radio(RADIO_CE, RADIO_CS);
Input_State recivePackage;
Send_Package sendPackage;

void setup() {
#ifdef DEBUG_CONSOLE
  Serial.begin(115200);
  Serial.println("Starte Programm und deffiniere Pins");
#endif
  radio.begin();                       // Initialisirung des Senders
#ifdef DEBUG_CONSOLE
  Serial.println(radio.isChipConnected());
#endif
  radio.setPALevel(RF24_PA_MIN);       // Sendestärke auf LOW setzen, umw eniger störungen in Räumen zu bekommen
  radio.stopListening();
  byte adressTemp[6];
  for (int dd = 0; dd < 6; dd++) {
    adressTemp[dd] = (byte) addrRfSend[dd];
  }
  radio.openWritingPipe(adressTemp);
  radio.stopListening();
  adressTemp[6];
  for (int dd = 0; dd < 6; dd++) {
    adressTemp[dd] = (byte) addrRfRecive[dd];
  }
  radio.openReadingPipe(1, adressTemp);
  radio.startListening();
}


void loop() {
  // Empfangen
  if (radio.available()) {
    radio.read( &recivePackage, sizeof(Input_State) );
    if (recivePackage.validate == validateNumber) {
      missingPackages = 0;
#ifdef DEBUG_CONSOLE
/*
      Serial.println("empfangen");
      Serial.println("Paket empfangen: ");
      Serial.println(recivePackage.leftStickY);
      Serial.println(recivePackage.leftStickX);
      Serial.println(recivePackage.validate);
      Serial.println("...");*/
      Serial.print("leftStick: ");
      Serial.println(recivePackage.leftStick);
      Serial.print("rightStick: ");
      Serial.println(recivePackage.rightStick);
      Serial.print("left1: ");
      Serial.println(recivePackage.left1);
      Serial.print("left2: ");
      Serial.println(recivePackage.left2);
      Serial.print("right1: ");
      Serial.println(recivePackage.right1);
      Serial.print("right2: ");
      Serial.println(recivePackage.right2);
      Serial.print("switchTop: ");
      Serial.println(recivePackage.switchTop);
#endif
      // Senden Back Akku
      radio.stopListening();
      sendPackage.timestamp++;
      radio.write(&sendPackage, sizeof(Send_Package));
      radio.startListening();
    } else {
      missingPackages++;
    }
  }
}
