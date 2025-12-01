#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "button.h"
#include "xl9555.h"
#include "ap_wifi.h"
#include "nvs_flash.h"

#define XL9555_SDA  GPIO_NUM_10
#define XL9555_SCL  GPIO_NUM_11

#define TAG     "main"

//按位表示
static volatile uint16_t xl9555_button_level = 0xFFFF;

//获取xl95555对应电平
int get_button_level(int gpio)
{
    return (xl9555_button_level&gpio)?1:0;
}


//xl95555的GPIO电平变化回调函数
void xl9555_input_callback(uint16_t io_num,int level)
{
    if(level)
    {
        xl9555_button_level |= io_num;
    }
    else
    {
        xl9555_button_level &= ~io_num;
    }
}

void long_press(int gpio)
{
    ap_wifi_apcfg(); //启动AP配网模式
}

void button_init(void)
{
    //注册按键的事件驱动
    button_config_t button_cfg = 
    {
        .active_level = 0,
        .getlevel_cb = get_button_level,
        .gpio_num = IO0_1,
        .long_cb = long_press,
        .long_press_time = 3000,
    };
    button_event_set(&button_cfg);
}

void wifi_stat_callback(WIFI_STATE stat)
{
    if(stat == WIFI_STATE_CONNECTED)
    {
        ESP_LOGI(TAG,"wifi connected");
    }
    if(stat == WIFI_STATE_DISCONNECTED)
    {
        ESP_LOGI(TAG,"wifi disconnected");
    }
}

void i2c_and_xl9555_init(void)
{
    xl9555_init(XL9555_SDA,XL9555_SCL,GPIO_NUM_17,xl9555_input_callback);
    xl9555_ioconfig(0xFFFF);
}

void app_main(void)
{
    nvs_flash_init();
    i2c_and_xl9555_init();
    button_init();
    ap_wifi_init(wifi_stat_callback);
}
