/*****************************************
 *  PW
 *  (c) Lawrie Abbott 2026
 *****************************************/

#include <Arduino.h>
#include <stdint.h>

#ifndef _PW_H
#define _PW_H
#define AZero A2

// const
const char SoftwareVersion[4] = "0.90" ;
const uint32_t timeout = 8000;       // set this for max allowable continuous wind time (milliseconds)
                                     // needs to be longer than wind up/down times. My 1990s motor takes 5.5 - 6 seconds
                                     // added for safety
const uint32_t debounceDelay = 120;   // switch debounce time (ms) 50 - 70 is about minimum.
                                      // 120 min for non snap action switches
const uint32_t changeoverDelay = 100;  // added delay on change of relay state (ms)
const uint32_t update_interval = 100;  // set for how often the continuous function checks are done (ms)
const uint32_t button_held = 400;   // time between testing if button is held down                     
const int mVperAmp = 100; // 185 for 5A, 100 for 20A, and 66 for 30A Module - change to suit ACS712 module used
const float maxAmps = 8; // Amps - Set this for required maximum current ie the current that will trigger winding halt
                       // No load connection should read O Amps
const bool AT_REST = false;  // set condition so that relay common terminals are 0 Volts in standby condition
const bool up = true;  // 

class PW {
	
private:

  bool buttonPressedUp = false;
  bool buttonPressedDn = false;
  bool Winding = false;
  bool inhibit_stop = false;
  bool timeout_counting = false;
  bool inhibit_drive = false;
  bool end_of_travel_up = false;
  bool end_of_travel_down = false;
  bool button_debounceUp = false;
  bool button_debounceDn = false;
  bool button_heldUp = false;
  bool button_heldDn = false;

  uint32_t initTimeUp = 0;
  uint32_t initTimeDn = 0;
  uint32_t lastTimeUp = 0;
  uint32_t lastTimeDn = 0;

  uint32_t newTime = 0;
  uint32_t oldTime = 0;
  
  float mAmps = 0;
  uint32_t initTimeTimeout = 0;
  
  void WindWindow(bool up);
  
public:

  PW();

  byte RelayA;
  byte RelayB;
  byte SwitchUp;
  byte SwitchDn;
  int CurrentSens;
  int AZero;
 
  bool abort_wind = false;
  bool side;
  
  void Up();
  void Down();
  void Timeout();
  void ContinuousUp();
  void ContinuousDn();
  void Sensor();
  void WindowStop();
  void Init(byte RelayA, byte RelayB, byte SwitchUp, byte SwitchDn, int CurrentSens, bool side);
  void PrintStatus();
  void SwitchStateUp();
  void SwitchStateDn();
  //void SwitchState();
  
};

#endif
