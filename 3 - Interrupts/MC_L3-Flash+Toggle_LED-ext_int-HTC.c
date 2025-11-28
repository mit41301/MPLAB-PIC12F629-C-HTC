/************************************************************************
*                                                                       *
*   Filename:      MC_L3-Flash+Toggle_LED-ext_int-HTC.c                 *
*   Date:          9/6/12                                               *
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
*   Description:    Lesson 3, example 6                                 *
*                                                                       *
*   Demonstrates handling of multiple interrupt sources                 *
*                                                                       *
*   Toggles an LED when pushbutton on INT is pressed                    *
*   (high -> low transition triggering external interrupt)              *
*   while another LED flashes at 1 Hz (driven by Timer0 interrupt)      *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP0 = flashing LED                                              *
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
#define sF_LED  sGPIO.GP0           // flashing LED (shadow)
#define sB_LED  sGPIO.GP1           // "button pressed" indicator LED (shadow)


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
    TRISIO = 0b111100;              // configure GP0 and GP1 (only) as outputs
    
    // configure Timer0
    OPTION_REGbits.T0CS = 0;        // select timer mode
    OPTION_REGbits.PSA = 1;         // no prescaler (assigned to WDT)
                                    // -> increment every 1 us
                                    
    // configure external interrupt
    OPTION_REGbits.INTEDG = 0;      // trigger on falling edge 
    
    // enable interrupts
    INTCONbits.T0IE = 1;            // enable Timer0 interrupt
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
    static uint16_t     fl_t_cnt = 0;   // LED flash timebase counter
    
    // Service all triggered interrupt sources
    
    if (INTCONbits.INTF)
    {
        //*** Service external interrupt
        //
        //  Triggered on high -> low transition on INT pin
        //  caused by externally debounced pushbutton press
        // 
        //  Toggles LED on every high -> low transition
        //  
        INTCONbits.INTF = 0;            // clear interrupt flag
    
        // toggle LED
        sB_LED = ~sB_LED;               // (via shadow register)    
    }
    
    if (INTCONbits.T0IF)
    {
        //*** Service Timer0 interrupt
        //
        //  TMR0 overflows every 250 clocks = 250 us
        //
        //  Flashes LED at 1 Hz by toggling on every 2000th interrupt
        //      (every 500 ms)
        //   
        TMR0 += 256-250+3;              // add value to Timer0
                                        //   for overflow after 250 counts
        INTCONbits.T0IF = 0;            // clear interrupt flag
        
        // Flash LED (toggle every 500 ms)
        //
        ++fl_t_cnt;                     // increment interrupt count (every 250 us)
        if (fl_t_cnt == 500000/250)     // until 500 ms has elapsed
        {
            fl_t_cnt = 0;               //   reset interrupt count
            sF_LED = ~sF_LED;           //   toggle LED (via shadow register)
        }
    }
}
