/************************************************************************
*                                                                       *
*   Filename:      MC_L4-2xToggle_LED-IOC-HTC.c                         *
*   Date:          10/6/12                                              *
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
*   Description:    Lesson 4, example 2                                 *
*                                                                       *
*   Demonstrates handling of multiple interrupt-on-change interrupts    *
*   (without software debouncing)                                       *
*                                                                       *
*   Toggles LED on GP0 when pushbutton on GP2 is pressed                *
*   (high -> low transition)                                            *
*   and LED on GP1 when pushbutton on GP4 is pressed                    *
*   (low -> high transition)                                            *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP0 = indicator LED 1                                           *
*       GP1 = indicator LED 2                                           *
*       GP2 = pushbutton 1 (externally debounced, active low)           *
*       GP4 = pushbutton 2 (externally debounced, active high)          *
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
#define sB1_LED sGPIO.GP0   // "button 1 pressed" indicator LED (shadow)
#define sB2_LED sGPIO.GP1   // "button 2 pressed" indicator LED (shadow)
#define nPB1    2           // pushbutton 1 (ext debounce, active low) on GP2
#define nPB2    4           // pushbutton 2 (ext debounce, active high) on GP4


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

volatile uint8_t    lGPIO;          // last state of GPIO (for change detection)


/***** MAIN PROGRAM *****/
void main()
{
    //*** Initialisation
    
    // configure port
    GPIO = 0;               // start with all LEDs off
    sGPIO.port = 0;         //   update shadow
    TRISIO = 0b111100;      // configure GP0 and GP1 (only) as outputs
    lGPIO = GPIO;           // update last port state (for pin change detection)
    IOC = 1<<nPB1|1<<nPB2;  // enable interrupt-on-change on pushbuttons 1 and 2
    
    // enable interrupts
    INTCONbits.GPIE = 1;        // enable port change interrupt
    ei();                       // enable global interrupts

                                    
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
    uint8_t     cGPIO;      // current state of GPIO (used by IOC handler)
    
    //*** Service port change interrupt
    //
    //  Triggered on any transition on IOC-enabled input pin
    //  caused by externally debounced pushbutton press
    //
    //  Toggles LED1 on every high -> low transition of PB1
    //      and LED2 on every low -> high transition of PB2
    //
    //  (only port change interrupts are enabled)       
    //   
    INTCONbits.GPIF = 0;            // clear interrupt flag
    
    // determine which pins have changed
    cGPIO = GPIO;                   // save current GPIO state
                                    //   (GPIO read clears IOC mismatch condition)
    lGPIO ^= cGPIO;                 // XOR with last state to detect changes
    
    // toggle LED 1 only on button 1 press (active low)
    if ((lGPIO & 1<<nPB1)           // if button 1 changed
        && (!(cGPIO & 1<<nPB1)))    //  and button 1 is down (low)
        {         
            sB1_LED = ~sB1_LED;     //   toggle LED 1 (via shadow register)
        }
        
    // toggle LED 2 only on button 2 press (active high)
    if ((lGPIO & 1<<nPB2)           // if button 2 changed
        && (cGPIO & 1<<nPB2))       //  and button 2 is down (high)
        {         
            sB2_LED = ~sB2_LED;     //   toggle LED 2 (via shadow register)
        }    
    
    // update last GPIO state (for next time)
    lGPIO = cGPIO;                  // new "last state" = current
}
