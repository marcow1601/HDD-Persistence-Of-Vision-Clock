#include "sevenseg.h"

uint8_t code[16] = {
  0
};

uint8_t digit1 = 10;
uint8_t digit2 = 10;
uint8_t digit3 = 10;
uint8_t digit4 = 10;

boolean colon = false;

void init7seg(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t dp, uint8_t colAn, uint8_t colCat)
{
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
  
}


