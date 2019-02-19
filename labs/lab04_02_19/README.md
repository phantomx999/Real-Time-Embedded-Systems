##### CSCI5143 Spring 2019

#### Mini Lab 4 : Periodic Nonpreemptive Scheduler
> DUE: Tuesday, February 19 before class <br>
> Submit report via Canvas
> Submit code via GitHub

This is a continuation of last week's lab. You will be getting communication up and running so that you can determine if any of your tasks are missing their deadlines. In addition, you will be adding two LEDs to your system attached to general IO. One will be toggled using a PWM signal and the other will be toggled as a scheduled task.

```
You can work with other people on the code.
Please respond to the questions individually.
```

<hr>

**__YOUR TASK__**

#### 1. Get Communication Functional

If you have Windows, then you should purchase a USB-TTL dongle from Dr. Larson ($5), unless you have taken on the task of upgrading to the latest LUFA and you have it functional. If you don't have Windows, you should be able to (probably will) get the LUFA library functional, but if not, purchase the dongle. The 2 forms of communication operate differently. One uses interrupts and the other polls, but then enters a busy wait for retrieving communication. This can be disruptive to your scheduler. If you don't try to communicate while you are running your scheduler you should be good. If you are polling for communication, you may add this as a task to be scheduled, but it is not necessary.

The LUFA libraries communicate through the USB cable used for flashing your program. You have to get the code, compile it locally, then set paths in your system environment to point to the compiled code. The makefile is slightly modified to link to LUFA. Finally, there are commands to add to your code to poll for communication activity.

Here are the directions: https://github.umn.edu/course-material/repo-rtes-public/tree/master/ExampleCode/basic-lufa

The example code you should be using to test it is here:
https://github.umn.edu/course-material/repo-rtes-public/blob/master/ExampleCode/basic-lufa/example/main.c

There is example code for a basic UI menu here:
https://github.umn.edu/course-material/repo-rtes-public/blob/master/ExampleCode/basic-menu/example/main.c

The USB-TTL dongle uses UART for communication connecting through the RX/TX pins on your board (i.e. Arduino pins 0 and 1).

The directions are here: https://github.umn.edu/course-material/repo-rtes-public/tree/master/ExampleCode/basic-serial

An example of using this for a UI is here:
https://github.umn.edu/course-material/repo-rtes-public/blob/master/ExampleCode/basic-serial/main.c

A student created a special print library for this:
https://github.umn.edu/course-material/repo-rtes-public/blob/master/ExampleCode/print_library/main.c

#### 2. Toggle RED LED in a scheduled task.

Add a task to your scheduler that simply toggles a RED LED connected to PortE pin 6 (i.e. AStar pin 7). Use the breadboard, a resistor, and an LED from your kit. Toggle at a rate of 4 Hz (ON at 0, Off at 250ms, On 500ms, ...).

Here are some resources on using breadboards and LEDs:
- https://computers.tutsplus.com/tutorials/how-to-use-a-breadboard-and-build-a-led-circuit--mac-54746
- https://www.youtube.com/watch?v=w0c3t0fJhXU
- http://robotmill.com/2011/02/16/arduino-easy-led-traffic-light/

#### 3. Toggle Green LED using a PWM signal

Set up PWM to generate a waveform to toggle a green LED connected to PortB pin 6 (i.e. AStar pin 10), which is also known as OC1B. This means you have to use timer 1 for your PWM signal. Toggle at 4 Hz, which is the same rate as the other LED.

#### 5. Analysis

Answer the following questions and submit a .pdf on Canvas.

1. You might see some differences in behavior between the two general IO LEDs. If you see it, describe the difference. Explain why these LEDs might behave differently even though they are both toggling at the same rate.

2. You probably won't see any missed deadlines given the current task structure. Modify TaskDelay1() period=5 and Task 4 DelayDelay() period=10. Run your scheduler for a minute, then print out the missed deadlines. Report on your findings. (Don't use your UI during this time, as it interferes with the scheduling.)
