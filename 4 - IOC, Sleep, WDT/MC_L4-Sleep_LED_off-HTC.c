/************************************************************************
*                                                                       *
*   Filename:      MC_L4-Sleep_LED_off-HTC.c                            *
*   Date:          10/6/12                                              *
*   File Version:  1.1                                                  *
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
*   Description:    Lesson 4, example 3                                 *
*                                                                       *
*   Demonstrates sleep mode                                             *
*                                                                       *
*   Turn on LED, wait for button pressed, turn off LED, then sleep      *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = indicator LED                                             *
*       GP3 = pushbutton (active low)                                   *
*                                                                       *
************************************************************************/

#include <xc.h>


/***** CONFIGURATION *****/
// int reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);

// Pin assignments
#define LED     GPIObits.GP1    // indicator LED on GP1
#define nLED    1               //   (port bit 1)
#define BUTTON  GPIObits.GP3    // pushbutton on GP3 (active low)


/***** MAIN PROGRAM *****/
void main()
{
    //***** Initialisation
    
    // configure port    
    TRISIO = ~(1<<nLED);        // configure LED pin (only) as output


    //***** Main code 
     
    // turn on LED    
    LED = 1;                  
    
    // wait for button press
    while (BUTTON == 1)         // wait until button low
        ;

    // go into standby (low power) mode
    LED = 0;                    // turn off LED
    SLEEP();                    // enter sleep mode

    for (;;)                    // (this loop should never execute)
        ;
}
