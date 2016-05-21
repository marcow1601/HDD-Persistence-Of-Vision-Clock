/****************************************************************
 * 
 * Hard drive disk (HDD) Persistence-of-Vision (POV) Clock 
 * 
 * School project
 * 
 * Author: Marco Wagner
 * April/May 2016
 * 
******************************************************************/

#include <Servo.h>          // Library to output 1-2ms pulses @ 50Hz
#include <avr/io.h>         // Definition of interrupt names
#include <avr/interrupt.h>  // ISR interrupt service routine
#include <Wire.h>           // I²C library

#include <TimerOne.h>       // Timer based interrupt events
#include <RTClib.h>         // Adafruit DS3231 RTC-library

#include "sevenseg.h"       // external ressources for 7 segment display control


//############################
//######### Defines ##########
//############################

#define PIN_R       3
#define PIN_G       5
#define PIN_B       6
#define PIN_HALL    2       // This is the INT0 Pin of the ATMega8
#define PIN_MOTOR   4

#define DIVISIONS   64     // Number of segments the clock face is divided into
#define OFFSET      (int)((DIVISIONS)*0.715f) // Required segment offset approx. 257°, empirical value/depends on hall placement

// modes of 4 digital 7 segment display with colon
#define TIME        true
#define TEMP        false


//############################
//######### Macros ###########
//############################

#define runEvery(t) for (static uint16_t _lasttime;\
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t);\
                         _lasttime += (t))


//############################
//######## Objects ###########
//############################

Servo motor;      // create servo object to control a servo
RTC_DS3231 rtc;   // create RTC object


//############################
//####### Variables ##########
//############################

struct LED
{
  bool red : 1;
  bool green : 1;
  bool blue : 1;  
};

LED segment[(int)DIVISIONS];
DateTime now;


// Global, interrupt accessible variables
volatile uint16_t revTime = 0;
volatile uint16_t lastRev = micros();
volatile uint16_t segTime = 0;
volatile uint16_t currentSegment = OFFSET;


//############################
//####### Functions ##########
//############################

void setup()
{
  // Zero-init segment color storage array
  for(uint16_t i=0; i<DIVISIONS; i++){
    segment[i].red = 0;
    segment[i].green = 0;
    segment[i].blue = 0;
  }
 
  Serial.begin(115200);
  
  pinMode(PIN_HALL, INPUT);
  
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_B, LOW);

  // Attach the brushless speed controller on PIN_MOTOR to the servo object
  motor.attach(PIN_MOTOR);    
  
  // Initialize DS3231 RTC readout
  rtc.begin();      // Initialize the rtc object
  now = rtc.now();  // Request current time from DS3231 RTC

  // Prepare manual control of hardware timer 1
  Timer1.initialize();
  Timer1.attachInterrupt(draw);

  // Attach an external interrupt to the hall sensor pin triggered on a rising signal edge
  attachInterrupt(digitalPinToInterrupt(PIN_HALL), hallISR, RISING);

  //init7seg(0,1,2,3,4,5,6,7,8,9,10,11,12,13);
  //setMode(TIME);
}

void loop()
{
  
  // Get current time from DS3231 RTC every second
  runEvery(1000)
  {
    now = rtc.now();
    /*
    Serial.println("------------------------------------");
    Serial.println("hr-min-sec");
    Serial.print(now.hour(), DEC);
    Serial.print("-");
    Serial.print(now.minute(), DEC);
    Serial.print("-");
    Serial.println(now.second(), DEC);
*/
    //setOutput(now.hour, now.min);
  }

  runEvery(50){
    fillSegments(now);
  }

  /*runEvery(5)
  {
    multiplex();
  }*/
  
}


// Teach Brushless Speed Controller minimum and maximum throttle values
void escCalibration()
{
  // Sweep ESC control one cycle Min-Max-Min on power-up
  for(uint8_t i = 20; i<= 180 ; i++)
  {
    motor.write(i);
    delay(5);
  }
  delay(100);
  for(uint8_t i = 179; i>= 20; i--)
  {
    motor.write(i);
    delay(5);
  }
  // Wait for initialization sound to finish
  delay(5000);
}


