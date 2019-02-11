#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Comment this to remove "heartbeat" and led flash in PCINT
//#define DEBUG_MAIN 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "common.h"
#include "leds.h"
#include "buttons.h"
#include "timers.h"

/****************************************************************************
   ALL INITIALIZATION
****************************************************************************/
void initialize_system(void)
{
	initialize_leds();
	light_show();
  initialize_buttons();
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;

  initialize_system();

  // Set up to fire ISR upon button A and C activity
  SetUpButton(&_button_A);

  // You can pass a function pointer to implement button behavior.
  // Do this, or change the code in buttons.c to toggle the led
  // SetUpButtonAction(&_button_A, 1, xxx);

  // Do some setup for the timers.
  // SetUpTimerCTC(xxx);

  //*************************************************************//
  //*******         THE CYCLIC CONTROL LOOP            **********//
  //*************************************************************//

  // FILL THIS IN TO BLINK LED at the requested frequency using system time.

  // without keyword volatile, the compiler optimizes away count
  volatile uint32_t count = 0;

  sei();
  while(1) {

    #ifdef DEBUG_MAIN
    // might be useful to see a heartbeat if in debug mode
    ++count;
    if (count == 50000) {
      TOGGLE_BIT(*(&_yellow)->port, _yellow.pin);
      count = 0;
    }
    #endif

  } /* end while(1) loop */
} /* end main() */


// ********************* PUT YOUR TIMER ISRs HERE *************** //

// ISR(TIMER0_COMPA_vect) {}
