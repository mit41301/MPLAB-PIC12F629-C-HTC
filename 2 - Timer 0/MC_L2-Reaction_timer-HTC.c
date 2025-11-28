/************************************************************************
*                                                                       *
*   Filename:      MC_L2-Reaction_timer-HTC.c                           *
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
*   Description:    Lesson 2, example 1                                 *
*                   Reaction Timer game.                                *
*                                                                       *
*   User must attempt to press button within defined reaction time      *
*   after "start" LED lights.  Success is indicated by "success" LED.   *
*                                                                       *
*       Starts with both LEDs unlit.                                    *
*       2 sec delay before lighting "start"                             *
*       Waits up to 1 sec for button press                              *
*       (only) on button press, lights "success"                        *
*       1 sec delay before repeating from start                         *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = success LED                                               *
*       GP2 = start LED                                                 *
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

// Pin assignments
#define START   GPIObits.GP2        // LEDs
#define SUCCESS GPIObits.GP1

#define BUTTON  GPIObits.GP3        // pushbutton


/***** CONSTANTS *****/
#define MAXRT   200                 // Maximum reaction time in ms


/***** MAIN PROGRAM *****/
void main()
{
    uint8_t     cnt_8ms;            // counter: increments every 8 ms

    //*** Initialisation  
    
    // configure port
    TRISIO = 0b111001;              // configure GP1 and GP2 as outputs
    
    // configure Timer0
    OPTION_REGbits.T0CS = 0;        // select timer mode
    OPTION_REGbits.PSA = 0;         // assign prescaler to Timer0
    OPTION_REGbits.PS = 0b100;      // prescale = 32
                                    // -> increment every 32 us

    //*** Main loop                                
    for (;;)
    {
        // start with both LEDs off
        GPIO = 0;                   

        // delay 2 sec
        __delay_ms(2000);           // delay 2000 ms             

        // indicate start 
        START = 1;                  // turn on start LED

        // wait up to 1 sec for button press
        cnt_8ms = 0;
        while (BUTTON == 1 && cnt_8ms < 1000/8)
        {
            TMR0 = 0;               // clear timer0
            while (TMR0 < 8000/32)  // wait for 8 ms (32 us/tick)
                ;
            ++cnt_8ms;              // increment 8 ms counter
        }
        
        // check elapsed time
        if (cnt_8ms < MAXRT/8)      // if time < max reaction time (8 ms/count)
            SUCCESS = 1;            //   turn on success LED

        // delay 1 sec
        __delay_ms(1000);           // delay 1000 ms     
    }  
}
