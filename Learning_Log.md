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
gpioinfo//会列出所有 GPIO 引脚的编号、功能状态
```

##### 查看某个引脚的当前电平:

```
gpioget gpiochip0 17//（例：GPIO17）
```

##### 输出高/低电平:

```
gpioset gpiochip0 17=1   # 输出高电平
gpioset gpiochip0 17=0   # 输出低电平

```

##### 树莓派引脚状态图：

![image-20251126164859509](D:\HARDWARE\freefar_project\Hardware_Embedded_Project\pictures\gpio图)

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

### 树莓派Thonny环境中Python语言的学习

```Python
//函数的书写
def triple_number (number) :
    return number*3

//字符串大写
def upper(str):
    return str.upper()

//字符串连接打印
def jia(a,b):
    C=a+" "+b
    print(c)

//函数的应用
a="hello"
b="world"
jia(upper(a),upper(b))

//简单打印
print("hello pi")

//布尔类型
a=True
print(a)
b = 3.14

//检查类型的函数
print(type(b))
c=triple number(b)
print(c)


//分支语句：注意缩写
temperature = 15
if temperture>20://注意写法条件后有冒号，而且不用带括号
    print("it's warm")
elif temperture >10://elif
    print("not so warm")
else://后有冒号 注意
    print("it's freezing")
print("end of program")


//条件约束中：且用and 或用or 记得和c语言区分开

//输入写法


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

