/************************************************************************
*                                                                       *
*   Filename:      MC_L5-Flash_LED_32k_XTAL-HTC.c                       *
*   Date:          20/6/12                                              *
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
*   Description:    Lesson 5, example 3                                 *
*                                                                       *
*   Demonstrates use of LP oscillator mode                              *
*   (using 32.768 kHz crystal)                                          *
*                                                                       *
*   LED flashes at 1 Hz (50% duty cycle),                               *
*   with timing derived from 8192 Hz instruction clock                  *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1         = flashing LED                                      *
*       OSC1, OSC2  = 32.768 kHz crystal                                *
*                                                                       *
************************************************************************/

#include <xc.h>
#include <stdint.h>


/***** CONFIGURATION *****/
// ext reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, LP oscillator
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_LP);

// Pin assignments
#define sFLASH  sGPIO.GP1           // flashing LED (shadow)


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
    TRISIO = ~(1<<1);               // configure GP1 (only) as an output
    
    // configure Timer0
    OPTION_REGbits.T0CS = 0;        // select timer mode
    OPTION_REGbits.PSA = 0;         // assign prescaler to Timer0
    OPTION_REGbits.PS = 0b100;      // prescale = 32
                                    // -> increment at 256 Hz with 8192 Hz inst clock
                
    /*** Main loop ***/
    for (;;)
    {
        // TMR0<7> cycles at 1 Hz, so continually copy to LED
        sFLASH = (TMR0 & 1<<7) != 0;    // sFLASH = TMR0<7>
        
        GPIO = sGPIO.port;              // copy shadow to GPIO
    }   
}
