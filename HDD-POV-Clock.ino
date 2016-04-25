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

#include "libraries/DS3231/DS3231.h" // http://www.rinkydinkelectronics.com/library.php?id=73


#define PIN_R       3
#define PIN_G       5
#define PIN_B       6
#define PIN_HALL    2       // This is the INT0 Pin of the ATMega8

#define runEvery(t) for (static uint16_t _lasttime;\
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t);\
                         _lasttime += (t))

struct LED
{
  bool red : 1;
  bool green : 1;
  bool blue : 1;  
};

Servo esc;  // create servo object to control a servo
DS3231  rtc(SDA, SCL); // create DS3231 RTC object

//############################
//####### Variables ##########
//############################

LED segment[256] = {0};

// Global, interrupt accessible variables
volatile uint16_t revTime = 0;
volatile uint16_t lastRev = micros();
volatile uint16_t segTime = 0;
volatile uint8_t currentSegment = 0;

uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;

void setup()
{
  pinMode(PIN_HALL, INPUT);
  
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_B, LOW);
  
  esc.attach(9);  // Attaches the servo on pin 9 to the servo object
  rtc.begin();    // Initialize the rtc object

  Timer1.initialize();
  Timer1.attachInterrupt(draw);

  attachInterrupt(digitalPinToInterrupt(PIN_HALL), hallISR, FALLING);

  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014
}

void loop()
{
  //escCalibration();
  // Get current time from DS3231 RTC every second
  runEvery(1000)
  {
    Time now = rtc.getTime();
    //hour = hour from "Time now"
    //minute = minute from "Time now"
    //second = second from "Time now"

    fillSegments();
  }
}

void escCalibration()
{
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

void hallISR()
{
  revTime = micros() - lastRev;
  lastRev = micros();
  segTime = (int) revTime / 256.0f;

  Timer1.setPeriod(segTime);
}


void draw()
{
  digitalWrite(PIN_R, segment[currentSegment].red);
  digitalWrite(PIN_G, segment[currentSegment].green);
  digitalWrite(PIN_B, segment[currentSegment].blue);
}

void updateTime()
{
  
}

void fillSegments()
{
  for(uint8_t i = 0; i<256; i++)
  {
    if((int)((hour/12.0f)*256.0f) == i){
      segment[i].red = 1;
      segment[i].green = 0;
      segment[i].blue = 0;

      segment[(i==0)?255:(i-1)].red = 1;
      segment[(i==0)?255:(i-1)].green = 0;
      segment[(i==0)?255:(i-1)].blue = 0;

      segment[(i==255)?0:(i+1)].red = 1;
      segment[(i==255)?0:(i+1)].green = 0;
      segment[(i==255)?0:(i+1)].blue = 0;
    }
    else if((int)((minute/60.0f)*256.0f) == i){
      segment[i].red = 0;
      segment[i].green = 0;
      segment[i].blue = 1;

      segment[(i==0)?255:(i-1)].red = 0;
      segment[(i==0)?255:(i-1)].green = 0;
      segment[(i==0)?255:(i-1)].blue = 1;

      segment[(i==255)?0:(i+1)].red = 0;
      segment[(i==255)?0:(i+1)].green = 0;
      segment[(i==255)?0:(i+1)].blue = 1;
    }
    else if((int)((second/60.0f)*256.0f) == i){
      segment[i].red = 0;
      segment[i].green = 1;
      segment[i].blue = 0;

      segment[(i==0)?255:(i-1)].red = 0;
      segment[(i==0)?255:(i-1)].green = 1;
      segment[(i==0)?255:(i-1)].blue = 0;

      segment[(i==255)?0:(i+1)].red = 0;
      segment[(i==255)?0:(i+1)].green = 1;
      segment[(i==255)?0:(i+1)].blue = 0;
    }
    else {
      segment[i].red = 0;
      segment[i].green = 0;
      segment[i].blue = 0;

      segment[(i==0)?255:(i-1)].red = 0;
      segment[(i==0)?255:(i-1)].green = 0;
      segment[(i==0)?255:(i-1)].blue = 0;

      segment[(i==255)?0:(i+1)].red = 0;
      segment[(i==255)?0:(i+1)].green = 0;
      segment[(i==255)?0:(i+1)].blue = 0;
    }
  }
}
