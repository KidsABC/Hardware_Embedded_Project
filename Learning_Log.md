# 学习笔记

## 一、树莓派相关

- 树莓派是硬件部分和软件部分组成的整体。 

## 二、Linux常用命令行的应用

![](https://github.com/KidsABC/Hardware_Embedded_Project/blob/fuyifan/pictures/2.png)

### 用户身份

- 普通用户pi切换为超级用户root：sudo su 
- 超级用户root切换为普通用户pi：su pi

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

https://blog.csdn.net/lengyuefeng212/article/details/108393959?fromshare=blogdetail&sharetype=blogdetail&sharerId=108393959&sharerefer=PC&sharesource=freefarsw&sharefrom=from_link