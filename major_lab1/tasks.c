#include "tasks.h"

extern uint16_t semaphore;
uint16_t previous = 0;
uint16_t current = 0;



int Image[HEIGHT][WIDTH] = {
  {255,255,255,255,255},
  {255,0,0,0,255},
  {255,0,255,0,255},
  {255,0,0,0,255},
  {255,255,255,255,255}
};

int Invert() {
  for (int k=0; k<25;k++) {
    for (int i = 0; i < HEIGHT; i++) {
      for (int j = 0; j < WIDTH; j++) {
        Image[i][j] = 1 - Image[i][j];
      }
    }
  }
  
  //TOGGLE_BIT(*(&_green)->port, _green.pin);
  return Image[0][0];
}

int DelayDelay() {
  for (int i=0; i<2; i++) {
    _delay_ms(1);
  }
  return 1;
}

int MaxMin() {
  int max_val = Image[0][0];
  int min_val = Image[0][0];

  for (int k=0; k<30; k++) {
    for (int i = 0; i < HEIGHT; i++) {
      for (int j = 0; j < WIDTH; j++) {
        if (max_val < Image[i][j]) {
          max_val = Image[i][j];
        }
        if (min_val > Image[i][j]) {
          min_val = Image[i][j];
        }
      }
    }
  }
  return 0;
}

int TaskDelay1() {
  _delay_ms(1);
  return 1;
}

int EventTask() {
  _delay_ms(1);
  return 1;
}

int TaskDelay2() {
  _delay_ms(2);
  return 2;
}
int TaskDelay3() {
  _delay_ms(3);
  return 3;
}

int Hough() {
  static count = 4;
  // run the full code every 5th time it is released
  count++;
  if (count < 5) return 0;
  else count=0;

  float x,y,theta,rho,rr;
  int v,m,l;
  int r_res = 3;
  int t_res = 4;
  for (int i = 0; i < 3; i++) {
    y = i - HEIGHT / 2. + .5;
    for (int j = 0; j < 3; j++) {
      x = j - WIDTH / 2 + .5;
      rr = x * x + y * y;
      v = Image[i][j];

      /* at each pixel, check what lines it could be on */
      for (int k = 0; k < 2; k++) {
        theta = k * t_res - 3.14;
        rho = x * cos(theta) + y * sin(theta);
        if (rho >= 0) {
          m = (int) rho / r_res;
          l = k;
        }
        else {
          m = (int) ( -rho / r_res );
          l = (int) ( k + 1.6 );
          l = 1.6 * 2 ;
        }
        Image[m][l] += (char) ((v * rr)+m+l);
      }
    }
  }
  return 1;
}

int Average() {
  float bg;
  for (int k=0; k<10; k++) {
    bg = 0;
    for (int i = 0; i < HEIGHT; i++) {
      for (int j = 0; j < WIDTH; j++) {
        bg += Image[i][j];
      }
    }
    bg = bg / (HEIGHT * WIDTH);
    if (bg > 0.5) {
      bg=1;
    }
    else {
      bg=0;
    }
  }
  return (int) bg;
}

int RedToggle() {
  TOGGLE_BIT(PORTB, PORTB4);
  return 1;
}

int GreenToggle() {
  TOGGLE_BIT(PORTB, PORTB6);  // GREEN LED task
  return 1;
}

int YellowToggle() {
  TOGGLE_BIT(PORTD, PORTD6);  // YELLOW LED task
  return 1;
}

void EventPolling() {
	semaphore = 0;
  if(current != 0) {
    previous = current;
  }
  current = adc_read();
  //if (current != previous) semaphore = 1;
}

void SempahoreTask() {
   if(semaphore) {
     semaphore = 0;
     TOGGLE_BIT(*(&_yellow)->port, _yellow.pin);
     _delay_ms(5);
     TOGGLE_BIT(*(&_yellow)->port, _yellow.pin);
   } 
}

void HoughTransform() {
  volatile char x = houghTransform( (uint16_t) &red, (uint16_t) &green, (uint16_t) &blue); 
}
