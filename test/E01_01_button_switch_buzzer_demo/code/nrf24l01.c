#include "nrf24l01.h"
#include "motor.h"
const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};

// 初始化函数
void NRF24L01_SPI_Init(void)
{
	gpio_init(NRF24L01_CE, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(NRF24L01_IRQ, GPI, GPIO_HIGH, GPI_PULL_DOWN);
	spi_init(SPI_1, SPI_MODE0, 9*1000*1000, SPI1_SCK_E12, SPI1_MOSI_E14, SPI1_MISO_E13, 0xFFFFF); // 9Mhz
	gpio_init(NRF24L01_CSN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	spi_transmit_recieve_byte(SPI_1, 0xff);
}

// 往串行Flash读取写入一个字节数据并接收一个字节数据
uint8_t SPI1_ReadWriteByte(uint8_t byte)
{
	return spi_transmit_recieve_byte(SPI_1, byte);
}

// 检测24L01是否存在
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
   
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址 
	
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;   //检测24L01错误	
	return 0;		 	//检测到24L01
}	

// SPI写寄存器，reg:指定寄存器地址，value:写入的值
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
	CS_LOW;	// 使能SPI传输
	status = SPI1_ReadWriteByte(reg);	//发送寄存器号
	SPI1_ReadWriteByte(value);	//写入寄存器的值
	CS_HIGH;	//禁止SPI传输		
	return (status);
}

// 读取SPI寄存器值，reg:指定寄存器地址
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;
	CS_LOW;	// 使能SPI传输
	SPI1_ReadWriteByte(reg);	//发送寄存器号
	reg_val = SPI1_ReadWriteByte(0XFF);	//读取寄存器内容
	CS_HIGH;	//禁止SPI传输	
	return (reg_val);
}

// 在指定位置写指定长度的数据
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,uint8_t_ctr;
	CS_LOW;	// 使能SPI传输
	status = SPI1_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
	for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
  {
    SPI1_ReadWriteByte(*pBuf++); //写入数据	 
  }
	CS_HIGH;	//禁止SPI传输
	return status;
}

// 在指定位置读出指定长度的数据
uint8_t NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,uint8_t_ctr;
	CS_LOW;	// 使能SPI传输
	status = SPI1_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
	for(uint8_t_ctr=0;uint8_t_ctr<len;uint8_t_ctr++)
  {
    pBuf[uint8_t_ctr]=SPI1_ReadWriteByte(0XFF);//读出数据
  }
	CS_HIGH;	//禁止SPI传输
	return status;
}

// 启动NRF24L01发送一次数据
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
	CE_LOW;
	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);	//写数据到TX BUF  32个字节
	CE_HIGH;	// 启动发送
	while(NRF24L01_IRQ_PIN_READ()!=0);//等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}


// 启动NRF24L01接收一次数据
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;	
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		return 0; 
	}	  
	return 1;//没收到任何数据
}

// 该函数初始化NRF24L01到RX模式
void NRF24L01_RX_Mode(void)
{
	CE_LOW;
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0F);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //设置RF通信频率
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启
	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 
	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	CE_HIGH; //CE为高,进入接收模式 
}

// 该函数初始化NRF24L01到TX模式
void NRF24L01_TX_Mode(void)
{
	CE_LOW;
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //设置RX节点地址,主要为了使能ACK	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	CE_HIGH;	// 启动发送
}
