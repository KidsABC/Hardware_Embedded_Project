#include "onenet_dm.h"
#include "led_ws2812.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include <string.h>

static ws2812_strip_handle_t    ws2812_handle = NULL;

static int led_brightness = 0;
static int led_status = 0;

static int ws2812_red = 0;
static int ws2812_green = 0;
static int ws2812_blue = 0;

void onenet_dm_init(void)               //初始化物模型数据
{
    //初始化ws2812
    ws2812_init(GPIO_NUM_18,3,&ws2812_handle);

    //LED定时器初始化
    ledc_timer_config_t led_timer = 
    {
        .clk_cfg = LEDC_CHANNEL_0,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 5000,
        .timer_num = LEDC_TIMER_0,
    };
    ledc_timer_config(&led_timer);

    //PWM初始化
    ledc_channel_config_t led_channel = 
    {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = GPIO_NUM_15,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&led_channel);
    ledc_fade_func_install(0);              //启动渐变
}

void onenet_porperty_handle(cJSON* property)      //处理下行数据
{
    /*
    {
        "id": "123",
        "version": "1.0",
        "params": {
            "Brightness":50,
            "LightSwitch":true,
            "RGBColor":{
                "Red":100,
                "Green":100,
                "Blue":100,
            }
        }
    }
    */
   cJSON* param_js = cJSON_GetObjectItem(property,"params");            //获取cJSON对象
   if(param_js)
   {
    cJSON* name_js = param_js->child;
    while(name_js)
    {
        if(strcmp(name_js->string,"Brightness") == 0)
        {
            led_brightness = cJSON_GetNumberValue(name_js);
            int duty = led_brightness*4095/100;
            ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,duty,0);
        }
        else if(strcmp(name_js->string,"LightSwitch") == 0)
        {
            if(cJSON_IsTrue(name_js))
            {
                led_status = 1;
                led_brightness = 50;
                int duty = 50*4095/100;
                ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,duty,0);
            }
            else
            {
                led_status = 0;
                led_brightness = 0;
                ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,0,0);
            }
        }
        else if(strcmp(name_js->string,"RGBColor") == 0)
        {
           ws2812_red = cJSON_GetNumberValue( cJSON_GetObjectItem(name_js,"Red"));
           ws2812_green = cJSON_GetNumberValue( cJSON_GetObjectItem(name_js,"Green"));
           ws2812_blue = cJSON_GetNumberValue( cJSON_GetObjectItem(name_js,"Blue"));
           for(int i = 0;i<3;i++)
           {
                ws2812_write(ws2812_handle,i,ws2812_red,ws2812_green,ws2812_blue);
           }
        }
        name_js = name_js->next;
    }
   }
}

cJSON* onenet_porperty_upload(void)               //生成上报的JSON数据
{
     /*
     {
         "id": "123",
         "version": "1.0",
         "params": {
             "Brightness":{
                 "value":50,
             },
             "LightSwitch":{
                 "value":ture,
             },
             "RGBColor":{
                 "value":{
                     "Red":100,
                     "Green":100,
                     "Blue":100,
                 }
             }
         }
     }
     */
    cJSON* root = cJSON_CreateObject();          //创建根节点对象
    cJSON_AddStringToObject(root,"id","123");
    cJSON_AddStringToObject(root,"version","1.0");

    cJSON* param_js = cJSON_AddObjectToObject(root,"params");

    //亮度
    cJSON* brightness_js = cJSON_AddObjectToObject(param_js,"Brightness");
    cJSON_AddNumberToObject(brightness_js,"value",led_brightness);

    //开关
    cJSON* lightSwitch_js = cJSON_AddObjectToObject(param_js,"LightSwitch");
    cJSON_AddBoolToObject(lightSwitch_js,"value",led_status);

    //RGB值
    cJSON* color_js = cJSON_AddObjectToObject(param_js,"RGBColor");
    cJSON* color_value_js = cJSON_AddObjectToObject(color_js,"value");
    cJSON_AddNumberToObject(color_value_js,"Red",ws2812_red);
    cJSON_AddNumberToObject(color_value_js,"Green",ws2812_green);
    cJSON_AddNumberToObject(color_value_js,"Blue",ws2812_blue);

    return root;
}
