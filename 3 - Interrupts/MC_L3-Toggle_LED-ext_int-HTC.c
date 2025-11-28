/************************************************************************
*                                                                       *
*   Filename:      MC_L3-Toggle_LED-ext_int-HTC.c                       *
*   Date:          9/6/12                                               *
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
*   Files required: none                                                *
*                                                                       *
*************************************************************************
*                                                                       *
*   Description:    Lesson 3, example 5                                 *
*                                                                       *
*   Demonstrates use of external interrupt (INT pin)                    *
*                                                                       *
*   Toggles LED when pushbutton on INT is pressed                       *
*    (high -> low transition)                                           *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = indicator LED                                             *
*       INT = pushbutton (active low)                                   *
*                                                                       *
************************************************************************/

#include <xc.h>
#include <stdint.h>


/***** CONFIGURATION *****/
// ext reset, no code protect, no brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);

// Pin assignments
#define sB_LED  sGPIO.GP1           // indicator LED (shadow)


/***** GLOBAL VARIABLES *****/
volatile union {                    // shadow copy of GPIO
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
    GPIO = 0;                       // start with all LEDs off
    sGPIO.port = 0;                 //   update shadow
    TRISIO = ~(1<<1);               // configure GP1 (only) as an output
    
    // configure external interrupt
    OPTION_REGbits.INTEDG = 0;      // trigger on falling edge 
    
    // enable interrupts
    INTCONbits.INTE = 1;            // enable external interrupt
    ei();                           // enable global interrupts

                                    
    //*** Main loop
    for (;;)
    {
        // continually copy shadow GPIO to port
        GPIO = sGPIO.port; 
            
    }   // repeat forever
}


/***** INTERRUPT SERVICE ROUTINE *****/
void interrupt isr(void)
{
    //*** Service external interrupt
    //
    //  Triggered on high -> low transition on INT pin
    //  caused by externally debounced pushbutton press
    // 
    //  Toggles LED on every high -> low transition
    //  
    //  (only external interrupts are enabled)
    //   
    INTCONbits.INTF = 0;            // clear interrupt flag
    
    // toggle LED
    sB_LED = ~sB_LED;               // (via shadow register)
}
