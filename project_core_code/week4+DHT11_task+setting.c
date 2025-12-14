#include "settings.h"
#include "lcd.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "delay.h"
#include "timer.h"


void micro_Init(MicrowaveCtrl* microwave)
{
	
	LCD_Fill(0, 0, 128, 160, WHITE);

	microwave->state = STATE_STANDBY;   //待机
	microwave->cooking_time = 0;
	microwave->power = POWER_M;			//中
	microwave->isheating = 0;			//未加热
	microwave->door_state = 0;     		//0开1关

	led_on(&led1);

	//开机
	Show_Str(0, 20, BLUE, WHITE, "Microwave Demo V1.1", 16, 0);
	delay_ms(200);
	LCD_Fill(0, 0, 128, 35, WHITE);

	//功率、倒计时
	Show_Str(0, 40, BLUE, WHITE, "Time:    s", 16, 0);
	LCD_ShowNum(0 + 5 * 8, 40, microwave->cooking_time, 3, 16);
	Show_Str(0, 60, BLUE, WHITE, "Power:0000", 16, 0);

	power_display(microwave);

	TIM_SetCompare3(TIM2, 0);

}

//开始准备（关门关灯）
void plan_cooking(void)
{
	TIM_SetCompare2(TIM2, 8);
	led_off(&led1);
}


//结束准备（开门开灯）
void end_cooking(void)
{
	TIM_SetCompare2(TIM2, 3);
	led_on(&led1);
}



//暂停加热
void stop_cooking(MicrowaveCtrl* microwave)
{
	microwave->isheating = 0;

	TIM_SetCompare3(TIM2, 0);
	TIM_SetCompare3(TIM3, 0);

	TIM_Cmd(TIM4, DISABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);

	LCD_Fill(0, 0, 128, 35, WHITE);
	Show_Str(0, 20, BLUE, WHITE, "Heating stopped", 16, 0);
}


//开始加热
void start_cooking(MicrowaveCtrl* microwave)
{
	u16 pwm;
	if (microwave->cooking_time > 0 && microwave->door_state == 1)
	{
		microwave->isheating = 1;

		switch (microwave->power)
		{
		case POWER_L: 		pwm = 50;   	break;
		case POWER_M: 		pwm = 70; 		break;
		case POWER_H: 		pwm = 100; 		break;
		default:      		pwm = 70;  		break;    

		}
		TIM_SetCompare3(TIM2, pwm);

		TIM_SetCompare3(TIM3, 4);

		LCD_Fill(0, 0, 128, 35, WHITE);
		Show_Str(0, 20, BLUE, WHITE, "Heating", 16, 0);

		TIM_Cmd(TIM4, ENABLE);
		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	}
}

//功率显示
void power_display(MicrowaveCtrl* microwave)
{
	// 显示功率等级 
	u8* powerText;
	switch (microwave->power) {
	case POWER_L:  		 powerText = "Low";			break;
	case POWER_M:        powerText = "Medium";      break;
	case POWER_H:        powerText = "High";        break;
	default:			 powerText = "Medium";		break;
	}
	LCD_Fill(48, 60, 128, 75, WHITE);
	Show_Str(0 + 6 * 8, 60, RED, WHITE, powerText, 16, 0);
}


#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "settings.h"
#include "timer.h"
#include "beep.h"
#include "exti.h"
#include "dma.h"  
#include "dht11.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"      
#include "spi.h"


//开始任务
#define START_TASK_PRIO						1           
#define START_SIK_SIZE 						64			
TaskHandle_t StartTask_Handler;
static void start_task(void* pvParametres);

//DHT11任务
#define DHT11_TASK_PRIO						2           
#define DHT11_SIK_SIZE 					    128			
TaskHandle_t Dht11Task_Handler;
static void dht11_task(void* pvParametres);


led_d led1;                                          
led_d bep;												
static led_d p14;										
static MicrowaveCtrl microwave;							


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();
	uart_init(115200);
	LCD_Init();
	LED_Init(&led1, GPIOC, GPIO_Pin_13);
	Beep_Init(&bep, GPIOB, GPIO_Pin_15);
	TIM3_Init(99,14399);
	TIM2_Init(99,14399);
	EXTIX_Init();
	TIM4_Init(10000-1,7200-1);
	micro_Init(&microwave);

	xTaskCreate(start_task, "start tast", START_SIK_SIZE, NULL, START_TASK_PRIO, &StartTask_Handler);  
	vTaskStartScheduler();			


}

static void start_task(void* pvParametres)
{
	taskENTER_CRITICAL();     
	xTaskCreate(dht11_task, "dht11_task", DHT11_SIK_SIZE, NULL, DHT11_TASK_PRIO, &Dht11Task_Handler);
	vTaskDelete(StartTask_Handler);  
	taskEXIT_CRITICAL(); 

}

////DHT11任务
static void dht11_task(void* pvParameters)
{
	u8 temp, humi;
	while (1)
	{
		DHT_Read_Data(&temp, &humi, GPIOC, GPIO_Pin_14, &p14);
		Show_Str(0, 80, BLUE, WHITE, "Temp:00C", 16, 0);
		LCD_ShowNum(0 + 5 * 8, 80, temp, 2, 16);

		//异常处理
		if(microwave.isheating)
		{
			//高温报警
			if(temp >= 50)
			{
				stop_cooking(&microwave);
				buzzer_beep(4);
				end_cooking();
				microwave.state=STATE_STANDBY;
				microwave.door_state=0;
				LCD_Fill(0,140,128,160,WHITE);	
				Show_Str(0,140,BLUE,WHITE,"Over heat!",16,0);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}




