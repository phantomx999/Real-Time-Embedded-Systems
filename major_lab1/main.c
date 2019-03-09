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
#include "lufa.h"
#include "analog.h"
#include "serial.h"
#include "hough/hough.h"
#include "hough/image10.h"
#include "hough/testload.h"

// tri-state system
// Blocking (waiting for release). Ready (waiting for cpu). Running (has cpu)
#define BLOCKING 0
#define READY 1
#define RUNNING 2

// setting caps on priority and number of spawned tasks
#define MAX_PRIORITY 250
#define MAX_TASKS 3

//uint16_t semaphore;
//uint16_t previous;
//uint16_t current;

// used for system time
volatile uint64_t ms_ticks_0;
volatile uint64_t ms_ticks_3;
//volatile int release_A_flag = 0;

uint64_t green_toggle_count = 0;
uint64_t yellow_toggle_count = 0;
uint64_t red_toggle_count = 0;

volatile int experiment = 0;
//char in_ui_mode = "";
extern int in_ui_mode = 1;

// mutex access to ms_ticks
uint64_t get_ticks() {
  uint64_t temp;
  cli();
  temp = ms_ticks_0;
  sei();
  return temp;
}


void init() {
  DDRB |= ( 1 << DDB7 );
  PORTB |= ( 1 << PORTB7 );
  setupUART();
  adc_init();
  SetupHardware();
  sei();
}

/***********************

   Experiment

************************/
void SetUpExperiment(int num) {
  if(experiment == 0) {
    red_led =    {   RedToggle, 100, 0, 0, 1, 1, 1, 0, 0, 0, READY};
    green_led =  { GreenToggle, 100, 0, 0, 2, 2, 1, 0, 0, 0, READY};
    yellow_led = {YellowToggle, 100, 0, 0, 3, 3, 1, 0, 0, 0, READY};

	spawn_all_tasks();

	// SCHEDULER: timer 0, prescaler 64, period 1 ms
	SetUpTimerCTC(0, 64, 1);  // for RED led task and scheduling task
	SetUpTimerPWM(1, 256, 500, 0.5);  // for GREEN LED task
	SetUpTimerCTC(3, 1024, 400);  // for YELLOW led task
   }
   else if(experiment == 1) {
     
   }
   
}

/***********************

   Zero-All

************************/

void ZeroAll() {
  green_toggle_count = 0;
  yellow_toggle_count = 0;
  red_toggle_count = 0;
  ms_ticks_0 = 0;
  ms_ticks_3 = 0;
  for(int i = i < MAX_TASKS; i++) {
    tasks[i] = {0};
  }
  red_led = {0};
  green_led =  {0};
  yellow_led = {0};
}

/****************************************************************************
   TASK Data Structures
****************************************************************************/
// holds a task. All will be gathered into an array
typedef struct {
	int (*funptr)();
  int period; 						// milliseconds
  uint64_t next_release;  // absolute time of next release in ms
  int missed_deadlines;
  char id;
  int priority; 		// priority 1 has the highest priority
	int buffered;			// the number of jobs waiting to execute
  int max_buffered; // maximum bufferend while system running
  int releases;     // number of times released
  int executed;     // number of times executed
  int state;				// one of the 3 states
} Task;

// shared structure between scheduler (ISR) and server (main)
volatile Task tasks[MAX_TASKS];

Task red_led;
Task green_led;
Task yellow_led;

// Array is initially empty. Spawn tasks to add to scheduler.
int task_count = 0;

/****************************************************************************
   TASK Scheduling Functions
****************************************************************************/
/* The creation of a single task, which is added to the array of tasks
 * Assuming all tasks in phase and will be released at start of system.
 * param [in] funptr : the code to be executed for a single job of the task.
 * param [in] id : not used in this version, but might have future utility
 * param [in] p : (period) time between releases in ms
 * param [in] priority : a fixed priority system (probably RM)
 */
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
	// @TODO: confirm that all tasks are spawned without error.
  // spawn(fptr, id, period, priority)
  // **** vvvvvvvvvv   FILL THIS IN   vvvvvvvvv ******* //
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
	light_show();

    initialize_buttons();

    red_led.funptr();
    green_led.funptr();
    yellow_led.funptr();
    ms_delay(1000);
    red_led.funpter();
    green_led.funptr();
    yellow_led.funptr();

    DDRB |= (1 << DDB6);  // GREEN LED
    DDRB |= (1 << DDB4);  // RED LED
    DDRE |= (1 << DDE6);
    DDRD |= (1 << DDD6);  // YELLOW LED

}

