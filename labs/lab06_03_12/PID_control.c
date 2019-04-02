//Andrew Steinbrueck(3949010), Xin Yang(5064064)

#include motor.h

volatile int16_t global_counts_m2;
#define SCALER 1

void PID_Control(int32_t ref){
  	//scale reference position
  	scale_ref = ref*(2200/360);
  	
  	
  	//get the time and positions changes
  	int32_t previous_position = global_counts_m2; //this is from motor.cc
  	 _delay_ms(10);
  	int32_t current_position = global_counts_m2; 

	int duty_cycle = 0;
	
	int scaled_motor_t = SCALER*(scale_ref - global_counts_m2) + SCALER*((current_position - previous_position)/10);
	if (scaled_motor_t >= 0){
		motorForward();
		duty_cycle = scaled_motor_t;
	}else if(scaled_motor_t < 0){
		motorBackward();
		duty_cycle = -scaled_motor_t;
	}
	
	SetUpTimerPWM(1, 1024, 2200, duty_cycle);
}