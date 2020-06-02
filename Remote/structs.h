// ############################################## Inputs

int validateNumber = 56985; // Diese nummer muss mit dem Empfänger übereinstimmen!!

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
  char akku = 0;
  int validate = validateNumber;
  // Alles was ihr so empfangen wollt
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
  uint16_t timeout = 60;// Sekunden
  int validate = 4557;
}
Config;
