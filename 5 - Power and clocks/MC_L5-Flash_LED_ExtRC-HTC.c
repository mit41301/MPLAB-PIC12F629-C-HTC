/************************************************************************
*                                                                       *
*   Filename:      MC_L5-Flash_LED_ExtRC-HTC.c                          *
*   Date:          20/6/12                                              *
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
*   Description:    Lesson 5, example 5                                 *
*                                                                       *
*   Demonstrates use of external RC oscillator (~1 kHz)                 *
*                                                                       *
*   LED on GP1 flashes at approx 1 Hz (50% duty cycle),                 *
*   with timing derived from ~256 Hz instruction clock                  *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1  = flashing LED                                             *
*       OSC1 = R (10k) / C (82n)                                        *
*                                                                       *
************************************************************************/

#include <xc.h>


/***** CONFIGURATION *****/
// ext reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, ext RC oscillator (~ 1kHz) + clkout
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_EXTRCCLK);


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/
    
    // configure port
    TRISIO = ~(1<<1);               // configure GP1 (only) as an output

    // configure Timer0
    OPTION_REGbits.T0CS = 0;        // select timer mode
    OPTION_REGbits.PSA = 0;         // assign prescaler to Timer0
    OPTION_REGbits.PS = 0b101;      // prescale = 64
                                    //  -> increment at 4 Hz with 256 Hz inst clock
                
    /*** Main loop ***/
    for (;;)
    {
        // TMR0<1> cycles at 1 Hz, so continually copy to LED (GP1)
        GPIO = TMR0;                // copy TMR0 to GPIO
    } 
}
