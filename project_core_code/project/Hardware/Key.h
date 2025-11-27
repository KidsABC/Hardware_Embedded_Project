#ifndef __KEY_H
#define __KEY_H

//∫Í∂®“Â

#define						PIN1(x)						GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)(x))
#define						PIN2(x)						GPIO_WriteBit(GPIOA,GPIO_Pin_5,(BitAction)(x))
#define						PIN3(x)						GPIO_WriteBit(GPIOA,GPIO_Pin_6,(BitAction)(x))
#define						PIN4(x)						GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)(x))

#define						PIN5 							GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)
#define						PIN6	 						GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)
#define						PIN7							GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
#define						PIN8							GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)


void Key_Init(void);
uint8_t get_key(void);
void scan(void);
uint8_t get_key_num(void);

#endif
