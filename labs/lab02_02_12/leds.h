#ifndef LEDS_H_
#define LEDS_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include <inttypes.h>

#include "common.h"

#define INIT_YELLOW _yellow = (IO_struct) { &DDRC, &PORTC, 7, &PINC }; \
  DDRC |= (1<<DDC7);
#define INIT_GREEN  _green = (IO_struct) { &DDRD, &PORTD, 5, &PIND }; \
  DDRD |= (1<<DDD5);
#define INIT_RED _red = (IO_struct) { &DDRB, &PORTB, 0, &PINB }; \
  DDRB |= (1<<DDB0);

IO_struct _yellow;
IO_struct _red;
IO_struct _green;

/* initialize all the on-board leds by calling the macros defined above
 * for initializing the individual on-board leds
 */
void initialize_leds();

/* Flash the designated on-board led for 250ms on, then 250ms off.
 * Assumes led is initialized */
void flash_led(IO_struct * color, int inverted);

/* Flash all the leds for a sanity check light show */
void light_show();

#endif
