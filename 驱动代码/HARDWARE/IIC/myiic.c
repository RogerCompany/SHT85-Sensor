#include "myiic.h"
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

// 初始化IIC
void IIC_Init(void)
{			
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // 使能GPIOB时钟

    // GPIOB8,B9初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      // 上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // 初始化
    IIC_SCL=1;
    IIC_SDA=1;
}

// 产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     // sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;     // START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;     // 钳住I2C总线，准备发送或接收数据 
}

// 产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}

// 等待应答信号到来
// 返回值：1，接收应答失败
//         0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN(); // SDA设置为输入  
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
	IIC_SCL=0; // 时钟输出0 	   
	return 0;  
} 

// 产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2); // 在SCL为低电平期间，SDA为低电平，为有效应答
	
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

// 不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2); // 在SCL为低电平期间，SDA为高电平，为无效应答
	
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}	

// IIC发送一个字节
// 返回从机有无应答
// 1，有应答
// 0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	   // 设置为输出模式   
    IIC_SCL=0;     // 拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7; // 二进制数只有1和0，刚好对应拉高低电平
        txd<<=1;               // 移掉最高位
        delay_us(2);           // 对TEA5767这三个延时都是必须的
        IIC_SCL=1;     
        delay_us(2);           // SCL高电平期间，SDA必须稳定，所以延时2us
        IIC_SCL=0;	           // 拉低电平，为下一次发数据改变或不改变发送做准备
        delay_us(2);   
    }	 
} 

// 读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	SDA_IN(); // SDA设置为输入
    for(i=0;i<8;i++ )
	{
		// 产生一个上升沿，以读数据做准备
        IIC_SCL=0; 
        delay_us(2);
		    IIC_SCL=1;
		
        receive<<=1; // 最低为相应需要读的
		
        if(READ_SDA)receive++;  // 判断接受到SDA信号为高，则receive相应为加一
		
		    delay_us(1); 
     }	
    // 注意：读完数据后要进行应答	
    if (!ack)
        IIC_NAck(); // 发送nACK
    else
        IIC_Ack(); // 发送ACK   
    return receive;
}
