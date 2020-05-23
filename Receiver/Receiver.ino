#define DEBUG_CONSOLE        // Deaktiviert die Console WIRD EMPFOHLEN ZU DEAKTIVIEREN
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Structs and min Values
int missingPackages = 0;
int validateNumber = 56985;
char addrRfSend[6] = "Adres";
char addrRfRecive[6] = "adres";
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
  char akku = 100;
  int validate = validateNumber;
  // Alles was ihr so empfangen wollt
}
Send_Package;

#define RADIO_CE 3
#define RADIO_CS 4
RF24 radio(RADIO_CE, RADIO_CS);
Input_State recivePackage;
Send_Package sendPackage;

void setup() {
#ifdef DEBUG_CONSOLE
  Serial.begin(115200);
  Serial.println("Starte Programm und deffiniere Pins");
  Serial.println();
#endif
  radio.begin();                       // Initialisirung des Senders
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
    } else {
      missingPackages++;
    }
    // Senden Back Akku
    radio.stopListening();
    radio.write(&sendPackage, sizeof(Send_Package));
    radio.startListening();
  }
}
