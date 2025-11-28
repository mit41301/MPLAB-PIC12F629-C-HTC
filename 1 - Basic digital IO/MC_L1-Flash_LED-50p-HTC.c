/************************************************************************
*                                                                       *
*   Filename:      MC_L1-Flash_LED-50p-HTC.c                            *
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
*   Description:    Lesson 1, example 3                                 *
*                                                                       *
*   Flashes a LED at approx 1 Hz.                                       *
*   LED continues to flash until power is removed.                      *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = flashing LED                                              *
*                                                                       *
************************************************************************/

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ  4000000     // oscillator frequency for _delay()


//***** CONFIGURATION *****/
// ext reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);


/***** GLOBAL VARIABLES *****/
uint8_t     sGPIO = 0;          // shadow copy of GPIO


/***** MAIN PROGRAM *****/
void main()
{
    //*** Initialisation  
    
    // configure port
    TRISIO = ~(1<<1);           // configure GP1 (only) as an output


    //*** Main loop
    for (;;)
    {
        // toggle LED on GP1
        sGPIO ^= 1<<1;          // flip shadow bit corresponding to GP1
        GPIO = sGPIO;           // write to GPIO

        // delay 500 ms
        __delay_ms(500);

    }   // repeat forever
}

