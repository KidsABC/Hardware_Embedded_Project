#include "stm32f10x.h"                  // Device header
#include "key.h"
/**
  * 函    数：定时中断初始化
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数配置为外部时钟，定时器相当于计数器
  */
void Timer_Init(uint16_t arr,uint16_t psc)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);			//开启TIM2的时钟
	
	
	//TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x0F);
																//选择外部时钟模式2，时钟从TIM_ETR引脚输入
																//注意TIM2的ETR引脚固定为PA0，无法随意更改
																//最后一个滤波器参数加到最大0x0F，可滤除时钟信号抖动
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period =arr;					//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler =psc;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM1的时基单元	
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	TIM_InternalClockConfig(TIM1);	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);					//开启TIM1的更新中断
	
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;				//选择配置NVIC的TIM2线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*TIM使能*/
	TIM_Cmd(TIM1, ENABLE);			//使能TIM1，定时器开始运行
}

/**
  * 函    数：返回定时器CNT的值
  * 参    数：无
  * 返 回 值：定时器CNT的值，范围：0~65535
  */
//uint16_t Timer_GetCounter(void)
//{
//	return TIM_GetCounter(TIM2);	//返回定时器TIM2的CNT
//}

/* 定时器中断函数，可以复制到使用它的地方
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
*/
void TIM1_UP_IRQHander(void)//相当于是另一个循环
{
	if(TIM_GetFlagStatus(TIM1,TIM_IT_Update)==SET)
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		scan();
		
	}
}
