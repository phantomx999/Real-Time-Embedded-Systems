


#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Uncomment this to print out debugging statements.
//#define DEBUG 1


#ifdef VIRTUAL_SERIAL
#include <VirtualSerial.h>
#else
#warning VirtualSerial not defined, USB IO may not work
#define SetupHardware();
#define USB_Mainloop_Handler();
#endif



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
#include "lufa.h"
#include "analog.h"
#include "serial.h"
#include "hough.h"
#include "image10.h"
#include "testload.h"

// tri-state system
// Blocking (waiting for release). Ready (waiting for cpu). Running (has cpu)
#define BLOCKING 0
#define READY 1
#define RUNNING 2

// setting caps on priority and number of spawned tasks
#define MAX_PRIORITY 250
#define MAX_TASKS 3

// used for system time

volatile uint64_t ms_ticks_0;


volatile uint64_t green_toggle_count;
volatile uint64_t yellow_toggle_count;
volatile uint64_t red_toggle_count;
uint32_t green_delay;
uint32_t yellow_delay;
char temp_found;
char task_id;
char c;
char print_menu;

volatile int experiment = 0;
char in_ui_mode;
int zero_out = 0;
int change_green = 0;
uint16_t semaphore;

uint32_t red_freq;
uint32_t green_freq;
uint32_t yellow_freq;
	
char yellow_sei;



void init() {
  DDRB |= ( 1 << DDB7 );
  PORTB |= ( 1 << PORTB7 );
  DDRD |= ( 1 << DDD4 );
  PORTD |= ( 1 << PORTD4);
  setupUART();
  adc_init();
  SetupHardware();
  sei();
}



// shared structure between scheduler (ISR) and server (main)
extern volatile Task tasks[MAX_TASKS];


// Array is initially empty. Spawn tasks to add to scheduler.
int task_count = 0;

int spawn(int(*fp)(), int id, int p, int priority) {
	if (task_count == MAX_TASKS) {
		return ERROR;
	}
  tasks[task_count].funptr = fp;
  tasks[task_count].period = p;
  tasks[task_count].next_release = p;
  tasks[task_count].missed_deadlines = 0;
  tasks[task_count].id = id;
  tasks[task_count].priority = priority;
	tasks[task_count].buffered = 1;
  tasks[task_count].max_buffered = 1;
  tasks[task_count].releases = 0;
  tasks[task_count].executed = 0;
  tasks[task_count].state = READY;
  ++task_count;
	return 1;
}

// A fixed priority system based on the period of a task
// The smaller the period, the higher the priority.
void spawn_all_tasks() {
  spawn(HoughTransform, 4, 100, 1);
  spawn(EventPolling, 5, 350, 2);
  spawn(SempahoreTask, 6, 375, 3);
}




/****************************************************************************
   ALL INITIALIZATION
****************************************************************************/
void initialize_system(void)
{
    init();
	initialize_leds();
	//light_show();

    initialize_buttons();
    SetupHardware();
	
	ms_ticks_0 = 0;
	green_toggle_count = 0;
    yellow_toggle_count = 0;
    red_toggle_count = 0;
	task_count = 0;

    DDRB |= (1 << DDB6);  // GREEN LED
    DDRB |= (1 << DDB4);  // RED LED
   // DDRE |= (1 << DDE6);
    DDRD |= (1 << DDD6);  // YELLOW LED
    
    spawn_all_tasks();
    
    ///////////////CHANGE THESE TO RUN EXPERIMENT/////////////////
    red_freq = 100;
    green_freq = 200;
    yellow_freq = 100;
    green_delay = 0;
	yellow_delay = 0;
	yellow_sei = 0;
	///////////////////////////////

	// SCHEDULER: timer 0, prescaler 64, period 1 ms
	SetUpTimerCTC(0, 64, 1);  // for RED led task and scheduling task
	SetUpTimerPWM(1, 256, green_freq, 0.5);  // for GREEN LED task
	SetUpTimerCTC(3, 256, yellow_freq);  // for YELLOW led task
	

	
	
	in_ui_mode = 0;
	print_menu = 0;

}


/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;


  initialize_system();

  sei();


  while(1) {
    USB_Mainloop_Handler();
    
    if (in_ui_mode){
    	if (( c= fgetc(stdin)) != EOF){
    		handleInput(c);
    	}
    }else{
    	if ((ms_ticks_0 % red_freq) == 0) {
			TOGGLE_BIT(PORTB, PORTB4);
			red_toggle_count++;
    	}
    
      	task_id = -1;
      	temp_found = 0;
      	while(!temp_found){
        	task_id++;
        	if (task_id == MAX_TASKS) break;
        	if(tasks[task_id].state == READY){
          		temp_found = 1;
         	}
      	}

	  	// Execute the task, then do housekeeping in the task array
	  	if (-1 != task_id) {
        	cli();
	    	tasks[task_id].state = RUNNING;
        	sei();
	    	tasks[task_id].funptr();
        	cli();
        	tasks[task_id].executed += 1;
	    	tasks[task_id].buffered -= 1;
	    	if (tasks[task_id].buffered > 0) {
		  		tasks[task_id].state = READY;
        	} 
        	else {
    	  		tasks[task_id].state = BLOCKING;
	    	}
        	sei();
      	}
    }

	} // end while (1)
  } /* end main


/****************************************************************************
   ISR for TIMER used as SCHEDULER
****************************************************************************/
// Timer set up in timers.c always enables COMPA
ISR(TIMER0_COMPA_vect) {
	if (!in_ui_mode){
		ms_ticks_0++;
	
    
  		for(int task_n=0; task_n < (MAX_TASKS); task_n++){
    		if(ms_ticks_0 % tasks[task_n].period == 0){
    			if (task_n == 2 && semaphore == 1){
      				int temp2 =   (ms_ticks_0/tasks[task_n].period);
      				tasks[task_n].missed_deadlines = temp2 - tasks[task_n].executed;
      				tasks[task_n].state = READY;
        			tasks[task_n].buffered++;    			
    			}else if (task_n != 2){
      				int temp2 =   (ms_ticks_0/tasks[task_n].period);
      				tasks[task_n].missed_deadlines = temp2 - tasks[task_n].executed;
      				tasks[task_n].state = READY;
        			tasks[task_n].buffered++;    			
    			}

      	
    		}
  		} 		
	}
   
  	
  	if (ms_ticks_0 == 15000){
  		in_ui_mode = 1;
  		if (print_menu == 0){
  			printf("P, p\n\r");
  			print_menu = 1;
  		}
  		//ms_ticks_0 = 0;
  		
  	} 
}

ISR(TIMER1_COMPA_vect) {
	if (!in_ui_mode){
		green_toggle_count+=2;
  		
  		volatile uint32_t count;
  		for (count = 0; count < green_delay; count++){
  			_delay_ms(5);
  		}
	}

}

ISR(TIMER3_COMPA_vect) {
	if (!in_ui_mode){
		if (yellow_sei) sei();
    	yellow_toggle_count++;
    	TOGGLE_BIT(PORTD, PORTD6);	
    	
    	volatile uint32_t count;
  			for (count = 0; count < yellow_delay; count++){
  			_delay_ms(5);
  		}	
	}
}

