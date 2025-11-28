/************************************************************************
*                                                                       *
*   Filename:      MC_L2-Timer_debounce-HTC.c                           *
*   Date:          8/6/12                                               *
*   File Version:  1.2                                                  *
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
*   Description:    Lesson 2, example 3a                                *
*                                                                       *
*   Demonstrates use of Timer0 to implement debounce counting algorithm *
*                                                                       *
*   Toggles LED when pushbutton is pressed then released                *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = flashing LED                                              *
*       GP3 = pushbutton switch (active low)                            *
*                                                                       *
************************************************************************/

#include <xc.h>
#include <stdint.h>


/***** CONFIGURATION *****/
// int reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);
         
// Pin assignments
#define sFLASH  sGPIO.GP1           // flashing LED (shadow)
#define BUTTON  GPIObits.GP3        // pushbutton


/***** GLOBAL VARIABLES *****/
union {                             // shadow copy of GPIO
    uint8_t         port;
    struct {
        unsigned    GP0     : 1;
        unsigned    GP1     : 1;
        unsigned    GP2     : 1;
        unsigned    GP3     : 1;
        unsigned    GP4     : 1;
        unsigned    GP5     : 1;
    };
} sGPIO;


/***** MAIN PROGRAM *****/
void main()
{
    //*** Initialisation
    
    // configure port
    GPIO = 0;                       // start with LED off
    sGPIO.port = 0;                 //   update shadow
    TRISIO = 0b111101;              // configure GP1 (only) as an output
    
    // configure Timer0
    OPTION_REGbits.T0CS = 0;        // select timer mode
    OPTION_REGbits.PSA = 0;         // assign prescaler to Timer0
    OPTION_REGbits.PS = 0b101;      // prescale = 64
                                    // -> increment every 64 us
             
    //*** Main loop 
    for (;;)
	{
        // wait for button press, debounce using timer0:
        TMR0 = 0;                   // reset timer
        while (TMR0 < 157)          // wait at least 10 ms (157 x 64 us = 10 ms)
            if (BUTTON == 1)        //   if button up,
                TMR0 = 0;           //     restart wait
        
        // toggle LED
        sFLASH = !sFLASH;           // toggle flashing LED (shadow)
        GPIO = sGPIO.port;          // write to GPIO

        // wait for button release, debounce using timer0:
        TMR0 = 0;                   // reset timer
        while (TMR0 < 157)          // wait at least 10ms (157 x 64us = 10 ms)
            if (BUTTON == 0)        //   if button down,
                TMR0 = 0;           //     restart wait
    }  
}
