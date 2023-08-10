#include "zf_common_headfile.h"

#define PWM_Motor TIM3_PWM_CH4_B1
void inigpio(){

	gpio_init(A0, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(A2, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(A1, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(A3, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(B4, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(B5, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(D12, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(D13, GPO, GPIO_LOW, GPO_PUSH_PULL);	
	gpio_init(B1,GPO,0,GPO_AF_PUSH_PULL);
	pwm_init(PWM_Motor,10000,0);
	pwm_set_duty(PWM_Motor,8000);
}

void backward(){
	gpio_set_level(A0,GPIO_HIGH);
	gpio_set_level(A2,GPIO_LOW);
	gpio_set_level(A1,GPIO_HIGH);
	gpio_set_level(A3,GPIO_LOW);
	gpio_set_level(B4,GPIO_HIGH);
	gpio_set_level(B5,GPIO_LOW);
	gpio_set_level(D12,GPIO_LOW);
	gpio_set_level(D13,GPIO_HIGH);
	
}


void forward(){
	gpio_set_level(A0,GPIO_LOW); 
	gpio_set_level(A2,GPIO_HIGH); 
	gpio_set_level(A1,GPIO_LOW);
	gpio_set_level(A3,GPIO_HIGH);
	gpio_set_level(B4,GPIO_LOW);
	gpio_set_level(B5,GPIO_HIGH);
	gpio_set_level(D13,GPIO_LOW);
	gpio_set_level(D12,GPIO_HIGH);
}

void left(){
  gpio_set_level(A0,GPIO_LOW);
	gpio_set_level(A2,GPIO_HIGH);   //后左
	
	gpio_set_level(A1,GPIO_HIGH);
	gpio_set_level(A3,GPIO_LOW);  //后右
	
	gpio_set_level(B4,GPIO_HIGH);
	gpio_set_level(B5,GPIO_LOW);   //前左
	
	gpio_set_level(D12,GPIO_HIGH);
	gpio_set_level(D13,GPIO_LOW); //前右

}

void right(){
	  gpio_set_level(A0,GPIO_HIGH);
		gpio_set_level(A2,GPIO_LOW);
	
		gpio_set_level(A1,GPIO_LOW);
		gpio_set_level(A3,GPIO_HIGH);
	
		gpio_set_level(B4,GPIO_LOW);
		gpio_set_level(B5,GPIO_HIGH);
		gpio_set_level(D12,GPIO_LOW);
		gpio_set_level(D13,GPIO_HIGH);

}

void stop(){
		gpio_set_level(A0,GPIO_LOW);
		gpio_set_level(A2,GPIO_LOW);
		gpio_set_level(A1,GPIO_LOW);
		gpio_set_level(A3,GPIO_LOW);
		gpio_set_level(B4,GPIO_LOW);
		gpio_set_level(B5,GPIO_LOW);
		gpio_set_level(D12,GPIO_LOW);
		gpio_set_level(D13,GPIO_LOW);
}
void turnleft(){
	gpio_set_level(A1,GPIO_LOW);
	gpio_set_level(A3,GPIO_HIGH);  //后右
	
	gpio_set_level(D12,GPIO_HIGH);
	gpio_set_level(D13,GPIO_LOW); //前右

	gpio_set_level(A0,GPIO_HIGH);
	gpio_set_level(A2,GPIO_LOW);
	
	gpio_set_level(B4,GPIO_HIGH);
	gpio_set_level(B5,GPIO_LOW);
	//system_delay_ms(630);//延时650
}

void turnright(){
	gpio_set_level(A1,GPIO_HIGH);
	gpio_set_level(A3,GPIO_LOW);  //后右
	
	gpio_set_level(D12,GPIO_LOW);
	gpio_set_level(D13,GPIO_HIGH); //前右

	gpio_set_level(A0,GPIO_LOW);
	gpio_set_level(A2,GPIO_HIGH);
	
	gpio_set_level(B4,GPIO_LOW);
	gpio_set_level(B5,GPIO_HIGH);
//	system_delay_ms(630);//延时650
}

void xie_leftbackward(){
	gpio_set_level(D13,GPIO_LOW);
	gpio_set_level(D12,GPIO_LOW); //右前
	
	gpio_set_level(A0,GPIO_LOW);
	gpio_set_level(A2,GPIO_LOW);   //左后
	
	gpio_set_level(A1,GPIO_HIGH);
	gpio_set_level(A3,GPIO_LOW);
	gpio_set_level(B4,GPIO_HIGH);
	gpio_set_level(B5,GPIO_LOW);
	
}
void xie_rightbackward(){
	gpio_set_level(D13,GPIO_HIGH);
	gpio_set_level(D12,GPIO_LOW); //右前
	
	gpio_set_level(A0,GPIO_HIGH);
	gpio_set_level(A2,GPIO_LOW); //左后
	
	gpio_set_level(A1,GPIO_LOW);
	gpio_set_level(A3,GPIO_LOW);
	gpio_set_level(B4,GPIO_LOW);
	gpio_set_level(B5,GPIO_LOW);
	
}




