#include "ReciverFunctions.h"

void setup() {
  reciveSetup();
}

void loop() {
  reciveLoop();
  // Bitte nicht länger als 50ms Brauchen. Sonst gehen Pakete verloren

  //--------- Beispiel Anfang----------
  sendPackage.timestamp++;
  sendPackage.akku = random(100);
  sendPackage.userValue_1 = 100;
  sendPackage.userValue_2 = 56900;
  // Maximal 24 Zeichen (eine Zeile gerade)
  strncpy(sendPackage.string, "Hallo, ein Text", sizeof(sendPackage.string));
  //--------- Beispiel Ende----------
}

void reactToPackage() {
  // Hier bitte den Code einfügen
  //, auf den reagiert werden soll, wenn neue Daten rein gekommen sind
  // Gerne auch, auf den Menue Button reagieren um zu sehen, wann die Fernbedinung nichts mehr senden wird.
  /*bool leftStick = 0;
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
  int validate = validateNumber;*/
}

void reactToTimeout() {
  // Hier code, der auf einen Timeout reagieren soll einfügen
  // Achtung, wenn der Menue Button gedrückt wird kommt ein Timeout, aber der Menue Button ist TRUE
}
