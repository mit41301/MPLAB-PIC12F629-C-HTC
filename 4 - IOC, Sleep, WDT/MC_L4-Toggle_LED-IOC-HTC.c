/************************************************************************
*                                                                       *
*   Filename:      MC_L4-Toggle_LED-IOC-HTC.c                           *
*   Date:          10/6/12                                              *
*   File Version:  1.4                                                  *
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
*   Description:    Lesson 4, example 1                                 *
*                                                                       *
*   Demonstrates use of interrupt-on-change interrupts                  *
*   (without software debouncing)                                       *
*                                                                       *
*   Toggles LED when pushbutton is pressed (high -> low transition)     *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = indicator LED                                             *
*       GP2 = pushbutton (externally debounced, active low)             *
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
#define BUTTON  GPIObits.GP2        // pushbutton


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
    IOCbits.IOC2 = 1;               // enable IOC on GP2 input
    
    // enable interrupts
    INTCONbits.GPIE = 1;            // enable port change interrupt
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
    //*** Service port change interrupt
    //
    //  Triggered on any transition on IOC-enabled input pin
    //  caused by externally debounced pushbutton press
    //
    //  Toggles LED on every high -> low transition
    //
    //  (only port change interrupts are enabled)       
    //      
    GPIO;                           // read GPIO to clear mismatch condition
    INTCONbits.GPIF = 0;            // clear interrupt flag
    
    // toggle LED only on button press
    if (!BUTTON)                    // if button is down
        sB_LED = ~sB_LED;           //   toggle LED (via shadow register)
}
