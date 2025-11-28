/************************************************************************
*                                                                       *
*   Filename:      MC_L4-Wakeup-HTC.c                                   *
*   Date:          10/6/12                                              *
*   File Version:  1.1                                                  *
*                                                                       *
*   Author:        David Meiklejohn                                     *
*   Company:       Gooligum Electronics                                 *
*                                                                       *
*************************************************************************
*                                                                       *
*   Architecture:  Midrange PIC                                         *
*   Processor:     12F629                                               *
*   Compiler:      MPLAB XC8 v1.00 (Free mode)                          *
*                                                                       *
*************************************************************************
*                                                                       *
*   Files required: stdmacros-HTC.h     (provides debounce macros)      *
*                                                                       *
*************************************************************************
*                                                                       *
*   Description:    Lesson 4, example 4                                 *
*                                                                       *
*   Demonstrates use of interrupt-on-change for wake-up from sleep mode *
*                                                                       *
*   Toggle LED, sleeping while LED is off:                              *
*       Turn on LED, debounce pushbutton then wait for button press     *
*       turn off LED, debounce pushbutton, sleep then repeat            *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = on/off indicator LED                                      *
*       GP3 = pushbutton (active low)                                   *
*                                                                       *
************************************************************************/

#include <xc.h>

#include "stdmacros-HTC.h"  // DbnceHi() - debounce switch, wait for high
                            // Requires: TMR0 at 256 us/tick


/***** CONFIGURATION *****/
// int reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);

// Pin assignments
#define LED     GPIObits.GP1    // on/off indicator LED on GP1
#define nLED    1               //   (port bit 1)
#define BUTTON  GPIObits.GP3    // pushbutton on GP3 (active low)
#define nBUTTON 3               //   (port bit 3)


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/
    
    // configure port
    TRISIO = ~(1<<nLED);        // configure LED pin (only) as output

    // configure Timer0 (for DbnceHi() macro)
    OPTION_REGbits.T0CS = 0;        // select timer mode
    OPTION_REGbits.PSA = 0;         // assign prescaler to Timer0
    OPTION_REGbits.PS = 0b111;      // prescale = 256
                                    // -> increment every 256 us
                                    
    // configure interrupt-on-change
    IOC |= 1<<nBUTTON;          // enable IOC on pushbutton input
    INTCONbits.GPIE = 1;        // enable wake-up (interrupt) on port change

    
    /*** Main loop ***/
    for (;;)
    {
        // turn on LED
        LED = 1;                    
    
        // wait for stable button high
        // (in case it is still bouncing after wakeup)
        DbnceHi(BUTTON);            
    
        // wait for button press
        while (BUTTON == 1)         // wait until button low
            ;

        // go into standby (low power) mode
        LED = 0;                    // turn off LED
        DbnceHi(BUTTON);            // wait for stable button release
        INTCONbits.GPIF = 0;        // clear port change interrupt flag
        SLEEP();                    // enter sleep mode
    }
}
