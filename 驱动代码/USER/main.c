#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "myiic.h"
#include "sht85.h"

// ����ȫ�ֱ��������ڴ洢�ɼ�����ʪ������
sht85_TypeDef sht_member;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ����ϵͳ�ж����ȼ�����2
	delay_init(168);     // ��ʼ����ʱ����
	uart_init(115200);   // ��ʼ�����ڲ�����Ϊ115200
	LED_Init();          // ��ʼ��LED
    sht85_init();        // ��ʼ��sht85��ʪ�ȴ�����
    
    while(1)
    {
        // ��ȡ��ʪ�ȴ��������������ݣ�����ṹ�������
        sht85_read(&sht_member); 
        // ͨ������1�����ʪ����Ϣ
        printf("�¶ȣ�%.2f���϶�\tʪ�ȣ�%.2f%%\r\n",sht_member.fTemperature,sht_member.fHumidity);
        LED1 = !LED1;  // ��˸LED�ƽ�������״ָ̬ʾ
        delay_ms(500); // ���500ms�ɼ�һ��
    }

}