// This is the hall sensor triggered Interrupt Service Routine
void hallISR()
{
  revTime = micros() - lastRev;

  // Ignore misleading interrupt triggering
  if(revTime >= 5000){
    Timer1.stop();
    
    lastRev = micros();
    segTime = (int)((revTime / DIVISIONS)+0.5f);

    currentSegment=OFFSET;
    
    Timer1.setPeriod(segTime);
    Timer1.start();
  }
}


// Switch LED strip color the the one required of the currently active segment
void draw()
{  
  digitalWrite(PIN_R, segment[currentSegment].red);
  digitalWrite(PIN_G, segment[currentSegment].green);
  digitalWrite(PIN_B, segment[currentSegment].blue);
  
  currentSegment++;
  if(currentSegment >= DIVISIONS) currentSegment = 0;
}


// Calculate current position of clock hands across the clock face DIVISIONS according to the current time
void fillSegments(DateTime time)
{
  /*
   * Calculation of clock hand positions
   * Hour:    0-24       (Hour hand completes run twice a day)
   * Minute: 0-60
   * Second: 0-60
   */
  // Acquire hour/minute/second from DateTime-variable
  uint8_t h = time.hour();
  uint8_t m = time.minute();
  uint8_t s = time.second();

  // Convert 24h-format to 12h format
  if(h >= 12) h -= 12;

  // Current position of clock hands across the DIVISIONS
  uint8_t secHand = (uint8_t)((((uint16_t) s)*DIVISIONS)/60.0f+0.5f);          // Always round to the nearest whole segment number
  uint8_t minHand = (uint8_t)((((uint16_t) m)*DIVISIONS)/60.0f+0.5f);
  uint8_t hourHand = (uint8_t)((((uint16_t) h)*DIVISIONS)/12.0f+0.5f);
  //uint8_t hourHand = (uint8_t)((((uint16_t)(time.hour % 12)+(time.minute/60.0f))*DIVISIONS)/12.0f+0.5f);       // Hour hand moves slowly with progressing minute count in current hour

  /*Serial.println("Positions");
  Serial.print(hourHand);
  Serial.print("-");
  Serial.print(minHand);
  Serial.print("-");
  Serial.println(secHand);*/
   
  
  // Mirror hand positions because of counter-clockwise spinning spindle
  secHand = DIVISIONS-secHand;
  minHand = DIVISIONS-minHand;
  hourHand = DIVISIONS-hourHand;
  
  
  for(uint16_t i = 0; i<DIVISIONS; i++)
  {
    if(hourHand == i){
      segment[i].red = 0;
      segment[i].green = 0;
      segment[i].blue = 1;

      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].red = 0;
      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].green = 0;
      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].blue = 1;

      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].red = 0;
      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].green = 0;
      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].blue = 1;
    }
    else if(minHand == i){
      segment[i].red = 0;
      segment[i].green = 1;
      segment[i].blue = 0;

      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].red = 0;
      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].green = 1;
      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].blue = 0;

      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].red = 0;
      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].green = 1;
      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].blue = 0;
    }
    else if(secHand == i){
      segment[i].red = 1;
      segment[i].green = 1;
      segment[i].blue = 0;

      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].red = 1;
      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].green = 1;
      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].blue = 0;

      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].red = 1;
      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].green = 1;
      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].blue = 0;
    }
    else {
      segment[i].red = 1;
      segment[i].green = 1;
      segment[i].blue = 1;

      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].red = 1;
      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].green = 1;
      segment[(i==0)?((int)(DIVISIONS-1)):(i-1)].blue = 1;

      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].red = 1;
      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].green = 1;
      segment[(i==((int)(DIVISIONS-1)))?0:(i+1)].blue = 1;
    }
  }
}
