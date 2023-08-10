#ifndef __QMC5883_H
#define __QMC5883_H
#include "zf_common_headfile.h"


void QMC_gpio_init();
void QMC5883_Start();
void QMC5883_Stop();
void QMC5883_SendACK(uint8 ack);
uint8 QMC5883_RecvACK();
void QMC5883_SendByte(uint8 dat);
uint8 QMC5883_RecvByte(uint8 ack);
uint8 Single_Write_QMC5883(uint8 REG_Address,uint8 REG_data);
uint8 Single_Read_QMC5883(uint8 REG_Address);
void Init_QMC5883();
float HMC5883_Get_Angle(void);
void Multiple_Read_QMC5883(void);


#endif