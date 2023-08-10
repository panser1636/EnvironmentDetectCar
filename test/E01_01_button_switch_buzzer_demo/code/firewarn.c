#include "zf_common_headfile.h"
#include "firewarn.h"
//if change pin should change isr
#define fire1_in D7 
#define fire2_in D5
#define fire3_in D14
#define fire4_in D6
#define fire5_in D4
#define BEEP E9
void fire_gpioinit()
{	gpio_init(fire1_in,GPI,0,GPI_PULL_UP);
	gpio_init(fire2_in,GPI,0,GPI_PULL_UP);
	gpio_init(fire3_in,GPI,0,GPI_PULL_UP);
	gpio_init(fire4_in,GPI,0,GPI_PULL_UP);
	gpio_init(fire5_in,GPI,0,GPI_PULL_UP);
	gpio_init(BEEP,GPO,0,GPO_PUSH_PULL);
//	exti_init(fire1_in, EXTI_TRIGGER_FALLING);
//	exti_init(fire2_in, EXTI_TRIGGER_FALLING);
//	exti_init(fire3_in, EXTI_TRIGGER_FALLING);
//	exti_init(fire4_in, EXTI_TRIGGER_FALLING);
//	exti_init(fire5_in, EXTI_TRIGGER_FALLING);
}
uint8 firedetect(){
		uint8 A,B,C,D,E,F=0;
		A=gpio_get_level(fire1_in);
		B=gpio_get_level(fire2_in);
		C=gpio_get_level(fire3_in);
		D=gpio_get_level(fire4_in);
		E=gpio_get_level(fire5_in);
		F=A&B&C&D&E;
		if(F==0){
		system_delay_ms(20);
		A=gpio_get_level(fire1_in);
		B=gpio_get_level(fire2_in);
		C=gpio_get_level(fire3_in);
		D=gpio_get_level(fire4_in);
		E=gpio_get_level(fire5_in);
		F=A&B&C&D&E;
		if(F==0) return 0;
		else return 1;
		}
		else return 1;	
}

void Beep_Short(){
  uint8 i=0;
	for(i=0;i<8;i++){ 
gpio_set_level(BEEP,1);
system_delay_ms(200+100*i);
gpio_set_level(BEEP,0);
system_delay_ms(200+100*i);
	}
}










