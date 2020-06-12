//#define DEBUG_CONSOLE        // Deaktiviert die Console WIRD EMPFOHLEN ZU DEAKTIVIEREN
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

extern void reactToPackage();
extern void reactToTimeout();

// Structs and min Values
int missingPackages = 0;
int validateNumber = 56985;         // -> Diese Nummer stellt sicher, das das Empfangene Paket auch zu MEINER Fernbedienung gehört. Muss also mit der in der Fernbedienung übereinstimmen
char addrRfSend[6] = "00001";
char addrRfRecive[6] = "00000";

unsigned long timeout = millis();
unsigned int timeoutDiff = 100;

// Bitte nicht verändern, außer sie wird auch in der Fernbedineung verändert.
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
  unsigned long buttonStates = 0;
  unsigned int leftStickX = 512;
  unsigned int leftStickY = 512;
  unsigned int rightStickX = 512;
  unsigned int rightStickY = 512;
  int validate = validateNumber;
}
Input_State;

// Diese struct muss 100% mit dem Struct in der Fernbedienung übereinstimmen 100%
typedef struct
{
  unsigned long timestamp = 0;
  char akku = 190;
  int userValue_1 = 0;
  int userValue_2 = 0;
  char string[25];
  int validate = validateNumber;
  // Alles was ihr so empfangen wollt, muss aber auch bei Fernbedienung angepasst werden
}
Send_Package;

#define RADIO_CE 5
#define RADIO_CS 4
RF24 radio(RADIO_CE, RADIO_CS);
Input_State recivePackage;
Send_Package sendPackage;

void reciveSetup() {
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
  timeout = millis();
  pinMode(2, OUTPUT);
}


void reciveLoop() {
  if (millis() - timeout >= timeoutDiff) {
    // Pakete verloren gegangen
#ifdef DEBUG_CONSOLE
    Serial.print("Timeout: ");
    Serial.print(millis());
    Serial.print(" - ");
    Serial.print(timeout);
    Serial.print(" >=" );
    Serial.println(timeoutDiff);
#endif
    missingPackages++;
    reactToTimeout();
    timeout = millis();
    digitalWrite(2, LOW);   // turn the LED off
  }
  // Empfangen
  if (radio.available()) {
    radio.read( &recivePackage, sizeof(Input_State) );
    if (recivePackage.validate == validateNumber) {
      reactToPackage();
      timeout = millis();
      digitalWrite(2, HIGH);   // turn the LED on
      missingPackages = 0;
#ifdef DEBUG_CONSOLE
      //Serial.println("empfangen");
      Serial.println("Paket empfangen: ");
      Serial.print("leftStickY: ");
      Serial.println(recivePackage.leftStickY);
      Serial.print("leftStickX: ");
      Serial.println(recivePackage.leftStickX);
      Serial.print("rightStickY: ");
      Serial.println(recivePackage.rightStickY);
      Serial.print("rightStickX: ");
      Serial.println(recivePackage.rightStickX);
      Serial.print("menueButton: ");
      Serial.println(recivePackage.menueButton);
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
      Serial.print("validate: ");
      Serial.println(recivePackage.validate);
#endif
      // Senden Back
      radio.stopListening();
      radio.write(&sendPackage, sizeof(Send_Package));
      radio.startListening();
    } else {
      missingPackages++;
    }
  }
}
