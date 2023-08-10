#include "zf_common_headfile.h"
#include "math.h"
extern uint8 BUF[8]; 	//���ݻ���
extern uint8 ge,shi,bai,qian,wan;
#define SDA  B14 
#define SCL		B12
#define	Slave_Address   0x1a

void QMC_gpio_init(){
gpio_init(SDA,GPO,1,GPO_PUSH_PULL);
gpio_init(SCL,GPO,1,GPO_PUSH_PULL);
}


/**************************************
��ʼ�ź�
**************************************/
void QMC5883_Start()
{		
		gpio_set_dir(SDA,GPO,GPO_PUSH_PULL);
    gpio_set_level(SDA,1);                  //����������
    gpio_set_level(SCL,1);
    system_delay_us(3);                 //��ʱ
    gpio_set_level(SDA,0);                    //�����½���
    system_delay_us(3);                 //��ʱ
    gpio_set_level(SCL,0);                    //����ʱ����
}

/**************************************
ֹͣ�ź�
**************************************/
void QMC5883_Stop()
{		
		gpio_set_dir(SDA,GPO,GPO_PUSH_PULL);
    gpio_set_level(SCL,0);
    gpio_set_level(SDA,0);	//����������
		system_delay_us(3);		
		gpio_set_level(SCL,1);                   //����ʱ����
    system_delay_us(3);                 //��ʱ
    gpio_set_level(SDA,1);                    //����������
    system_delay_us(3);                 //��ʱ
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void QMC5883_SendACK(uint8 ack)  //0 send answer and 1 notsend
{		
	  gpio_set_level(SCL,0);
		gpio_set_dir(SDA,GPO,GPO_PUSH_PULL);
    gpio_set_level(SDA,ack);       //дӦ���ź�
		system_delay_us(3); 
    gpio_set_level(SCL,1);                    //����ʱ����
    system_delay_us(3);                 //��ʱ
    gpio_set_level(SCL,0);                    //����ʱ����
    //system_delay_us(5);                 //��ʱ
}

/**************************************
����Ӧ���ź�
**************************************/
uint8 QMC5883_RecvACK()
{		uint8 ucErrTime=0;
		gpio_set_dir(SDA,GPI,GPI_FLOATING_IN);
		gpio_set_level(SDA,1);
		system_delay_us(3);
    gpio_set_level(SCL,1);       //����ʱ����
    system_delay_us(3);                //��ʱ
    while(gpio_get_level(SDA)==1)
		{  ucErrTime++;
			if(ucErrTime>250){
				QMC5883_Stop();
				return 1;
			}
		}
		                   //��λ��־ ��Ӧ���ź�
    gpio_set_level(SCL,0);       //����ʱ����
		//system_delay_us(5);     //��ʱ
    return 0;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void QMC5883_SendByte(uint8 dat)
{
    uint8 i;
		gpio_set_dir(SDA,GPO,GPO_PUSH_PULL);
		gpio_set_level(SCL,0);
    for (i=0; i<8; i++)         //8λ������
    {		
//			if((dat&0x80)>>7)
//			gpio_set_level(SDA,1);
//			else
//			gpio_set_level(SDA,0);
      gpio_set_level(SDA,(dat&0x80)>>7);
			dat<<= 1;              //�Ƴ����ݵ����λ
			system_delay_us(3);	
			gpio_set_level(SCL,1);           //����ʱ����
      system_delay_us(3);             //��ʱ
      gpio_set_level(SCL,0);          //����ʱ����
      system_delay_us(3);             //��ʱ
    }
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
uint8 QMC5883_RecvByte()//uint8 ack
{
    uint8 i;
    uint8 rece_data = 0;
		gpio_set_dir(SDA,GPI,GPI_FLOATING_IN);
    for (i=0; i<8; i++)         //8λ������
    {		
				gpio_set_level(SCL,0);
				system_delay_us(3);
				gpio_set_level(SCL,1);
				rece_data<<=1;                          //����ʱ����
       if(gpio_get_level(SDA)==1)
					rece_data++;   
					system_delay_us(3); 
	}					 
		//QMC5883_SendACK(ack);
		return rece_data;
}

//************************д�뵥�ֽ�����***************************

uint8 Single_Write_QMC5883(uint8 REG_Address,uint8 REG_data)
{		uint8 ret=0;
    QMC5883_Start();                  //��ʼ�ź�
    QMC5883_SendByte(Slave_Address);   //�����豸��ַ+д�ź�
    ret&= QMC5883_RecvACK();
	  QMC5883_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf 
    ret&= QMC5883_RecvACK();
		QMC5883_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf
		ret&= QMC5883_RecvACK();
	  QMC5883_Stop();
                  //����ֹͣ�ź�
		return ret;
}

//************************��ȡ���ֽ�����*************************
uint8 Single_Read_QMC5883(uint8 REG_Address)
{   
	uint8 REG_data=0;
	QMC5883_Start();                          //��ʼ�ź�
	QMC5883_SendByte(Slave_Address);           //�����豸��ַ+д�ź�
	while(QMC5883_RecvACK()==0);
	QMC5883_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ	
	while(QMC5883_RecvACK()==0);
	QMC5883_Start();                          //��ʼ�ź�
	QMC5883_SendByte(Slave_Address+1);         //�����豸��ַ+���ź�
	while(QMC5883_RecvACK()==0);
	REG_data=QMC5883_RecvByte(); //0             //�����Ĵ�������
	QMC5883_SendACK(1);   
	QMC5883_Stop();	//ֹͣ�ź�
  return REG_data; 
}

//******************************************************
//��������QMC5883�ڲ��Ƕ����ݣ���ַ��Χ0x00~0x05
//******************************************************
void Multiple_Read_QMC5883(void)
{   
    uint8 i;
    QMC5883_Start();                          //��ʼ�ź�
    QMC5883_SendByte(Slave_Address);          //�����豸��ַ+д�ź�
    QMC5883_SendByte(0x00);                   //���ʹ洢��Ԫ��ַ����0x00��ʼ	
    QMC5883_Start();                          //��ʼ�ź�
    QMC5883_SendByte(Slave_Address+1);        //�����豸��ַ+���ź�
	 for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = QMC5883_RecvByte();          //BUF[0]�洢����
        if (i == 5)
        {
           QMC5883_SendACK(1);                //���һ��������Ҫ�ط�Ӧ��NOACK
        }
        else
        {
          QMC5883_SendACK(0);                 //Ӧ��ACK
       }
   }
    QMC5883_Stop();                           //ֹͣ�ź�
    system_delay_ms(5);
}

//��ʼ��QMC5883��������Ҫ��ο�pdf�����޸�****
void Init_QMC5883()
{	uint8 a,b,c;
	QMC_gpio_init();
	while(!Single_Write_QMC5883(0x0b,0x01));  //�������ʱ��Ĵ���
	while(!Single_Write_QMC5883(0x20,0x40));
	while(!Single_Write_QMC5883(0x21,0x01));
	while(!Single_Write_QMC5883(0x09,0x0d));
	
//	Single_Write_QMC5883(0x20,0x40);  //
//	Single_Write_QMC5883(0x21,0x01);  //	
//  Single_Write_QMC5883(0x09,0x0d);  //���ƼĴ�������
}

//double HMC5883_Get_Angle(void)
//{
//    uint8 i ;
//		int X,Y,Z=0;
//    double Angle ;
//    short Recive_Data[8] ;   //store temperary data
//		Init_QMC5883();
//    for(i=0; i<6; i++)
//    {
//        Recive_Data[i] =  (i+3) ;  //get data
//    } 
//    
//    X = Recive_Data[1]<<8 | Recive_Data[0];//Combine MSB and LSB of X Data output register
//    Z = Recive_Data[3]<<8 | Recive_Data[2];//Combine MSB and LSB of Z Data output register
//    Y = Recive_Data[5]<<8 | Recive_Data[4];//Combine MSB and LSB of Y Data output register
//    
//		if(X>0x7fff)X-=0xffff;	  
//		if(Y>0x7fff)Y-=0xffff;
//		if(Z>0x7fff)Z-=0xffff;	
//    Angle= atan2((double)Y,(double)X) * (180 / 3.14159265) + 180; // angle in degrees
//    return 10*Angle ;
//}

void Conversion(uint16 temp_data)  
{  
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;  
	  qian=temp_data/1000+0x30 ;   
    temp_data=temp_data%1000; 
    bai=temp_data/100+0x30   ;   
    temp_data=temp_data%100;   
    shi=temp_data/10+0x30    ;   
    temp_data=temp_data%10; 
    ge=temp_data+0x30;
}



























