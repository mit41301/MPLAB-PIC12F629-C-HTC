/************************************************************************
*                                                                       *
*   Filename:      MC_L6-Comp_2input-HTC.c                              *
*   Date:          25/6/12                                              *
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
*   Description:    Lesson 6, example 7                                 *
*                                                                       *
*   Demonstrates comparator input multiplexing                          *
*                                                                       *
*   Turns on: LED 1 when CIN+ > 2.5 V                                   *
*         and LED 2 when CIN- > 2.5 V                                   *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       CIN+ = input 1 (LDR/resistor divider)                           *
*       CIN- = input 2 (LDR/resistor divider)                           *
*       GP4  = indicator LED 1                                          *
*       GP5  = indicator LED 2                                          *
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
#define sLED1   sGPIO.GP4       // indicator LED 1
#define sLED2   sGPIO.GP5       // indicator LED 2


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
                                //   -ref is CIN+ or CIN- (selected by CIS)
                                //   no external output,
                                //   comparator on
    CMCONbits.CINV = 1;         // inverted output
                                // -> COUT = 1 if -ref > CVref
                                
    // configure voltage reference
    VRCONbits.VRR = 1;          // select low range
    VRCONbits.VR = 12;          //   CVref = 0.500*Vdd
    VRCONbits.VREN = 1;         // enable voltage reference
                                // -> CVref = 2.5 V (if Vdd = 5.0 V) 
    
    
    /*** Main loop ***/
    for (;;)
    {
        // test input 1
        CMCONbits.CIS = 1;          // select CIN+ pin as -ref
        __delay_us(10);             // wait 10 us to settle
        sLED1 = CMCONbits.COUT;     // turn on LED1 if CIN+ > CVref
                    
        // test input 2
        CMCONbits.CIS = 0;          // select CIN- pin as -ref
        __delay_us(10);             // wait 10 us to settle
        sLED2 = CMCONbits.COUT;     // turn on LED2 if CIN- > CVref
            
        // display test results           
        GPIO = sGPIO.port;          // copy shadow GPIO to port
    }
}
