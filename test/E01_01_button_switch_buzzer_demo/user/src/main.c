#include "zf_common_headfile.h"
#include <stdio.h>
#include <string.h>
#include "motor.h"
#include "oled.h"
#include "dht11.h"
#include "nrf24l01.h"
#include "zf_driver_timer.h"
#include "hcsr04.h"
#include "uart_esp32.h"
#include "servo.h"
#include "QMC5883.h"
#include "math.h"
#include "lm386.h"
#include "onenet.h"
#include "firewarn.h"

/////////////////////////////////全局变量//////////////////////////////////////////
//uint8_t res=5;
uint8 BUF[8]; 
uint8 ge=0,shi=0,bai=0,qian=0,wan=0;
uint8 Z_1=0,Z_2=0,Z_3=0;
uint32 F_1=0,F_2=0,F_3=0,F_4=0,F_5=0,F_6=0;
uint8 long_Z_1=0,long_Z_2=0,long_Z_3=0;
uint32 long_F_1=0,long_F_2=0,long_F_3=0,long_F_4=0,long_F_5=0,long_F_6=0;
extern uint8  gps_tau1201_flag; 
extern gps_info_struct gps_tau1201;
float lati=0,longi=0;
uint8_t res[3];  //APP数据

uint8 cnt_sent=0;

uint8 recv_buf;


void car_ctrl(uint8_t data)
{
	switch (data)	{
		case 0x01:forward();			
			break;
		case 0x02:backward();
			break;
		case 0x03:turnleft();
			break;
		case 0x04:turnright();
			break;
		default:stop();
			break;
	}	
}
///////////////////

//////===============================系统集合=========================================///////
void system_init(){
    clock_init(SYSTEM_CLOCK_120M);    // 初始化芯片时钟 工作频率为 120MHz
    debug_init();
	  inigpio();  //电机初始化
	  OLED_Init();
	  PWM_Init();//舵机初始化
//		hcsr04_init(); //超声波模块
//		DHT11_GPIO_Config(); //温度传感器
//		lm386_init();  //噪声传感器
		gps_init();
		printf("gps init done\n");
		ESP01S_Init();  //esp8266初始化
		ESP01S_Clear();
		printf("esp init done");
		//Serial_Readinit(); //
	

		uart_init(UART_5, 115200, UART5_TX_C12, UART5_RX_D2); //
//		NRF24L01_SPI_Init();  //射频模块
//		while(NRF24L01_Check());
//		NRF24L01_RX_Mode();
//	  uart_esp32_init();
		//fire_gpioinit();
	
	

	  
		//Init_QMC5883();
}


void oled_dispaly(DHT11_Data_TypeDef DHT11_Data,uint16 data_lm386,uint8 parse_flag){
		while(!Read_DHT11(&DHT11_Data));
		OLED_ShowString(1,1,"Tem:");
		OLED_ShowNum(1,5,DHT11_Data.temp_int,2);
		OLED_ShowString(1,7,".");
	  OLED_ShowNum(1,8,DHT11_Data.temp_deci,1);
		OLED_ShowString(1,10,"C");
		OLED_ShowString(1,12,"Noi");
		OLED_ShowString(2,1,"Hum:");
		OLED_ShowNum(2,5,DHT11_Data.humi_int,2);
//////////		OLED_ShowString(2,7,".");
//////////		OLED_ShowNum(2,8,DHT11_Data.humi_deci,1);
		OLED_ShowString(2,7,"%");
		OLED_ShowNum(2,11,recv_buf,2);
		OLED_ShowString(2,13,"dB");

	
	
		if(gps_tau1201_flag)
	{
			gps_tau1201_flag = 0;
			if(!parse_flag)          //开始解析数据
			{
				lati=gps_tau1201.latitude;
				longi=gps_tau1201.longitude;  
				  Z_1=lati/100;
          Z_2=(uint8)(lati/10)%10;
					Z_3=(uint8)lati%10;
					F_1 = (uint32)(lati*10)%10;
          F_2 = (uint32)(lati*100)%10;
          F_3 = (uint32)(lati*1000)%10;
          F_4 = (uint32)(lati*10000)%10;
          F_5=  (uint32)(lati*100000)%10;
          F_6 = (uint32)(lati*1000000)%10;
					
					long_Z_1=longi/100;
          long_Z_2=(uint8)(longi/10)%10;
					long_Z_3=(uint8)longi%10;
					long_F_1 = (uint32)(longi*10)%10;
          long_F_2 = (uint32)(longi*100)%10;
          long_F_3 = (uint32)(longi*1000)%10;
          long_F_4 = (uint32)(longi*10000)%10;
          long_F_5=  (uint32)(longi*100000)%10;
          long_F_6 = (uint32)(longi*1000000)%10;
				
				OLED_ShowString(3,1,"lat:");
				OLED_ShowNum(3,5,Z_1,1);
				OLED_ShowNum(3,6,Z_2,1);
				OLED_ShowNum(3,7,Z_3,1);
				OLED_ShowString(3,8,".");
				OLED_ShowNum(3,9,F_1,1);
				OLED_ShowNum(3,10,F_2,1);
				OLED_ShowNum(3,11,F_3,1);
				OLED_ShowNum(3,12,F_4,1);
				OLED_ShowNum(3,13,F_5,1);
				OLED_ShowNum(3,14,F_6,1);
				
				OLED_ShowString(4,1,"lon:");
				OLED_ShowNum(4,5,long_Z_1,1);
				OLED_ShowNum(4,6,long_Z_2,1);
				OLED_ShowNum(4,7,long_Z_3,1);
				OLED_ShowString(4,8,".");
				OLED_ShowNum(4,9, long_F_1,1);
				OLED_ShowNum(4,10,long_F_2,1);
				OLED_ShowNum(4,11,long_F_3,1);
				OLED_ShowNum(4,12,long_F_4,1);
				OLED_ShowNum(4,13,long_F_5,1);
				OLED_ShowNum(4,14,long_F_6,1);				
			}
	
		}
		system_delay_ms(500);
	}
		
	

