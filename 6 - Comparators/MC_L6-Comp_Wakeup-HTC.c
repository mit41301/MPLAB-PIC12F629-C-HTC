/************************************************************************
*                                                                       *
*   Filename:      MC_L6-Comp_Wakeup-HTC.c                              *
*   Date:          24/6/12                                              *
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
*   Description:    Lesson 6, example 5                                 *
*                                                                       *
*   Demonstrates wake-up on comparator change                           *
*                                                                       *
*   Turns on LED for 1 sec when comparator output changes               *
*   then sleeps until the next change                                   *
*   (threshold is 1.5 V, set by programmable voltage ref,               *
*    with hysteresis used to reduce sensitivity to small changes)       *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       CIN+ = voltage to be measured (e.g. pot output or LDR)          *
*       COUT = comparator output (fed back to input via resistor)       *
*       GP5  = indicator LED                                            *
*                                                                       *
************************************************************************/

#include <xc.h>

#define _XTAL_FREQ   4000000    // oscillator frequency for _delay()


/***** CONFIGURATION *****/
// ext reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);

// Pin assignments
#define LED     GPIObits.GP5    // indicator LED
#define nLED    5               // (port bit 5)


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/
    
    // configure port
    TRISIO = ~(1<<nLED);        // configure LED pin (only) as an output
    
    // configure comparator
    CMCONbits.CM = 0b101;       // select mode 5:
                                //   +ref is CVref,
                                //   external output enabled,
                                //   comparator on
    CMCONbits.CIS = 1;          // -ref is CIN+
    CMCONbits.CINV = 1;         // inverted output 
                                // -> COUT = 1 if CIN+ > CVref,
                                //    COUT pin enabled
    TRISIObits.TRISIO2 = 0;     // configure COUT (GP2) as an output
                                
    // configure voltage reference
    VRCONbits.VRR = 1;          // select low range
    VRCONbits.VR = 7;           //   CVref = 0.292*Vdd
    VRCONbits.VREN = 1;         // enable voltage reference
                                // -> CVref = 1.5 V (if Vdd = 5.0 V) 
 
    // delay 10 ms to allow comparator and voltage ref to settle
    __delay_ms(10);
       
    // enable comparator interrupt (for wake on change)
    INTCONbits.PEIE = 1;        // enable peripheral interrupts
    PIE1bits.CMIE = 1;          // enable comparator interrupt

                                    
    /*** Main loop ***/
    for (;;)
    {
        // turn off LED
        LED = 0;
        
        // go into standby (low power) mode
        CMCON;                  // read CMCON to clear comparator mismatch
        PIR1bits.CMIF = 0;      // clear comparator interrupt flag
        SLEEP();                // enter sleep mode
        
        // turn on LED for 1 second
        LED = 1;                // turn on LED
        __delay_ms(1000);       // delay 1 sec
    } 
}

