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

volatile uint32_t ms_timer = 0;
//volatile int flag_A;

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
  // timer 0
  //SetUpTimerCTC(0, 64, 500);
  // timer 1 
  SetUpTimerCTC(1, 64, 250);
  // timer 3
  SetUpTimerCTC(3, 1024, 1);

  
  
  //*************************************************************//
  //*******         THE CYCLIC CONTROL LOOP            **********//
  //*************************************************************//

  // FILL THIS IN TO BLINK LED at the requested frequency using system time.

  // without keyword volatile, the compiler optimizes away count
  volatile uint32_t count = 0;
  uint32_t release_time = 500;

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

    if(ms_timer >= release_time){
      ms_timer = 0;
      TOGGLE_BIT(*(&_red)->port, _red.pin);
    }

    /*
    if(flag_A == 1){
      for(int i = 0; i < 2; i++){
        fn_release_A();
      }
      flag_A = 0;
    }
    */
      
  } /* end while(1) loop */
} /* end main() */


// ********************* PUT YOUR TIMER ISRs HERE *************** //

//ISR(TIMER0_COMPA_vect) {}

ISR(TIMER3_COMPA_vect) {
  //ms_timer = TCNT0;
  //if(ms_timer >= release_time){
 //   TOGGLE_BIT(*(&_red)->port, _red.pin);
 //   ms_timer = 0;
 //   TCNT0 = 0;
 // }
 ms_timer++;
}

ISR(TIMER1_COMPA_vect) {
  TOGGLE_BIT(*(&_yellow)->port, _yellow.pin);
}