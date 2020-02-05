#include "myiic.h"
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

// ��ʼ��IIC
void IIC_Init(void)
{			
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // ʹ��GPIOBʱ��

    // GPIOB8,B9��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     // ��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      // ����
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // ��ʼ��
    IIC_SCL=1;
    IIC_SDA=1;
}

// ����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     // sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;     // START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;     // ǯסI2C���ߣ�׼�����ͻ�������� 
}

// ����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}

// �ȴ�Ӧ���źŵ���
// ����ֵ��1������Ӧ��ʧ��
//         0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN(); // SDA����Ϊ����  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0; // ʱ�����0 	   
	return 0;  
} 

// ����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2); // ��SCLΪ�͵�ƽ�ڼ䣬SDAΪ�͵�ƽ��Ϊ��ЧӦ��
	
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

// ������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2); // ��SCLΪ�͵�ƽ�ڼ䣬SDAΪ�ߵ�ƽ��Ϊ��ЧӦ��
	
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}	

// IIC����һ���ֽ�
// ���شӻ�����Ӧ��
// 1����Ӧ��
// 0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	   // ����Ϊ���ģʽ   
    IIC_SCL=0;     // ����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7; // ��������ֻ��1��0���պö�Ӧ���ߵ͵�ƽ
        txd<<=1;               // �Ƶ����λ
        delay_us(2);           // ��TEA5767��������ʱ���Ǳ����
        IIC_SCL=1;     
        delay_us(2);           // SCL�ߵ�ƽ�ڼ䣬SDA�����ȶ���������ʱ2us
        IIC_SCL=0;	           // ���͵�ƽ��Ϊ��һ�η����ݸı�򲻸ı䷢����׼��
        delay_us(2);   
    }	 
} 

// ��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	SDA_IN(); // SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
		// ����һ�������أ��Զ�������׼��
        IIC_SCL=0; 
        delay_us(2);
		    IIC_SCL=1;
		
        receive<<=1; // ���Ϊ��Ӧ��Ҫ����
		
        if(READ_SDA)receive++;  // �жϽ��ܵ�SDA�ź�Ϊ�ߣ���receive��ӦΪ��һ
		
		    delay_us(1); 
     }	
    // ע�⣺�������ݺ�Ҫ����Ӧ��	
    if (!ack)
        IIC_NAck(); // ����nACK
    else
        IIC_Ack(); // ����ACK   
    return receive;
}
