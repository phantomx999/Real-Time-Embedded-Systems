##### CSCI5143 Spring 2019

#### Lab 5 : PWM controlled LEDs
> DUE: Tuesday, February 26 before class <br>
> Behavior will be checked in class.

Using the code from previous labs, create an interactive PWM-controlled LED system. Assessment will be conducted during class (i.e. Gautham and I will come around and confirm that the LED behavior is being modified through user interaction.)

- Use OC1B (PB6) for one LED, which is the same from Lab 04. For this LED, you will interactively control the toggle rate of the LED.

- Use OC3A (PC6) for the other LED. For this LED, you will interactively control the brightness of the LED.

Use either your buttons or your serial communication to control the LEDs. For example, when you release button A, this could increase the toggle rate of the OC1B LED, up to the maximum rate, then start over or then have it decrease. Button C could be used to similarly control the brightness of the LED. (The specifics of the behavior and how it is controlled is not really important. What I will be looking for is the behavior of the LED changing based on user input.)
