/************************************************************************
*                                                                       *
*   Filename:      MC_L6-Comp_LED-neg-HTC.c                             *
*   Date:          23/6/12                                              *
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
*   Files required: none                                                *
*                                                                       *
*************************************************************************
*                                                                       *
*   Description:    Lesson 6, example 1b                                *
*                                                                       *
*   Demonstrates use of comparator output inversion bit                 *
*                                                                       *
*   Turns on LED when voltage on CIN+ < voltage on CIN-                 *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       CIN+ = voltage to be measured (e.g. pot output or LDR)          *
*       CIN- = threshold voltage (set by voltage divider resistors)     *
*       GP5  = indicator LED                                            *
*                                                                       *
************************************************************************/

#include <xc.h>


/***** CONFIGURATION *****/
// ext reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);

// Pin assignments
#define LED     GPIObits.GP5    // indicator LED on GP5
#define nLED    5               //   (port bit 5)


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/ 
    
    // configure port
    TRISIO = ~(1<<nLED);        // configure LED pin (only) as an output
    
    // configure comparator
    CMCONbits.CM = 0b010;       // select mode 2:
                                //   +ref is CIN+, -ref is CIN-,
                                //   comparator on, no external output
    CMCONbits.CINV = 1;         // inverted output
                                // -> COUT = 1 if CIN+ < CIN-
    
    /*** Main loop ***/
    for (;;)
    {
        // continually display comparator output
        LED = CMCONbits.COUT;             
    }
}
