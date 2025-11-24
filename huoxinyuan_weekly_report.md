# 第一周周报：2025.11.17-2025.11.23

## 一、本周项目完成内容和新学知识

### 1.目前的进度(包括实现的功能和环境搭建等)

（1）进行了ESP32S3的环境搭建（ESP-IDF）

（2）了解了ESP-IDF的部分基本操作

（3）实现了ESP32S3的LED点亮和按键逻辑

LED点亮：

void led_breath_init(void)

{

  //初始化定时器

  ledc_timer_config_t ledc_timer = 

  {

​    .clk_cfg = LEDC_AUTO_CLK,  //时钟源

​    .speed_mode = LEDC_LOW_SPEED_MODE,  //速度模式

​    .timer_num = LEDC_TIMER_0,    //定时器编号

​    .duty_resolution = LEDC_TIMER_12_BIT,  //占空比分辨率

​    .freq_hz = 5000,   //PWM频率

  };

  ledc_timer_config(&ledc_timer);

  //初始化PWM通道

  ledc_channel_config_t ledc_channel = 

  {

​    .channel = LEDC_CHANNEL_0,    //PWM通道

​    .duty = 0,            //占空比

​    .gpio_num = GPIO_NUM_15,     //GPIO口

​    .speed_mode = LEDC_LOW_SPEED_MODE,//速度模式

​    .timer_sel = LEDC_TIMER_0,    //定时器选择

  };

  ledc_channel_config(&ledc_channel);

  //渐变

  ledc_fade_func_install(0);

}

按键逻辑：

static void button_handle(void* arg)

{

  button_info_t *btn_info = button_head;

  int interval = (int)(arg);

  for(;btn_info;btn_info = btn_info->next)

  {

​    int gpio_num = btn_info->btn_cfg.gpio_num;

​    switch(btn_info->state)

​    {

​      case BUTTON_RELEASE:     //按键松开

​        if(btn_info->btn_cfg.getlevel_cb(gpio_num) == btn_info->btn_cfg.active_level)

​        {

​          btn_info->state = BUTTON_PRESS;

​          btn_info->press_cnt += interval;

​        }

​        break;

​      case BUTTON_PRESS:      //消抖状态（按键按下）

​        if(btn_info->btn_cfg.getlevel_cb(gpio_num) == btn_info->btn_cfg.active_level)

​        {

​          btn_info->press_cnt += interval;

​          if(btn_info->press_cnt >= 20)

​          {

​            if(btn_info->btn_cfg.short_press_cb)

​            {

​              btn_info->btn_cfg.short_press_cb(gpio_num);

​            }

​            btn_info->state = BUTTON_HOLD;

​          }

​        }

​        else

​        {

​          btn_info->state = BUTTON_RELEASE;

​          btn_info->press_cnt = 0;

​        }

​        break;

​      case BUTTON_HOLD:       //按键按住

​        if(btn_info->btn_cfg.getlevel_cb(gpio_num) == btn_info->btn_cfg.active_level)

​        {

​          btn_info->press_cnt += interval;

​          if(btn_info->press_cnt >= btn_info->btn_cfg.long_press_time)

​          {

​            if(btn_info->btn_cfg.long_press_cb)

​            {

​              btn_info->btn_cfg.long_press_cb(gpio_num);

​            }

​            btn_info->state = BUTTON_LONG_PRESS_HOLD;

​          }

​        }

​        else

​        {

​          btn_info->state = BUTTON_RELEASE;

​          btn_info->press_cnt = 0;

​        }

​        break;

​      case BUTTON_LONG_PRESS_HOLD: //等待松开

​        if(btn_info->btn_cfg.getlevel_cb(gpio_num) != btn_info->btn_cfg.active_level)

​        {

​          btn_info->state = BUTTON_RELEASE;

​          btn_info->press_cnt = 0;

​        }

​        break;

​      default:break;

​    }

  }

}

### 2.遇到的问题以及解决方法



描述：1.函数指针 `getlevel_cb` 被定义为 **返回 `void`**，但在代码中试图把它当int返回值使用

​	    2.烧录时串口选择错误

解决：

1.(1) 将函数返回值类型修改为int

2.(1)查阅手册，最后在管理器中找到对应串口

### 3.新学习到的知识点

掌握部分ESP32S3的基础知识：GPIO和按键

## 二、下周安排

掌握ESP32的WIFI连接的简单实现，AP配网

### 三、项目源码更新说明