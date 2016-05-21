#include "sevenseg.h"

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
uint8_t colAn;
uint8_t colCat;

uint8_t code[16] = {
  0
};

uint8_t digit1 = 10;
uint8_t digit2 = 10;
uint8_t digit3 = 10;
uint8_t digit4 = 10;

boolean colon = false;

uint8_t mpxCount = 0;

void init7seg(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t dp, uint8_t colAn, uint8_t colCat)
{
  d1 = d1;
  d2 = d2;
  d3 = d3;
  d4 = d4;
  a = a;
  b = b;
  c = c;
  d = d;
  e = e;
  f = f;
  g = g;
  dp = dp;
  colAn = colAn;
  colCat = colCat;
  
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
  pinMode(dp, OUTPUT);

  pinMode(colAn, OUTPUT);
  pinMode(colCat, OUTPUT);
  
}

void setMode(boolean colon)
{
  colon = colon;
}

void setOutput(uint8_t high, uint8_t low)
{
  digit1 = (uint8_t)(high/10);
  digit2 = high-digit1;

  if(digit1 == 0) digit1 = 10;
  if(digit1 == 0 && digit2 == 0) digit2 = 10;

   digit3 = (uint8_t)(low/10);
   digit4 = low-digit3;
}

void multiplex()
{
  // Disable all digits
  digitalWrite(d1,HIGH);
  digitalWrite(d2,HIGH);
  digitalWrite(d3,HIGH);
  digitalWrite(d4,HIGH);

  // Set segments to be active


  // Enable current digit
  if(mpxCount == 0){
    digitalWrite(d1,LOW);    
  }
  else if(mpxCount == 1){
    digitalWrite(d2,LOW);   
  }
  else if(mpxCount == 2){
    digitalWrite(d3,LOW);
  }
  else if(mpxCount == 3){
    digitalWrite(d4,LOW);
  }

  mpxCount++;
  if(mpxCount == 4) mpxCount = 0;
  
}


