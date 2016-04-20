// Main HDD POV Clock arduino code

#include <Servo.h>            // Library to output 1-2ms pulses @ 50Hz
#include <avr/io.h>         // Definition of interrupt names
#include <avr/interrupt.h>  // ISR interrupt service routine

// CURRENT STATUS: Only testing of single elements later needed

// CURRENTLY BEING TESTED: ESC control for HDD BLDC-motor!!

Servo esc;  // create servo object to control a servo

// This is the INT0 Pin of the ATMega8
int hallSensor = 2;
// INT data exchange variables
volatile uint16_t revTime = 0;
volatile uint16_t lastRev = micros();

// Install the interrupt routine.
//ISR(INT0_vect) {
//revTime = micros();
//}

void setup() {
  esc.attach(9);  // attaches the servo on pin 9 to the servo object

  // https://gonium.net/md/2006/12/20/handling-external-interrupts-with-arduino/
  //pinMode(hallSensor, INPUT);
  // Global Enable INT0 interrupt
  //GICR |= ( 1 < < INT0);
  // Signal change triggers interrupt
  //MCUCR |= ( 1 << ISC00);
  //MCUCR |= ( 0 << ISC01);

  //http://fluuux.de/2013/04/interrupts-mit-arduino-benutzen/
  attachInterrupt(0, hallISR, FALLING);
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
