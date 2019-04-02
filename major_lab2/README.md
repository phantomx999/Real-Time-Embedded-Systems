### Motor Control Lab
#### Major Lab 2 : PD Motor Control
Due: Tuesday, April 2 at 11:55pm
Submit via Github

**__You may develop code with one other person in the class. HOWEVER, each person must individually experiment with the system to complete the report.
If you work with someone, please provide her/his/their name at the top of your report.__**

<hr>

#### Hardware Set-Up

|Color | Function| Pin |
|-----|-----|-----|
| Motor Red | motor power | motor 2, green header (B6) |
| Motor Black | motor power | motor 2, green header (B6)
| Motor Green | encoder GND | |
| Motor Blue | encoder Vcc | D1 (astar 2) |
| Motor Yellow | encoder A | B4, PCINT 4 (astar 8) |
| Motor White | encoder B  | B5, PCINT 5 (astar 9) |
|-|-|-|
| Pot 1 | kP | ADC10, D7 (astar 6) |
| Pot 2 | kD | ADC9, D6 (astar 12) |
| Power | TBD | |

If you are using serial communication with the dongle, then you will also be using the Rx and Tx pins, D2 and D3, a.k.a. astar pins 0 and 1.

You may choose to connect these in a different way, but please make it very obvious in the report how to connect everything.

#### Introduction

