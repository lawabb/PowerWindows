/*****************************************
 *  PW
 *  (c) Lawrie Abbott 2020
 *****************************************/

#include "PW.h"
#include "utility.h"


PW::PW() {}

void PW::Init(byte a, byte b, byte c, byte d, int e, bool o ) 
{
  RelayA = a;
  RelayB = b;
  SwitchUp = c;
  SwitchDn = d;
  CurrentSens = e;
  side = o;
}

void PW::WindWindow(bool direction)
{ 
  if (!inhibit_drive) {
    serial_print_val("Winding", direction, side);
  
    Winding = true;
    
    // Set relays to drive in required direction
    digitalWrite(RelayA, direction);
    digitalWrite(RelayB, !direction);
    delay(changeoverDelay);
  }
}

void PW::WindowStop()
{ 
  if (!inhibit_stop) {
    serial_print("STOP", side);
    
    // reset winding, button state flags
    Winding = false;
    abort_wind = false;
    timeout_counting = false;
    
    // set both relays to same state (and de-energised)
    digitalWrite(RelayA, HIGH);
    digitalWrite(RelayB, HIGH);
    delay(changeoverDelay);
  }
 else {
   serial_print("STOP inhibited", side);
 }
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
      buttonPressedUp = false; // set button pressed flag
      WindWindow(1);  // Wind window up          
    }
  }    
  // WINDOW UP STOP
  else if (!digitalRead(SwitchUp) && Winding ) {
 
    // first read of switch
    if (!buttonPressedUp) {
      buttonPressedUp = true; // set button pressed flag
      initTime = millis();  // set start debounce time
    }
    if (((millis()- initTime) > debounceDelay) ) {
      buttonPressedUp = false; // set button pressed flag
      WindowStop();  
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
      buttonPressedDn = false;
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
    
    if (((millis()- initTime) > debounceDelay) ) {
       buttonPressedDn = false; // set button pressed flag
       WindowStop();  
    }
  }
}

void PW::Timeout() 
{ 
  // TIMEOUT STOP - not working on button held down
  // serial_print_val("timeout", (millis()-initTime), side);

  if (Winding && !timeout_counting) {
    initTimeTimeout = millis(); 
    timeout_counting = true;
  }

  if (timeout_counting && Winding && (millis()-initTimeTimeout >= timeout)) {
    serial_print_val("TIMEOUT", (millis()-initTimeTimeout), side);
    inhibit_drive = true;
    inhibit_stop = false;
    abort_wind = true;
  }
}  

void PW::Continuous() 
{
  //  check if a a SWITCH is CONTINUOUSLY HELD 
  newTime = millis();
  
  if (Winding && (newTime >= oldTime + update_interval)) {

    // if switch is  activated disable stop function 
    if (!(digitalRead(SwitchUp)) || !(digitalRead(SwitchDn))) {
      inhibit_stop = true;
      abort_wind = false;
    }

    //when switch is off allow to abort winding
    if (((digitalRead(SwitchUp)) && (digitalRead(SwitchDn))) && inhibit_stop){
      serial_print("Switches OFF ", side);
      inhibit_stop = false;
      abort_wind = true; 
      inhibit_drive = false;
    }
  } 
  if (newTime >= oldTime + update_interval) {
    oldTime = newTime;
  }  
  // disable drive after 'button held' timeout until button is released
  if ((digitalRead(SwitchUp)) && (digitalRead(SwitchDn))) {
      //serial_print("Drive Inhibit OFF ", side);
      inhibit_drive = false;
  }
}

void PW::Sensor() 
{
  // Measure current SENSOR regularly and STOP if over setpoint
  new_sens_time = millis();
  if (new_sens_time > old_sens_time + 300) {  // 300ms delay
    old_sens_time = new_sens_time;
  
    // Amps = (((analogRead(Ax)/maxAnalog)*maxmVDC)-ACSoffset)/mvperAmp
    int reada = analogRead(CurrentSens);
    mAmps = (((((reada / 1024.0) * 5000) - ACSoffset) / mVperAmp) * 1000); // converted to mA
    
    /*  // for debugging
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
    Serial.println(mAmps);
    */

    if (abs(mAmps) >= maxAmps*1000) {
      serial_print_uval("Overcurrent: Amps = ", mAmps/1000.0, side);
      
      inhibit_stop = false;
      abort_wind = true;
      
      WindowStop();
    }
  }
}
