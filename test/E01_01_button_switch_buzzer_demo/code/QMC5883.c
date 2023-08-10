#include "zf_common_headfile.h"
#include "math.h"
extern uint8 BUF[8]; 	//数据缓存
extern uint8 ge,shi,bai,qian,wan;
#define SDA  B14 
#define SCL		B12
#define	Slave_Address   0x1a

void QMC_gpio_init(){
gpio_init(SDA,GPO,1,GPO_PUSH_PULL);
gpio_init(SCL,GPO,1,GPO_PUSH_PULL);
}


/**************************************
起始信号
**************************************/
void QMC5883_Start()
{		
		gpio_set_dir(SDA,GPO,GPO_PUSH_PULL);
    gpio_set_level(SDA,1);                  //拉高数据线
    gpio_set_level(SCL,1);
    system_delay_us(3);                 //延时
    gpio_set_level(SDA,0);                    //产生下降沿
    system_delay_us(3);                 //延时
    gpio_set_level(SCL,0);                    //拉低时钟线
}

/**************************************
停止信号
**************************************/
void QMC5883_Stop()
{		
		gpio_set_dir(SDA,GPO,GPO_PUSH_PULL);
    gpio_set_level(SCL,0);
    gpio_set_level(SDA,0);	//拉低数据线
		system_delay_us(3);		
		gpio_set_level(SCL,1);                   //拉高时钟线
    system_delay_us(3);                 //延时
    gpio_set_level(SDA,1);                    //产生上升沿
    system_delay_us(3);                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void QMC5883_SendACK(uint8 ack)  //0 send answer and 1 notsend
{		
	  gpio_set_level(SCL,0);
		gpio_set_dir(SDA,GPO,GPO_PUSH_PULL);
    gpio_set_level(SDA,ack);       //写应答信号
		system_delay_us(3); 
    gpio_set_level(SCL,1);                    //拉高时钟线
    system_delay_us(3);                 //延时
    gpio_set_level(SCL,0);                    //拉低时钟线
    //system_delay_us(5);                 //延时
}

/**************************************
接收应答信号
**************************************/
uint8 QMC5883_RecvACK()
{		uint8 ucErrTime=0;
		gpio_set_dir(SDA,GPI,GPI_FLOATING_IN);
		gpio_set_level(SDA,1);
		system_delay_us(3);
    gpio_set_level(SCL,1);       //拉高时钟线
    system_delay_us(3);                //延时
    while(gpio_get_level(SDA)==1)
		{  ucErrTime++;
			if(ucErrTime>250){
				QMC5883_Stop();
				return 1;
			}
		}
		                   //进位标志 读应答信号
    gpio_set_level(SCL,0);       //拉低时钟线
		//system_delay_us(5);     //延时
    return 0;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void QMC5883_SendByte(uint8 dat)
{
    uint8 i;
		gpio_set_dir(SDA,GPO,GPO_PUSH_PULL);
		gpio_set_level(SCL,0);
    for (i=0; i<8; i++)         //8位计数器
    {		
//			if((dat&0x80)>>7)
//			gpio_set_level(SDA,1);
//			else
//			gpio_set_level(SDA,0);
      gpio_set_level(SDA,(dat&0x80)>>7);
			dat<<= 1;              //移出数据的最高位
			system_delay_us(3);	
			gpio_set_level(SCL,1);           //拉高时钟线
      system_delay_us(3);             //延时
      gpio_set_level(SCL,0);          //拉低时钟线
      system_delay_us(3);             //延时
    }
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
uint8 QMC5883_RecvByte()//uint8 ack
{
    uint8 i;
    uint8 rece_data = 0;
		gpio_set_dir(SDA,GPI,GPI_FLOATING_IN);
    for (i=0; i<8; i++)         //8位计数器
    {		
				gpio_set_level(SCL,0);
				system_delay_us(3);
				gpio_set_level(SCL,1);
				rece_data<<=1;                          //拉高时钟线
       if(gpio_get_level(SDA)==1)
					rece_data++;   
					system_delay_us(3); 
	}					 
		//QMC5883_SendACK(ack);
		return rece_data;
}

//************************写入单字节数据***************************

uint8 Single_Write_QMC5883(uint8 REG_Address,uint8 REG_data)
{		uint8 ret=0;
    QMC5883_Start();                  //起始信号
    QMC5883_SendByte(Slave_Address);   //发送设备地址+写信号
    ret&= QMC5883_RecvACK();
	  QMC5883_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf 
    ret&= QMC5883_RecvACK();
		QMC5883_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf
		ret&= QMC5883_RecvACK();
	  QMC5883_Stop();
                  //发送停止信号
		return ret;
}

//************************读取单字节数据*************************
uint8 Single_Read_QMC5883(uint8 REG_Address)
{   
	uint8 REG_data=0;
	QMC5883_Start();                          //起始信号
	QMC5883_SendByte(Slave_Address);           //发送设备地址+写信号
	while(QMC5883_RecvACK()==0);
	QMC5883_SendByte(REG_Address);            //发送存储单元地址，从0开始	
	while(QMC5883_RecvACK()==0);
	QMC5883_Start();                          //起始信号
	QMC5883_SendByte(Slave_Address+1);         //发送设备地址+读信号
	while(QMC5883_RecvACK()==0);
	REG_data=QMC5883_RecvByte(); //0             //读出寄存器数据
	QMC5883_SendACK(1);   
	QMC5883_Stop();	//停止信号
  return REG_data; 
}

//******************************************************
//连续读出QMC5883内部角度数据，地址范围0x00~0x05
//******************************************************
void Multiple_Read_QMC5883(void)
{   
    uint8 i;
    QMC5883_Start();                          //起始信号
    QMC5883_SendByte(Slave_Address);          //发送设备地址+写信号
    QMC5883_SendByte(0x00);                   //发送存储单元地址，从0x00开始	
    QMC5883_Start();                          //起始信号
    QMC5883_SendByte(Slave_Address+1);        //发送设备地址+读信号
	 for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = QMC5883_RecvByte();          //BUF[0]存储数据
        if (i == 5)
        {
           QMC5883_SendACK(1);                //最后一个数据需要回非应答NOACK
        }
        else
        {
          QMC5883_SendACK(0);                 //应答ACK
       }
   }
    QMC5883_Stop();                           //停止信号
    system_delay_ms(5);
}

//初始化QMC5883，根据需要请参考pdf进行修改****
void Init_QMC5883()
{	uint8 a,b,c;
	QMC_gpio_init();
	while(!Single_Write_QMC5883(0x0b,0x01));  //设置清除时间寄存器
	while(!Single_Write_QMC5883(0x20,0x40));
	while(!Single_Write_QMC5883(0x21,0x01));
	while(!Single_Write_QMC5883(0x09,0x0d));
	
//	Single_Write_QMC5883(0x20,0x40);  //
//	Single_Write_QMC5883(0x21,0x01);  //	
//  Single_Write_QMC5883(0x09,0x0d);  //控制寄存器配置
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



























