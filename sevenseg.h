#ifndef _sevenseg_h
#define _sevenseg_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif


void init7seg(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t dp, uint8_t colAn, uint8_t colCat);

void setMode(boolean colon);
void setOutput(uint8_t hour, uint8_t minute);

void multiplex();
void enable7seg();
void disable7seg();



#endif
