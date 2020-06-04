#include "ReciverFunctions.h"

void setup() {
  reciveSetup();
}

void loop() {
  reciveLoop();
  // Bitte nicht länger als 50ms Brauchen. Sonst gehen Pakete verloren
  sendPackage.timestamp++;
  sendPackage.akku = random(100);
  sendPackage.userValue_1 = 100;
  sendPackage.userValue_2 = 56900;
  strncpy(sendPackage.string, "Hallo, ein Text", sizeof(sendPackage.string));// Maximal 24 Zeichen (eine Zeile gerade)
}
