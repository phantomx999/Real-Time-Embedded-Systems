#include "PID_control.h"

int64_t PID_Control(int32_t ref,int32_t Kp, int32_t Kd, int64_t previous_t){
  int64_t Pr = ref*(2200/360);
  
  int32_t previous_position = global_counts_m2;
  _delay_ms(10);
  int32_t Pm = global_counts_m2;
  
  int32_t duty_cycle = 0;
  
  int64_t T = (Kp*(Pr - Pm))/10 + (Kd*((previous_position - Pm)/10))/10;
  
  if(previous_t != T) {
    printf("Pr: %d, ", Pr);
    printf("Pm: %d, ", Pm);
    printf("T: %d\n\r", T);
    //printf("global_counts_m2: %d\n\r", global_counts_m2);
  }
  
  if(T>2) {
    motorForward();
    duty_cycle = ((T*100)/2200);
	if (duty_cycle > 60){
		duty_cycle = 60;
	}
	SetUpTimerPWM(1, 1024, 2200, duty_cycle);
  }
  else if(T<-2) {
    motorBackward();
    duty_cycle = (T/2200)*100;
	if (duty_cycle > 60){
		duty_cycle = 60;
	}
	SetUpTimerPWM(1, 1024, 2200, duty_cycle);
  }
  else {
    SetUpTimerPWM(1, 1024, 2200, 0);
  }
  return(T);
  USB_Mainloop_Handler();
}