void ReleaseA() {
//  release_A_flag=1;
  in_ui_mode = 1;
  while(in_ui_mode) {
    if(ms_ticks_0 >= 15000 && experiment != 0) {  //end experiment if greater than 15 seconds
      printf("Experiment is over");
    }
    char *p = "p"
    handleCommand("p");
    printf("Missed deadlines for RED LED task = %d", red_led.missed_deadlines);
    printf("Missed deadlines for YELLOW LED task = %d", yellow_led.missed_deadlines);
    printf("Missed deadlines for EVENT POLLING task = %d", tasks[1].missed_deadlines);
    printf("Missed deadlines for SEMAPHORE task = %d", tasks[2].missed_deadlines);
    printf("Missed deadlines for HOUGHTRANSFORM task = %d", tasks[0].missed_deadlines);
    printf("Missed deadlines for GREEN LED task = %d", green_led.missed_deadlines);
    printf("Missed deadlines for GREEN LED COUNTING task = %d", green_led.missed_deadlines);
    if(ms_ticks_0 >= 15000 && experiment != 0) {  //end experiment if greater than 15 seconds
      ZeroAll();
    }
    handleInput("");
  }
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;
  
  while(in_ui_mode) {
    char *p = "p"
    handleInput("p");
    printf("Missed deadlines for RED LED task = %d", red_led.missed_deadlines);
    printf("Missed deadlines for YELLOW LED task = %d", yellow_led.missed_deadlines);
    printf("Missed deadlines for EVENT POLLING task = %d", tasks[1].missed_deadlines);
    printf("Missed deadlines for SEMAPHORE task = %d", tasks[2].missed_deadlines);
    printf("Missed deadlines for HOUGHTRANSFORM task = %d", tasks[0].missed_deadlines);
    printf("Missed deadlines for GREEN LED task = %d", green_led.missed_deadlines);
    printf("Missed deadlines for GREEN LED COUNTING task = %d", green_led.missed_deadlines);
    handleInput("");
  }
  handleInput("z");
  ZeroAll();
  SetUpExperiment(experiment);
  
  SetUpButton(&_button_A);
  SetUpButtonAction(&_button_A, 1, ReleaseA );

  int task_id = -1;
  int highest = MAX_PRIORITY + 1;

  initialize_system();

  int temp;
  int i;

  // used to toggle led to show liveness
  int period_ms = 1000;
  uint64_t expires = period_ms;

  // Set up to fire ISR upon button A activity
  // Upon the release (2nd input param) of the button, it will call fn ReleaseA
  //SetUpButton(&_button_A);
  //SetUpButtonAction(&_button_A, 1, ReleaseA );

  // HERE WE GO
  ms_ticks_0 = 0;
  ms_ticks_3 = 0;
  sei();

  char c;
  

  //*******         THE CYCLIC CONTROL LOOP            **********//
  //*************************************************************//
  while(1) {
    USB_Mainloop_Handler();
    if(ms_ticks_0 >= 15000 && experiment != 0) {  //end experiment if its past 15 seconds
      ReleaseA();
    }
    
    if ((ms_ticks_0 % 100) == 0 && (red_led.state == BLOCKING) && (red_led.buffered > 0)) {
      red_led.state = READY;
    }
    
    if(red_led.state == READY) {
      cli();
	  red_led.state = RUNNING;
      sei();
	  red_led.funptr();
	  red_toggle_count++;
      cli();
      red.executed += 1;
	  red.buffered -= 1;
	  if (red_led.buffered > 0) {
		red_led.state = READY;
      } 
      else {
    	red_led.state = BLOCKING;
    	red_led.buffered++;
	   }
       sei();
       red_led.missed_deadlines = (ms_ticks_0/red_led.period) - red_led.executed;
    }
    else {
      task_id = -1;
      temp = 0;
      while((task_id < 3) && (temp == 0)){
        task_id++;
        if(tasks[task_id].state == READY){
          temp = 1;
         }
      }
      if(task_id == 3) {
        task_id = -1;
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
	} // end if -1 != task_id
  } /* end while(1) loop */
} /* end main() */

/****************************************************************************
   ISR for TIMER used as SCHEDULER
****************************************************************************/
// Timer set up in timers.c always enables COMPA
ISR(TIMER0_COMPA_vect) {
  if(in_ui_mode) return;
  if(ms_ticks_0 >= 15000 && experiment != 0) {  //end experiment if its past 15 seconds
      ReleaseA();
      return;
  }
  ms_ticks_0++;
  for(int task_n=0; task_n < (MAX_TASKS); task_n++){
    if(ms_ticks_0 % tasks[task_n].period == 0){
      int temp2 = (ms_ticks_0/tasks[task_n].period);
      tasks[task_n].missed_deadlines = temp2 - tasks[task_n].executed;
      tasks[task_n].state = READY;
      ++tasks[task_n].buffered;
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  if(in_ui_mode) return;
  green_led.funptr();
  green_toggle_count++;
  if(experiment == 2) {
    delay_ms(20);
  }
  else if(experiment == 4) {
    delay_ms(30);
  }
  else if(experiment == 6) {
    delay_ms(105);
  }
  if(ms_ticks_0 % green_led.period == 0) {
    green_led.missed_deadlines = (ms_ticks_0/green_led.period) - green_toggle_count;
  }
}

ISR(TIMER3_COMPA_vect) {
  if(in_ui_mode) return;
  if(experiment == 8) {
    sei();
  }
  ms_timer_3++;
  if((ms_timer_3 % 4) == 0) {
    yellow_led.funptr();
    if(experiment == 3) {
      delay_ms(20);
    }
    else if(experiment == 5) {
      delay_ms(30);
    }
    else if(experiment == 7) {
      delay_ms(105);
    }
    else if(experiment == 8) {
      delay_ms(105);
    }
    yellow_toggle_count++;
    if((ms_ticks_3/4) % yellow_led.period == 0) {
      yellow_led.missed_deadlines = ((ms_ticks_3/4)/yellow_led.period) - yellow_toggle_count;
    }
  }
}
