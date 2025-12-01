#include "settings.h"
#include "lcd.h"
#include "led.h"
#include "beep.h"
#include "delay.h"
#include "timer.h"

void micro_Init(MicrowaveCtrl* microwave)
{
	LCD_Fill(0, 0, 128, 160, WHITE);

	
	microwave->state = STATE_STANDBY;   
	microwave->cooking_time = 0;
	microwave->power = POWER_M;	
	microwave->isheating = 0;			
	microwave->door_state = 0;     		//0开1关

	led_on(&led1);

	LCD_ShowString(0, 20, "Microwave Demo V1.1", BLUE, WHITE, 16, 0);
	delay_ms(2000);
	LCD_Fill(0, 0, 128, 35, WHITE);

	LCD_ShowString(0, 40, "Time:    s", BLUE, WHITE, 16, 0);
	LCD_ShowIntNum(0 + 5 * 8, 40, microwave->cooking_time, 3, BLUE, WHITE, 16);
	LCD_ShowString(0, 60, "Power:0000", BLUE, WHITE, 16, 0);
	power_display(microwave);

	TIM_SetCompare3(TIM2, 0);

}


//暂停加热
void stop(MicrowaveCtrl* microwave)
{
	microwave->isheating = 0;

	TIM_SetCompare3(TIM2, 0);
	
	TIM_Cmd(TIM4, DISABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);

	LCD_Fill(0, 0, 128, 35, WHITE);
	LCD_ShowString(0, 20, "Stop", BLUE, WHITE, 16, 0);
}


//开始加热
void start(MicrowaveCtrl* microwave)
{
	u16 pwm;
	if (microwave->cooking_time > 0 && microwave->door_state == 1)
	{
		microwave->isheating = 1;

		switch (microwave->power)
		{
		case POWER_L: 		
			pwm = 50;   	
			break;
		case POWER_M: 		
			pwm = 70; 		
			break;
		case POWER_H: 		
			pwm = 100; 		
			break;
		default:      		
			pwm = 70;  		
			break;    

		}
		TIM_SetCompare3(TIM2, pwm);

		TIM_SetCompare3(TIM3, 4);

		LCD_Fill(0, 0, 128, 35, WHITE);
		LCD_ShowString(0, 20, "Heating", BLUE, WHITE, 16, 0);

		TIM_Cmd(TIM4, ENABLE);
		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	}
}



#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "led.h"
#include "sys.h"


// 状态
typedef enum {
	STATE_STANDBY,          	
	STATE_TIME_SETTING,     	
	STATE_POWER_SETTING,		
	STATE_COMPLETED,			
}MicrowaveState;

//等级
typedef enum {
	POWER_L,          			
	POWER_M,      				
	POWER_H, 				    
}PowerLevel;

//微波炉控制结构体
typedef struct {
	MicrowaveState state;     
	u16 cooking_time;		  
	PowerLevel power;          
	u8 door_state;			   
	u8 isheating;			   
}MicrowaveCtrl;

void micro_Init(MicrowaveCtrl* microwave);
void stop(MicrowaveCtrl* microwave);
void start(MicrowaveCtrl* microwave);



#endif




#include "timer.h"
#include "lcd.h"
#include "settings.h"

// 控制加热
void TIM2_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // TIM2 CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_Cmd(TIM2, ENABLE);
}

// TIM3控制电机转盘
void TIM3_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;      	//TIM3_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  		// TIM3_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_Cmd(TIM3, ENABLE);
}


void SERVO_SetAngle(u8 angle)
{
	if (angle > 180) angle = 180;

	uint16_t pulse = 500 + (angle * 2000) / 180;

	TIM_SetCompare1(TIM3, pulse);
}


// 更新中断
void TIM4_PWM_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); 
	TIM_Cmd(TIM4, ENABLE);					 
}





