#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "myiic.h"
#include "sht85.h"

// 定义全局变量，用于存储采集的温湿度数据
sht85_TypeDef sht_member;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置系统中断优先级分组2
	delay_init(168);     // 初始化延时函数
	uart_init(115200);   // 初始化串口波特率为115200
	LED_Init();          // 初始化LED
    sht85_init();        // 初始化sht85温湿度传感器
    
    while(1)
    {
        // 读取温湿度传感器测量的数据，传入结构体变量中
        sht85_read(&sht_member); 
        // 通过串口1输出温湿度信息
        printf("温度：%.2f摄氏度\t湿度：%.2f%%\r\n",sht_member.fTemperature,sht_member.fHumidity);
        LED1 = !LED1;  // 闪烁LED灯进行运行状态指示
        delay_ms(500); // 间隔500ms采集一次
    }

}
