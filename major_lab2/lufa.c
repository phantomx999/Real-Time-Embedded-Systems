#include "lufa.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>


extern char set_up_experiment;

#define menuString "r(efence value) c(ontroler period) p(KP) d(KD) g(o)\r\n"

extern int64_t ref;
extern int32_t PD_Period;
extern volatile int32_t Kp;
extern volatile int32_t Kd;
extern char in_ui_mode;
extern int32_t go_experiment = 0;


void handleInput( char c ) {
  // WARNING: This uses a busy-wait, thus will block main loop until done
  const int COMMAND_BUFF_LEN = 16;
  int i;
  char command[COMMAND_BUFF_LEN+1];

  USB_Mainloop_Handler();

  // Get chars until end-of-line received or buffer full
  for (i = 0; i < COMMAND_BUFF_LEN; i++) {

    // first char rcd as input param. next char fetched at bottom of for-loop
    // if its backspace, reset index
    if (c == '\b') {
      i -= 2;
      printf("\b \b");
      continue;
    }
    // if newline, go back 1 to write over
    else if (c == '\n') {
      i--;
      continue;
    }
    // if return, we have complete input
    else if (c == '\r') {
      break;
    }

    // otherwise, we got a char - echo it back
    printf("%c", c);
    USB_Mainloop_Handler();
    command[i] = c;

    // in busy-wait until char received
    c = EOF;
    while ( c == EOF ) {
      USB_Mainloop_Handler();
      c = fgetc(stdin);
    }
  }

  // echo back the return
  printf("\r\n");
  USB_Mainloop_Handler();

  // buffer is full - ignore input
  if (i == COMMAND_BUFF_LEN) {
    printf("Buffer full\r\n");
    command[COMMAND_BUFF_LEN-1]='\r';
    USB_Mainloop_Handler();
    return;
  }

  command[i] = '\0';
  if (strlen(command) == 0) {
    return;
  }
  handleCommand( command );
  return;
}

void handleCommand( char *command ) {
  char com_r;
  char com_c;
  char com_p;
  char com_d;
  
  switch (command[0]) {
    case('r'):
    case('R');
      sscanf(command, "%c", "%d", &com_r, &ref);
      printf("Set reference value as %d\n\r", ref);
      break;
    case('c'):
    case('C'):
      sscanf(command, "%c", "%d", &com_c, &PD_Period);
      printf("Set PD control value %d\n\r", PD_Period);
      break;
    case('p');
    case('P'):
      sscanf(command, "%c", "%d", &com_p, &Kp);
      printf("Set PD control value %d\n\r", Kp);
      break;
    case('d'):
    case('D'):
      sscanf(command, "%c", "%d", &com_d, &Kd);
      printf("Set PD control value %d\n\r", Kd);
      break;
    case('g'):
    case('G'):
      printf("START!!!\n\r");
      go_experiment = 1;
      break;
    default:
      print(menuString);
  }
  USB_Mainloop_Handler();
}