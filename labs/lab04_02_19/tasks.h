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

#define HEIGHT 5
#define WIDTH 5

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

int ToggleRed();

#endif
