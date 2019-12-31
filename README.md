#  Power Windows 
Arduino project to add power window control to 'old' car

Electronic control for vehicle originally with manual window winders,
and motorised actuators retrofitted.

My requirement is for 2 door ute (well this isn't mine - but close -oneday). So require 4 relays, 2 up/down switches required.
See proposed schematic.  
 ![XR6XG.png](XR6XG.png)     
 ![Schem.pdf](https://github.com/lawabb/PowerWindows/blob/master/Schematic_Power%20Windows_Sheet_1_20191224233008.pdf)

Proposed schema:  
Mount 2 switches on centre console and arduino and 4 relay board and current sense boards nearby and run 1 pair wires 
  carrying drive current to each door.
  
  Momentarially operating a Left or Right switch (or both) should drive in the correct direction until either.  
  The winding is  
    1. Halted by user  ie momentary action of (either up or down) switch  
    2. Window reaches maximum travel and halts due to measured current being above set point. ie the electric motor stalls (I'm working on approx 5-10A, will need to test)  
    3. Timed-out after a fixed period. (For circumstance where window is slow but not tripping overcurrent)  
    
  Holding switch operated should  
    1. Not block operation of other user  
    2. Operate window winding in desired direction until either  
      &nbsp;&nbsp;a. Timeout occurs  
      &nbsp;&nbsp;b. User releases switch   
      &nbsp;&nbsp;c. Current sense detects end of travel
    
      
References:  
  https://www.youtube.com/watch?v=-DV6hUSxZSk   Arduino power Window control conversion  
  https://www.youtube.com/watch?v=lisprJs5sNU   Using ACS712 Hall effect current sensor  
  
  Formula for using ACS712
  source: forum.arduino.cc/index.php?topic=143265.0  
  float a =((float) sensorValue / 512.0 - 1.0) * 2.5 / 2 * 20;
  
  Somewhere else... This is what I'm going to try first. Where   
  maxAnalog = 1024  // Analog IP range 0-1023  
  maxmVDC = 5000    // DC voltage 0-5V  
  ACSoffset = 2500    // Nominal voltage with zero current (2.5V)  
  mvperAmp = 100 // mV Out per Amp in. 185 for 5A, 100 for 20A, and 66 for 30A Module  
  
  Amps = (((analogRead(Ax)/maxAnalog)*maxmVDC)-ACSoffset)/mvperAmp  
    
  4 relay board https://www.aliexpress.com/item/32905958885.html?spm=a2g0s.9042311.0.0.27424c4dgulU3c  
  current sensor https://www.aliexpress.com/item/4000205887443.html?spm=a2g0s.9042311.0.0.19764c4dDLUdVT
   
lawabb@gmail.com  
Version 0.2  - Rewrite to not require interrupts. Have no current sense hardware as yet.. no tested code for it yet.  
Version 0.3  - Have no current sense hardware as yet, but code for it added untested.  
&nbsp;&nbsp; TODO - code for remote operation  
&nbsp;&nbsp; TODO - refactor code... there is plenty of code duplication  
Version 0.4 - Have added 'PW' class to simplify code maintenance/changes and reduce duplication  
Version 0.5  Changes to  SENSOR - not tested with hardware  