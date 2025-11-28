/************************************************************************
*                                                                       *
*   Filename:      MC_L6-Comp_LED-intref-HTC.c                          *
*   Date:          23/6/12                                              *
*   File Version:  1.1                                                  *
*                                                                       *
*   Author:        David Meiklejohn                                     *
*   Company:       Gooligum Electronics                                 *
*                                                                       *
*************************************************************************
*                                                                       *
*   Architecture:  Mid-range PIC                                        *
*   Processor:     12F629                                               *
*   Compiler:      MPLAB XC8 v1.00 (Free mode)                          *
*                                                                       *
*************************************************************************
*                                                                       *
*   Files required: none                                                *
*                                                                       *
*************************************************************************
*                                                                       *
*   Description:    Lesson 6, example 2                                 *
*                                                                       *
*   Demonstrates basic use of programmable voltage reference            *
*                                                                       *
*   Turns on LED when voltage on CIN+ < 1.5 V                           *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       CIN+ = voltage to be measured (e.g. pot output or LDR)          *
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
#define LED     GPIObits.GP5    // indicator LED
#define nLED    5               // (port bit 5)


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/ 
    
    // configure port
    TRISIO = ~(1<<nLED);        // configure LED pin (only) as an output
    
    // configure comparator
    CMCONbits.CM = 0b110;       // select mode 6:
                                //   +ref is CVref,
                                //   no external output,
                                //   comparator on
    CMCONbits.CIS = 1;          // -ref is CIN+
    CMCONbits.CINV = 0;         // output not inverted
                                // -> COUT = 1 if CIN+ < CVref
                                
    // configure voltage reference
    VRCONbits.VRR = 1;          // select low range
    VRCONbits.VR = 7;           //   CVref = 0.292*Vdd
    VRCONbits.VREN = 1;         // enable voltage reference
                                // -> CVref = 1.5 V (if Vdd = 5.0 V) 

    
    /*** Main loop ***/
    for (;;)
    {
        // continually display comparator output
        LED = CMCONbits.COUT;             
    }
}
