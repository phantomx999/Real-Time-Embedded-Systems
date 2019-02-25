#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Uncomment this to print out debugging statements.
//#define DEBUG 1
/*
#ifdef VIRTUAL_SERIAL
#include <VirtualSerial.h>
#else
#warning VirtualSerial not defined, USB IO may not work
#define SetupHardware();
#define USB_Mainloop_Handler();
#endif
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "timers.h"
#include "buttons.h"
#include "tasks.h"
#include "leds.h"


// used for system time
volatile uint64_t ms_ticks;
//volatile int release_A_flag = 0;

// mutex access to ms_ticks
uint64_t get_ticks() {
  uint64_t temp;
  cli();
  temp = ms_ticks;
  sei();
  return temp;
}

volatile int buttonA_release_counter = -1;
volatile int buttonC_release_counter = -1;

volatile int green_led_periods[] = {700, 500, 100, 1000};
volatile float red_led_dutyCycle[] = {0.5, 0.7, 0.8, 0.9};


/*
void init() {
  SetupHardware();
  sei();
}
*/


/****************************************************************************
   ALL INITIALIZATION
****************************************************************************/
void initialize_system(void)
{
   //init();
	initialize_leds();
	light_show();

  initialize_buttons();


    DDRB |= (1 << DDB6);
    DDRC |= (1 << DDC6);

	// SCHEDULER: timer 0, prescaler 64, period 1 ms
	//SetUpTimerCTC(0, 64, 1);
	SetUpTimerPWM(1, 256, 1000, 0.5);
	SetUpTimerPWM(3, 256, 500, 0.5);

}

void ReleaseA() {
  //release_A_flag=1;
  if(buttonA_release_counter <= 3){
    buttonA_release_counter++;
  }
  else{
    buttonA_release_counter = 0;
  }
  // toggle the green to confirm button release recognized
 // PORTD &= ~(1<<PORTD5);
 // _delay_ms(100);
 // PORTD |= (1<<PORTD5);
  //cli();
}

void ReleaseC() {
  if(buttonC_release_counter <= 3){
    buttonC_release_counter++;
  }
  else{
    buttonC_release_counter = 0;
  }
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;

  int task_id = -1;

	initialize_system();

  int temp;
  int i;

  // used to toggle led to show liveness
  int period_ms = 1000;
  uint64_t expires = period_ms;

  // Set up to fire ISR upon button A activity
  // Upon the release (2nd input param) of the button, it will call fn ReleaseA
  SetUpButton(&_button_A);
  SetUpButtonAction(&_button_A, 1, ReleaseA );
  
  SetUpButton(&_button_C);
  SetUpButtonAction(&_button_C, 1, ReleaseC );

  // HERE WE GO
  ms_ticks = 0;
  sei();

  char c;

  //*******         THE CYCLIC CONTROL LOOP            **********//
  //*************************************************************//
  while(1) {

    /*
    // heartbeat
    if (get_ticks() >= expires) {
      TOGGLE_BIT(*(&_yellow)->port, _yellow.pin);
      expires = get_ticks() + period_ms;
    }
    */
   
    //USB_Mainloop_Handler();
    
    
    if(buttonA_release_counter == 0){
      SetUpTimerPWM(1, 256, green_led_periods[2], 0.5);
    }
    else if(buttonA_release_counter == 1){
      SetUpTimerPWM(1, 256, green_led_periods[3], 0.5);
    }
    else if(buttonA_release_counter == 2){
      SetUpTimerPWM(1, 256, green_led_periods[2], 0.5);
    }
    else if(buttonA_release_counter == 3){
      SetUpTimerPWM(1, 256, green_led_periods[3], 0.5);
    }
    
    
    if(buttonC_release_counter == 0){
      SetUpTimerPWM(3, 256, 500, red_led_dutyCycle[0]);
    }
    else if(buttonC_release_counter == 1){
      SetUpTimerPWM(3, 256, 500, red_led_dutyCycle[1]);
    }
    else if(buttonC_release_counter == 2){
      SetUpTimerPWM(3, 256, 500, red_led_dutyCycle[2]);
    }
    else if(buttonC_release_counter == 3){
      SetUpTimerPWM(3, 256, 500, red_led_dutyCycle[3]);
    }

   
  } /* end while(1) loop */
} /* end main() */

/****************************************************************************
   ISR for TIMER used as SCHEDULER
****************************************************************************/
// Timer set up in timers.c always enables COMPA
/*
ISR(TIMER0_COMPA_vect) {


  // ms_ticks is down here because want all tasks to release at 0 ticks
  //++ms_ticks;
  ms_ticks++;
   
}
*/

ISR(TIMER1_COMPA_vect) {
  TOGGLE_BIT(PORTB, PORTB6);
}

ISR(TIMER3_COMPA_vect) {
  TOGGLE_BIT(PORTC, PORTC6);
}

