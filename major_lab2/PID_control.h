#ifndef PID_CONTROL_H_
#define PID_CONTROL_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "motor.h"
#include "timers.h"
#include "lufa.h"
#include "leds.h"
#include "common.h"

volatile int16_t global_counts_m2;

int64_t PID_Control(int32_t ref, int32_t Kp, int32_t Kd, int64_t previous_t);

#endif