/************************************************************************
*                                                                       *
*   Filename:      MC_L5-IntRC+CLKOUT-HTC.c                             *
*   Date:          19/6/12                                              *
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
*   Description:    Lesson 5, example 1                                 *
*                                                                       *
*   Demonstrates CLKOUT function in Internal RC oscillator mode         *
*                                                                       *
*   Toggles a pin as quickly as possible                                *  
*   for comparison with 1 MHz CLKOUT signal                             *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP2    = fast-changing output                                   *
*       CLKOUT = 1 MHz clock output                                     *
*                                                                       *
************************************************************************/

#include <xc.h>


/***** CONFIGURATION *****/
// ext reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, 4 Mhz int clock with CLKOUT
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCCLK);

// Pin assignments
#define OUT     GP2         // fast-changing output


/***** MAIN PROGRAM *****/
void main()
{
    /*** Initialisation ***/
    
    // configure port
    TRISIO = 0;             // configure all pins (except GP3 and GP4/CLKOUT)
                            //   as outputs
    
    /*** Main loop ***/
    for (;;)  
    {              
        OUT = ~OUT;         // toggle output pin as fast as possible
    }
}