void mode_gps(){//DHT11_Data_TypeDef DHT11_Data,uint16 data_lm386){
	if(cnt_sent++>=2){//降低同步的速
			//double temperature,humidity=0;
			//temperature=(double)DHT11_Data->temp_int+((double)DHT11_Data->temp_deci)/10;
			//humidity=(double)DHT11_Data->humi_int+((double)DHT11_Data->humi_deci)/10;
			//while(!Read_DHT11(&DHT11_Data));
			  uint8 a=10,b=10;
				Esp_PUB(a,gps_tau1201.latitude,gps_tau1201.longitude,a,a); //发送给云端的函数
				cnt_sent = 0;
		}
}

void route_design(double latitude, double longitude)
{
				uint8 sta = 0; //所在的区段
				uint8 isinit = 0; //sta是否已经初始化 1：已初始化 0：未初始化
				uint8 isready = 1; //是否已经在路线上 1：已在 0：不在
				double lat[4] = {23.169036,23.169036,23.168918,23.168918};//各个点的经纬度
				double lon[4] = {113.335544,113.336544,113.336544,113.335544};
				double tresh=0.00002;
				forward();
				system_delay_ms(500);
				if(!isinit)//判断小车位于的区段
				{
						if(fabs(latitude-lat[0])<tresh && longitude-lon[0]>tresh && lon[1]-longitude>tresh){sta = 1;isinit = 1;}
						else if(fabs(longitude-lon[1])<tresh && latitude -lat[1]>tresh && lat[2]-latitude>tresh){sta = 2;isinit = 1;}
						else if(fabs(latitude-lat[3])<tresh && lon[2]-longitude >tresh  && longitude-lon[3]>tresh){sta = 3;isinit = 1;}
						else if(fabs(longitude-lon[3])<tresh&& lat[3]-latitude> tresh && latitude-lat[0]>tresh){sta = 4;isinit = 1;}
						else {isready = 0;}
				}
	
				switch(sta)
				{
					case 1: 
									if(fabs(latitude-lat[0])<tresh) //确认没有偏离路线
									{
												if(longitude >= lon[1])
										{    sta=2;
												 turnright();
												 forward();
										
										}///*向右走*/
												else{
												forward();
												}//*向前走*/
									}
									else
									{
										if(latitude>lat[0]&&latitude-lat[0]>=0.0001)
										{		left();
												system_delay_ms(3000);
										}//向左走
										else if(latitude<lat[0]&&latitude-lat[0]>=0.0001) {
											right();
											system_delay_ms(3000);
										}//向右走
									}
									break;
					case 2: 
									if(fabs(longitude-lon[1])<tresh)
									{
											if(latitude >= lat[2]){sta=3;
												turnright();
												 forward();
											}
											else{ forward();
											}///*向右走*/
									}
									else{
										if(longitude > lon[1]&&longitude-lon[1]>=0.0001){backward();system_delay_ms(3000);}//向后走
										else if(longitude < lon[1]&&lon[1]-longitude>=0.0001) {forward();system_delay_ms(3000); }//向前走
									} 
									break;
					case 3: 
									if(fabs(latitude-lat[3])<tresh){
												if(longitude <= lon[3]){sta=4;turnright();
												 forward();}///*向左走*/
												else{forward();}//*向后走*/
									}
									else{
										if(latitude > lat[3]&&latitude-lat[3]>=0.0001){left();system_delay_ms(3000);}//向左走
										else if(latitude<lat[3]&&lat[3]-latitude>=0.0001) {right();system_delay_ms(3000);}//向右走
										break;
									}
					case 4: 
									if(fabs(longitude-lon[3])<tresh){
												if(latitude <= lat[0]){sta=1;turnright();
												 forward();}//*向前走*/
												else{forward();}///*向左走*/
									}
									else
									{
										if(longitude>lon[3]&&longitude-lon[3]>=0.0001){backward();}//向后走
										else if(longitude<lon[3]&&lon[3]-longitude>=0.0001) {forward(); }//向前走
									}break;
						
				}
}

