// ############################################## Inputs

int validateNumber = 56986; // Diese nummer muss mit dem Empfänger übereinstimmen!!
const int validateConfig = 56786;

typedef struct
{
  byte menue = 0;
  byte horizontal = 127;
  byte vertikal = 127;
}
Menue_State;

typedef struct
{
  byte PA0 = 0;
  byte PA1 = 0;
  byte PA2 = 0;
  byte PA3 = 0;
  byte PA4 = 0;
  byte PA5 = 0;
  byte PA6 = 0;
  byte PA7 = 0;

  byte PB0 = 0;
  byte PB1 = 0;
  byte PB2 = 0;
  byte PB3 = 0;
  byte PB4 = 0;
  byte PB5 = 0;
  byte PB6 = 0;
  byte PB7 = 0;

  byte PC0 = 0;
  byte PC1 = 0;
  byte PC4 = 0;
  byte PC5 = 0;
  byte PC6 = 0;
  byte PC7 = 0;

  byte PD0 = 0;
  byte PD1 = 0;
  byte PD2 = 0;
  byte PD3 = 0;
  
  int validate = validateNumber;
}
Input_State;

typedef struct
{
  unsigned long timestamp = 0;
  char akku = 190;
  int userValue_1 = 0;
  int userValue_2 = 0;
  char string[25] = "-";
  int validate = validateNumber;
  // Alles was ihr so empfangen wollt, muss aber auch bei Fernbedienung angepasst werden
}
Reciv_Package;

// ############################################## Config
typedef struct
{
  char addrRfSend[6] = "00000";
  char addrRfRecive[6] = "00001";
  byte rfStaerke = 1;
  bool webserverEnabled = true;
  bool btEnabled = false;
  bool recive = true;
  uint16_t timeout = 120;// Sekunden
  int validate = validateConfig;
}
Config;
