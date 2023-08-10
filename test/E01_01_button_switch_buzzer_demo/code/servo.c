#include "zf_common_headfile.h"
#include "servo.h"
#include "zf_driver_pwm.h"
#include "hcsr04.h"
#include "oled.h"
#include "motor.h"

#define PWM_Pin  TIM2_PWM_CH1_A15 


void PWM_Init(){
	gpio_init(A15,GPO,0,GPO_AF_PUSH_PULL);
	pwm_init(PWM_Pin,50,0);

}


void patrol(){
	uint8 i=0;
	int32 distance;
	for(i=0;i<5;i++){
	pwm_set_duty(PWM_Pin,250+i*250);
	distance=Hcsr04GetDistance();
	if(distance<40){
		switch(i){
			case 0:
				left();
				system_delay_ms(1000);
				stop();
			break;
			case 1:xie_leftbackward();
				system_delay_ms(1000);
				stop();
			break;
			case 2:	backward();
				system_delay_ms(1000);
				stop();
			break;
			case 3:xie_rightbackward();
				system_delay_ms(1000);
				stop();
			break;
			case 4:right();
				system_delay_ms(1000);
				stop();
			break;
		}
	}
	system_delay_ms(1000);
	}
}

void Non_patrol(){
pwm_set_duty(PWM_Pin,750);
	
}