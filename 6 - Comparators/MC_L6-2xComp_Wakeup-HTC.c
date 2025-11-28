/************************************************************************
*                                                                       *
*   Filename:      MC_L6-2xComp_Wakeup-HTC.c                            *
*   Date:          28/6/12                                              *
*   File Version:  1.1                                                  *
*                                                                       *
*   Author:        David Meiklejohn                                     *
*   Company:       Gooligum Electronics                                 *
*                                                                       *
*************************************************************************
*                                                                       *
*   Architecture:  Mid-range PIC                                        *
*   Processor:     16F684                                               *
*   Compiler:      MPLAB XC8 v1.00 (Free mode)                          *
*                                                                       *
*************************************************************************
*                                                                       *
*   Files required: none                                                *
*                                                                       *
*************************************************************************
*                                                                       *
*   Description:    Lesson 6, example 10                                *
*                                                                       *
*   Demonstrates wake-up on dual comparator change                      *
*                                                                       *
*   Device wakes when input crosses a low or high threshold             * 
*                                                                       *
*   Turns on Low  LED when C2IN+ < C1IN-                                *
*         or High LED when C2IN+ > C2IN-                                *
*   then sleeps until the next change                                   *
*                                                                       *
*   (thresholds are set by external voltage dividers)                   *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       C2IN+ = voltage to be measured (e.g. pot output or LDR)         *
*       C1IN- = low threshold (resistor divider)                        *
*       C2IN- = high threshold (resistor divider)                       *
*       RC3   = "Low" lED                                               *
*       RC2   = "High" LED                                              *
*                                                                       *
************************************************************************/

#include <xc.h>

#define _XTAL_FREQ   4000000    // oscillator frequency for _delay()


/***** CONFIGURATION *****/
//  ext reset, no code or data protect, no brownout detect, 
//  no watchdog, power-up timer enabled, int clock with I/O,
//  no failsafe clock monitor, two-speed start-up disabled 
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & 
         WDTE_OFF & PWRTE_ON & FOSC_INTOSCIO &
         FCMEN_OFF & IESO_OFF);

// Pin assignments
#define LO      PORTCbits.RC3   // "Low" LED
#define nLO     3               // (port bit 3)
#define HI      PORTCbits.RC2   // "High" LED
#define nHI     2               // (port bit 2)


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/
    
    // configure ports
    PORTC = 0;                  // start with both LEDs off
    TRISC = ~(1<<nLO|1<<nHI);   // configure LED pins (only) as outputs
    
    // configure comparators
    CMCON0bits.CM = 0b011;      // select mode 3:
                                //   C1 -ref is C1IN-,
                                //   C1 +ref is C2IN+,
                                //   C2 -ref is C2IN-,
                                //   C2 +ref is C2IN+,
                                //   no external outputs,
                                //   both comparators on
    CMCON0bits.C1INV = 1;       // C1 output inverted
                                //  -> C1OUT = 1 if C2IN+ < C1IN-
    CMCON0bits.C2INV = 0;       // C2 output not inverted 
                                //  -> C2OUT = 1 if C2IN+ > C2IN-
 
    // enable comparator interrupts (for wake on change)
    INTCONbits.PEIE = 1;        // enable peripheral interrupts
    PIE1bits.C1IE = 1;          // and comparator interrupts
    PIE1bits.C2IE = 1;

                                    
    /*** Main loop ***/
    for (;;)
    {
        // test for and wait while input low
        while (CMCON0bits.C1OUT)    // while C2IN+ < C1IN-
        {
            LO = 1;             //   turn on "low" LED
            __delay_us(5);      //   short delay to settle
        }

        // test for and wait while input high
        while (CMCON0bits.C2OUT)    // while C2IN+ > C2IN-
        {
            HI = 1;             //   turn on "high" LED
            __delay_us(5);      //   short delay to settle
        }
            
        // turn off LEDs
        PORTC = 0;
        
        // go into standby (low power) mode
        CMCON0;                 // read CMCON0 to clear comparator mismatch
        PIR1bits.C1IF = 0;      // clear comparator interrupt flags
        PIR1bits.C2IF = 0;
        SLEEP();                // enter sleep mode
    }
}

