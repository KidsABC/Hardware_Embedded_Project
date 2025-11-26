#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "stm32f10x.h"


typedef struct {
	GPIO_TypeDef* port;
	u16 Pin;
}led_d;

void LED_Init(led_d* led, GPIO_TypeDef* port, u16 Pin);
void led_on(led_d* led);
void led_off(led_d* led);

#endif




#include "led.h"

void LED_Init(led_d* led, GPIO_TypeDef* port, u16 Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if (port == GPIOA)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	}
	else if (port == GPIOB)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	}
	else if (port == GPIOC)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	}

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(port, &GPIO_InitStructure);

	led->port = port;
	led->Pin = Pin;
	GPIO_ResetBits(port, Pin);
}

void led_on(led_d* led)
{
	GPIO_ResetBits(led->port, led->Pin);
}

void led_off(led_d* led)
{
	GPIO_SetBits(led->port, led->Pin);
}

#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

#define KEY1   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)
#define KEY2   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)

#define KEY1_PRESS  1
#define KEY2_PRESS  2

void KEY_Init(void);
uint8_t KEY_Scan(uint8_t mode);

#endif




#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"



//----------------------------------------------
//  定义按键引脚
//----------------------------------------------
void KEY_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      // 上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	// KEY1 = PB12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// KEY2 = PB1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// 读取按键（非中断逻辑）
// 返回 1 = 未按下，0 = 按下
uint8_t KEY_Scan(uint8_t mode)
{
	static uint8_t key_up = 1;

	if (mode) key_up = 1;

	if (key_up && (KEY1 == 0 || KEY2 == 0))
	{
		delay_ms(10);
		key_up = 0;

		if (KEY1 == 0) return KEY1_PRESS;
		if (KEY2 == 0) return KEY2_PRESS;
	}
	else if (KEY1 == 1 && KEY2 == 1)
	{
		key_up = 1;
	}
	return 0;
}

#ifndef __DHT11_H
#define __DHT11_H

#include "sys.h"
#include "delay.h"
#include "led.h"

u8 DHT_Read_Data(u8* temp, u8* humi, GPIO_TypeDef* port, uint16_t pin, led_d* dht);

#endif


#include "dht11.h"
#include "delay.h"
#include "sys.h"

/*
 * DHT11 数据引脚用 led_d 结构表示，保持你项目的统一接口方式
 * 需要在 main.c 中：  led_d p14;
 */




 /* 设置 GPIO 输出模式 */
static void DHT11_IO_OUT(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

/* 设置 GPIO 输入模式 */
static void DHT11_IO_IN(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

/* 拉低 DHT11（开始信号） */
static void DHT11_Start(GPIO_TypeDef* port, uint16_t pin)
{
    DHT11_IO_OUT(port, pin);
    GPIO_ResetBits(port, pin);     // 拉低 >18ms
    delay_ms(20);
    GPIO_SetBits(port, pin);       // 拉高 20-40us
    delay_us(30);

    DHT11_IO_IN(port, pin);        // 主机释放总线
}

/* 等待 DHT11 响应 */
static u8 DHT11_Check(GPIO_TypeDef* port, uint16_t pin)
{
    u8 retry = 0;
    while (GPIO_ReadInputDataBit(port, pin) && retry < 100)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 100) return 1;  // 无响应

    retry = 0;
    while (!GPIO_ReadInputDataBit(port, pin) && retry < 100)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 100) return 1;

    return 0;  // 响应成功
}

/* 读取 1 字节 */
static u8 DHT11_Read_Byte(GPIO_TypeDef* port, uint16_t pin)
{
    u8 i, byte = 0;

    for (i = 0; i < 8; i++)
    {
        while (GPIO_ReadInputDataBit(port, pin)); // 等待拉低
        while (!GPIO_ReadInputDataBit(port, pin)); // 等待拉高
        delay_us(40);

        if (GPIO_ReadInputDataBit(port, pin))   // >40us 为 1
            byte |= (1 << (7 - i));

        while (GPIO_ReadInputDataBit(port, pin)); // 等待结束
    }
    return byte;
}

/*
 * 读取温湿度主函数
 * temp：温度
 * humi：湿度
 * dht 引脚通过 GPIO + led_d 结构传入：DHT_Read_Data(&temp, &humi, GPIOC, GPIO_Pin_14, &p14)
 */
u8 DHT_Read_Data(u8* temp, u8* humi, GPIO_TypeDef* port, uint16_t pin, led_d* dht)
{
    u8 buf[5];
    u8 i;

    DHT11_Start(port, pin);
    if (DHT11_Check(port, pin))
        return 1;  // 错误

    for (i = 0; i < 5; i++)
        buf[i] = DHT11_Read_Byte(port, pin);

    if ((buf[0] + buf[1] + buf[2] + buf[3]) != buf[4])
        return 2;  // 校验错误

    *humi = buf[0];
    *temp = buf[2];

    return 0; // 成功
}

#ifndef __BEEP_H
#define __BEEP_H

#include "sys.h"
#include "stm32f10x.h"
#include "led.h"    


void Beep_Init(led_d* bep, GPIO_TypeDef* port, u16 Pin);

void beep_on(led_d* bep);
void beep_off(led_d* bep);

void buzzer_beep(u8 times);
void buzzer_alarm_long(u16 duration_ms);
void buzzer_beep_short(void);
void buzzer_error_alarm(void);
void buzzer_complete_beep(void);

#endif




#include "beep.h"
#include "delay.h"   


void Beep_Init(led_d* bep, GPIO_TypeDef* port, u16 Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (port == GPIOA)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (port == GPIOB)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (port == GPIOC)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStructure);

    bep->port = port;
    bep->Pin = Pin;

    GPIO_SetBits(port, Pin);   //默认关闭
}


void beep_on(led_d* bep)
{
    GPIO_ResetBits(bep->port, bep->Pin);
}


void beep_off(led_d* bep)
{
    GPIO_SetBits(bep->port, bep->Pin);
}


void buzzer_beep(u8 times)
{
    extern led_d bep;  //从 main.c 引用beep对象

    for (uint8_t i = 0; i < times; i++)
    {
        beep_on(&bep);
        delay_ms(100);
        beep_off(&bep);
        delay_ms(80);
    }
}

//蜂鸣器长鸣报警
void buzzer_alarm_long(u16 duration_ms)
{
    extern led_d bep;  //从 main.c 引用beep对象
    beep_on(&bep);
    delay_ms(duration_ms);
    beep_off(&bep);
}

//蜂鸣器短促提示音
void buzzer_beep_short(void)
{
    extern led_d bep;  //从 main.c 引用beep对象
    beep_on(&bep);
    delay_ms(50);
    beep_off(&bep);
}

//蜂鸣器错误报警音
void buzzer_error_alarm(void)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        extern led_d bep;  //从 main.c 引用beep对象
        beep_on(&bep);
        delay_ms(100);
        beep_off(&bep);
        delay_ms(100);
    }
}

//蜂鸣器完成提示音
void buzzer_complete_beep(void)
{
    extern led_d bep;  //从 main.c 引用beep对象
    beep_on(&bep);
    delay_ms(500);
    beep_off(&bep);
}
