/************************************************************************
*                                                                       *
*   Filename:      MC_L4-WDTdemo+clr-HTC.c                              *
*   Date:          10/6/12                                              *
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
*   Description:    Lesson 4, example 5b                                *
*                                                                       *
*   Demonstrates clearing the watchdog timer                            *
*                                                                       *
*   Turn on LED for 1 s, turn off, then enter endless loop,             *
*   in which the WDT is repeatedly cleared, so LED stays off,           *
*   even if the watchdog is enabled                                     *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = indicator LED                                             *
*                                                                       *
************************************************************************/

#include <xc.h>

#define _XTAL_FREQ  4000000     // oscillator frequency for _delay()


/***** CONFIGURATION *****/
#define     WATCHDOG            // define to enable watchdog timer

#ifdef WATCHDOG
    // ext reset, no code or data protect, no brownout detect,
    // watchdog, power-up timer enabled, int RC clock
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF &  
         WDTE_ON & PWRTE_OFF & FOSC_INTRCIO);
#else
    // ext reset, no code or data protect, no brownout detect,
    // no watchdog, power-up timer enabled, int RC clock
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF &  
         WDTE_OFF & PWRTE_OFF & FOSC_INTRCIO);
#endif   

// Pin assignments
#define LED     GPIObits.GP1    // indicator LED on GP1
#define n_LED   1               //   (port bit 1)


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/
    
    // configure port
    TRISIO = ~(1<<n_LED);           // configure LED pin (only) as output
    
    // configure watchdog timer
    OPTION_REGbits.PSA = 1;             // assign prescaler to WDT
    OPTION_REGbits.PS = 0b111;          // prescale = 128
                                        // -> WDT timeout = 2.3 s
 
    
    /*** Main code ***/
    
    LED = 1;                    // turn on LED
    
    __delay_ms(1000);           // delay 1 sec
   
    LED = 0;                    // turn off LED 
    
    for (;;)                    // repeatedly clear watchdog timer forever
        CLRWDT(); 
}

