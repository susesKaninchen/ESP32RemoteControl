// ############################################## Pin DEFINES ##############################################
//https://docs.google.com/spreadsheets/d/1cvpiqykm8GjEDwQNzvs4OwQ_SFqLQqxTZ7avuxnM_1c/edit
// ############################################## Mosfet Enable
#define PIN_MOSFET 22
// ############################################## TFT
/*
   ESP32      TFT
   3V3        VCC
   GND        GND
   D4/CS      CS
   D16        RESET
   D17        A0
   D23/MOSI   SDA/MOSI
   D18/CLK    SCK/CLK
   3V3        LED
*/
//#define TFT_CS         4 -> In Libary deffiniert
//#define TFT_RST        16-> In Libary deffiniert
//#define TFT_DC         17-> In Libary deffiniert

// ############################################## Antenne DEFINES
#define RADIO_CE 21
#define RADIO_CS 5

// ############################################## Shiftregister DEFINES
#define PIN_SR_ST_CP 26
#define PIN_SR_SH_CP 25
#define PIN_SR_DS 27
#define PIN_SR_INPUT 2
// ############################################## Analoge DEFINES
#define PIN_STICK_RX 34
#define PIN_STICK_RY 35
#define PIN_STICK_LX 36
#define PIN_STICK_LY 39
// ############################################## Akku
#define PIN_AKKU_VOLTAGE 32
