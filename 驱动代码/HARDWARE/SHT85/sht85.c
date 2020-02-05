#include "sht85.h"
#include "myiic.h"
#include "delay.h"

/* 静态函数区域 */

// 计算温度值
static float CalcTemperature(uint16_t rawValue)
{
  // calculate temperature [℃]
  // T = -45 + 175 * rawValue / (2^16-1)
  return 175.0f * (float)rawValue / 65535.0f - 45.0f;
}

// 计算湿度值
static float CalcHumidity(uint16_t rawValue)
{
  // calculate relative humidity [%RH]
  // RH = rawValue / (2^16-1) * 100
  return 100.0f * (float)rawValue / 65535.0f;
}


/* 外部API函数区域 */

// 初始化函数
void sht85_init()
{
    IIC_Init();
}

// 单周期模式获取温湿度
void sht85_read(sht85_TypeDef *sht_member)
{
    IIC_Start();
    IIC_Send_Byte(SLAVE_ADDR<<1 | write); // 写从机设备地址
    IIC_Ack();
    IIC_Send_Byte(0x24); // MSB
    IIC_Ack();
    IIC_Send_Byte(0X00); // LSB
    IIC_Ack();
    IIC_Stop();
    
    // 读取数据
    IIC_Start();
    delay_ms(50);
    IIC_Send_Byte(SLAVE_ADDR<<1 | read);
    IIC_Ack();
    // 获取温度
    sht_member->temperature[0] = IIC_Read_Byte(1); // MSB
    sht_member->temperature[1] = IIC_Read_Byte(1); // LSB
    sht_member->crc_value[0] = IIC_Read_Byte(1); // CRC值（暂时不做校验比对）
    // 获取湿度
    sht_member->humidity[0] = IIC_Read_Byte(1);
    sht_member->humidity[1] = IIC_Read_Byte(1);
    sht_member->crc_value[1] = IIC_Read_Byte(0);
    IIC_Stop();
    // 转化为实际温度和湿度数据
    sht_member->fTemperature = CalcTemperature(sht_member->temperature[0]<<8 | sht_member->temperature[1]);
    sht_member->fHumidity = CalcHumidity(sht_member->humidity[0]<<8 | sht_member->humidity[1]);
}
