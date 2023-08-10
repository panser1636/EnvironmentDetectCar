#include "dht11.h"
#include "zf_common_headfile.h"


void DHT11_GPIO_Config(void)                //通信引脚初始化--高电平
{		
	gpio_init(DATA, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	gpio_set_level(DATA,1); 
}

static uint8_t Read_Byte(void)   //读取一个字节函数
{	  
   uint8_t i, temp=0;

	 for(i=0;i<8;i++)            //8位数据
	 { 
		 while(gpio_get_level(DATA)==0);  //每位数据有50us低电平，略过
		 system_delay_us(40);             //电平0有26-28us高电平，若是0延时过后会检测到0，1则检测1
		 if(gpio_get_level(DATA)==1)      //发送的是高电平
		 {
		 while(gpio_get_level(DATA)==1);  //等发完
		 temp|=(uint8_t)(0x01<<(7-i));    //将高电平1移到对应高位
		 }  
     else 
		 {
       temp&=(uint8_t)~(0x01<<(7-i));  //将低电平0移到对应高位
     } 

	 }
	  return temp;
}

uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  

   gpio_set_dir(DATA,GPO,GPO_PUSH_PULL);
   gpio_set_level(DATA,0);             //拉低总线至少18ms
   system_delay_ms(20);           
	 gpio_set_level(DATA,1);           //拉高20-40us
	 system_delay_us(30);  
   gpio_set_dir(DATA, GPI, GPI_FLOATING_IN); //浮空输入
	if(gpio_get_level(DATA)==0)       //从机响应
 {
   while(gpio_get_level(DATA)==0);	
	 while(gpio_get_level(DATA)==1);	 //跳过两个响应电平
	 DHT11_Data->humi_int= Read_Byte();				//读8位数据湿度整数
	 
	 DHT11_Data->humi_deci= Read_Byte();			//读8位数据湿度小数
			
	 DHT11_Data->temp_int= Read_Byte();			//读8位数据温度整数
	
	 DHT11_Data->temp_deci= Read_Byte();	  //读8位数据温度小数
					
	 DHT11_Data->check_sum= Read_Byte();	    //读8位校验和
	 
	 gpio_set_dir(DATA, GPO,GPO_PUSH_PULL);  //读完拉高电平
	 gpio_set_level(DATA,1);
	 
	 if(DHT11_Data->check_sum == (uint8_t)(DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci))
	 {return 1;}       //校验和正确，数据正确
	 else return 1;  //校验和错误
	 }
	 else 
	   return 0;  //从机保持高电平无响应
}
	  

