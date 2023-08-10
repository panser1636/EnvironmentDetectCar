//单片机头文件
//#include "main.h"

//网络设备驱动
#include "onenet.h"

//硬件驱动
#include "zf_driver_uart.h"

//C库
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "zf_driver_delay.h"
#include "zf_common_fifo.h"
#include "zf_common_headfile.h"

#define ESP01S_WIFI_INFO		"AT+CWJAP=\"Redmi K30i 5G\",\"147258369hyx\"\r\n"
const char* ClintID="temperature";
const char* username="lI0m9249Qa";
const char* passwd="version=2018-10-31&res=products%2FlI0m9249Qa%2Fdevices%2Ftemperature&et=1721311683&method=md5&sign=8b%2Fj85CGvLOZRMO39jTEiA%3D%3D";
const char* Url="mqtts.heclouds.com";
const char* pubtopic="$sys/lI0m9249Qa/temperature/thing/property/post";
const char* subtopic="$sys/lI0m9249Qa/temperature/thing/property/post/reply";
const char* func1="tempwet";
const char* func2="pos";
const char* func3="noise";

unsigned char ESP01S_buf[256];
//uint8 ESP01S_buf[256];
unsigned short ESP01S_cnt = 0, ESP01S_cntPre = 0;

#define ONENET_BUFFER_SIZE (256)
static  fifo_struct        onenet_receiver_fifo;
static  uint8              onenet_receiver_buffer[ONENET_BUFFER_SIZE];   // ??????

uint8 rec_state = 0; //0:not received; 1:received 

uint8_t aRxBuffer;			//接收中断缓冲
//==========================================================
//	函数名称：	ESP01S_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP01S_Clear(void)
{

	memset(ESP01S_buf, 0, sizeof(ESP01S_buf));
	ESP01S_cnt = 0;
	rec_state = 0;

}

