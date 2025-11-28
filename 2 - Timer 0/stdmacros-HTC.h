/************************************************************************
*                                                                       *
*   Filename:      stdmacros-HTC.h                                      *
*   Date:          8/6/12                                               *
*   File Version:  1.2                                                  *
*                                                                       *
*   Author:        David Meiklejohn                                     *
*   Company:       Gooligum Electronics                                 *
*                                                                       *
*************************************************************************
*                                                                       *
*   Architecture:  Baseline or Midrange PIC                             *
*   Processor:     any                                                  *
*   Compiler:      MPLAB XC8 v1.00 (Free mode)                          *
*                                                                       *
*************************************************************************
*                                                                       *
*   Files required: none                                                *
*                                                                       *
*************************************************************************
*                                                                       *
*   Description:    Library of useful macros                            *
*                                                                       *
*   DbnceHi - debounce switch, wait for high input                      *
*   DbnceLo - debounce switch, wait for low input                       *
*                                                                       *
************************************************************************/


/***** DEBOUNCE MACROS *****/

#define DEBOUNCE 10*1000/256    // switch debounce count = 10 ms/(256us/tick)

// DbnceHi()
//
// Debounce switch on given input pin
// Waits for switch input to be high continuously for DEBOUNCE*256/1000 ms
//
// Uses: TMR0       Assumes: TMR0 running at 256 us/tick
//
#define DbnceHi(PIN) TMR0 = 0;                  /* reset timer              */ \
                     while (TMR0 < DEBOUNCE)    /* wait until debounce time */ \
                         if (PIN == 0)          /*   if input low,          */ \
                             TMR0 = 0           /*     restart wait         */


// DbnceLo()
//
// Debounce switch on given input pin
// Waits for switch input to be low continuously for DEBOUNCE*256/1000 ms
//
// Uses: TMR0       Assumes: TMR0 running at 256 us/tick
//
#define DbnceLo(PIN) TMR0 = 0;                  /* reset timer              */ \
                     while (TMR0 < DEBOUNCE)    /* wait until debounce time */ \
                         if (PIN == 1)          /*   if input high,         */ \
                             TMR0 = 0           /*     restart wait         */
                             
