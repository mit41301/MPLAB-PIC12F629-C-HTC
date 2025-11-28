/************************************************************************
*                                                                       *
*   Filename:      MC_L4-WDTdemo+LED-HTC.c                              *
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
*   Description:    Lesson 4, example 5a                                *
*                                                                       *
*   Demonstrates watchdog timer                                         *
*       plus differentiation of WDT time-out from POR reset             *
*                                                                       *
*   Turn on LED for 1 s, turn off, then enter endless loop              *
*   If WDT enabled, processor resets after 2.3s                         *
*   Turns on WDT LED to indicate WDT reset                              *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = flashing LED                                              *
*       GP2 = WDT-reset indicator LED                                   *
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
#define F_LED   GPIObits.GP1    // "flashing" LED on GP1
#define nF_LED  1               //   (port bit 1)
#define W_LED   GPIObits.GP2    // WDT LED to indicate WDT time-out reset
#define nW_LED  2               //   (port bit 2)


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/
    
    // configure port
    TRISIO = ~(1<<nF_LED|1<<nW_LED);    // configure LED pins as outputs
    
    // configure watchdog timer
    OPTION_REGbits.PSA = 1;             // assign prescaler to WDT
    OPTION_REGbits.PS = 0b111;          // prescale = 128
                                        // -> WDT timeout = 2.3 s

    
    /*** Main code ***/
    
    // test for WDT-timeout reset
    if (!STATUSbits.nTO)        // if WDT timeout has occurred,
        W_LED = 1;              //   turn on "error" LED

    // flash LED
    F_LED = 1;                  // turn on "flash" LED
    __delay_ms(1000);           // delay 1 sec
    F_LED = 0;                  // turn off "flash" LED 
    
    // wait forever
    for (;;)                    
        ;
}
