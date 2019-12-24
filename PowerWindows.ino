// Power Windows control for vehicle with previous manual window winders
// and new motorised actuators fitted.
// My case is for 2 door utility (ute). So 4 relays, 2 up/down switches required.
// 
// Proposed schema: 
// Mount 2 switches on centre console and arduino and 4 relay board and current sense boards nearby and run 1 pair wires 
// carrying drive current to each door.
// Momentarially operating a Left or Right switch (or both) should drive in the correct direction until either. The winding is
// 1. Halted by user  ie momentary action of (either) switch
// 2. Reaches maximum travel and halts due to current sense 
// 3. Timesout after a fixed period. (For circumstance where window is slow but not tripping overcurrent)
// Holding switch operated should
// 1. Not block operation of other user
// 2. Operate window winding in desired direction until either 
// a. Timeout occurs
// b. User releases switch
// references: 
// https://www.youtube.com/watch?v=-DV6hUSxZSk   Arduino power Window control conversion
// https://www.youtube.com/watch?v=lisprJs5sNU   Using ACS712 Hall effect current sensor  -see code at bottom
//  float a =((float) sensorValue / 512.0 - 1.0) * 2.5 / 2 * 20;
//  source: forum.arduino.cc/index.php?topic=143265.0
// 4 relay board https://www.aliexpress.com/item/32905958885.html?spm=a2g0s.9042311.0.0.27424c4dgulU3c
// current sensor https://www.aliexpress.com/item/4000205887443.html?spm=a2g0s.9042311.0.0.19764c4dDLUdVT
//
// lawabb@gmail.com
// Version 0.2  - Rewrite to not require interrupts. Have no current sense hardware as yet.. no tested code for it yet. 

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

// declarations
bool buttonPressedUp_L = false;
bool buttonPressedDn_L = false;
bool buttonPressedUp_R = false;
bool buttonPressedDn_R = false;
bool Winding_L = false;
bool Winding_R = false;
bool abort_wind_L = false;
bool abort_wind_R = false;
bool inhibit_stop_L = false;
bool inhibit_stop_R = false;

int direction = 0;
uint32_t timeout = 5000;       // max continuous wind time (milliseconds)
uint32_t initTime_L = 0;       // used for debounce and timeout
uint32_t initTime_R = 0;       // used for debounce and timeout
uint32_t debounceDelay = 50;   // the debounce time (ms)
uint32_t changeoverDelay = 200;// 

uint32_t newTime_L = 0;
uint32_t oldTime_L = 0;
uint32_t newTime_R = 0;
uint32_t oldTime_R = 0;


// float amps = 0;
// float maxAmps = 0;
// float minAmps = 0;
// float lastAmps = 0;
// float noise = 0;


void setup() {
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

  Serial.println("Done Setup!");
}

//****************************************************************************************
// RIGHT SIDE FUNCTIONS

void WindWindow_R(bool direction)
{ 
  Serial.print("Winding RIGHT ");
  Serial.println(direction);
  
  Winding_R = true;
  
  // Set relays to drive in required direction
  digitalWrite(RelayA_R, direction);
  digitalWrite(RelayB_R, !direction);
  delay(changeoverDelay);
}

void WindowStop_R()
{
  Serial.println("STOP RIGHT");
  
  // reset winding, button state, abort flags
  Winding_R = false;
  buttonPressedUp_R = false;
  buttonPressedDn_R = false;
  
  // set both relays to same state (and de-energised)
  digitalWrite(RelayA_R, HIGH);
  digitalWrite(RelayB_R, HIGH);
  delay(changeoverDelay);
}

