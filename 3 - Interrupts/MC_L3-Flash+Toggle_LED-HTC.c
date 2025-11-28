/************************************************************************
*                                                                       *
*   Filename:      MC_L3-Flash+Toggle_LED-HTC.c                         *
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
*   Description:    Lesson 3, example 4                                 *
*                                                                       *
*   Demonstrates use of Timer0 interrupt to run multiple                *
*   concurrent background tasks                                         *
*                                                                       *
*   Toggles LED when the pushbutton is pressed                          *
*   while another LED flashes at 1 Hz (50% duty cycle)                  *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP1 = "button pressed" indicator LED                            *
*       GP2 = flashing LED                                              *
*       GP3 = pushbutton (active low)                                   *
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


/***** CONSTANTS *****/
#define MAX_DB_CNT  20/2    // maximum debounce count = debounce period / sample rate
                            //   (20 ms debounce period / 2 ms per sample)


/***** GLOBAL VARIABLES *****/
volatile union {                // shadow copy of GPIO
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

volatile bit    PB_dbstate;     // debounced pushbutton state (1 = released)
volatile bit    PB_change;      // pushbutton state change flag (1 = changed)


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
                                    
    // initialise variables
    PB_dbstate = 1;                 // initial pushbutton state = released
    PB_change = 0;                  // clear pushbutton change flag (no change)
    
    // enable interrupts
    INTCONbits.T0IE = 1;            // enable Timer0 interrupt
    ei();                           // enable global interrupts
 
                                    
    //*** Main loop
    for (;;)
    {
        // check for debounced button press
        if (PB_change && !PB_dbstate)       // if button state changed and pressed (low)
        {
            sB_LED = ~sB_LED;               //   toggle LED (via shadow register)
            PB_change = 0;                  //   clear button change flag
        }
        
        // continually copy shadow GPIO to port
        GPIO = sGPIO.port; 
            
    }   // repeat forever
}


/***** INTERRUPT SERVICE ROUTINE *****/
void interrupt isr(void)
{
    static uint8_t      db_t_cnt = 0;   // debounce sample timebase counter
    static uint8_t      db_s_cnt = 0;   // debounce sample counter
    static uint16_t     fl_t_cnt = 0;   // LED flash timebase counter
    
    //*** Service Timer0 interrupt
    //
    //  TMR0 overflows every 250 clocks = 250 us
    //
    //  Debounces pushbutton:
    //    samples every 2 ms (every 8th interrupt)
    //    -> PB_dbstate = debounced state
    //       PB_change  = change flag (1 = new debounced state)   
    //   
    //  Flashes LED at 1 Hz by toggling on every 2000th interrupt
    //      (every 500 ms)   
    // 
    //   (only Timer0 interrupts are enabled)
    //   
    TMR0 += 256-250+3;              // add value to Timer0
                                    //   for overflow after 250 counts
    INTCONbits.T0IF = 0;            // clear interrupt flag
    
    // Debounce pushbutton 
    //   use counting algorithm: accept change in state
    //   only if new state is seen a number of times in succession
    //
    // sample switch every 2 ms 
    ++db_t_cnt;                     // increment interrupt count (every 250 us)
    if (db_t_cnt == 2000/250)       // until 2 ms has elapsed
    {
        db_t_cnt = 0;               //   reset interrupt count
        
        // compare raw pushbutton with current debounced state
        if (BUTTON == PB_dbstate)   // if raw pushbutton matches current debounce state,
            db_s_cnt = 0;           //   reset debounce count
        else                        // else raw pushbutton has changed state
        {
            ++db_s_cnt;                     // increment debounce count
            if (db_s_cnt == MAX_DB_CNT)     // when max count is reached
            {                               //   accept new state as changed:
                PB_dbstate = !PB_dbstate;   //     toggle debounced state
                db_s_cnt = 0;               //     reset debounce count
                PB_change = 1;              //     set pushbutton changed flag
                                            //     (polled and cleared in main loop)
            }
        }   
    }
    
    // Flash LED (toggle every 500 ms)
    //
    ++fl_t_cnt;                     // increment interrupt count (every 250 us)
    if (fl_t_cnt == 500000/250)     // until 500 ms has elapsed
    {
        fl_t_cnt = 0;               //   reset interrupt count
        sF_LED = ~sF_LED;           //   toggle LED (via shadow register)
    }
}
