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

//#include <ServoTimer2.h>    // Library to output 1-2ms pulses @ 50Hz; using Timer 2
#include <avr/io.h>         // Definition of interrupt names
#include <avr/interrupt.h>  // ISR interrupt service routine
#include <Wire.h>           // I²C library

#include <TimerOne.h>       // Timer based interrupt events
#include <RTClib.h>         // Adafruit DS3231 RTC-library

#include "sevenseg.h"       // external resources for 7 segment display control

//############################
//######### Defines ##########
//############################

#define PIN_R       4
#define PIN_G       5
#define PIN_B       6
#define PIN_HALL    2       // This is the INT0 Pin of the ATMega8
//#define PIN_MOTOR   3

#define DIVISIONS   256     // Number of segments the clock face is divided into
#define OFFSET      (int)((DIVISIONS)*0.681f) // Required segment offset approx. 257°, empirical value/depends on hall placement


// Color codes "RGB"
const uint8_t black = 0;
const uint8_t red = 4;
const uint8_t green = 2;
const uint8_t blue = 1;
const uint8_t yellow = 6;
const uint8_t purple = 5;
const uint8_t cyan = 3;
const uint8_t white = 7;

// modes of 4 digital 7 segment display with colon
#define TIME        true
#define TEMP        false

/**********************************************************************************************************************************/
//############################
//## USER CONFIGURABLE AREA ##
//############################
/**********************************************************************************************************************************/

uint8_t width = 5;                    // Number of segments that a clock hand occupies; MUST BE an odd number

// Desired color of clock face; replace predefined color-codes by any of the ones given above in the "Defines" area
uint8_t bgr_clr = black;    // Background color
uint8_t hr_clr = red;       // Hour hand
uint8_t min_clr = purple;    // Minute hand
uint8_t sec_clr = cyan;     // Second hand


//############################
//######### Macros ###########
//############################

#define runEvery(t) for (static uint16_t _lasttime;\
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t);\
                         _lasttime += (t))


//############################
//### Variables and Objects###
//############################

//ServoTimer2 motor;      // create servo object to control a servo

RTC_DS3231 rtc;   // create RTC object
DateTime now;     // DateTime instance returned by DS3231 RTC
float temperature;

struct LED
{
  bool red : 1;
  bool green : 1;
  bool blue : 1;  
};


/*
 * Storage array for required color of all segments
 * 
 * 2 pages
 * Page 0: active/visible page
 * Page 1: hidden page for updating
 * 
 */
LED segment[2][(int)DIVISIONS];

boolean modeToggleFlag = false;   // Stores whether on next check the 7 segment mode should be toggled between TIME and TEMP or not 


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
    segment[0][i].red = 0;
    segment[0][i].green = 0;
    segment[0][i].blue = 0;
    segment[1][i].red = 0;
    segment[1][i].green = 0;
    segment[1][i].blue = 0;
  }
  
  pinMode(PIN_HALL, INPUT);
  
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_B, LOW);

  // Attach the brushless speed controller on PIN_MOTOR to the servo object
  //motor.attach(PIN_MOTOR);    
  
  // Initialize DS3231 RTC readout
  rtc.begin();  // Initialize the rtc object
  now = rtc.now();  // Request current time from DS3231 RTC
  temperature = rtc.getTemperature(); // Request current temperature from DS3231 RTC

  // Prepare manual control of hardware timer 1
  Timer1.initialize();
  Timer1.attachInterrupt(draw);

  // Attach an external interrupt to the hall sensor pin triggered on a rising signal edge
  attachInterrupt(digitalPinToInterrupt(PIN_HALL), hallISR, RISING);

  /*
   * Arduino Nano Pin connection overview
   * Digital  0     f
   *          1     c
   *          7     dp
   *          8     b
   *          9     a
   *         10     e 
   *         11     g
   *         12     d
   *         13     colon 
   */
  
  init7seg(16,15,17,14, 9,8,0,12,10,1,11,7, 13);  
  setMode(TIME);
  setOutput((uint8_t)now.hour(), (uint8_t)now.minute());
  
}

