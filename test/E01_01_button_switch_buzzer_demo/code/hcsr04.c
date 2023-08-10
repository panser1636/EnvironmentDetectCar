#include "zf_driver_timer.h"
#include "zf_common_headfile.h"
#include "hcsr04.h"
#include "zf_driver_timer.h"

#define trigger_pin B13
#define feedback_pin B15
#define timer TIM_1

void hcsr04_init()
{ gpio_init(trigger_pin,GPO,0,GPO_PUSH_PULL);
	gpio_init(feedback_pin,GPI,0,GPI_FLOATING_IN);
	timer_init(timer,TIMER_US);
}

uint32 Hcsr04GetDistance(void)
{
				uint16 t= 0;
				uint32 Distance = 0;
				gpio_set_level(trigger_pin,1);    					
				system_delay_us(10);									
				gpio_set_level(trigger_pin,0);								
				while(gpio_get_level(feedback_pin)==0);      				
				timer_start(timer);
				while(gpio_get_level(feedback_pin)==1);
				timer_stop(timer); 
				t = timer_get(timer);  
				Distance = (t/58);
				timer_clear(timer);	
        return Distance;   //cm
}

uint32 Distan_average()
{ uint8 i=0;
	uint32 sum;
		for(i=0;i<3;i++){
			sum=sum+Hcsr04GetDistance();
			system_delay_ms(10);
		}
	return sum/6;

}
