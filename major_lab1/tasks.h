#ifndef TASKS_H_
#define TASKS_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "common.h"
#include "leds.h"
#include "hough.h"
#include "image10.h"
#include "testload.h"
#include "analog.h"
#include "serial.h"

extern uint16_t semaphore;
extern uint16_t previous;
extern uint16_t current;

#define HEIGHT 5
#define WIDTH 5

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

//int Image[HEIGHT][WIDTH];

int Invert();

int DelayDelay();

int MaxMin();

int TaskDelay1();

int EventTask();

int TaskDelay2();

int TaskDelay3();

int Hough();

int Average();

int RedToggle();

int GreenToggle();

int YellowToggle();

void EventPolling();

void SempahoreTask();

void HoughTransform();

#endif
