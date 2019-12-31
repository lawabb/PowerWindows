/*****************************************
 *  PW
 *  (c) Lawrie Abbott 2019
 *****************************************/

#include <Arduino.h>
#include <stdint.h>

#ifndef _PW_H
#define _PW_H

// const
const uint32_t timeout = 8000;       // set this for max allowable continuous wind time (milliseconds)
const uint32_t debounceDelay = 50;   // switch debounce time (ms) - 50 is about right
const uint32_t changeoverDelay = 200;  // added delay on change of relay state (ms)
const uint32_t update_interval = 20; // set for how often the continous function checks are done (ms)
 
const int ACSoffset = 2500;  //  millivolts ie 2.5V  - don't change this
const int mVperAmp = 100; // 185 for 5A, 100 for 20A, and 66 for 30A Module - change to suit module used
const int maxAmps = 13; // Amps - Set this for required maximum current ie the current that will trigger winding halt
                        // Note: no input current defaults to approx minus 10 up to 12.5A - have picked 13A for test.


class PW {
	
private:

  uint32_t direction;
  uint32_t old_sens_time = 0;
  uint32_t new_sens_time = 0;
  void WindWindow(bool direction);
  
public:

  PW();

  byte RelayA;
  byte RelayB;
  byte SwitchUp;
  byte SwitchDn;
  int CurrentSens;
  
  bool buttonPressedUp;
  bool buttonPressedDn;
  bool Winding;
  bool abort_wind;
  bool inhibit_stop;
  
  uint32_t initTime;
  uint32_t newTime;
  uint32_t oldTime;
  int32_t mAmps;
  
  void Up();
  void Down();
  void Timeout();
  void Continuous();
  void Sensor();
  void WindowStop();
  void Init(byte RelayA, byte RelayB, byte SwitchUp, byte SwitchDn, int CurrentSens, bool buttonPressedUp,
      bool buttonPressedDn, bool Winding, bool abort_wind, bool inhibit_stop, uint32_t initTime, uint32_t newTime, uint32_t oldTime, int32_t mAmps);
 
};

#endif
