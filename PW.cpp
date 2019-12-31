/*****************************************
 *  PW
 *  (c) Lawrie Abbott 2019
 *****************************************/

#include "PW.h"


PW::PW() {}

void PW::Init(byte a, byte b, byte c, byte d, byte e, bool f, bool g, bool h, bool i, bool j, uint32_t k, uint32_t l, uint32_t m, uint32_t n ) 
{
  RelayA = a;
  RelayB = b;
  SwitchUp = c;
  SwitchDn = d;
  CurrentSens = e;
  buttonPressedUp = f;
  buttonPressedDn = g;
  Winding = h;
  abort_wind = i;
  inhibit_stop = j;
  initTime = k;
  newTime = l;
  oldTime = m;
  Amps = n;
}

void PW::WindWindow(bool direction)
{ 
  Serial.print("Winding  ");
  Serial.println(direction);
  
  Winding = true;
  
  // Set relays to drive in required direction
  digitalWrite(RelayA, direction);
  digitalWrite(RelayB, !direction);
  delay(changeoverDelay);
}

void PW::WindowStop()
{ 
  Serial.println("STOP ");
  
  // reset winding, button state flags
  Winding = false;
  buttonPressedUp = false;
  buttonPressedDn = false;
  
  // set both relays to same state (and de-energised)
  digitalWrite(RelayA, HIGH);
  digitalWrite(RelayB, HIGH);
  delay(changeoverDelay);
}

void PW::Up()
{ 
  // START  UP
  if (!digitalRead(SwitchUp) && !Winding)  {  //SW activated and motor not running

    // first read of switch
    if (!buttonPressedUp) {
      buttonPressedUp = true; // set button pressed flag
      initTime = millis();  // set start debounce time
    }      
    // Wait debounce delay
    if ((millis()- this->initTime) > debounceDelay) {
      WindWindow(1);  // Wind window up          
    }     
  }

  // STOP  UP
  else if (!digitalRead(SwitchUp) && Winding) {

    // first read of switch
    if (!buttonPressedUp) {
      buttonPressedUp = true; // set button pressed flag
      initTime = millis();  // set start debounce time
    }
    if (!digitalRead(SwitchUp) && Winding)  {
      // Wait debounce delay
      if (((millis()- initTime) > debounceDelay) && !inhibit_stop) {
        WindowStop();  
      }
    }
  }
}

void PW::Down()
{
  // START  DOWN
  if (!digitalRead(SwitchDn) && !Winding)  {  //SW activated and motor not running
    
	  // first read of switch
    if (!buttonPressedDn) {
      buttonPressedDn = true; // set button pressed flag
      initTime = millis();  // set start debounce time
    } 
    // Wait debounce delay
    if ((millis()- initTime) > debounceDelay) {
      WindWindow(0);  // Wind window down          
    }     
  }

  // STOP  DOWN
  else if (!digitalRead(SwitchDn) && Winding) {
    
    // first read of switch
    if (!buttonPressedDn) {
      buttonPressedDn = true; // set button pressed flag
      initTime = millis();  // set start debounce time
    }  
    if (!digitalRead(SwitchDn) && Winding )  {
      // Wait debounce delay
      if (((millis()- initTime) > debounceDelay) && !inhibit_stop) {
        WindowStop();  
      }
    }
  }
}

void PW::Timeout() 
{ 
  // TIMEOUT STOP 
  if (Winding && millis()-initTime >= timeout) {
    
    Serial.print(millis()-initTime);
    Serial.println(" ms  TIMEOUT_");
    
    inhibit_stop = false;
    abort_wind = true;
    
    WindowStop();
  }
}  

void PW::Continuous() 
{
  //  CONTINUOUS (up and down)
  newTime = millis()- initTime;

  if ((!digitalRead(SwitchUp) || !digitalRead(SwitchDn)) && (newTime > oldTime + update_interval)) {
    inhibit_stop = false;
  
    if ((!digitalRead(SwitchUp) || !digitalRead(SwitchDn)) && !abort_wind) {
      inhibit_stop = true;
    }
  }
  else if (digitalRead(SwitchUp) && digitalRead(SwitchDn)){
    inhibit_stop = false;
    abort_wind = false;
  }  
  oldTime = newTime;
}

void PW::Sensor() 
{
  // Amps = (((analogRead(Ax)/maxAnalog)*maxmVDC)-ACSoffset)/mvperAmp
  Amps = abs((((analogRead(CurrentSens) / 1024.0) * 5000) - ACSoffset) / mVperAmp);
  
  if (Amps >= maxAmps) {
    Serial.print("Overcurrent - ");
    Serial.println(Amps);
    
    inhibit_stop = false;
    abort_wind = true;
    
    WindowStop();
  }
}
