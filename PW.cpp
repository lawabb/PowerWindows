/*****************************************
 *  PW
 *  (c) Lawrie Abbott 2019
 *****************************************/

#include "PW.h"


PW::PW() {}

void PW::Init(byte a, byte b, byte c, byte d, int e, bool f, bool g, bool h, bool i, bool j, uint32_t k, uint32_t l, uint32_t m, int32_t n ) 
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
  mAmps = n;

  old_sens_time = millis();
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
  // WINDOW UP START
  if (!digitalRead(SwitchUp) && !Winding)  {  //SW activated and motor not running

    // first read of switch
    if (!buttonPressedUp) {
      buttonPressedUp = true; // set button pressed flag
      initTime = millis();  // set start debounce time
    }      
    // Wait debounce delay
    if ((millis()- initTime) > debounceDelay) {
      WindWindow(1);  // Wind window up          
    }     
  }

  // WINDOW UP STOP
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
  // WINDOW DOWN START
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

  // WINDOW DOWN STOP
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
    
    Serial.println("TIMEOUT");
    
    inhibit_stop = false;
    abort_wind = true;
    
    WindowStop();
  }
}  

void PW::Continuous() 
{
  //  A SWITCH is CONTINUOUSLY HELD (either up or down)
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
  // Measure current SENSOR regularly and STOP if over setpoint
  new_sens_time = millis();
  if (new_sens_time > old_sens_time + 15*update_interval) {
    old_sens_time = new_sens_time;
  
    // Amps = (((analogRead(Ax)/maxAnalog)*maxmVDC)-ACSoffset)/mvperAmp
    int reada = analogRead(CurrentSens);
    mAmps = (((((reada / 1024.0) * 5000) - ACSoffset) / mVperAmp) * 1000); // converted to mA
    
    /*
    Serial.print(old_sens_time/1000.0);
    Serial.print("    ");
    Serial.print(reada);
    Serial.print("  "); 
    Serial.print(reada / 1024.0) ;
    Serial.print("  ");
    Serial.print((reada / 1024.0) * 5000);
    Serial.print("  ");
    Serial.print(((reada / 1024.0) * 5000) - ACSoffset);
    Serial.print("  ");
    Serial.print((((reada / 1024.0) * 5000) - ACSoffset) / mVperAmp);
    Serial.print("  ");
    Serial.println(Amps);
    */

    if (abs(mAmps) >= maxAmps*1000) {
      Serial.print("Overcurrent: Amps = ");
      Serial.println(mAmps/1000.0);
      
      inhibit_stop = false;
      abort_wind = true;
      
      WindowStop();
    }
  }
}