Consider a geared system in which gear rotation leads to linear motion (maybe it's in a factory). Here you see a LEGO mock-up of a rack-and-pinion system.

<p align="center">
<img src="Rack.png" alt="Rack and Pinion" width=50%/>
</p>

If we attached a motor to the circular gear (replacing the hand crank), rotation of the motor would translate to rotational motion of the gear (pinion), which would translate to the movement of the linear gear (the rack). Now imagine putting a mechanical hand at the end of the rack to pick up parts at various places along its trajectory. If everything is calibrated, we can determine the position of the rack (and the mechanical hand) given the position of the circular gear (i.e. its angle from some point 0), which we can determine from the relative rotation of the motor. We might use this system to move objects around (better yet, it might be part of a larger system that rotates the entire mechanism thus the hand would be operable within a full circle).

Keeping this type of a system in mind for this lab, you will implement a motor controller using PD control as part of a larger system of several periodic tasks. The system should be able to maintain positional control of the motor given a trajectory (i.e. a series of reference/goal positions). Other tasks you will need to schedule are the reading of a potentiometer for speeding up the motor (which is essentially increasing gains in the PD equation), recording data, and managing UI through serial communication.

For positional control, the motor should reach any referenced motor position without significant oscillation. You can modify the rise time (i.e. the time it takes to reach the reference position) by turning the potentiometer "up" or "down." Your goal is to find appropriate values for the gains Kd and Kp in the PD equation. In addition, you will develop an "interpolator" that can follow a positional trajectory (i.e. can reach a series of goal positions).

Your system will be composed of
- A PD controller (that relies on the accurate reading of the encoder). Design this so that the user can easily modify the period of this task. It is a separate task from the ISR for the encoder.
- Gains control of the motor using potentiometers.
- A user interface using USB/serial communication.
- Data recording for analysis.

#### PD Controller

The PD controller maintains the position of the motor using the measured and reference positions. It processes the encoder values to generate a drive signal to the motor. The encoder provides a measure of position and with this a measure of speed. The sampling rate of the encoder readings is not in your control, because it is interrupt-driven. The frequency of the PD controller should be carefully considered to keep the system stable and accurate.

In general, the PD controller (as a discrete equation) can be defined as:

```
Error = Reference - Measured
P = Error
D = Change in error over change in time (dt)
Pr = Position reference (goal input from the user)
Pm = Position measured (from the encoder reading in the ISR)
```

```
T = torque (or motor drive). This should be in range of -TOP to TOP
T = kP(P) + kD( delta(P) / dt )
T = kP(Pr-Pm) + kD(delta(Pr-Pm) / dt )
kP = Proportional gain (user defined value)
kD = Derivative gain (user defined value)
```

Note that when controlling position, the derivative term at time _i_ becomes:
```
delta( Pr - Pm ) / dt = ( Pr - Pm(i) ) - ( Pr - Pm(i-1) ) / dt
= ( Pr - Pr - Pm(i) + Pm(i-1) ) / dt
= - Velocity
T = kP(Pr-Pm) - kd(Velocity)
```

T is a signal that can be used directly to control the motor, except check that it is in range { -TOP, TOP }, and use absolute(T) and set motor direction appropriately. You should always be generating motor commands, regardless of whether the reference position is changing or not. This means that at any time, if you move the wheel manually when controlling position, the PD controller should bring it back to the current reference position. In other words, even if the motor is where it should be, do not stop sending commands to the motor, instead send it 0 (or whatever torque value your controller produces).

The measured position and velocity (Pm and Vm) can be computed using the encoder. The encoder generates a pair of signals as the motor shaft rotates, which are used as input signals into 2 general I/O port pins. Use the code example for the PCINT ISR for these port pins and count the number of signal changes to track the position of the motor. The velocity can be determined by tracking the time between changes in position. Be careful that you track this correctly. If the motor is moving, the velocity should not be zero even if there was no change since the last time you measured the position (this is because encoder counts are discrete not continuous).

Position reference (Pr) is provided through the user interface.
Kp and Kd (i.e. gains) are the terms that you define that determine how your system behaves. You will need to determine these values experimentally -- there will be a reasonable range of gain values within which you can use the potentiometer to increase or decrease to modify the rise time.

**Things to Watch For**:
- Are your types correct? Do not declare something as a uint if it needs a sign!
- Are your types correct? Is there an implicit cast that is causing something to go to 0?
- Are you maintaining the sign in your torque equation THEN converting to an absolute value and setting the direction appropriately based on the sign?
- Is the frequency of your PD controller so fast that you don't see positional change from release to release? If so, keep track of how much time has passed since the last value change to get an accurate measure of velocity.

#### Motor

The motor is attached to the Motor2 port (green header, middle). This corresponds to OC1B for the PWM drive signal and PE2 as the directional signal. You use the torque value generated from the controller to adjust the duty cycle of the waveform, thus the speed of the motor.

The encoder channels should be connected to Port B4 and B5, which correspond to PCINT4 and PCINT5 (a.k.a. AStar pins 8 and 9). Power through Port D1 (AStar pin 2).

#### Potentiometers

Connect the potentiometers to Port D6 and Port D7. These correspond to ADC9 and ADC10 (a.k.a. AStar pins 6 and 12).

#### User Interface

The user interface should supply the following functionality at a minimum (feel free to add in whatever makes your life easier for programming and debugging):

Using key presses:
- R/r : Set the reference position in **degrees**. Value is RELATIVE to current position.
- Z/z: Zero the encoder marking the current position as 0 degrees.
- V/v: View the current values Kd, Kp, Vm, Pr, Pm, and T
- S/s: Send saved data

You might find it useful to have functionality to calibrate your potentiometers. For example, you might indicate that the current potentiometer position is zero such that any readings greater should increase the gain and readings less than the current value should decrease the gain. Or you can set the step size, such that a change of x in the potentiometer reading corresponds to a "step size" change in the gain, for example every time the potentiometer changes by 5 units, the kD will increase by 0.10.

You will need to figure out how to log your data. This might involve storing it in a buffer, then periodically dumping it to the terminal, or perhaps you are constantly printing to the terminal. You probably want a menu option to start and stop logging. "screen" on linux and mac will only show a screen full of data at a time, thus you will want to dump your data into a file. There is a logging option for screen -- more on this later, I have to track down the commands to do this.

<hr>

#### Code Deliverable

1. Hand in all of the code necessary to compile your project. Please put sufficient comments in your code so that we can follow what you are doing. Also, if your code is not working in some aspect, please include that in your report. Your code should include the following:
  - PD Controller
  - UI to specify a relative reference position
  - Gains Control using potentiometers
  - Data logging functionality


#### Report Deliverable (Please do this individually!)

2. Answer the following questions related to your motor and encoder.
  <ol type="a">
  <li> What is the granularity of the encoder in degrees?
  <li> What is the minimum drive signal required to move your motor from a stopped position? Specify this in terms of the TOP and Match values.
  <li> What is the approximate frequency of the encoder task (i.e. PCINT ISR) at 25% duty cycle and 75% duty cycle? (You can use ms_ticks to determine this.)
  </ol>

3. State how you are "scheduling" PD controller, UI, the potentiometer readings, and data logging/output. If there is an associated period, state the period and briefly justify why you used that period. __Note that you must schedule (or pseudo-schedule) your PD controller such that its period can be modified for one of the experiments.__

4. Experiment with the gains to establish a "reasonable" range. Do not set the torque to 0 at the moment you cross the reference position. Make sure you have settled into that position. You will probably have a threshold that, when T is below it, you set T to 0. Report your range of gain values for kP and kD.

  It might be useful to do the following to tune your gains.
  - Use only P. Start with a very low gain so that the motor runs very slowly and takes its time getting to the set position.
  - Increase the gain to get a fast response (i.e. get to your set point quickly). Then increase it a little more to oscillate.
  - Add in D to dampen the response and eliminate oscillation.

5. Experiment with the gains using two reference positions. Pick a “low,” “high,” and “ideal” gain based on above experiments.
  - Use each gain value to reach a reference position that is 2 full rotations from current position.
  - Use each gain value to reach a reference position that is 5 degrees from current position.
  **How precisely can you obtain your reference position? For each situation, report the error of the system (i.e. reference - measured) when it has settled, thus it is no longer moving.**

  <it>This type of error is a steady-state error. It __might__ be remedied by adding the I term of the PID controller. If you are so inclined, you can incorporate this into your system to see if it has any impact. The I term is the accumulation of error. As part of your PID, you would add I += P and modify the equation to T = kP\*P + kI\*I + kD\*D. This is optional.</it>


6. Now run the same experiments, except slow your controller task frequency to 2 Hz (or 5 Hz if this is too disruptive). **Report on your observations and explain why the system behaved the way that it did.**

7. Graph Pm, Pr and T while trying to reach a reference position of 180 degrees in the two scenarios above (i.e. using a _reasonable_ period for the controller and one that is too slow). It should be slow enough to cause visible oscillations, but not so slow that the motor looks like its going to oscillate itself apart. “While” means to record while executing but you don’t need to graph in real-time. You can do a screen dump of the data, then cut and paste into a file or send to a file. Use Excel or Open Office or whatever is your favorite/easiest graphing app to generate the plots.
