#include "stm32f10x.h"                  // Device header
#include "key.h"
#include "Buzzer.h"
/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);			
	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13|GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);			
}


uint8_t K1,K2,K3,K4;

uint8_t get_key(void)
{
	
	uint8_t key_dow=0;
	uint8_t key_value;
	PIN1(1);PIN2(1);PIN3(1);PIN4(1);
	
	if(PIN5==1||PIN6==1||PIN7==1|| PIN8==1)
		{	
		if(PIN5==1||PIN6==1||PIN7==1|| PIN8==1)
			{
				if(key_dow==0){
					
					key_dow = 1;
					Buzzer_ON();
					PIN1(0);PIN2(0);PIN3(0);PIN4(1);
					K1=PIN5;K2=PIN6;K3=PIN7;K4=PIN8;
					if(K1==1&&K2==0&&K3==0&&K4==0) key_value=1;
					if(K1==0&&K2==1&&K3==0&&K4==0) key_value=2;
					if(K1==0&&K2==0&&K3==1&&K4==0) key_value=3;
					if(K1==0&&K2==0&&K3==0&&K4==1) key_value='A';
			
					PIN1(0);PIN2(0);PIN3(1);PIN4(0);
					K1=PIN5;K2=PIN6;K3=PIN7;K4=PIN8;
					if(K1==1&&K2==0&&K3==0&&K4==0) key_value=4;
					if(K1==0&&K2==1&&K3==0&&K4==0) key_value=5;
					if(K1==0&&K2==0&&K3==1&&K4==0) key_value=6;
					if(K1==0&&K2==0&&K3==0&&K4==1) key_value='B';
			
					PIN1(0);PIN2(1);PIN3(0);PIN4(0);
					K1=PIN5;K2=PIN6;K3=PIN7;K4=PIN8;
					if(K1==1&&K2==0&&K3==0&&K4==0) key_value=7;
					if(K1==0&&K2==1&&K3==0&&K4==0) key_value=8;
					if(K1==0&&K2==0&&K3==1&&K4==0) key_value=9;
					if(K1==0&&K2==0&&K3==0&&K4==1) key_value='_';
			
				PIN1(1);PIN2(0);PIN3(0);PIN4(0);
					K1=PIN5;K2=PIN6;K3=PIN7;K4=PIN8;
					if(K1==1&&K2==0&&K3==0&&K4==0) key_value='*';
					if(K1==0&&K2==1&&K3==0&&K4==0) key_value=0;
					if(K1==0&&K2==0&&K3==1&&K4==0) key_value='#';
					if(K1==0&&K2==0&&K3==0&&K4==1) key_value='D';
				}
			}
			
		}
	else
	{
		Buzzer_OFF();
		key_value=0xFF;
		key_dow=1;
		}
	return key_value;
}


//按键扫描加消抖
uint8_t key_num;

uint8_t get_key_num(void)
{	
	uint8_t temp;
	if(key_num!=0xFF)
{	
	temp=key_num;
	key_num=0;
	return temp;
	
}
	return 0xFF;
}

void scan(void)
{
	static uint8_t Count;
	static uint8_t Catate,Pstate;
	Count++;
	if(Count>=20)
	{
		Count=0;
		Pstate=Catate;
		Catate=get_key();
		if(Catate==0xFF && Pstate!=0xFF)
		{	
			key_num=Pstate;
			
		}
		
	}
}

