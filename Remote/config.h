// ############################################## Compiler DEFINES ##############################################
// Einfach auskommentieren, wenn ihr etwas deaktivieren wollt
//#define DEBUG_CONSOLE				// Deaktiviert die Console WIRD EMPFOHLEN ZU DEAKTIVIEREN
//#define ONLY_WEB					// Aktiviert = Deaktiviert die Inputs an der Fernbedienung

// ############################################## EEPROM
#define EEPROM_SIZE 64

// ############################################## DEFINES ##############################################
#ifdef ONLY_WEB
bool onlyOnline = true;
#else
bool onlyOnline = false;
#endif
bool newWebInput = false;

// Wenn die Analogen eingänge mehr als diesen wert abweichen wird die AFK Schwelle getriggert
#define MIN_ANALOG_DIFF 60

#define DELAY_LOOP 50 //ms
#define DELAY_TASK 1000 //ms
#define DELAY_WIFI_CONNECTION 200 //ms
#define DELAY_ENTER_MENUE 500
#define DELAY_UPDATE_MENUE 400
#define DELAY_UPDATE_MENUE_VALUE 100

#define PORT_WEBSERVER 80

// Muss noch geändert werden, ich habe an einen Wifi Speicher gedacht oder die nutzung vom Wifimanager
const char* ssid     = "Seewald";
const char* password = "nicoleundgina";