void loop()
{
  // Get current time from DS3231 RTC every second
  runEvery(1000)
  {
    now = rtc.now();
    
    temperature = rtc.getTemperature();
    uint8_t preDecimal = (uint8_t)temperature;
    uint8_t decimal = (uint8_t)((temperature-preDecimal)*100);

    makeVisible();

    if(modeToggleFlag){
      setMode(getMode()^1); // Toggle 7 segment mode between time and temperature
      modeToggleFlag = false;
    }

    if(getMode()) setOutput((uint8_t)now.hour(), (uint8_t)now.minute());
    else setOutput(preDecimal, decimal);
  }

  runEvery(500){
    fillSegments(now);
  }

  runEvery(2)
  {
    multiplex();
  }

  runEvery(10000)
  {
    modeToggleFlag = true;
  }
 
}

// Teach Brushless Speed Controller minimum and maximum throttle values
/*void escCalibration()
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
}*/


// This is the hall sensor triggered Interrupt Service Routine
void hallISR()
{
  revTime = micros() - lastRev;
  
  // Ignore misleading interrupt triggering
  if((revTime >= 5000)){
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
  digitalWrite(PIN_R, segment[0][currentSegment].red);
  digitalWrite(PIN_G, segment[0][currentSegment].green);
  digitalWrite(PIN_B, segment[0][currentSegment].blue);
  
  currentSegment++;
  if(currentSegment >= DIVISIONS) currentSegment = 0;
}


// Calculate current position of clock hands across the clock face DIVISIONS according to the current time; store update in hidden page
void fillSegments(DateTime time)
{
  /*
   * Calculation of clock hand positions
   * Hour:    0-23       (Hour hand completes run twice a day)
   * Minute: 0-59
   * Second: 0-59
   */
   
  // Acquire hour/minute/second from DateTime-variable
  uint8_t h = time.hour();
  uint8_t m = time.minute();
  uint8_t s = time.second();

  // Convert 24h-format hour value to 12h format
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

  // Clear entire hidden page of segment buffer array with background color
  for(uint16_t i = 0; i<DIVISIONS; i++){
    segment[1][i].red = (bgr_clr >> 2) & 1;
    segment[1][i].green = (bgr_clr >> 1) & 1;
    segment[1][i].blue = (bgr_clr) & 1;
  }

  
  for(int8_t i = -((width-1)/2); i<=((width-1)/2); i++){
    // Draw hour hand
    segment[1][(hourHand+i+DIVISIONS)%DIVISIONS].red = (hr_clr >> 2) & 1;
    segment[1][(hourHand+i+DIVISIONS)%DIVISIONS].green = (hr_clr >> 1) & 1;
    segment[1][(hourHand+i+DIVISIONS)%DIVISIONS].blue = (hr_clr) & 1;

    // Draw minute hand
    segment[1][(minHand+i+DIVISIONS)%DIVISIONS].red = (min_clr >> 2) & 1;
    segment[1][(minHand+i+DIVISIONS)%DIVISIONS].green = (min_clr >> 1) & 1;
    segment[1][(minHand+i+DIVISIONS)%DIVISIONS].blue = (min_clr) & 1;

    // Draw second hand
    segment[1][(secHand+i+DIVISIONS)%DIVISIONS].red = (sec_clr >> 2) & 1;
    segment[1][(secHand+i+DIVISIONS)%DIVISIONS].green = (sec_clr >> 1) & 1;
    segment[1][(secHand+i+DIVISIONS)%DIVISIONS].blue = (sec_clr) & 1;
  }
}

// Copy content from hidden to visible page; must be as time efficient as possible to avoid flickering of output
void makeVisible()
{
  for(uint16_t i = 0; i<DIVISIONS; i++){
    segment[0][i] = segment[1][i];
  }
}
