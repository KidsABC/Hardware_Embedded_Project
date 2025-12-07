# 学习笔记

## 一、树莓派相关

### 树莓派基本概念

树莓派是硬件部分和软件部分组成的整体。 

### 电源引脚

- 电源引脚:5v,3.3v,GND(Ground)。
- 常规GPIO控制引脚:可以通过编写程序控制这些引脚的高低电平
- 特殊GPIO通讯引脚:SPI通讯,12C通讯,TxD/RxD串口通讯。

### 引脚编码及终端命令行

#### 树莓派的引脚共有三种编码

- 板载编码：按照树莓派主板上引脚排针标号，能够直接看见

- BCM编码：Python控制树莓派引脚的，使用的是BCM编码

- WiringPi编码：C语言控制树莓派引脚的，使用的是WiringPi编码

#### 终端窗口查看引脚编号

##### 方式一:查看板载编码：

```
pinout
```

##### 方式二:查看全部编码：

```
gpioinfo //会列出所有 GPIO 引脚的编号、功能状态
```

##### 查看某个引脚的当前电平:

```
gpioget gpiochip0 17 //（例：GPIO17）
```

##### 输出高/低电平:

```
gpioset gpiochip0 17=1   //输出高电平
gpioset gpiochip0 17=0   //输出低电平

```

##### 树莓派引脚状态图：

