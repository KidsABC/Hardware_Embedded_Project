# 第一周周报：2025.11.24-2025.12.7

## 一、本周项目完成内容和新学知识

### 1.目前的进度(包括实现的功能和环境搭建等)

学会了无线连接，有线连接，解决了上周遗留的问题。学会了VN从连接，配置静态IP地址，树莓派文件传输，桌面编译环境，Python环境搭建，c++环境部署。学习了Linux系统操作命令和编译器的使用。学会了串口通信点亮LED灯。

串口通信点亮LED灯：

```c
#include <wiringPi.h>

#define Pin 25

int main()
{
    if(wiringPiSetup() < 0)
        return 1;
    pinMode(Pin,OUTPUT);
	for(int i=0;i<10;i+ +)
    {
		digitalWVrite(Pin,1);
		delay (200);
		digitalwrite(Pin,0);
        delay (200);
    }		
    return 0;
}
```



### 2.遇到的问题以及解决方法



### 3.新学习到的知识点

1、串口通信点亮LED灯

2、Linux系统操作命令和编译器的使用

## 二、下周安排

继续树莓派4B的课程学习