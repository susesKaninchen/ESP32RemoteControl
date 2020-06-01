// Einfach auskommentieren, wenn ihr etwas deaktivieren wollt
#define DEBUG_CONSOLE				// Deaktiviert die Console WIRD EMPFOHLEN ZU DEAKTIVIEREN
//#define ONLY_WEB					// Aktiviert = Deaktiviert die Inputs an der Fernbedienung

// ############################################## EEPROM
#define EEPROM_SIZE 64

// ############################################## Compiler DEFINES ##############################################
#ifdef ONLY_WEB
bool onlyOnline = true;
#else
bool onlyOnline = false;
#endif
bool newWebInput = false;

#define DELAY_LOOP 30 //ms
#define DELAY_TASK 500 //ms
#define DELAY_WIFI_CONNECTION 100 //ms
#define DELAY_ENTER_MENUE 1000
#define DELAY_UPDATE_MENUE 600
#define DELAY_UPDATE_MENUE_VALUE 10

#define PORT_WEBSERVER 80

// Muss noch geändert werden, ich habe an einen Wifi Speicher gedacht oder die nutzung vom Wifimanager
const char* ssid     = "Seewald";
const char* password = "nicoleundgina";
