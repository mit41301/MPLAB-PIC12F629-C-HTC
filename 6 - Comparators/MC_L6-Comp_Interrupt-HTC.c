/************************************************************************
*                                                                       *
*   Filename:      MC_L6-Comp_Interrupt-HTC.c                           *
*   Date:          24/6/12                                              *
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
*   Description:    Lesson 6, example 4                                 *
*                                                                       *
*   Demonstrates use of comparator interrupt                            *
*   (assumes hysteresis is used to reduce triggering)                   *
*                                                                       *
*   Turns on LED when voltage on CIN+ < 1.5 V                           *
*                                                                       *
*************************************************************************
*                                                                       *
*   Pin assignments:                                                    *
*       CIN+ = voltage to be measured (e.g. pot output or LDR)          *
*       COUT = comparator output (fed back to input via resistor)       *
*       GP5  = indicator LED                                            *
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
#define sLED    sGPIO.GP5       // indicator LED (shadow)


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
    /*** Initialisation ***/
    
    // configure port
    GPIO = 0;                       // start with LED off
    sGPIO.port = 0;                 //   update shadow
    TRISIO = ~(1<<5);               // configure GP5 (only) as an output
    
    // configure comparator
    CMCONbits.CM = 0b101;       // select mode 5:
                                //   +ref is CVref,
                                //   external output enabled,
                                //   comparator on
    CMCONbits.CIS = 1;          // -ref is CIN+
    CMCONbits.CINV = 1;         // inverted output 
                                // -> COUT = 1 if CIN+ > CVref,
                                //    COUT pin enabled
    TRISIObits.TRISIO2 = 0;     // configure COUT (GP2) as an output
                                
    // configure voltage reference
    VRCONbits.VRR = 1;          // select low range
    VRCONbits.VR = 7;           //   CVref = 0.292*Vdd
    VRCONbits.VREN = 1;         // enable voltage reference
                                // -> CVref = 1.5 V (if Vdd = 5.0 V) 
    
    // enable interrupts
    INTCONbits.PEIE = 1;        // enable peripheral interrupts
    ei();                       // enable global interrupts
                                // enable comparator interrupt:
    CMCON;                      //   read CMCON to clear mismatch
    PIR1bits.CMIF = 0;          //   clear interrupt flag
    PIE1bits.CMIE = 1;          //   set enable bit

                                    
    /*** Main loop ***/
    for (;;)
    {
        // continually copy shadow GPIO to port
        GPIO = sGPIO.port;
    } 
}


/***** INTERRUPT SERVICE ROUTINE *****/
void interrupt isr(void)
{
    //*** Service comparator interrupt
    //
    //   Triggered on any comparator output change,
    //   caused by comparator input crossing 1.5 V threshold
    //
    //  Turns on indicator LED if CIN+ < 1.5 V
    //
    //  (only port change interrupts are enabled)       
    //   
    CMCON;                      // read CMCON to clear mismatch condition
    PIR1bits.CMIF = 0;          // clear interrupt flag
    
    // turn on LED if CIN+ < 1.5 V
    sLED = ~CMCONbits.COUT;     // display inverse of comparator output
}
