#ifndef __SHT85_H
#define __SHT85_H

#include "sys.h"
#include "stdlib.h"	  

#define SLAVE_ADDR  0x44

// ���崫������������
typedef struct __sht85__
{
    unsigned char    temperature[2]; // [0]��MSB,[1]��LSB
    unsigned char    humidity[2];
    unsigned char    crc_value[2];
    float fTemperature; // True value
    float fHumidity;
} sht85_TypeDef;

// �����дģʽ����
typedef enum
{
    write = 0,
    read
} mode_TypeDef;


// API��������
void sht85_init(void);
void sht85_read(sht85_TypeDef *sht_member);

#endif /* __SHT85_H */
