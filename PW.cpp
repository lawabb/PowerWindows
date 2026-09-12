/*****************************************
 *  PW 2.0
 *  (c) Lawrie Abbott 2026
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

void PW::SwitchStateUp()
{
  // Read input once every loop except if in debounce wait time
  // Up button

  if (button_debounceUp) {
    // Check if debounce delay completed
    lastTimeUp =  millis();
    if ((lastTimeUp - initTimeUp) > debounceDelay) {
      serial_print("Up Button OK", side);
      button_debounceUp = false; // unset debounce flag
      buttonPressedUp = true; // set button pressed flag 
      } 
    else {
        //serial_print("Continue debounce for Up", side);
        }
      }
  else {
    // read the switch
    if (!digitalRead(SwitchUp)) {
      initTimeUp = millis();  // set start debounce time
      //buttonPressedUp = true to be set after debounce time
      if (buttonPressedUp && !button_debounceUp && (lastTimeUp - millis()) > button_held) {
        //serial_print_val("Button Held", lastTimeUp, side);
        button_heldUp = true;
        ContinuousUp();
        }
      else {
        button_debounceUp = true;
        button_heldUp = false;
        }
      } 
    else {
      buttonPressedUp = false; // set button pressed flag
      }
    }         
}

void PW::SwitchStateDn()
{
  // Read input once every loop except if in debounce wait time
  // Dn button

  if (button_debounceDn) {
    // Check if debounce delay completed
    lastTimeDn =  millis();
    if ((lastTimeDn - initTimeDn) > debounceDelay) {
      serial_print("Dn Button OK", side);
      button_debounceDn = false; // unset debounce flag
      buttonPressedDn = true; // set button pressed flag 
      } 
    else {
      //serial_print("Continue debounce for Dn", side);
      }
    }

  else {
    // read the switch
    if (!digitalRead(SwitchDn)) {
      initTimeDn = millis();  // set start debounce time
      //buttonPressedDn = true to be set after debounce time
      if (buttonPressedDn && !button_debounceDn && (lastTimeDn - millis()) > button_held) {
        //serial_print_val("Button Held", lastTimeDn, side);
        button_heldDn = true;
        ContinuousDn();
        }
      else {
        button_debounceDn = true;
        button_heldDn = false;
        }
      } 
    else {
      buttonPressedDn = false; // set button pressed flag
      }
    }         
}

void PW::WindWindow(bool up)   // up or !up (ie down)
{ 
  if (!inhibit_drive) {
    serial_print_val("Winding", up, side);
  
    Winding = true;
    
    // Set relays to drive in required direction
    digitalWrite(RelayA, up);
    digitalWrite(RelayB, !up);
    delay(changeoverDelay);
    } 
  else {
    serial_print_val("Inhibit", inhibit_drive, side);
    }
}

void PW::WindowStop()
{
  //Stop winding unless inhibited by held down buttons
  if (button_heldUp) {
    // Do nothing until button_heldUp flag is cleared 
    button_heldUp = false;
    }
  else if (button_heldDn) {
    // Do nothing until button_heldDn flag is cleared 
    button_heldDn = false;
    }   
  else {
    // reset winding, button state flags
    inhibit_drive = false;
    Winding = false;
    abort_wind = false;
    timeout_counting = false;
    button_heldUp = false;
    button_heldDn = false;

    serial_print("STOP, set relays to rest. ", side);
    // set both relays to same state (and de-energised)
    digitalWrite(RelayA, AT_REST);
    digitalWrite(RelayB, AT_REST);
      
    delay(changeoverDelay); 
    } 
}

void PW::Up()
{ 
  // WINDOW UP START
  if (buttonPressedUp && !Winding)  {  //SW activated and motor not running

   // reset 'end of travel' flag for down direction
   // bail out if already at end of travel up
    end_of_travel_down = false;
    if (end_of_travel_up) {
      return;
      }
    WindWindow(1);  // Wind window up 
    //serial_print("Winding Up", side);    
    } 
  // WINDOW UP STOP
  else if (buttonPressedUp && Winding ) {
    WindowStop();  
    }
}

void PW::Down()
{
  // WINDOW DOWN START
  if (buttonPressedDn && !Winding)  {  
    
    // reset 'end of travel' flag for up direction
    // bail out if already at end of travel down
    end_of_travel_up = false;
    if (end_of_travel_down) {
      return;
      }
    WindWindow(0);  // Wind window down
    //serial_print("Winding Down", side);          
    }     
  // WINDOW DOWN STOP
  else if (buttonPressedDn && Winding) {
    WindowStop();  
    }
}

void PW::Timeout() 
{ 
  // TIMEOUT STOP

  if (Winding && !timeout_counting) {
    initTimeTimeout = millis(); 
    timeout_counting = true;
    }

  if (timeout_counting && Winding && (millis()-initTimeTimeout >= timeout)) {
    serial_print_val("TIMEOUT", (millis()-initTimeTimeout), side);
    inhibit_drive = true;
    inhibit_stop = false;
    abort_wind = true;
    timeout_counting = false;
    }
}  

void PW::ContinuousUp() 
{
  //  deal with a SWITCH that is CONTINUOUSLY HELD 
  
  if (Winding && button_heldUp) {

    // if switch is  activated disable stop function 
    if (buttonPressedUp) {
      inhibit_stop = true;
      abort_wind = false;
      }

    //when switch is off allow to abort winding
    else if (!buttonPressedUp && inhibit_stop){
      serial_print("Switch OFF ", side);
      inhibit_stop = false;
      abort_wind = true; 
      inhibit_drive = false;
      }
    else {
      serial_print_val("Switch OFF ?", inhibit_stop, side);
      }
    }
}


void PW::ContinuousDn() 
{
  //  deal with a SWITCH that is CONTINUOUSLY HELD 
  
  if (Winding && button_heldDn) {

    // if switch is  activated disable stop function 
    if (buttonPressedDn) {
      inhibit_stop = true;
      abort_wind = false;
      }

    //when switch is off allow to abort winding
    else if (!buttonPressedDn && inhibit_stop){
      serial_print("Switch OFF ", side);
      inhibit_stop = false;
      abort_wind = true; 
      inhibit_drive = false;
      }
    else {
      serial_print_val("Switch OFF ?", inhibit_stop, side);
      }
    }
}

void PW::Sensor() 
{
  // Measure current SENSOR every loop  and STOP if over setpoint

  int reada = analogRead(CurrentSens);
  //Serial.println(reada);

  // Azero is being measured at 1/2 Vcc, should be approx 1024/2 = 512
  int azero = analogRead(AZero);
  //Serial.println(azero);

  int rawCurrent = reada - azero;
  //Serial.println(rawCurrent);

  //Note: rawCurrent can be + or - depending direction of motor
  // and does'nt matter for overcurrent situation
  mAmps = abs(rawCurrent)*5.0/1024*10000;

  if (mAmps >= maxAmps*1000) {
    serial_print_uval("Overcurrent: Amps = ", mAmps/1000.0, side);
    
    inhibit_stop = false;
    abort_wind = true;
    
    if (mAmps > 0) {           // down
      end_of_travel_down = true;
      end_of_travel_up = false;
      } 
    else {                  // up
      end_of_travel_down = false;
      end_of_travel_up = true;
    }
    
    WindowStop();
    delay(1000);
    }
}

// for debugging
void PW::PrintStatus()
{
  Serial.println("");
  serial_print_uval(" *** buttonPressedUp = ", buttonPressedUp, side);
  serial_print_uval(" *** buttonPressedDn = ", buttonPressedDn, side);
  serial_print_uval(" *** Winding = ", Winding, side);
  serial_print_uval(" *** inhibit_stop = ", inhibit_stop, side);
  serial_print_uval(" *** timeout_counting = ", timeout_counting, side);
  serial_print_uval(" *** inhibit_drive = ", inhibit_drive, side);
  serial_print_uval(" *** end_of_travel_up = ", end_of_travel_up, side);
  serial_print_uval(" *** end_of_travel_down = ", end_of_travel_down, side);
  serial_print_uval(" *** up = ", up, side);
}