int main (void)
{		
	
	 ////////////////////main函数变量声明////////////////////////////////////////
		DHT11_Data_TypeDef DHT11_Data;
		double angle=0;
		int X=0,Y=0,Z=0;
		uint8 check=0;
		uint16 a =0x09;
		uint16 b;
		uint16 data_lm386;
		uint8 parse_flag=1;
		uint8 cnt_sent = 0;
		//char data_lm386_str[16];
		res[0] = 0x05;	// 停止
	  res[1] = 0x01;	// gps模式
	  res[2] = 0x01;	// 舵机不旋转
		uint8_t rx_buf[33];   //射频模块信息
		char data_lm386_str[10];
//	////////////////初始化部分
		system_init();
		
	

while(1)
{		 
	 printf("in!");
	/////////////噪声串口调试
	  //data_lm386 = (uint8)lm386_convert();
		//sprintf(data_lm386_str, "%d", data_lm386);
		//uart_write_byte(UART_5, data_lm386);
  	//uart_write_string(UART_5, "\t");
	  system_delay_ms(200);	

		//data_lm386 = lm386_convert2();
		if(gps_tau1201_flag){
			gps_tau1201_flag=0;
			parse_flag=gps_data_parse();
		}
		else parse_flag=1;
//  oled_dispaly(DHT11_Data,data_lm386,parse_flag);
//		
		mode_gps();//DHT11_Data,recv_buf);
		
		
		
		//if(gps_tau1201.latitude&&gps_tau1201.longitude){
		//route_design(gps_tau1201.latitude,gps_tau1201.longitude);
		}
}
//		
//		if (res[1] == 0x01) { // gps模式
//				route_design(gps_tau1201.latitude,gps_tau1201.longitude);
//	 
//			}	else if (res[1] == 0x02) {	// app控制
//				car_ctrl(res[0]);
//			} else {	// 射频控制
//						if (NRF24L01_RxPacket(rx_buf) == 0) 
//					{
//							rx_buf[32] = '\0';	// 如果成功接收，加入字符串结束符
//							car_ctrl(rx_buf[0]);
//							//system_delay_ms(200);
//							// 后续操作
//						}
//			}
//			if (res[2] == 0x02) {
//				  patrol();               // 避障传感器旋转
//			} else {
//				   Non_patrol();              // 不旋转
//			
//			}
			
			

	
//		car_ctrl(res);

//		patrol();
//	turnleft();
//	stop();
//	system_delay_ms(1500);
//	patrol();



/////############################################################
//	clock_init(SYSTEM_CLOCK_120M);                                              // 初始化芯片时钟 工作频率为 120MHz
//  debug_init();                                                               // 初始化默认 Debug UART

//	uint8_t rx_buf[33];
//	uint8_t res;
//	NRF24L01_SPI_Init();
//	inigpio();
//	while(NRF24L01_Check())	 //检测NRF24L01是否存在
//	{

//	}
//	
//	NRF24L01_RX_Mode();
//	while(1) 
//	{
//		if (NRF24L01_RxPacket(rx_buf) == 0) 
//		{
//			rx_buf[32] = '\0';	// 如果成功接收，加入字符串结束符
//			car_ctrl(rx_buf[0]);
//		}
//	}
//}
