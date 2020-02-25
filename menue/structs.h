// ############################################## Inputs

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
}
Input_State;

// ############################################## Config
typedef struct
{
  uint32_t addrRF = 1256354;
  byte rfStaerke = 1;
  bool webserverEnabled = true;
  bool btEnabled = false;
  bool recive = false;
  uint32_t timeout = 5 * 60 * 1000;
  int validate = 123456;
}
Config;