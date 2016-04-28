#ifndef _sevenseg_h
#define _sevenseg_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif



void setOutput(uint16_t value);   //
void setMode(boolean colon);      //
void multiplex();
void enable7seg();
void disable7seg();



#endif

