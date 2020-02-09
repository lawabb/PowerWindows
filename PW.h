/*****************************************
 *  PW
 *  (c) Lawrie Abbott 2020
 *****************************************/

#include <Arduino.h>
#include <stdint.h>

#ifndef _PW_H
#define _PW_H

// const
const uint32_t timeout = 8000;       // set this for max allowable continuous wind time (milliseconds)
const uint32_t debounceDelay = 70;   // switch debounce time (ms) - 50-70 is about right
const uint32_t changeoverDelay = 200;  // added delay on change of relay state (ms)
const uint32_t update_interval = 10; // set for how often the continuous function checks are done (ms)
 
const int ACSoffset = 2500;  //  millivolts ie 2.5V  - don't change this
const int mVperAmp = 100; // 185 for 5A, 100 for 20A, and 66 for 30A Module - change to suit module used
const int maxAmps = 20; // Amps - Set this for required maximum current ie the current that will trigger winding halt
                        // Note: no input current defaults to approx minus 10 up to 12.5A - have picked 13A for test.


class PW {
	
private:

  bool buttonPressedUp = false;
  bool buttonPressedDn = false;
  bool Winding = false;
  bool inhibit_stop = false;
  bool timeout_counting = false;
  bool inhibit_drive = false;
  
  uint32_t initTime = 0;
  uint32_t newTime = 0;
  uint32_t oldTime = 0;
  uint32_t mAmps = 0;
  uint32_t direction;
  uint32_t old_sens_time = 0;
  uint32_t new_sens_time = 0;
  uint32_t initTimeTimeout = 0;

  void WindWindow(bool direction);
  
public:

  PW();

  byte RelayA;
  byte RelayB;
  byte SwitchUp;
  byte SwitchDn;
  int CurrentSens;

  bool abort_wind = false;
  bool side;
  
  void Up();
  void Down();
  void Timeout();
  void Continuous();
  void Sensor();
  void WindowStop();
  void Init(byte RelayA, byte RelayB, byte SwitchUp, byte SwitchDn, int CurrentSens, bool side);
};

#endif
