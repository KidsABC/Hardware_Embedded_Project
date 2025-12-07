# 第二周周报：2025.12.1-2025.12.7

## 一、本周项目完成内容和新学知识

### 1.目前的进度(包括实现的功能和环境搭建等)

（1）学习了部分ONENET

​		ONENET平台支持多种协议接入：MQTT,HTTP,CoAP（主要使用MQTT）

​		MQPP参数包含：clientId，username，password

（2）物模型（设备的功能描述）数据交互

#ifndef _ONENET_MQTT_H_
#define _ONENET_MQTT_H_
#include "esp_err.h"

//产品ID
#define ONENET_PROJECT_ID              "pTR3Bu6JBz"

//产品密钥
#define ONENET_PROJECT_ACCESS_KEY      "ib1lAZ99lyWXBQuR7W2nWUPjmSNH/7cRX+sJWb+le8c="

//设备名称
#define ONENET_DEVICE_NAME             "esp32led01"

esp_err_t onenet_start(void);                   //启动MQTT连接至ONENET

esp_err_t onenet_post_property_data(const char* data);

#endif

\#ifndef _ONENET_DM_H_

\#define _ONENET_DM_H_

\#include "cJSON.h"

void onenet_dm_init(void);        //初始化物模型数据

void onenet_porperty_handle(cJSON* property);    //处理下行数据

cJSON* onenet_porperty_upload(void);        //生成上报的JSON数据

\#endif 

### 2.遇到的问题以及解决方法



描述：1.AP连接失败

解决：

1.正在解决，可能的情况：Wi-Fi信号，网络凭证，设备兼容性，重启设备，设备激活失败等



## 二、下周安排

掌握ONENET

### 三、项目源码更新说明