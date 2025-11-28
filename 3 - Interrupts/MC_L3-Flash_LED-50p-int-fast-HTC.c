/************************************************************************
*                                                                       *
*   Filename:      MC_L3-Flash_LED-50p-int-fast-HTC.c                   *
*   Date:          8/6/12                                               *
*   File Version:  1.3                                                  *
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
*   Description:    Lesson 3, example 1a                                *
*                                                                       *
*   Demonstrates use of Timer0 interrupt to perform a background task   *
*                                                                       *
*   Flash LED at approx 7.6 Hz (50% duty cycle)                         *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP2 = flashing LED                                              *
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
#define sF_LED  sGPIO.GP2           // flashing LED (shadow)


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
    TRISIO = ~(1<<2);               // configure GP2 (only) as an output

    // configure Timer0
    OPTION_REGbits.T0CS = 0;        // select timer mode
    OPTION_REGbits.PSA = 0;         // assign prescaler to Timer0
    OPTION_REGbits.PS = 0b111;      // prescale = 256
                                    // -> increment every 256 us    

    // enable interrupts
    INTCONbits.T0IE = 1;            // enable Timer0 interrupt
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
    //*** Service Timer0 interrupt
    //
    //  TMR0 overflows every 65.5 ms
    //
    //  Flashes LED at ~7.6 Hz by toggling on each interrupt
    //      (every ~65.5 ms)
    //
    //  (only Timer0 interrupts are enabled)
    //    
    INTCONbits.T0IF = 0;            // clear interrupt flag
    
    // toggle LED
    sF_LED = ~sF_LED;               // (via shadow register)
}
