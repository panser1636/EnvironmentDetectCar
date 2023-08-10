#include "nrf24l01.h"
#include "motor.h"
const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};

// ��ʼ������
void NRF24L01_SPI_Init(void)
{
	gpio_init(NRF24L01_CE, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(NRF24L01_IRQ, GPI, GPIO_HIGH, GPI_PULL_DOWN);
	spi_init(SPI_1, SPI_MODE0, 9*1000*1000, SPI1_SCK_E12, SPI1_MOSI_E14, SPI1_MISO_E13, 0xFFFFF); // 9Mhz
	gpio_init(NRF24L01_CSN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	spi_transmit_recieve_byte(SPI_1, 0xff);
}

// ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
uint8_t SPI1_ReadWriteByte(uint8_t byte)
{
	return spi_transmit_recieve_byte(SPI_1, byte);
}

// ���24L01�Ƿ����
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
   
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ 
	
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;   //���24L01����	
	return 0;		 	//��⵽24L01
}	

// SPIд�Ĵ�����reg:ָ���Ĵ�����ַ��value:д���ֵ
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
	CS_LOW;	// ʹ��SPI����
	status = SPI1_ReadWriteByte(reg);	//���ͼĴ�����
	SPI1_ReadWriteByte(value);	//д��Ĵ�����ֵ
	CS_HIGH;	//��ֹSPI����		
	return (status);
}

// ��ȡSPI�Ĵ���ֵ��reg:ָ���Ĵ�����ַ
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;
	CS_LOW;	// ʹ��SPI����
	SPI1_ReadWriteByte(reg);	//���ͼĴ�����
	reg_val = SPI1_ReadWriteByte(0XFF);	//��ȡ�Ĵ�������
	CS_HIGH;	//��ֹSPI����	
	return (reg_val);
}

// ��ָ��λ��дָ�����ȵ�����
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,uint8_t_ctr;
	CS_LOW;	// ʹ��SPI����
	status = SPI1_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
  {
    SPI1_ReadWriteByte(*pBuf++); //д������	 
  }
	CS_HIGH;	//��ֹSPI����
	return status;
}

// ��ָ��λ�ö���ָ�����ȵ�����
uint8_t NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,uint8_t_ctr;
	CS_LOW;	// ʹ��SPI����
	status = SPI1_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(uint8_t_ctr=0;uint8_t_ctr<len;uint8_t_ctr++)
  {
    pBuf[uint8_t_ctr]=SPI1_ReadWriteByte(0XFF);//��������
  }
	CS_HIGH;	//��ֹSPI����
	return status;
}

// ����NRF24L01����һ������
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
	CE_LOW;
	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);	//д���ݵ�TX BUF  32���ֽ�
	CE_HIGH;	// ��������
	while(NRF24L01_IRQ_PIN_READ()!=0);//�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}


// ����NRF24L01����һ������
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;	
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		return 0; 
	}	  
	return 1;//û�յ��κ�����
}

// �ú�����ʼ��NRF24L01��RXģʽ
void NRF24L01_RX_Mode(void)
{
	CE_LOW;
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0F);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //����RFͨ��Ƶ��
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��
	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 
	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	CE_HIGH; //CEΪ��,�������ģʽ 
}

// �ú�����ʼ��NRF24L01��TXģʽ
void NRF24L01_TX_Mode(void)
{
	CE_LOW;
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //����RX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	CE_HIGH;	// ��������
}