void right_up()
{
  // START RIGHT UP
  if (!digitalRead(SwitchUp_R) && !Winding_R)  {  //SW activated and motor not running

    // first read of switch
    if (!buttonPressedUp_R) {
      buttonPressedUp_R = true; // set button pressed flag
      initTime_R = millis();  // set start debounce time
    }   
    // Wait debounce delay
    if ((millis()- initTime_R) > debounceDelay) {
      WindWindow_R(1);  // Wind window up          
    }     
  }

  // STOP RIGHT UP
  else if (!digitalRead(SwitchUp_R) && Winding_R) {
    
    // first read of switch
    if (!buttonPressedUp_R) {
      buttonPressedUp_R = true; // set button pressed flag
      initTime_R = millis();  // set start debounce time
    }
    
    if (!digitalRead(SwitchUp_R) && Winding_R )  {
      // Wait debounce delay
      if (((millis()- initTime_R) > debounceDelay) && !inhibit_stop_R) {       
        WindowStop_R();  
      }
    }
  }
}

void right_down()
{
  // START RIGHT DOWN
  if (!digitalRead(SwitchDn_R) && !Winding_R)  {  //SW activated and motor not running

    // first read of switch
    if (!buttonPressedDn_R) {
      buttonPressedDn_R = true; // set button pressed flag
      initTime_R = millis();  // set start debounce time
    }   
    // Wait debounce delay
    if ((millis()- initTime_R) > debounceDelay) {     
      WindWindow_R(0);  // Wind window down          
    }     
  }

  // STOP RIGHT DOWN
  else if (!digitalRead(SwitchDn_R) && Winding_R) {
    
    // first read of switch
    if (!buttonPressedDn_R) {
      buttonPressedDn_R = true; // set button pressed flag
      initTime_R = millis();  // set start debounce time
    }
    
    if (!digitalRead(SwitchDn_R) && Winding_R )  {
      // Wait debounce delay
      if (((millis()- initTime_R) > debounceDelay) && !inhibit_stop_R) {       
        WindowStop_R();  
      }
    }
  }
}

void right_timeout() {
  
  // TIMEOUT STOP RIGHT
  if (Winding_R && millis()-initTime_R >= timeout) {
    
    Serial.print(millis()-initTime_R);
    Serial.println(" ms  TIMEOUT. _RIGHT");

    inhibit_stop_R = false;
    abort_wind_R = true;
    
    WindowStop_R();   
  }
}

void right_continuous() {
  
  // RIGHT CONTINUOUS (up and down)
  newTime_R = millis()- initTime_R;

  if ((!digitalRead(SwitchUp_R) || !digitalRead(SwitchDn_R)) && (newTime_R > oldTime_R+20)) {
    inhibit_stop_R = false;
  
    if ((!digitalRead(SwitchUp_R) || !digitalRead(SwitchDn_R)) && !abort_wind_R) {
      inhibit_stop_R = true;
    }
  }
  else if (digitalRead(SwitchUp_R) && digitalRead(SwitchDn_R)){
    inhibit_stop_R = false;
    abort_wind_R = false;
  }  
  oldTime_R = newTime_R;
}

//*************************************************************************************
// LEFT SIDE FUNCTIONS

void WindWindow_L(bool direction)
{ 
  Serial.print("Winding LEFT ");
  Serial.println(direction);
  
  Winding_L = true;
  
  // Set relays to drive in required direction
  digitalWrite(RelayA_L, direction);
  digitalWrite(RelayB_L, !direction);
  delay(changeoverDelay);  // needed and beneficial!
}

void WindowStop_L()
{ 
  Serial.println("STOP LEFT");
  
  // reset winding, button state, abort flags
  Winding_L = false;
  buttonPressedUp_L = false;
  buttonPressedDn_L = false;
  
  // set both relays to same state (and de-energised)
  digitalWrite(RelayA_L, HIGH);
  digitalWrite(RelayB_L, HIGH);
  delay(changeoverDelay);
}

void left_up()
{ 
  // START LEFT UP
  if (!digitalRead(SwitchUp_L) && !Winding_L)  {  //SW activated and motor not running

    // first read of switch
    if (!buttonPressedUp_L) {
      buttonPressedUp_L = true; // set button pressed flag
      initTime_L = millis();  // set start debounce time
    }      
    // Wait debounce delay
    if ((millis()- initTime_L) > debounceDelay) {
      WindWindow_L(1);  // Wind window up          
    }     
  }

  // STOP LEFT UP
  else if (!digitalRead(SwitchUp_L) && Winding_L) {

    // first read of switch
    if (!buttonPressedUp_L) {
      buttonPressedUp_L = true; // set button pressed flag
      initTime_L = millis();  // set start debounce time
    }
    if (!digitalRead(SwitchUp_L) && Winding_L)  {
      // Wait debounce delay
      if (((millis()- initTime_L) > debounceDelay) && !inhibit_stop_L) {
        WindowStop_L();  
      }
    }
  }
}

