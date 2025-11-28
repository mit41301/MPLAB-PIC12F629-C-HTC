/************************************************************************
*                                                                       *
*   Filename:      MC_L3-Flash+PB_LED-int-HTC.c                         *
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
*   Description:    Lesson 3, example 2                                 *
*                                                                       *
*   Demonstrates use of Timer0 interrupt to perform a background task   *
*   while performing other actions in repsonse to changing inputs       *
*                                                                       *
*   One LED simply flashes at 1 Hz (50% duty cycle).                    *
*   The other LED is only lit when the pushbutton is pressed.           *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = "button pressed" indicator LED                            *
*       GP2 = flashing LED                                              *
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
#define sB_LED  sGPIO.GP1           // "button pressed" indicator LED (shadow)
#define sF_LED  sGPIO.GP2           // flashing LED (shadow)
#define BUTTON  GPIObits.GP3        // pushbutton


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
    TRISIO = 0b111001;              // configure GP1 and GP2 (only) as outputs
    
    // configure Timer0
    OPTION_REGbits.T0CS = 0;        // select timer mode
    OPTION_REGbits.PSA = 1;         // no prescaler (assigned to WDT)
                                    // -> increment every 1 us
             
    // enable interrupts
    INTCONbits.T0IE = 1;            // enable Timer0 interrupt
    ei();                           // enable global interrupts
                                    
                                    
    //*** Main loop
    for (;;)
    {
        // check and respond to button press
        sB_LED = 0;             // assume button up -> indicator LED off
        if (BUTTON == 0)        // if button pressed (low)
            sB_LED = 1;         //  turn on indicator LED

        // continually copy shadow GPIO to port
        GPIO = sGPIO.port; 
            
    }   // repeat forever
}


/***** INTERRUPT SERVICE ROUTINE *****/
void interrupt isr(void)
{
    static uint16_t  cnt_t0 = 0;    // counts timer0 overflows
    
    //*** Service Timer0 interrupt
    //
    //  TMR0 overflows every 250 clocks = 250 us
    //
    //  Flashes LED at 1 Hz by toggling on every 2000th interrupt
    //      (every 500 ms)   
    // 
    //   (only Timer0 interrupts are enabled)
    //
    TMR0 += 256-250+3;              // add value to Timer0
                                    //   for overflow after 250 counts
    INTCONbits.T0IF = 0;            // clear interrupt flag
    
    // toggle LED every 500 ms
    ++cnt_t0;                       // increment interrupt count (every 250 us)
    if (cnt_t0 == 500000/250)       // on count overflow (every 500 ms),
    {     
        cnt_t0 = 0;                 //   reset count
        sF_LED = ~sF_LED;           //   toggle LED (via shadow register)
    }
}
