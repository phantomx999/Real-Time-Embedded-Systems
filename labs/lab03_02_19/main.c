#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Uncomment this to print out debugging statements.
//#define DEBUG 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "common.h"
#include "timers.h"
#include "buttons.h"

// tri-state system
// Blocking (waiting for release). Ready (waiting for cpu). Running (has cpu)
#define BLOCKING 0
#define READY 1
#define RUNNING 2

// setting caps on priority and number of spawned tasks
#define MAX_PRIORITY 250
#define MAX_TASKS 10

// used for system time
volatile uint64_t ms_ticks;

// mutex access to ms_ticks
uint64_t get_ticks() {
  uint64_t temp;
  cli();
  temp = ms_ticks;
  sei();
  return temp;
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
}

/****************************************************************************
   ALL INITIALIZATION
****************************************************************************/
void initialize_system(void)
{
	initialize_leds();
	light_show();

  initialize_buttons();

	spawn_all_tasks();

	// SCHEDULER: timer 0, prescaler 64, period 1 ms
	SetUpTimerCTC(0, 64, 1);
}

void ReleaseA() {
  // toggle the green to confirm button release recognized
  PORTD &= ~(1<<PORTD5);
  _delay_ms(100);
  PORTD |= (1<<PORTD5);
  cli();
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;

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
  SetUpButtonAction(&_button_A, 1, ReleaseA );

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

    // Determine highest priority ready task
    // vvvvv FILL THIS IN vvvvvvv
    task_id = -1;

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
			} else {
				tasks[task_id].state = BLOCKING;
			}
      sei();
		} // end if -1 != task_id
  } /* end while(1) loop */
} /* end main() */

/****************************************************************************
   ISR for TIMER used as SCHEDULER
****************************************************************************/
// Timer set up in timers.c always enables COMPA
ISR(TIMER0_COMPA_vect) {
  // vvvvvvvvv    FILL THIS IN    vvvvvvvvvvvvv
	// Find all tasks that are ready to release.
	// Determine if they have missed their deadline.
  // Check the task table to see if anything is ready to release


  // ms_ticks is down here because want all tasks to release at 0 ticks
  ++ms_ticks;
}
