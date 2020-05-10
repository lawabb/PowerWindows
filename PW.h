/*****************************************
 *  PW
 *  (c) Lawrie Abbott 2020
 *****************************************/

#include <Arduino.h>
#include <stdint.h>

#ifndef _PW_H
#define _PW_H
#define AMAX A2

// const
const char SoftwareVersion[3] = "0.8" ;
const uint32_t timeout = 6000;       // set this for max allowable continuous wind time (milliseconds)
                                     // needs to be longer than wind up/down times...
                                     // added for safety
const uint32_t debounceDelay = 70;   // switch debounce time (ms) - 50-70 is about minimum
const uint32_t changeoverDelay = 100;  // added delay on change of relay state (ms)
const uint32_t update_interval = 10;  // set for how often the continuous function checks are done (ms)
const int fudge = 2;  // trim for minimal error at zero current
                      // eg. If zero current analog reading = 508 then 
                      // fudge = Detected Amax -  2 x  Analog reading
                      // eg if Detected Amax = 1021 (note is lower than Vcc which should be 1023)
                      // fudge = 1021 - 2*508
                      //       = 5    
const int mVperAmp = 100; // 185 for 5A, 100 for 20A, and 66 for 30A Module - change to suit ACS712 module used
const int maxAmps = 8; // Amps - Set this for required maximum current ie the current that will trigger winding halt
                        // Note: no input connection defaults to approx minus 10 up to 12.5A.
const int sensor_interval = 20;  // time between reads of current on analog inputs (mS)
const bool AT_REST = false;  // set condition so that relay common terminals are 0 Volts in standby condition

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
  bool up;
  
  uint32_t initTime = 0;
  uint32_t newTime = 0;
  uint32_t oldTime = 0;
  int32_t mAmps = 0;
  uint32_t old_sens_time = 0;
  uint32_t new_sens_time = 0;
  uint32_t initTimeTimeout = 0;
  
  void WindWindow(bool up);
  
public:

  PW();

  byte RelayA;
  byte RelayB;
  byte SwitchUp;
  byte SwitchDn;
  int CurrentSens;
  float Amax;

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