//==========================================================
//	函数名称：	ESP01S_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP01S_WaitRecive(void)
{
//		if(rec_state == 1) 
//		{
//			rec_state = 0;
//			return REV_OK;
//		}
//		else return REV_WAIT;
	if(ESP01S_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(ESP01S_cnt == ESP01S_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		ESP01S_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	ESP01S_cntPre = ESP01S_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP01S_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP01S_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;
	//uart_tx_interrupt(ONENET_UART, 1); //??????,1:????
	uart_write_string(ONENET_UART, (const char *)cmd);
	printf("write string done!\n");
	//uart_tx_interrupt(ONENET_UART, 0); //??????,1:????
	
	
	//Usart_SendString(huart2, (unsigned char *)cmd, strlen((const char *)cmd));
//	uart_rx_interrupt(ONENET_UART, 1); //??????,1:????
//	while(timeOut--)
//	{
//		printf("SW!");
//		if(ESP01S_WaitRecive() == REV_OK)							//如果收到数据
//		{
//			//printf("SendCmd get the data\n");
//			printf("bufferdata:%s",(char *)ESP01S_buf);
//			if(strstr((const char *)ESP01S_buf, res) != NULL)		//如果检索到关键词
//			{
//				ESP01S_Clear();									//清空缓存
//				printf("Sendcmd match OK");
//				return 0;
//			}
//			return 0;
//		}
//		
//		system_delay_ms(1000);
//		//HAL_Delay(10);
//	}
//	
//	return 1;
return 0;

}



//==========================================================
//	函数名称：	ESP01S_Init
//
//	函数功能：	初始化ESP01S
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================


void ESP01S_Init(void)
{
	
	uart_init(ONENET_UART, 115200, ONENET_RX, ONENET_TX);
	//fifo_init(&onenet_receiver_fifo, FIFO_DATA_8BIT, onenet_receiver_buffer, ONENET_BUFFER_SIZE);
	//uart_rx_interrupt(ONENET_UART, 1); //??????,1:????
	
	
	char cmdBuf[512];
	
	ESP01S_Clear();
	//system_delay_ms(5000);
	printf("0. AT\r\n");
	//UsartPrintf(USART_DEBUG, "0. AT\r\n");
	while(ESP01S_SendCmd("AT\r\n", "OK")){}
	printf("the 1 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);
	
//	printf("1. RST\r\n");
//	//UsartPrintf(USART_DEBUG, "1. RST\r\n");
//	ESP01S_SendCmd("AT+RST\r\n", "");
//	printf("OK\n");
//     system_delay_ms(5000); 
	//HAL_Delay(500);	
	
	printf( "2. CWMODE\r\n");
	//UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while(ESP01S_SendCmd("AT+CWMODE=1\r\n", "OK")){}     //模式1(Station),默认保存Flash
	printf(" the 2 step is OK\n");	
	system_delay_ms(5000);
	//HAL_Delay(500);
	
	printf("3. AT+CWDHCP\r\n");
	//UsartPrintf(USART_DEBUG, "3. AT+CWDHCP\r\n");       //开启DHCP(获取分配IP),默认保存Flash
	while(ESP01S_SendCmd("AT+CWDHCP=1,1\r\n", "OK")){} 
	printf("the 3 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);
	
	printf("4. CWJAP\r\n");
	//UsartPrintf(USART_DEBUG, "4. CWJAP\r\n");           //链接WIFI
	while(ESP01S_SendCmd(ESP01S_WIFI_INFO, "OK")){}
		printf("the 4 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);

	printf("5.MQTTUSERCFG\r\n");
	//UsartPrintf(USART_DEBUG, "5.MQTTUSERCFG\r\n");           //用户信息配置
	sprintf(cmdBuf,"AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", ClintID,username,passwd);
	while(ESP01S_SendCmd(cmdBuf, "OK")){}
	printf("the 5 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);
	memset(cmdBuf,0,sizeof(cmdBuf));
	
	printf("6.MQTTCONN\r\n");
	//UsartPrintf(USART_DEBUG, "6.MQTTCONN\r\n");           //连接服务器
	sprintf(cmdBuf,"AT+MQTTCONN=0,\"%s\",1883,1\r\n",Url);
	while(ESP01S_SendCmd(cmdBuf, "OK")){}
	printf("the 6 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);
	memset(cmdBuf,0,sizeof(cmdBuf));
	
	printf("7.MQTTSUB\r\n");
	//UsartPrintf(USART_DEBUG, "7.MQTTSUB\r\n");           //订阅消息
	sprintf(cmdBuf,"AT+MQTTSUB=0,\"%s\",1\r\n",subtopic);
	while(ESP01S_SendCmd(cmdBuf, "OK")){}
	printf("the 7 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);
	memset(cmdBuf,0,sizeof(cmdBuf));
}


//功能：esp发送消息
//参数：无
//返回值：0：发送成功；1：发送失败
void Esp_PUB(uint8 temp,double latitude,double longitude,uint8 wet,uint16 noise)
{
	
	char cmdBuf[512];
	//printf("Publish\r\n");
	//UsartPrintf(USART_DEBUG, "Publish\r\n");           //订阅消息
  sprintf(cmdBuf,"AT+MQTTPUB=0,\"%s\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"%s\\\":{\\\"value\\\":[%d\\,%d]\\}\\,\\\"%s\\\":{\\\"value\\\":[%f\\,%f]\\}\\,\\\"%s\\\":{\\\"value\\\":%d\\}}}\",0,0\r\n",pubtopic,func1,temp,wet,func2,latitude,longitude,func3,noise);
	while(ESP01S_SendCmd(cmdBuf, "OK")){}
	//system_delay_ms(5000); //should stay some time to respond 
	printf("update data is OK!\n");
	//system_delay_ms(500);
	//HAL_Delay(500);
	memset(cmdBuf,0,sizeof(cmdBuf));
	system_delay_ms(500);
	//HAL_Delay(100);
	
}





/* USER CODE BEGIN 4 */


void ONENET_UART_RxCpltCallback()
{
		uint32 temp_length;
		uint8_t dat;
		
    //UNUSED(huart);
 
//	if(ESP01S_cnt >= sizeof(ESP01S_buf))  //溢出判断
//	{
//		ESP01S_cnt = 0;
//		memset(ESP01S_buf,0x00,sizeof(ESP01S_buf));
//		//HAL_UART_Transmit(&huart1, (uint8_t *)"接收缓存溢出", 10,0xFFFF);  	      
//	}
//	else
//	{
	  //printf("go in ONENET_UART_RxCpltCallback_while\n");
	
		if(ESP01S_cnt >= 256){ESP01S_cnt=0;}
		else {
			ESP01S_buf[ESP01S_cnt++] = uart_read_byte(ONENET_UART);
		}
		printf("ESP01S_buf[%d]:%s",ESP01S_cnt,(char*)ESP01S_buf);	 
	
//		while(uart_query_byte(ONENET_UART, &dat))
//		{
//			ESP01S_buf[ESP01S_cnt++] = dat;
////			printf("go in ONENET_UART_RxCpltCallback_while\n");
////			if(fifo_used(&onenet_receiver_fifo) > sizeof(ESP01S_buf))
////			{
////				fifo_clear(&onenet_receiver_fifo);
////				printf("clear fifo:onenet_receiver!\n");
////			}
////			else{
////				printf("fifowriting...\n");
////				fifo_write_buffer(&onenet_receiver_fifo, &dat, 1);
////			}
//			
//		}
//		printf("fiforeading..\n");
//		temp_length = fifo_used(&onenet_receiver_fifo); //??fifo?????
//    fifo_read_buffer(&onenet_receiver_fifo, ESP01S_buf, &temp_length, FIFO_READ_AND_CLEAN);
//	  rec_state = 1;
//		//ESP01S_buf[ESP01S_cnt++] = aRxBuffer;   //接收数据转存	
////		  if(aRxBuffer=='1')  HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
////        if(aRxBuffer=='0')  HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
// 	//}
//   fifo_clear(&onenet_receiver_fifo);
	
	//HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断
	//uart_rx_interrupt(ONENET_UART, 1); //??????,1:????
}


/* USER CODE END 4 */

