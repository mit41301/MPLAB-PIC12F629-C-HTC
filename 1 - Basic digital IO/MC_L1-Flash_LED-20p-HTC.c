/************************************************************************
*                                                                       *
*   Filename:      MC_L1-Flash_LED-20p-HTC.c                            *
*   Date:          8/6/12                                               *
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
*   Description:    Lesson 1, example 2                                 *
*                                                                       *
*   Flashes an LED at approx 1 Hz, with 20% duty cycle                  *
*   LED continues to flash until power is removed.                      *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = flashing LED                                              *
*                                                                       *
************************************************************************/

#include <xc.h>

#define _XTAL_FREQ  4000000     // oscillator frequency for _delay()


//***** CONFIGURATION *****/
// ext reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);


/***** MAIN PROGRAM *****/
void main()
{
    //*** Initialisation  
    
    // configure port
    TRISIO = 0b111101;          // configure GP1 (only) as an output


    //*** Main loop
    for (;;)
    {
        GPIO = 0b000010;        // turn on LED on GP1 (bit 1)
        
        __delay_ms(200);        // stay on for 200 ms

        GPIO = 0;               // turn off LED (clearing GPIO clears GP1)

        __delay_ms(800);        // stay off for 800 ms

    }                           // repeat forever
}
