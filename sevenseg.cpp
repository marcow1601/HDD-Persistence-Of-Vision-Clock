#include "sevenseg.h"

#define runEvery(t) for (static uint16_t _lasttime;\
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t);\
                         _lasttime += (t))




// Dynamic pin declaration
uint8_t d1;
uint8_t d2;
uint8_t d3;
uint8_t d4;
uint8_t a;
uint8_t b;
uint8_t c;
uint8_t d;
uint8_t e;
uint8_t f;
uint8_t g;
uint8_t dp;
uint8_t col;

// 7-segment binary code
// dp-a-b-c-d-e-f-g

uint8_t code[16] = {
  0b01111110,   // 0
  0b00110000,   // 1
  0b01101101,   // 2
  0b01111001,   // 3
  0b00110011,   // 4
  0b01011011,   // 5
  0b01011111,   // 6
  0b01110000,   // 7
  0b01111111,   // 8
  0b01111011,   // 9
  0b01110111,   // A
  0b00011111,   // b
  0b01001110,   // C
  0b00111101,   // d
  0b01001111,   // E
  0b01000111    // F
};

uint8_t digit1 = 10;
uint8_t digit2 = 10;
uint8_t digit3 = 10;
uint8_t digit4 = 10;

boolean colon = false;

uint8_t mpxCount = 0;

void init7seg(uint8_t dig1, uint8_t dig2, uint8_t dig3, uint8_t dig4, uint8_t segA, uint8_t segB, uint8_t segC, uint8_t segD, uint8_t segE, uint8_t segF, uint8_t segG, uint8_t segDP, uint8_t segColon)
{
  d1 = dig1;
  d2 = dig2;
  d3 = dig3;
  d4 = dig4;
  a = segA;
  b = segB;
  c = segC;
  d = segD;
  e = segE;
  f = segF;
  g = segG;
  dp = segDP;
  col = segColon;

  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d4, OUTPUT);

  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);

  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);
  digitalWrite(d4, LOW);

  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);

  // Optional pins
  if(dp != 99){
    pinMode(dp, OUTPUT);
    digitalWrite(dp, LOW);
  }
  if(col != 99){
    pinMode(col, OUTPUT);
    digitalWrite(col, LOW);
  }
}

void setMode(boolean actColon)
{
  colon = actColon;
}

void setOutput(uint8_t high, uint8_t low)
{
  digit1 = (uint8_t)(high/10);
  digit2 = high-(digit1*10);

  // Digit turned off if value is 10
  if(digit1 == 0) digit1 = 10;
  if(digit1 == 0 && digit2 == 0) digit2 = 10;

   digit3 = (uint8_t)(low/10);
   digit4 = low-(digit3*10);
}

void multiplex()
{
  // Disable all digits
  digitalWrite(d1,LOW);
  digitalWrite(d2,LOW);
  digitalWrite(d3,LOW);
  digitalWrite(d4,LOW);

  // Set segments to be active
  if((mpxCount == 0) && (digit1 != 10)){
    digitalWrite(a, (code[digit1] >> 6) & 1);
    digitalWrite(b, (code[digit1] >> 5) & 1);
    digitalWrite(c, (code[digit1] >> 4) & 1);
    digitalWrite(d, (code[digit1] >> 3) & 1);
    digitalWrite(e, (code[digit1] >> 2) & 1);
    digitalWrite(f, (code[digit1] >> 1) & 1);
    digitalWrite(g, (code[digit1]) & 1);

    if(dp != 99) digitalWrite(dp, (code[digit1] >> 7) & 1);
  }
  else if((mpxCount == 1) && (digit2 != 10)){
    digitalWrite(a, (code[digit2] >> 6) & 1);
    digitalWrite(b, (code[digit2] >> 5) & 1);
    digitalWrite(c, (code[digit2] >> 4) & 1);
    digitalWrite(d, (code[digit2] >> 3) & 1);
    digitalWrite(e, (code[digit2] >> 2) & 1);
    digitalWrite(f, (code[digit2] >> 1) & 1);
    digitalWrite(g, (code[digit2]) & 1);

    if(dp != 99) digitalWrite(dp, (code[digit2] >> 7) & 1);
  }
  else if((mpxCount == 2) && (digit3 != 10)){
    digitalWrite(a, (code[digit3] >> 6) & 1);
    digitalWrite(b, (code[digit3] >> 5) & 1);
    digitalWrite(c, (code[digit3] >> 4) & 1);
    digitalWrite(d, (code[digit3] >> 3) & 1);
    digitalWrite(e, (code[digit3] >> 2) & 1);
    digitalWrite(f, (code[digit3] >> 1) & 1);
    digitalWrite(g, (code[digit3]) & 1);

    if(dp != 99) digitalWrite(dp, (code[digit3] >> 7) & 1);
  }
  else if((mpxCount == 3) && (digit4 != 10)){
    digitalWrite(a, (code[digit4] >> 6) & 1);
    digitalWrite(b, (code[digit4] >> 5) & 1);
    digitalWrite(c, (code[digit4] >> 4) & 1);
    digitalWrite(d, (code[digit4] >> 3) & 1);
    digitalWrite(e, (code[digit4] >> 2) & 1);
    digitalWrite(f, (code[digit4] >> 1) & 1);
    digitalWrite(g, (code[digit4]) & 1);

    if(dp != 99) digitalWrite(dp, (code[digit4] >> 7) & 1);
  }

 
  // Enable current digit
  if(mpxCount == 0){
    digitalWrite(d1,HIGH);   
  }
  else if(mpxCount == 1){
    digitalWrite(d2,HIGH);  
  }
  else if(mpxCount == 2){
    digitalWrite(d3,HIGH);
  }
  else if(mpxCount == 3){
    digitalWrite(d4,HIGH);
  }

  mpxCount++;
  if(mpxCount == 4) mpxCount = 0;


  if(colon){
    runEvery(1000){
      digitalWrite(col, digitalRead(col)^1);
    }
  }
  else digitalWrite(col, LOW);
  
}