void left_down()
{
  // START LEFT DOWN
  if (!digitalRead(SwitchDn_L) && !Winding_L)  {  //SW activated and motor not running

    // first read of switch
    if (!buttonPressedDn_L) {
      buttonPressedDn_L = true; // set button pressed flag
      initTime_L = millis();  // set start debounce time
    }   
    if ((millis()- initTime_L) > debounceDelay) {
      WindWindow_L(0);  // Wind window down          
    }     
  }

  // STOP LEFT DOWN
  else if (!digitalRead(SwitchDn_L) && Winding_L) {
    
    // first read of switch
    if (!buttonPressedDn_L) {
      buttonPressedDn_L = true; // set button pressed flag
      initTime_L = millis();  // set start debounce time
    }
    
    if (!digitalRead(SwitchDn_L) && Winding_L )  {
      // Wait debounce delay
      if (((millis()- initTime_L) > debounceDelay) && !inhibit_stop_L) {
        WindowStop_L();  
      }
    }
  }
}

void left_timeout() {
  
  // TIMEOUT STOP LEFT
  if (Winding_L && millis()-initTime_L >= timeout) {
    
    Serial.print(millis()-initTime_L);
    Serial.println(" ms  TIMEOUT_LEFT");
    
    inhibit_stop_L = false;
    abort_wind_L = true;
    
    WindowStop_L();
  }
}  

void left_continuous() {
  
  // LEFT CONTINUOUS (up and down)
  newTime_L = millis()- initTime_L;

  if ((!digitalRead(SwitchUp_L) || !digitalRead(SwitchDn_L)) && (newTime_L > oldTime_L+20)) {
    inhibit_stop_L = false;
  
    if ((!digitalRead(SwitchUp_L) || !digitalRead(SwitchDn_L)) && !abort_wind_L) {
      inhibit_stop_L = true;
    }
  }
  else if (digitalRead(SwitchUp_L) && digitalRead(SwitchDn_L)){
    inhibit_stop_L = false;
    abort_wind_L = false;
  }  
  oldTime_L = newTime_L;
}


// *************************************************************************************************

// void current_L() {
//   amps = (516 - analogRead(CurrentSens_L)) * 27.027 / 1023;   // for 185mV per amp sensor
//   amps = (amps + lastAmps) / 2;
//   lastAmps = amps;
//   maxAmps = max(maxAmps, amps);
//   minAmps = min(minAmps, amps);
//   noise = maxAmps - minAmps;
//   Serial.print(amps);
//   Serial.print(" ");
//   Serial.println(noise);
//   if (Serial.read() != -1) {maxAmps = amps; minAmps = amps;}
//   delay(100);
// }

// void current_R() {
//   amps = (516 - analogRead(CurrentSens_R)) * 27.027 / 1023;   // for 185mV per amp sensor
//   amps = (amps + lastAmps) / 2;
//   lastAmps = amps;
//   maxAmps = max(maxAmps, amps);
//   minAmps = min(minAmps, amps);
//   noise = maxAmps - minAmps;
//   Serial.print(amps);
//   Serial.print(" ");
//   Serial.println(noise);
//   if (Serial.read() != -1) {maxAmps = amps; minAmps = amps;}
//   delay(100);
// }

void loop()
{
  left_up();
  left_down();
  left_continuous(); //check for continuous button down every 20ms
  left_timeout();
  
  right_up();
  right_down();
  right_continuous(); //check for continuous button down every 20ms
  right_timeout();

  if (abort_wind_L) {
    WindowStop_L();
  } 
  if (abort_wind_R) {
    WindowStop_R();
  }
}  





