/************************************************************************
*                                                                       *
*   Filename:      MC_L6-Comp_4input-HTC.c                              *
*   Date:          28/6/12                                              *
*   File Version:  1.1                                                  *
*                                                                       *
*   Author:        David Meiklejohn                                     *
*   Company:       Gooligum Electronics                                 *
*                                                                       *
*************************************************************************
*                                                                       *
*   Architecture:  Midrange PIC                                         *
*   Processor:     16F684                                               *
*   Compiler:      MPLAB XC8 v1.00 (Free mode)                          *
*                                                                       *
*************************************************************************
*                                                                       *
*   Files required: none                                                *
*                                                                       *
*************************************************************************
*                                                                       *
*   Description:    Lesson 6, example 8                                 *
*                                                                       *
*   Demonstrates dual comparator input multiplexing                     *
*                                                                       *
*   Turns on: LED 1 when C1IN+ > 2.5 V,                                 *
*             LED 2 when C1IN- > 2.5 V,                                 *
*             LED 3 when C2IN+ > 2.5 V,                                 *
*         and LED 4 when C2IN- > 2.5 V                                  *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       C1IN+ = input 1 (LDR/resistor divider)                          *
*       C2IN+ = input 2 (LDR/resistor divider)                          *
*       C2IN- = input 3 (LDR/resistor divider)                          *
*       C1IN- = input 4 (LDR/resistor divider)                          *
*       RC3   = indicator LED 1                                         *
*       RC2   = indicator LED 2                                         *
*       RA4   = indicator LED 3                                         *
*       RA5   = indicator LED 4                                         *
*                                                                       *
************************************************************************/

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ   4000000    // oscillator frequency for _delay()


/***** CONFIGURATION *****/
//  ext reset, no code or data protect, no brownout detect, 
//  no watchdog, power-up timer enabled, int clock with I/O,
//  no failsafe clock monitor, two-speed start-up disabled 
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & 
         WDTE_OFF & PWRTE_ON & FOSC_INTOSCIO &
         FCMEN_OFF & IESO_OFF);

// Pin assignments
#define sLED1   sPORTC.RC3      // indicator LED 1 (shadow)
#define sLED2   sPORTC.RC2      // indicator LED 2 (shadow)
#define sLED3   sPORTA.RA4      // indicator LED 3 (shadow)
#define sLED4   sPORTA.RA5      // indicator LED 4 (shadow) 


/***** GLOBAL VARIABLES *****/
union {                         // shadow copy of PORTA
    uint8_t         RA;
    struct {
        unsigned    RA0     : 1;
        unsigned    RA1     : 1;
        unsigned    RA2     : 1;
        unsigned    RA3     : 1;
        unsigned    RA4     : 1;
        unsigned    RA5     : 1;
    };
} sPORTA;

union {                         // shadow copy of PORTC
    uint8_t         RC;
    struct {
        unsigned    RC0     : 1;
        unsigned    RC1     : 1;
        unsigned    RC2     : 1;
        unsigned    RC3     : 1;
        unsigned    RC4     : 1;
        unsigned    RC5     : 1;
    };
} sPORTC;


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/ 
    
    // configure ports
    TRISA = 0b000011;           // configure RA0/C1IN+ and RA1/C1IN- as inputs,
                                //   rest of PORTA as outputs    
    TRISC = 0b000011;           // configure RC0/C2IN+ and RC1/C2IN- as inputs,
                                //   rest of PORTC as outputs
    
    // configure comparators
    CMCON0bits.CM = 0b010;      // select mode 2:
                                //   C1 +ref is CVref,
                                //   C1 -ref is C1IN+ or C1IN- (selected by CIS),
                                //   C2 +ref is CVref,
                                //   C2 -ref is C2IN+ or C2IN- (selected by CIS),
                                //   no external outputs,
                                //   both comparators on
    CMCON0bits.C1INV = 1;       // C1 output inverted 
                                // -> C1OUT = 1 if C1 -ref > CVref
    CMCON0bits.C2INV = 1;       // C2 output inverted 
                                // -> C2OUT = 1 if C2 -ref > CVref                            
                                
    // configure voltage reference
    VRCONbits.VRR = 1;          // select low range
    VRCONbits.VR = 12;          //   CVref = 0.500*Vdd
    VRCONbits.VREN = 1;         // enable voltage reference
                                // -> CVref = 2.5 V (if Vdd = 5.0 V) 
    
    
    /*** Main loop ***/
    for (;;)
    {
        // test inputs 1 (CIN1+) and 2 (C2IN+)
        CMCON0bits.CIS = 1;         // select C1IN+ and C2IN+ pins as -refs
        __delay_us(10);             // wait 10us to settle
        sLED1 = CMCON0bits.C1OUT;   // turn on LED1 if C1IN+ > CVref
        sLED2 = CMCON0bits.C2OUT;   // turn on LED2 if C2IN+ > CVref
                    
        // test inputs 3 (C2IN-) and 4 (C1IN-)
        CMCON0bits.CIS = 0;         // select C1IN- and C2IN- pins as -refs
        __delay_us(10);             // wait 10us to settle
        sLED3 = CMCON0bits.C2OUT;   // turn on LED2 if C2IN- > CVref
        sLED4 = CMCON0bits.C1OUT;   // turn on LED3 if C1IN- > CVref 
                        
        // display test results           
        PORTA = sPORTA.RA;          // copy shadow registers to ports
        PORTC = sPORTC.RC;
    }
}
