/************************************************************************
*                                                                       *
*   Filename:      MC_L5-POR+BODdemo-HTC.c                              *
*   Date:          20/6/12                                              *
*   File Version:  1.2                                                  *
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
*   Description:    Lesson 5, example 6                                 *
*                                                                       *
*   Demonstrates use of brown-out detect                                *
*   and differentiation between POR, BOD and MCLR resets                *
*                                                                       *
*   Turns on POR LED only if power-on reset is detected                 *
*   Turns on BOD LED only if brown-out detect reset is detected         *
*   Turns on indicator LED in all cases                                 *
*   (no POR or BOD implies MCLR, as no other reset sources are active)  *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       GP0 = "on" indicator LED (always turned on)                     *
*       GP1 = POR LED (indicates power-on reset)                        *
*       GP2 = BOD LED (indicates brown-out detected)                    *
*                                                                       *
************************************************************************/

#include <xc.h>
#include <stdint.h>


/***** CONFIGURATION *****/
// ext reset, no code protect, brownout detect, no watchdog,
// power-up timer enabled, int RC clock
__CONFIG(MCLRE_ON & CP_OFF & CPD_OFF & BOREN_ON & WDTE_OFF & 
         PWRTE_OFF & FOSC_INTRCIO);

// Pin assignments
#define sO_LED  sGPIO.GP0   // "on" indicator LED - always on (shadow)
#define sP_LED  sGPIO.GP1   // POR LED to indicate power-on reset (shadow)
#define sB_LED  sGPIO.GP2   // BOD LED to indicate brown-out (shadow)


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
    GPIO = 0;                   // start with all LEDs off
    sGPIO.port = 0;             //   update shadow    
    TRISIO = 0b111000;          // configure GP0, GP1 and GP2 as outputs
    
    // check for POR or BOD reset
    if (!PCONbits.nPOR)         // if power-on reset (/POR = 0),
    {
        PCONbits.nPOR = 1;      //   set POR and BOD flags for next reset
        PCONbits.nBOD = 1;
        sP_LED = 1;             //   enable POR LED (shadow)
    }
    if (!PCONbits.nBOD)         // if brown-out detect (/BOD = 0)
    {
        PCONbits.nBOD = 1;      //   set BOD flag for next reset
        sB_LED = 1;             //   enable BOD LED (shadow)   
    }     
    
    
    /*** Main code ***/
    
    // enable "on" indicator LED 
    sO_LED = 1;                 // (via shadow register)

    // light enabled LEDs
    GPIO = sGPIO.port;          // copy shadow GPIO to port

    // wait forever
    for (;;)                    
        ;
}
