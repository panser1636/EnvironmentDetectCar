//��Ƭ��ͷ�ļ�
//#include "main.h"

//�����豸����
#include "onenet.h"

//Ӳ������
#include "zf_driver_uart.h"

//C��
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

uint8_t aRxBuffer;			//�����жϻ���
//==========================================================
//	�������ƣ�	ESP01S_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP01S_Clear(void)
{

	memset(ESP01S_buf, 0, sizeof(ESP01S_buf));
	ESP01S_cnt = 0;
	rec_state = 0;

}

//==========================================================
//	�������ƣ�	ESP01S_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP01S_WaitRecive(void)
{
//		if(rec_state == 1) 
//		{
//			rec_state = 0;
//			return REV_OK;
//		}
//		else return REV_WAIT;
	if(ESP01S_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(ESP01S_cnt == ESP01S_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		ESP01S_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	ESP01S_cntPre = ESP01S_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP01S_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
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
//		if(ESP01S_WaitRecive() == REV_OK)							//����յ�����
//		{
//			//printf("SendCmd get the data\n");
//			printf("bufferdata:%s",(char *)ESP01S_buf);
//			if(strstr((const char *)ESP01S_buf, res) != NULL)		//����������ؼ���
//			{
//				ESP01S_Clear();									//��ջ���
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
//	�������ƣ�	ESP01S_Init
//
//	�������ܣ�	��ʼ��ESP01S
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
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
	while(ESP01S_SendCmd("AT+CWMODE=1\r\n", "OK")){}     //ģʽ1(Station),Ĭ�ϱ���Flash
	printf(" the 2 step is OK\n");	
	system_delay_ms(5000);
	//HAL_Delay(500);
	
	printf("3. AT+CWDHCP\r\n");
	//UsartPrintf(USART_DEBUG, "3. AT+CWDHCP\r\n");       //����DHCP(��ȡ����IP),Ĭ�ϱ���Flash
	while(ESP01S_SendCmd("AT+CWDHCP=1,1\r\n", "OK")){} 
	printf("the 3 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);
	
	printf("4. CWJAP\r\n");
	//UsartPrintf(USART_DEBUG, "4. CWJAP\r\n");           //����WIFI
	while(ESP01S_SendCmd(ESP01S_WIFI_INFO, "OK")){}
		printf("the 4 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);

	printf("5.MQTTUSERCFG\r\n");
	//UsartPrintf(USART_DEBUG, "5.MQTTUSERCFG\r\n");           //�û���Ϣ����
	sprintf(cmdBuf,"AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", ClintID,username,passwd);
	while(ESP01S_SendCmd(cmdBuf, "OK")){}
	printf("the 5 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);
	memset(cmdBuf,0,sizeof(cmdBuf));
	
	printf("6.MQTTCONN\r\n");
	//UsartPrintf(USART_DEBUG, "6.MQTTCONN\r\n");           //���ӷ�����
	sprintf(cmdBuf,"AT+MQTTCONN=0,\"%s\",1883,1\r\n",Url);
	while(ESP01S_SendCmd(cmdBuf, "OK")){}
	printf("the 6 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);
	memset(cmdBuf,0,sizeof(cmdBuf));
	
	printf("7.MQTTSUB\r\n");
	//UsartPrintf(USART_DEBUG, "7.MQTTSUB\r\n");           //������Ϣ
	sprintf(cmdBuf,"AT+MQTTSUB=0,\"%s\",1\r\n",subtopic);
	while(ESP01S_SendCmd(cmdBuf, "OK")){}
	printf("the 7 step is OK\n");
	system_delay_ms(5000);
	//HAL_Delay(500);
	memset(cmdBuf,0,sizeof(cmdBuf));
}


//���ܣ�esp������Ϣ
//��������
//����ֵ��0�����ͳɹ���1������ʧ��
void Esp_PUB(uint8 temp,double latitude,double longitude,uint8 wet,uint16 noise)
{
	
	char cmdBuf[512];
	//printf("Publish\r\n");
	//UsartPrintf(USART_DEBUG, "Publish\r\n");           //������Ϣ
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
 
//	if(ESP01S_cnt >= sizeof(ESP01S_buf))  //����ж�
//	{
//		ESP01S_cnt = 0;
//		memset(ESP01S_buf,0x00,sizeof(ESP01S_buf));
//		//HAL_UART_Transmit(&huart1, (uint8_t *)"���ջ������", 10,0xFFFF);  	      
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
//		//ESP01S_buf[ESP01S_cnt++] = aRxBuffer;   //��������ת��	
////		  if(aRxBuffer=='1')  HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
////        if(aRxBuffer=='0')  HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
// 	//}
//   fifo_clear(&onenet_receiver_fifo);
	
	//HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);   //�ٿ��������ж�
	//uart_rx_interrupt(ONENET_UART, 1); //??????,1:????
}


/* USER CODE END 4 */

