#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "timers.h"
#include "leds.h"

////////SET UP TIMER PWM fast mode//////////
int SetUpTimerPWM( int timer_num, int prescaler, int ms_period, float duty_cycle ) {
  if ((0 != timer_num) && ((1 != timer_num) && (3 != timer_num))) {
    return ERROR;
  }
  // determine the match value for the desired period given prescaler
  // @TODO check that this is an integer value -- if not give warning.
  // ********************************************************************* //
  // ******** MODIFY THIS LINE. Use the input parameters.  *************** //
  // ********************************************************************* //
  uint32_t top = (F_CPU/prescaler/1000) * ms_period;
  uint32_t match32 = duty_cycle * top;


  // Check that the match fits in its counter register (8-bit or 16-bit)
  if ((0 == timer_num) && (match32 > 255)) {
    return -1;
  } else if (match32 > 65535) {
    return ERROR;
  }

  // Get the clock-select bits for the chosen prescaler
  char CSbits = GetCSBits(prescaler);

  switch(timer_num) {
    case(1) : return SetUpTimer1_PWM(CSbits, (uint16_t) match32, top,  duty_cycle); break;
    case(3) : return SetUpTimer3_PWM(CSbits, (uint16_t) match32, top,  duty_cycle); break;
    default : return ERROR;
  }
  return 1;
}





/* Set up a timer using ctc mode.
 * parameter [in] timer_num : 0 (8-bit) or 1,3 (16-bit)
 * parameter [in] prescaler : 1,8,64, ... as indicated in datasheet options
 * parameter [in] ms_period : desired period of ISR
 * parameter [out] : -1 for error in setting up. 1 otherwise.
 */
int SetUpTimerCTC( int timer_num, int prescaler, int ms_period ) {
  if ((0 != timer_num) && ((1 != timer_num) && (3 != timer_num))) {
    return ERROR;
  }
  // determine the match value for the desired period given prescaler
  // @TODO check that this is an integer value -- if not give warning.
  // ********************************************************************* //
  // ******** MODIFY THIS LINE. Use the input parameters.  *************** //
  // ********************************************************************* //
  // uint32_t match32 = 1;
  uint32_t match32 = (F_CPU/prescaler/1000) * ms_period;
  //uint32_t match32 = processor_prescaler_val * (ms_period/1000);

  // Check that the match fits in its counter register (8-bit or 16-bit)
  if ((0 == timer_num) && (match32 > 255)) {
    return -1;
  } else if (match32 > 65535) {
    return ERROR;
  }

  // Get the clock-select bits for the chosen prescaler
  char CSbits = GetCSBits(prescaler);

  switch(timer_num) {
    case(0) : return SetUpTimer_0(CSbits, (uint8_t) match32); break;
    case(1) : return SetUpTimer_1(CSbits, (uint16_t) match32); break;
    case(3) : return SetUpTimer_3(CSbits, (uint16_t) match32); break;
    default : return ERROR;
  }
  return 1;
}

int SetUpTimer_0(char CSbits, uint8_t match) {

  // Clear the Control Registers for Timer 0
  TCCR0A = 0;
  TCCR0B = 0;

  // CTC mode is 2 = 010
  TCCR0A |= (1 << WGM01);
  // Clock Select Bits are bits 00000XXX in TCCR0B
  TCCR0B |= CSbits;
  // CTC uses OCR0A as top / match
  OCR0A = match;
  //////
  //TCNT0 = 0;
  /////
  // Enable the Interrupt on the OCR0A match
  TIMSK0 |= (1<<OCIE0A);
  
  return 1;
}

int SetUpTimer_1(char CSbits, uint16_t match) {
  // ********      FILL THIS IN **************************//
  TCCR1A = 0;
  TCCR1B = 0;
  
  ////for CTC and PWM
  TCCR1B |= (1 << WGM12);
  TCCR1B |= CSbits;
  
  ////for CTC, not PWM
  OCR1A = match;
  
  TIMSK1 |= (1<<OCIE1A);

  return 1;
}

int SetUpTimer_3(char CSbits, uint16_t match) {
  // ********      FILL THIS IN **************************//
  TCCR3A = 0;
  TCCR3B = 0;
  
  //  TCCR1A = (1 << COM1A0);
  TCCR3B |= (1 << WGM12);
  TCCR3B |= CSbits;
  OCR3A = match;
  TIMSK3 |= (1<<OCIE3A);
  return 1;
}

int SetUpTimer1_PWM(char CSbits, uint16_t match, uint32_t top, float duty_cycle) {
   TCCR1A = 0;
  TCCR1B = 0;
  //TIMSK1 = 0;
  
  ////for PWM, not CTC
  TCCR1A |= (1 << COM1B1);
  
  ////for PWM, not CTC
  TCCR1A |= (1 << WGM11);
  
  TCCR1B |= (1 << WGM13);
  TCCR1B |= (1 << WGM12);
  TCCR1B |= CSbits;
  
  
  ///for PWM, not CTC
  OCR1B = match;
  
   //// for PWM, not CTC
   ICR1 = top;
   
   //TIMSK1 |= (1<<OCIE1A);
   return 1;
  
  
}

int SetUpTimer3_PWM(char CSbits, uint16_t match, uint32_t top, float duty_cycle) {
   TCCR3A = 0;
  TCCR3B = 0;
  
  ////for PWM, not CTC
  TCCR3A |= (1 << COM3A1);
  
  ////for PWM, not CTC
  TCCR3A |= (1 << WGM31);
  
  TCCR3B |= (1 << WGM33);
  TCCR3B |= (1 << WGM32);
  TCCR3B |= CSbits;
  
  
  ///for PWM, not CTC
  OCR3A = match;
  
   //// for PWM, not CTC
   ICR3 = top;
   
   return 1;  
}


char GetCSBits(int prescaler) {
  // homegrown dictionary.
  // key: prescaler. value: clock select bits value
  switch(prescaler) {
    case(1) : return 1; break;
    case(8) : return 2; break;
    case(64) : return 3; break;
    case(256) : return 4; break;
    case(1024) : return 5; break;
    default : return 0;
  }
}
