/*****************************************
 *  Power Windows
 *  (c) Lawrie Abbott 2019
 *  
 *  Note: delay times and other const variables
 *  are set in PW.h
 *  
 *  _L for left _R for right
 *****************************************/

#include "PW.h"

#define RelayA_L 7
#define RelayB_L 6
#define RelayA_R 5 
#define RelayB_R 4
//
#define SwitchUp_L 11
#define SwitchDn_L 10
#define SwitchUp_R 9
#define SwitchDn_R 8
#define CurrentSens_L 14  // A0
#define CurrentSens_R 13  // A1

bool buttonPressedUp_L = false, buttonPressedUp_R = false;
bool buttonPressedDn_L = false, buttonPressedDn_R = false;
bool Winding_L = false,         Winding_R = false;
bool abort_wind_L = false,      abort_wind_R = false;
bool inhibit_stop_L = false,    inhibit_stop_R = false;
uint32_t initTime_L = 0,        initTime_R = 0;              
uint32_t newTime_L = 0,         newTime_R = 0;
uint32_t oldTime_L = 0,         oldTime_R = 0;
int32_t mAmps_L = 0,            mAmps_R = 0;

// Create Left and Right hand instances of PW (Power Windows) main functions
PW* PW_L = new PW();
PW* PW_R = new PW();

void setup()
{
  Serial.begin(115200);

  // Relay Outputs
  pinMode(RelayA_L, OUTPUT);
  pinMode(RelayB_L, OUTPUT);
  pinMode(RelayA_R, OUTPUT);
  pinMode(RelayB_R, OUTPUT);

  // Set to default/off
  digitalWrite(RelayA_L, HIGH);
  digitalWrite(RelayB_L, HIGH);
  digitalWrite(RelayA_R, HIGH);
  digitalWrite(RelayB_R, HIGH);
  
  // Control Switches
  pinMode(SwitchUp_L, INPUT_PULLUP);
  pinMode(SwitchDn_L, INPUT_PULLUP);
  pinMode(SwitchUp_R, INPUT_PULLUP);
  pinMode(SwitchDn_R, INPUT_PULLUP);

  // Analogue Current Sense
  pinMode(CurrentSens_L, INPUT);
  pinMode(CurrentSens_R, INPUT);

  PW_L->Init(RelayA_L, RelayB_L, SwitchUp_L, SwitchDn_L, CurrentSens_L, buttonPressedUp_L, buttonPressedDn_L, Winding_L, abort_wind_L, inhibit_stop_L, initTime_L, newTime_L, oldTime_L, mAmps_L);
  PW_R->Init(RelayA_R, RelayB_R, SwitchUp_R, SwitchDn_R, CurrentSens_R, buttonPressedUp_R, buttonPressedDn_R, Winding_R, abort_wind_R, inhibit_stop_R, initTime_R, newTime_R, oldTime_R, mAmps_R);

  Serial.println("Done Setup!");
}
void loop()
{

  PW_L->Up();
  PW_L->Down();
  PW_L->Timeout();
  PW_L->Continuous();
  PW_L->Sensor();

  PW_R->Up();
  PW_R->Down();
  PW_R->Timeout();
  PW_R->Continuous();
  PW_R->Sensor();

  if (PW_L->abort_wind) {   
    PW_L->WindowStop();
  }

  if (PW_R->abort_wind) {   
    PW_R->WindowStop();
  }  
} 
