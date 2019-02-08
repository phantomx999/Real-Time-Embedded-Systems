#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "buttons.h"
#include "leds.h"

uint32_t flag_A = 1;
uint32_t flag_C = 1;

void blink_green(IO_struct * color) {
  CLEAR_BIT(*color->port, color->pin);
  //_delay_ms(250);
  TOGGLE_BIT(*color->port, color->pin);
  //_delay_ms(250);
}

void blink_yellow(IO_struct * color){
  SET_BIT(*color->port, color->pin);
  //_delay_ms(1250);
  TOGGLE_BIT(*color->port, color->pin);
  //_delay_ms(250);
}

void turn_on_green(){
	SET_BIT(*(&_green)->port, _green.pin);
}

void turn_off_green(){
	CLEAR_BIT(*(&_green)->port, _green.pin);
}

void turn_on_yellow(){
	CLEAR_BIT(*(&_yellow)->port, _yellow.pin);
}

void turn_off_yellow(){
	SET_BIT(*(&_yellow)->port, _yellow.pin);
}

void light_flash_green() {
  flash_led(&_green, 1);
 // blink_green(&_green);
  //return;
}

void light_flash_yellow() {
  flash_led(&_yellow, 0);
 // blink_yellow(&_yellow);
  //return;
}

// comment this line out when you are done debugging
// #define DEBUG

void EmptyFunction() {}

void initialize_buttons() {

  // Declare the data structures for the buttons.
  INIT_BUTTONA;
  INIT_BUTTONC;

  // initialize the button function pointers with a function that does nothing.
  // This way, you can always have a call to all of these functions in your
  // code, even if the system you are designing doesn't use some button .
  fn_release_A = EmptyFunction;
  fn_press_A = EmptyFunction;
  fn_release_C = EmptyFunction;
  fn_press_C = EmptyFunction;
}

/* Set up either of the two buttons on the board.
* parameter [in] IO_struct button : for A or C as defined above
* parameter [out] : -1 for error in setting up. 1 otherwise.
*/
int SetUpButton(IO_struct * button) {

  // Configure the data direction to input
  CLEAR_BIT(*button->ddr, button->pin);
  // ***** What about CONFIG_INPUT(*button->ddr, button->pin) ???
  // Enable Button pull-up resistor
  SET_BIT(*button->port, button->pin);

  // PCICR: Pin Change Interrupt Control Register
  // PCIE0: Pin Change Interrupt Enable Bit:
  //    Any change on any enabled PCINT7..0 can fire ISR.
  PCICR |= (1 << PCIE0);

  // PCMSK0: Pin Change Mask for Interrupt0, which is for all pins 0 through 7
  // Enable interrupts on Button A (PCINT3) and Button C (PCINT0)
  if (button->pin == BUTTONA) {
    PCMSK0 |= (1 << PCINT3);
  }
  if (button->pin == BUTTONC) {
    PCMSK0 |= (1 << PCINT0);
  }

  pinb_previous = PINB & BUTTON_MASK;
  return 0;
}

/* Set up callback functions for either of two buttons on the board.
* parameter [in] IO_struct button : for A or C as defined above
* parameter [in] release : 1 (true) = call function on release
* parameter [in] callback : function to be called when event happens
* parameter [out] : -1 for error in setting up. 1 otherwise.
*/
int SetUpButtonAction(IO_struct * button, int release, void(*callback)(void)) {
  // Set up for the callback function to be used when button event happens
  if (button->pin == BUTTONA) {
    if (release) {
      fn_release_A = callback;
    } else {
      fn_press_A = callback;
    }
  }
  if (button->pin == BUTTONC) {
    if (release) {
      fn_release_C = callback;
    } else {
      fn_press_C = callback;
    }
  }
}

ISR(PCINT0_vect) {

  #ifdef DEBUG
  // If debugging, it will be good to flash an led to know that you made
  // it into the ISR. Remeber that red and buttonC share the port, so don't
  // use the red led. If you soldered your headers, you can also set up
  // an LED in general IO to give yourself more options.
  TOGGLE_BIT(*(&_green)->port, _green.pin);
  _delay_ms(150);
  TOGGLE_BIT(*(&_green)->port, _green.pin);
  #endif

  // read port B, mask for only button A and C bits
  uint8_t pinb_now = (PINB & BUTTON_MASK);

  // check that pin change persists. ignore if not.
  _delay_ms(1);
  if (pinb_now ^ (PINB & BUTTON_MASK)) {
    return;
  }

  // record which button(s) changed state from last interrupt
  // remember that pinb_now holds state for only button A and C
  uint8_t pinb_change = (pinb_now ^ pinb_previous);

  // ************************************************************/
  // ***********    COMPLETE THIS PART BELOW ********************/
  // ************************************************************/
  // Determine if Button A has changed since the last time
  // if it has changed, determine how it changed state (press or release)
  // then have the system react appropriately according to requirements.

  // Determine if Button C has changed since the last time
  // if it has changed, determine how it changed state (press or release)
  // then have the system react appropriately according to requirements.
  // Determine if Button C has changed since the last time

  // You may make generic code using the fn_<state>_<Button> functions,
  // but you do not have to. You can put the code here in the ISR.
  
  if(pinb_change & (1 << BUTTONA)){
    if(flag_A == 1){
      flag_A++;
      SetUpButtonAction(&_button_A, 1, turn_on_green);
    }
    else if(flag_A == 2) {
      flag_A++;
      SetUpButtonAction(&_button_A, 1, light_flash_green); 
    }
    else if(flag_A == 3) {
      flag_A = 1;
      SetUpButtonAction(&_button_A, 1, turn_off_green);
    }
  }
  
  if(pinb_change & (1 << BUTTONC)){
    if(flag_C == 1){
      flag_C++;
      SetUpButtonAction(&_button_C, 1, turn_on_yellow);
    }
    else if(flag_C == 2) {
      flag_C++;
      SetUpButtonAction(&_button_C, 1, light_flash_yellow); 
    }
    else if(flag_C == 3){
      flag_C = 1;
      SetUpButtonAction(&_button_C, 1, turn_off_yellow);
    }
  }

  pinb_previous = pinb_now;
}
