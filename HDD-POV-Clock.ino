/****************************************************************
 * 
 * Hard drive disk (HDD) Persistence-of-Vision (POV) Clock 
 * 
 * Marco Wagner
 * April 2016
 * 
******************************************************************/

#include <Servo.h>           // Library to output 1-2ms pulses @ 50Hz
#include <avr/io.h>          // Definition of interrupt names
#include <avr/interrupt.h>   // ISR interrupt service routine
#include <TimerOne.h>        // Arduino Timer 1 library


#define hallSensor          2
#define RED                 9
#define GREEN               10
#define BLUE                11

struct LED
{
  bool red : 1;
  bool green : 1;
  bool blue : 1;  
};

Servo esc;  // create servo object to control a servo


//############################
//####### Variables ##########
//############################

LED segment[256] = {0}; 

// Global, interrupt accessible variables
volatile uint16_t revTime = 0;
volatile uint16_t lastRev = micros();


void setup() {
  esc.attach(9);  // attaches the servo on pin 9 to the servo object

  //http://fluuux.de/2013/04/interrupts-mit-arduino-benutzen/
  attachInterrupt(digitalPinToInterrupt(hallSensor), hallISR, FALLING);
}

void loop() {
  escCalibration();

  
}

void escCalibration() {
  // Sweep ESC control one cycle Min-Max-Min on power-up
  for(uint8_t i = 20; i<= 180 ; i++)
  {
    esc.write(i);
    delay(5);
  }
  delay(100);
  for(uint8_t i = 179; i>= 20; i--)
  {
    esc.write(i);
    delay(5);
  }
  // Wait for initialization sound to finish
  delay(5000);
}

void hallISR() {
  revTime = micros() - lastRev;
  lastRev = micros();
}
