#include "sht85.h"
#include "myiic.h"
#include "delay.h"

/* ��̬�������� */

// �����¶�ֵ
static float CalcTemperature(uint16_t rawValue)
{
  // calculate temperature [��]
  // T = -45 + 175 * rawValue / (2^16-1)
  return 175.0f * (float)rawValue / 65535.0f - 45.0f;
}

// ����ʪ��ֵ
static float CalcHumidity(uint16_t rawValue)
{
  // calculate relative humidity [%RH]
  // RH = rawValue / (2^16-1) * 100
  return 100.0f * (float)rawValue / 65535.0f;
}


/* �ⲿAPI�������� */

// ��ʼ������
void sht85_init()
{
    IIC_Init();
}

// ������ģʽ��ȡ��ʪ��
void sht85_read(sht85_TypeDef *sht_member)
{
    IIC_Start();
    IIC_Send_Byte(SLAVE_ADDR<<1 | write); // д�ӻ��豸��ַ
    IIC_Ack();
    IIC_Send_Byte(0x24); // MSB
    IIC_Ack();
    IIC_Send_Byte(0X00); // LSB
    IIC_Ack();
    IIC_Stop();
    
    // ��ȡ����
    IIC_Start();
    delay_ms(50);
    IIC_Send_Byte(SLAVE_ADDR<<1 | read);
    IIC_Ack();
    // ��ȡ�¶�
    sht_member->temperature[0] = IIC_Read_Byte(1); // MSB
    sht_member->temperature[1] = IIC_Read_Byte(1); // LSB
    sht_member->crc_value[0] = IIC_Read_Byte(1); // CRCֵ����ʱ����У��ȶԣ�
    // ��ȡʪ��
    sht_member->humidity[0] = IIC_Read_Byte(1);
    sht_member->humidity[1] = IIC_Read_Byte(1);
    sht_member->crc_value[1] = IIC_Read_Byte(0);
    IIC_Stop();
    // ת��Ϊʵ���¶Ⱥ�ʪ������
    sht_member->fTemperature = CalcTemperature(sht_member->temperature[0]<<8 | sht_member->temperature[1]);
    sht_member->fHumidity = CalcHumidity(sht_member->humidity[0]<<8 | sht_member->humidity[1]);
}