![](https://github.com/KidsABC/Hardware_Embedded_Project/blob/fuyifan/pictures/3gpio.png)

> 在此图中，能用的引脚都在 gpiochip0，其他 gpiochip 4 / 6 / 12 / 13/ 14（系统内部功能/特殊外设或内部保留引脚）不要动也不用管。

代码详解

```
line 6:"GPI06"    unused     input       active-high
line 7:"GPI07"   "spio CS1"  output      active-low [used]

"GPI06"//引脚名称

unused//未被占用
"spio CS1"//被选中为spi接口

input//输入引脚
output//输出引脚

active-high//高电平有效
active-low [used]//低电平有效 且被系统占用，不能随便改
```

## 二、Linux常用命令行的应用

![](https://github.com/KidsABC/Hardware_Embedded_Project/blob/fuyifan/pictures/2.png)

### 用户身份

- 普通用户pi切换为超级用户root： sudo su 
- 超级用户root切换为普通用户pi： su pi

### 目录切换命令

- pwd                          #显示当前目录
- cd~                           #切换到主目录
- cd dir                       #切换到指定目录，dir表示文件路径
- cd..                           #切换到上一级目录

### 文件展示命令

- Is                              #展示当前目录下所有的文件和文件夹(不包含隐藏文件)

-  ls  -a                       #展示当前目录下所有文件和文件夹(包含隐藏文件)

### 创建文件命令

- touch file               #创建文件file

- mkdir dir               #创建目录dir


### 查看文件命令

- cat file                    #查看文件file内容

- more file               #查看文件file内容

- head file               #查看文件file前10行

- tail file                   #查看文件file后10行

### 删除文件命令

- rm file                  #删除文件file
- rm -r dir              #删除目录dir

### linux简单命令大全

[树莓派Linux指令](https://blog.csdn.net/lengyuefeng212/article/details/108393959?fromshare=blogdetail&sharetype=blogdetail&sharerId=108393959&sharerefer=PC&sharesource=freefarsw&sharefrom=from_link)

## 三、树莓派Thonny环境中Python语言的学习

### 简单语法

```python
#简单打印
print("hello pi")

#函数的书写
def triple_number (number) :#后面有冒号，参数直接传进来
    return number*3

#布尔类型
a=True
print(a)
b = 3.14

#检查类型的函数
print(type(b))
c=triple number(b)
print(c)
```

### 字符串和函数

```python
#功能将字符串大写 中间空格 进行连接

#字符串大写
def upper(str):
    return str.upper()

#字符串连接打印
def jia(a,b):
    C=a+" "+b
    print(c)

#函数的应用
a=input("enter a str :")
b=input("enter a str :")
jia(upper(a),upper(b))

```

### 分支语句

```python
#分支语句：注意缩写
temperature = 15
if temperture>20:#注意写法条件后有冒号，而且不用带括号
    print("it's warm")
elif temperture >10:#elif
    print("not so warm")
else:#后有冒号 注意
    print("it's freezing")
print("end of program")
```

### 条件约束

且用and  或用or  记得和c语言区分开

### 循环语句

```python
#for循环
for i in range(0,10):#从0开始遍历到9
    print("hello"+str(i))

#while循环
i=0
while i<10:
   print("hello"+str(i))#int强制转换为字符串 打印出来
   i=i+1
print("end")
```

#### 注意区分！

> [!CAUTION]
>
> > - `for i in nums:` → 直接遍历**元素**，`i`被指赋值为元素值
> > - `for i in range(len(nums))` → 遍历**索引**，需要用`nums[i]`取元素

### 输入语法

```python
str=input()#字符串
num=int(input("enter:"))#整型
num=float(input("enter:"))#浮点型
nums = [int(x) for x in input().split()]#整型数组
nums = [float(x) for x in input().split()]#浮点数数组


s = set(int(x) for x in input().split())#自动去重，适合需要无重复元素的场景
```

#### split(拆分)

##### 默认情况（不写参数）

按「任意空白字符」拆分（包括空格、多个连续空格、换行符\n、制表符\t等）。

```python
"a  b\tc\nd".split()  # 拆分成 ["a", "b", "c", "d"]
```

##### 指定分隔符（传参数）

按你写的字符拆分（比如逗号、冒号）.

```python
"1,2,3,4".split(",")  # 按逗号拆分成 ["1", "2", "3", "4"]
"name:Tom".split(":") # 按冒号拆分成 ["name", "Tom"]
```

### 数组列表

```python
#python中的列表其实是数组
nums=[9,8,5,4,1,8]
print(nums)#直接打印列表
print(nums [0] )
nums [0]=6
print(nums [0])

nums. append(7)#数组添加函数，在数组末尾加入7
print (nums)

new=[ ]#初始化列表

for i in nums:#i被赋值为数组的元素
new.append(i*3)

print (new)
```

### 寻找最大值

```python
def checkmax (nums) :
    max value=nums [0]
    for i in nums:
        if i>max value:
            max value=i
    print(max value)

nums=[int(x) for x in input("enter shuzu :").split()]#最外面是中括号
#类型（x） for x in input().split()
checkmax(nums)

```

### 库函数

```python
import time#引入时间库
print("stop 5s")
time.sleep(5.0)#时间暂停5秒
print("end")
```

## 四、python编程控制树莓派

### gpiozero

别看他在这里以库的形式出现，其实他只是树莓派专用库。

> [!NOTE]
>
> ❌ Python 本身 不包含 gpiozero
>
> ✔ gpiozero 是树莓派专用库（其他设备一般没有）

本质上来说，gpiozero是一个树莓派官方推荐的一个“控制硬件的简化库”

#### 常见的工具

| 名字           | 功能作用                    |
| :------------- | :-------------------------- |
| LED            | 控制 LED 灯（开、关、闪烁） |
| Button         | 读按键状态（按下 / 松开）   |
| Buzzer         | 控制蜂鸣器                  |
| PWMLED         | 控制亮度可调的 LED          |
| Motor          | 控直流电机                  |
| Servo          | 控 SG90/舵机                |
| DistanceSensor | HC-SR04 超声波测距          |
| MotionSensor   | PIR 人体感应                |

### LED闪烁

```python
from gpiozero import LED
#从 gpiozero 这个库里，把“LED”这个功能拿出来用。

import time#时间库函数

led = LED(17)

while True:
    led.on()      # 灯亮
    time.sleep(1)
    led.off()     # 灯灭
    time.sleep(1)
```

### Button按键

```python
from gpiozero import Button#按键库函数
import time

button = Button(26)
while True:
    print(button.is_pressed)#固定的函数名。别说还真别说，真的挺好用的
    #按键释放为False，按键按下为True
    time.sleep(1)
```

### 按键LED

按键按下灯亮，按键释放灯灭

```python
from gpiozero import Button, LED#中间用逗号分隔
from signal import pause
import time

led=LED(17)
button = Button(26)

#way1:条件和循环放一块里面
while True:
    print(button.is_pressed)
    if button.is pressed:
        led. on()
    else:
        led.off()
        time.sleep(0.01)
        
#way2:直接调用两个函数
button. when_pressed=led.on
button.when_released=led.off

pause ()
```

#### signal

signal是 Python 标准库（Python **自带**的）。和前面那个gpiozero不一样了

#### pause

pause（）让程序一直停在这里等待，不退出。

程序挂起，不结束，让事件**继续监听**。

> [!NOTE]
>
> 不然程序跑完就退出，我的按钮事件就全都没用了。

#### buttton

button.when_pressed 这个名字是固定的吗？

✔ 是**固定**的

这是 gpiozero 的 **事件回调** 属性。

Button 有固定的事件：

| 事件名              | 触发时机             |
| ------------------- | :------------------- |
| when_pressed        | 按下时触发           |
| when_released       | 松开时触发           |
| when_held           | 按住一段时间触发     |
| when_double_pressed | 连按两次触发（新版） |

#### LED

| 写法      | 作用                               |
| --------- | ---------------------------------- |
| led.on()  | 立刻执行亮灯                       |
| led.off() | 立刻执行灭灯                       |
| led.on    | **把亮灯这个功能“交出去”，不执行** |
| led.off   | 把灭灯这个功能“交出去”，不执行     |

> 不带括号：其实就是看别人什么时候打开这个开关，即<u>此时</u>**调用**函数。
>
> 带括号：不管了，走到这一句，直接执行。。。。

### 按键切换3LED

```python
from gpiozero import Button, LED
from signal import pause
import time

#初始化写在前面
#led1=LED(17)
#led2=LED(27)
#led3=LED(22)
led_list = [LED(17),LED(27),LED(22)]
button = Button(26,bounce_time=0.05)
# bounce_time:按键读取之后的0.05秒内，按键的状态，都不会再读取，其实就是按键消抖
led_index = 0#灯的索引

#led1.off()
#led2.off()
#led3.off()

def reset_leds();
    for led in led list:
        led.off()
        
reset_leds()
#用列表代替上方的代码

def switch_led():
    global led_index#全局全量关键词
    #way1:列表解决
    reset_leds()#调用切换函数时所有灯都是灭的
    led list[led_index].on() 
    led_index += 1
    if led index == len(led list):#超过长度初始化为0
        led_index=0
    #way2笨方法:
    if led index == 0:
        led1.on()
        led2.off()
        led3.off()
        led_index += 1
    elif led index == 1:
        led1.off()
        led2.on()
        led3.off()
        led_index += 1
    else:
        led1.off()
        led2.off()
        led3.on()
        led_index = 0
        
button.when_pressed =switch_led
pause()
```

