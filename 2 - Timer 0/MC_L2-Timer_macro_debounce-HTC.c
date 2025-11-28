/************************************************************************
*                                                                       *
*   Filename:      MC_L2-Timer_macro_debounce-HTC.c                     *
*   Date:          10/6/12                                              *
*   File Version:  1.3                                                  *
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
*   Files required: stdmacros-HTC.h     (provides debounce macros)      *
*                                                                       *
*************************************************************************
*                                                                       *
*   Description:    Lesson 2, example 3b                                *
*                                                                       *
*   Demonstrates use of Timer0-based debounce macros                    *
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

#include "stdmacros-HTC.h"  // DbnceHi() - debounce switch, wait for high
                            // DbnceLo() - debounce switch, wait for low
                            // Requires: TMR0 at 256 us/tick


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
    /*** Initialisation ***/
    
    // configure port
    GPIO = 0;                       // start with LED off
    sGPIO.port = 0;                 //   update shadow
    TRISIO = 0b111101;              // configure GP1 (only) as an output
    
    // configure Timer0 (for DbnceHi() macro)
    OPTION_REGbits.T0CS = 0;        // select timer mode
    OPTION_REGbits.PSA = 0;         // assign prescaler to Timer0
    OPTION_REGbits.PS = 0b111;      // prescale = 256
                                    // -> increment every 256 us
                                    
             
    /*** Main loop ***/
    for (;;)
    {
        // wait for button press 
        while (BUTTON == 1)         // wait until button low
            ;
        
        // toggle LED
        sFLASH = !sFLASH;           // toggle flashing LED (shadow)
        GPIO = sGPIO.port;          // write to GPIO

        // wait for button release
        DbnceHi(BUTTON);            // wait until button high (debounced)
    }  
}
