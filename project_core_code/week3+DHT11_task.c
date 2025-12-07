#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "settings.h"
#include "timer.h"
#include "beep.h" 
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
void start_task(void *pvParametres);             

//DHT11任务
#define DHT11_TASK_PRIO						2           
#define DHT11_SIK_SIZE 					    128			
TaskHandle_t Dht11Task_Handler;							
void dht11_task(void *pvParametres);           


int main(void)
{	 
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();
	uart_init(115200);
	LED_Init(&led1,GPIOC,GPIO_Pin_13);
	Beep_Init(&bep,GPIOB,GPIO_Pin_15); 
	LCD_Init();
	micro_Init(&microwave);
	
	xTaskCreate(start_task,"start tast",START_SIK_SIZE,NULL,START_TASK_PRIO,&StartTask_Handler);  
	vTaskStartScheduler();												
	while(1)
	{
		if(xTaskCreate(dht11_task,"dht11_task",DHT11_SIK_SIZE,NULL,DHT11_TASK_PRIO,&Dht11Task_Handler) != pdPASS)
		{
			printf("Task create failed!\r\n");
		}
	}
}

//开始任务
void start_task(void *pvParametres)
{ 
	taskENTER_CRITICAL();     

	xTaskCreate((TaskFunction_t	) dht11_task,
　　　　　　　　(const char* 	) "dht11_task",
　　　　　　　　(uint32_t 		) DHT11_SIK_SIZE,
　　　　　　　　(void* 		  	) NULL,
　　　　　　　　(UBaseType_t 	) NULL,
　　　　　　　　(StackType_t*   ) DHT11_TASK_PRIO,
　　　　　　　　(StaticTask_t*  ) &Dht11Task_Handler);

	vTaskDelete(StartTask_Handler); //删除开始任务         
	taskEXIT_CRITICAL(); //退出临界区       

}



//DHT11
void dht11_task(void *pvParameters)
{
		u8 tem,hum;
		while(1)
		{
			DHT_Read_Data(&tem,&hum,GPIOC,GPIO_Pin_14,&p14);
			Show_Str(0,80,BLUE,WHITE,"Temp:",16,0);
			LCD_ShowNum(0+5*8,80,temp,2,16);
		}
}
