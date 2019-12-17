// Power Windows control for vehicle with previous manual window winders
// and new motorised actuators fitted.
// My case is for 2 door utility (ute). So 4 relays, 2 up/down switches required.
// 
// Proposed schema: 
// Mount 2 switches on centre console and arduino and 4 relay board and current sense boards nearby and run 1 pair wires 
// carrying drive current to each door.
// Momentarially operating a Left or Right switch (or both) should drive in the correct direction until either. The winding is
// 1. Halted by user  ie momentary action of switch
// 2. Reaches maximum travel and halts due to current sense 
// 3. Timesout after a fixed period. (For circumstance where window is slow but not tripping overcurrent)
// references: 
// https://www.youtube.com/watch?v=-DV6hUSxZSk   Arduino power Window control conversion
// https://www.youtube.com/watch?v=lisprJs5sNU   Using ACS712 Hall effect current sensor  -see code at bottom
//  float a =((float) sensorValue / 512.0 - 1.0) * 2.5 / 2 * 20;
//  source: forum.arduino.cc/index.php?topic=143265.0
// 4 relay board https://www.aliexpress.com/item/32905958885.html?spm=a2g0s.9042311.0.0.27424c4dgulU3c
// current sensor https://www.aliexpress.com/item/4000205887443.html?spm=a2g0s.9042311.0.0.19764c4dDLUdVT
//
// lawabb@gmail.com
// Version 0.1

#define RelayA_L 7 
#define RelayB_L 6
#define RelayA_R 5 
#define RelayB_R 4
//
#define SwitchUp_L 11
#define SwitchDn_L 10
#define SwitchUp_R  9
#define SwitchDn_R  8
//
#define SwitchActive_L 3    // Interrupt1  - May not need/use, if can do with software
#define SwitchActive_R 2  // Interrupt0  - May not need/use, if can do with software
//
#define CurrentSens_L 14  // A0
#define CurrentSens_R 15  // A1

// declarations
bool buttonStateUp_L = false;
bool buttonStateDn_L = false;
bool buttonStateUp_R = false;
bool buttonStateDn_R = false;
bool Winding_L = false;
bool Winding_R = false;
bool abort_wind_L = false;
bool abort_wind_R = false;
int direction = 0;
uint32_t timeout = 10000;  // max continuous wind time (milliseconds)
uint32_t start_time_L;
uint32_t start_time_R;

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

  // Interrupts
  pinMode(SwitchActive_L, INPUT);
  pinMode(SwitchActive_R, INPUT); 

  Serial.println("Done Setup!");
}

// **** RIGHT SIDE FUNCTIONS

void Interrupt0() // Right hand cancel action
{
  if (Winding_R == true ) {
    abort_wind_R = true;                            
  }
}

void WindowStop_R()
{ 
  // reset winding, button state, abort flags
  Winding_R = false;
  buttonStateUp_R = false;
  buttonStateDn_R = false;
  abort_wind_R = false;
   
  // set both relays to same state (and de-energised)
  digitalWrite(RelayA_R, HIGH);
  digitalWrite(RelayB_R, HIGH);
}

void WindWindow_R(bool direction)
{ 
  Winding_R = true;
  
  // Set relays to drive in required direction
  digitalWrite(RelayA_R, direction);
  digitalWrite(RelayB_R, !direction);
}

// **** LEFT SIDE FUNCTIONS

void Interrupt1() // Left hand cancel action
{
  if (Winding_L == true ) {
    abort_wind_L = true;                            
  }
}

void WindowStop_L()
{
  // reset winding, button state, abort flags
  Winding_L = false;
  buttonStateUp_L = false;
  buttonStateDn_L = false;
  abort_wind_L = false;

  // set both relays to same state (and de-energised)
  digitalWrite(RelayA_L, HIGH);
  digitalWrite(RelayB_L, HIGH);
}

void WindWindow_L(bool direction)
{
  Winding_L = true;
  
  // Set relays to drive in required direction
  digitalWrite(RelayA_L, direction);
  digitalWrite(RelayB_L, !direction); 
}

void loop() 
{

  // left hand side wind window UP
  if (!digitalRead(SwitchUp_L) && !buttonStateUp_L) { 
    Serial.println("Left hand UP");
    // init timeout 
    start_time_L = millis();
    //start winding
    WindWindow_L(1);  // Wind window up
    buttonStateUp_L = true;
  }
  // left hand side wind window DOWN
  else if (!digitalRead(SwitchDn_L) && !buttonStateDn_L) { 
    Serial.println("Left hand DOWN");
    start_time_L = millis();
    WindWindow_L(0);
    buttonStateDn_L = true;   
  }

  // right hand side wind window UP
  if (!digitalRead(SwitchUp_R) && !buttonStateUp_R) { 
    Serial.println("Right hand UP");
    // init timeout
    start_time_R = millis();
    //start winding
    WindWindow_R(1);
    buttonStateUp_R = true;
  }
  // right hand side wind window DOWN
  else if (!digitalRead(SwitchDn_R) && !buttonStateDn_R) {
    Serial.println("Right hand DOWN");
    start_time_R = millis();
    WindWindow_R(0);
    buttonStateDn_R = true;   
  }    

  if (abort_wind_L) {
    WindowStop_L();
  }
  
  if (abort_wind_R) {
    WindowStop_R();
  }

  if ((millis() - start_time_L >= timeout) && Winding_L) {
    Serial.println("Left hand TIMEOUT");
    WindowStop_L();
    start_time_L = 0;
  }

  if ((millis() - start_time_R >= timeout) && Winding_R) {
    Serial.println("Right hand TIMEOUT");
    WindowStop_R();
    start_time_R = 0;
  }

}


// Sample code for current sensor

// float amps = 0;
// float maxAmps = 0;
// float minAmps = 0;
// float lastAmps = 0;
// float noise = 0;

// void setup() {
//   Serial.begin(9600);
// }
// 
// void loop() {
//   amps = (516 - analogRead(A0)) * 27.027 / 1023;   // for 185mV per amp sensor
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
