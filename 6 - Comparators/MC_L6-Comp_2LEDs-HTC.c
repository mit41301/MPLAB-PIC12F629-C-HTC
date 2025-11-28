/************************************************************************
*                                                                       *
*   Filename:      MC_L6-Comp_2LEDs-HTC.c                               *
*   Date:          25/6/12                                              *
*   File Version:  1.2                                                  *
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
*   Description:    Lesson 6, example 6                                 *
*                                                                       *
*   Demonstrates use of programmable voltage reference                  *
*   to test that a signal is within limits                              *
*                                                                       *
*   Turns on Low LED  when CIN+ < 2.0 V (low light level)               *
*         or High LED when CIN+ > 3.0 V (high light level)              *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       CIN+ = voltage to be measured (e.g. pot output or LDR)          *
*       GP5  = "Low" LED                                                *
*       GP4  = "High" LED                                               *
*                                                                       *
************************************************************************/

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ   4000000    // oscillator frequency for _delay()


/***** CONFIGURATION *****/
// ext reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);

// Pin assignments
#define sLO     sGPIO.GP5           // "Low" LED (shadow)
#define sHI     sGPIO.GP4           // "High" LED (shadow)


/***** GLOBAL VARIABLES *****/
volatile union {                    // shadow copy of GPIO
    uint8_t         port;
    struct {
        unsigned    GP0     : 1;
        unsigned    GP1     : 1;
        unsigned    GP2     : 1;
        unsigned    GP3     : 1;
        unsigned    GP4     : 1;
        unsigned    GP5     : 1;
    };
} sGPIO;


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/ 
    
    // configure port
    TRISIO = 0b001111;          // configure GP4 and GP5 as outputs
    
    // configure comparator
    CMCONbits.CM = 0b110;       // select mode 6:
                                //   +ref is CVref,
                                //   no external output,
                                //   comparator on
    CMCONbits.CIS = 1;          // -ref is CIN+
    CMCONbits.CINV = 1;         // inverted output
                                // -> COUT = 1 if CIN+ > CVref
                                
    // enable voltage reference
    VRCONbits.VREN = 1;        
                                
    
    /*** Main loop ***/
    for (;;)
    {
        // Test for low illumination
        // set low input threshold
        VRCONbits.VRR = 0;          // select high range
        VRCONbits.VR = 5;           //   CVref = 0.406*Vdd
                                    // -> CVref = 2.03 V (if Vdd = 5.0 V)        
        __delay_us(10);             // wait 10 us to settle
        
        // compare with input
        sLO = ~CMCONbits.COUT;      // turn on Low LED if CIN+ < CVref


        // Test for high illumination
        // set high input threshold
        VRCONbits.VRR = 0;          // select high range
        VRCONbits.VR = 11;          //   CVref = 0.594*Vdd
                                    // -> CVref = 2.97 V (if Vdd = 5.0 V) 
        __delay_us(10);             // wait 10 us to settle
        
        // compare with input
        sHI = CMCONbits.COUT;       // turn on High LED if CIN+ > CVref        
       
            
        // Display test results           
        GPIO = sGPIO.port;          // copy shadow GPIO to port
    }
}
