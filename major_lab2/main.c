/* Xin Yang and Andrew Steinbrueck have been working together for the code of major lab2*/
//student ids
//Xin Yang 5064064
//Andrew Steinbrueck 3949010

#ifdef VIRTUAL_SERIAL
#include <VirtualSerial.h>
#else
#warning VirtualSerial not defined, USB IO may not work
#define SetupHardware();
#define USB_Mainloop_Handler();
#endif


#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "common.h"
#include "timers.h"
#include "leds.h"
#include "motor.h"
#include "PID_control.h"
#include "lufa.h"
#include "serial.h"

volatile int16_t global_counts_m2;
volatile int ms_ticks; 


//for serial communication
char in_ui_mode = 1;
int32_t go_experiment;

char c;
volatile char print_menu = 0;

//for change the period of PD controller, start at 10Hz
int32_t PD_Period = 100;

//PD controller
volatile int32_t Kp;
volatile int32_t Kd;
int64_t ref = 0;
int64_t previous_t = 0;
int64_t current_t = 0;

void set_up_timer(){
	//set up CTC timer 0, to control the period of PD control
	//start at 10Hz, change state every 100ms
	SetUpTimerCTC(3, 1024, PD_Period); 
	
	//set up timer to keep track of system time, and get into ui mode every 10000 ms
	SetUpTimerCTC(0, 1024, 10); 
	
}

void zero_system() {
  global_counts_m2 = 0;
  Kp = 0;
  Kd = 0;
  print_menu = 0;
  previous_t = 0;
  current_t = 0;
}

void initialize_system(void) {
  initialize_leds();
  SetupHardware();
  setupUART();	
  light_show();
  setupEncoder();
  setupMotor2();
  set_up_timer();

  cli();
  sei();
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;
  sei();
  
  initialize_system();
  
  USB_Mainloop_Handler(); 
  
  while(1) {
    if(in_ui_mode == 1) {
      OffMotor2();
      USB_Mainloop_Handler();
    }
    if((c = fgetc(stdin)) != EOF) {
      handleInput(c);
    }
    if(go_experiment == 1) {
      set_up_timer();
      sei();
      go_experiment = 0;
      ms_ticks = 0;
      in_ui_mode = 0;
      OnMotor2();
    }
  }
  
}

ISR(TIMER0_COMPA_vect) {
  if (in_ui_mode == 0){
	if (ms_ticks < 20000){
		ms_ticks += 10;  //system time
	}else if (ms_ticks == 20000){
		TOGGLE_BIT(*(&_yellow)->port, _yellow.pin);
		in_ui_mode = 1;	
		if (print_menu == 0){
			printf("r(efence value) c(ontroler period) p(KP) d(KD) g(o)\r\n");
			print_menu = 1;
		}	
	}
  }
}

ISR(TIMER3_COMPA_vect) {
  previous_t = current_t;
	//run PD controller
  if(in_ui_mode == 0){
	current_t = PID_Control(ref, Kp, Kd, previous_t);
  }
}



