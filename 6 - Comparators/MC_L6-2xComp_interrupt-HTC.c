/************************************************************************
*                                                                       *
*   Filename:      MC_L6-2xComp_interrupt-HTC.c                         *
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
*   Description:    Lesson 6, example 9                                 *
*                                                                       *
*   Demonstrates use of interrupts with two comparators                 *
*   (assumes hysteresis is used to reduce triggering)                   *
*                                                                       *
*   Turns on: LED 1 when C1IN- < C2IN+,                                 *
*         and LED 2 when C2IN- < C2IN+                                  *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       C1IN- = input 1 (LDR/resistor divider)                          *
*       C2IN- = input 2 (LDR/resistor divider)                          *
*       C2IN+ = common reference (resistor divider)                     *
*       RC2   = indicator LED 1                                         *
*       RC3   = indicator LED 2                                         *
*                                                                       *
************************************************************************/

#include <xc.h>
#include <stdint.h>


/***** CONFIGURATION *****/
//  ext reset, no code or data protect, no brownout detect, 
//  no watchdog, power-up timer enabled, int clock with I/O,
//  no failsafe clock monitor, two-speed start-up disabled 
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & 
         WDTE_OFF & PWRTE_ON & FOSC_INTOSCIO &
         FCMEN_OFF & IESO_OFF);

// Pin assignments
#define sLED1   sPORTC.RC2      // indicator LED 1 (shadow)
#define sLED2   sPORTC.RC3      // indicator LED 2 (shadow)


/***** GLOBAL VARIABLES *****/
volatile union {                    // shadow copy of PORTC
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
    PORTC = 0;                  // start with LEDs off
    sPORTC.RC = 0;              //  (update shadow)
    TRISC = 0b000011;           // configure RC0/C2IN+ and RC1/C2IN- as inputs,
                                //   rest of PORTC as outputs

    // configure comparators
    CMCON0bits.CM = 0b110;      // select mode 6:
                                //   C1 -ref is C1IN-,
                                //   C1 +ref is C2IN+
                                //   C2 -ref is C2IN-,
                                //   C2 +ref is C2IN+,
                                //   both external outputs enabled,
                                //   both comparators on
    CMCON0bits.C1INV = 1;       // C1 output inverted 
                                //  -> C1OUT = 1 if C1IN- > C2IN+
    CMCON0bits.C2INV = 1;       // C2 output inverted 
                                //  -> C2OUT = 1 if C2IN- > C2IN+
                                // enable comparator external outputs:
    TRISAbits.TRISA2 = 0;       //   C1OUT (RA2)
    TRISCbits.TRISC4 = 0;       //   C2OUT (RC4)
                                
    // enable interrupts
    INTCONbits.PEIE = 1;        // enable peripheral interrupts
    ei();                       // enable global interrupts
                                // enable comparator interrupts:
    CMCON0;                     //   read CMCON0 to clear mismatch
    PIR1bits.C1IF = 0;          //   clear interrupt flags
    PIR1bits.C2IF = 0;
    PIE1bits.C1IE = 1;          //   set enable bits
    PIE1bits.C2IE = 1;

                                    
    /*** Main loop ***/
    for (;;)
    {
        // continually copy shadow PORTC to port
        PORTC = sPORTC.RC; 
    } 
}


/***** INTERRUPT SERVICE ROUTINE *****/
void interrupt isr(void)
{
    // Service all triggered interrupt sources
    
    if (PIR1bits.C1IF)
    {
        //*** Service Comparator 1 interrupt
        //
        //   Triggered on any comparator 1 output change,
        //   caused by C1IN- input crossing C2IN+ threshold
        //   
        CMCON0;                     // read CMCON0 to clear mismatch condition
        PIR1bits.C1IF = 0;          // clear interrupt flag
    
        // turn on LED 1 if C1IN- < C2IN+ 
        sLED1 = ~CMCON0bits.C1OUT;  // display inverse of comparator 1 output
    }
    
    if (PIR1bits.C2IF)
    {
        //*** Service Comparator 2 interrupt
        //
        //   Triggered on any comparator 2 output change,
        //   caused by C2IN- input crossing C2IN+ threshold
        //   
        CMCON0;                     // read CMCON0 to clear mismatch condition
        PIR1bits.C2IF = 0;          // clear interrupt flag
    
        // turn on LED 2 if C2IN- < C2IN+ 
        sLED2 = ~CMCON0bits.C2OUT;  // display inverse of comparator 2 output
    }
}
