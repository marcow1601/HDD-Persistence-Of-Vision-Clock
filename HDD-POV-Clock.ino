// Main HDD POV Clock arduino code

#include <Servo.h>

// CURRENT STATUS: Only testing of single elements later needed

// CURRENTLY BEING TESTED: ESC control for HDD BLDC-motor!!

Servo esc;  // create servo object to control a servo

void setup() {
  esc.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  delay(1000);

  escCalibration();

  esc.write(90);

  delay(1000);

  esc.write(150);

  delay(1000);

  esc.write(25);

  while(1);

  
}

void escCalibration() {
  for(uint16_t i = 20; i<= 180 ; i++)
  {
    esc.write(i);
    delay(5);
  }
  delay(100);
  for(uint16_t i = 179; i>= 20; i--)
  {
    esc.write(i);
    delay(5);
  }

  delay(5000);
}

