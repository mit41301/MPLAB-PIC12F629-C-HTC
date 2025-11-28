/************************************************************************
*                                                                       *
*   Filename:      MC_L1-Toggle_LED-HTC.c                               *
*   Date:          2/8/12                                               *
*   File Version:  1.3                                                  *
*                                                                       *
*   Author:        David Meiklejohn                                     *
*   Company:       Gooligum Electronics                                 *
*                                                                       *
*************************************************************************
*                                                                       *
*   Architecture:  Midrange PIC                                         *
*   Processor:     12F629                                               *
*   Compiler:      MPLAB XC8 v1.01 (Free mode)                          *
*                                                                       *
*************************************************************************
*                                                                       *
*   Files required: none                                                *
*                                                                       *
*************************************************************************
*                                                                       *
*   Description:    Lesson 1, example 5                                 *
*                                                                       *
*   Demonstrates use of counting algorithm for debouncing               *
*                                                                       *
*   Toggles LED when pushbutton is pressed then released,               *
*   using a counting algorithm to debounce switch                       *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = indicator LED                                             *
*       GP3 = pushbutton switch (active low)                            *
*                                                                       *
************************************************************************/

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ  4000000     // oscillator frequency for _delay()


/***** CONFIGURATION *****/
// int reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);


/***** GLOBAL VARIABLES *****/
uint8_t     sGPIO;                  // shadow copy of GPIO


/***** MAIN PROGRAM *****/
void main()
{
    uint8_t     db_cnt;             // debounce counter

    //*** Initialisation  
    
    // configure port
    GPIO = 0;                       // start with LED off
    sGPIO = 0;                      //   update shadow
    TRISIO = ~(1<<1);               // configure GP1 (only) as an output


    //*** Main loop
    for (;;)
    {
        // wait for button press, debounce by counting:
        for (db_cnt = 0; db_cnt <= 10; db_cnt++)
        {
            __delay_ms(1);          // sample every 1 ms
            if (GPIObits.GP3 == 1)  // if button up (GP3 high)
                db_cnt = 0;         //   restart count
        }                           // until button down for 10 successive reads

        // toggle LED on GP1
        sGPIO ^= 0b000010;          // toggle shadow GP1
        GPIO = sGPIO;               // write to GPIO

        // wait for button release, debounce by counting:
        for (db_cnt = 0; db_cnt <= 10; db_cnt++)
        {
            __delay_ms(1);          // sample every 1 ms
            if (GPIObits.GP3 == 0)  // if button down (GP3 low)
                db_cnt = 0;         //   restart count
        }                           // until button up for 10 successive reads
    } 
}
