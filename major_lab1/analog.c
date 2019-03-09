
#include <avr/io.h>
#include "analog.h"

// initialize adc
void adc_init() {
  // YOU NEED TO POWER THIS SOMEHOW.
  // I am using "Power" in main.c

    // AREF = AVcc
    ADMUX = (1<<REFS0);

    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

    ADCSRB |= (1<<MUX5);
}

// read adc value
uint16_t adc_read(void) {

    // start single conversion
    // write '1' to ADSC
    ADCSRA |= (1<<ADSC);

    // wait for conversion to complete
    // ADSC becomes '0' again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));

    return (ADC);
}
