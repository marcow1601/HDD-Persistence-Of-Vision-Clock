/*
 * Available pins
 * Digital  13
 *          12
 *          11
 *          10
 *          9
 *          8
 *          7
 *          4
 *          0   (no more Serial!!!)
 *          1
 *          
 * Analog   A0  referred to as  14
 *          A1                  15
 *          A2                  16
 *          A3                  17
 * 
 */

/*
 * 14 pin Common Cathode 4 digit + colon 7 segment display Pin Description
 * 
 *  1:  e         Anode 
 *  2:  d         Anode
 *  3:  dp        Anode
 *  4:  c         Anode
 *  5:  g         Anode
 *  6:  Digit 4   Cathode
 *  7:  colon     Cathode
 *  8:  Digit 1   Cathode
 *  9:  a         Anode
 * 10:  f         Anode
 * 11:  Digit 2   Cathode
 * 12:  Digit 3   Cathode
 * 13:  b         Anode
 * 14:  colon     Anode
 * 
 */

 


#ifndef _sevenseg_h
#define _sevenseg_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif


void init7seg(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t dp, uint8_t colAn);

void setMode(boolean colon);
void setOutput(uint8_t hour, uint8_t minute);

void multiplex();
void enable7seg();
void disable7seg();



#endif
