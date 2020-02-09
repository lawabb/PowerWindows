/*****************************************
 *  Power Windows
 *  (c) Lawrie Abbott 2020
 *  
 *  Note: delay times and other const variables
 *  are set in PW.h
 *  
 *  _L for left _R for right
 *****************************************/
 // version 0.6

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

bool left = 1;

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

  // Create Left and Right hand instances of PW (Power Windows) main functions
  PW_L->Init(RelayA_L, RelayB_L, SwitchUp_L, SwitchDn_L, CurrentSens_L, left);
  PW_R->Init(RelayA_R, RelayB_R, SwitchUp_R, SwitchDn_R, CurrentSens_R, !left); 
  
  Serial.println("Done Setup!");
}
void loop()
{
  PW_L->Up();
  PW_L->Down();
  PW_L->Timeout();
  PW_L->Continuous();
  //PW_L->Sensor();

  PW_R->Up();
  PW_R->Down();
  PW_R->Timeout();
  PW_R->Continuous();
  //PW_R->Sensor();

  if (PW_L->abort_wind) {   
    PW_L->WindowStop();
  }

  if (PW_R->abort_wind) {   
    PW_R->WindowStop();
  }  
} 
