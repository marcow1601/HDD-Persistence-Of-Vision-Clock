/****************************************************************
 * 
 * Hard drive disk (HDD) Persistence-of-Vision (POV) Clock 
 * 
 * Marco Wagner
 * April 2016
 * 
******************************************************************/

#include <Servo.h>          // Library to output 1-2ms pulses @ 50Hz
#include <avr/io.h>         // Definition of interrupt names
#include <avr/interrupt.h>  // ISR interrupt service routine
#include <TimerOne.h>       // Timer based interrupt events

#define PIN_R       3
#define PIN_G       5
#define PIN_B       6
#define PIN_HALL    2       // This is the INT0 Pin of the ATMega8

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
volatile uint16_t segTime = 0;

uint8_t pin = PIN_R;


void setup() {
  pinMode(PIN_HALL, INPUT);
  
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_B, LOW);
  
  esc.attach(9);  // attaches the servo on pin 9 to the servo object

  Timer1.initialize(4190);
  Timer1.attachInterrupt(blinken);

  //http://fluuux.de/2013/04/interrupts-mit-arduino-benutzen/
  attachInterrupt(digitalPinToInterrupt(PIN_HALL), hallISR, FALLING);
}

void loop() {
  //escCalibration();

  for(uint8_t i = 0; i<3; i++)
  {
   if(i==0){
    pin = PIN_R;
    digitalWrite(PIN_G, LOW);
    digitalWrite(PIN_B, LOW);
   }
   else if(i==1){
    pin = PIN_G;
    digitalWrite(PIN_R, LOW);
    digitalWrite(PIN_B, LOW);
   }
   else{
    pin = PIN_B;
    digitalWrite(PIN_R, LOW);
    digitalWrite(PIN_G, LOW);
   }

    delay(2000);
  }
  
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
  segTime = (int) revTime / 256.0f;

  Timer1.setPeriod(segTime);
}

void blinken() {
  digitalWrite(pin, digitalRead(pin)^1);
}
