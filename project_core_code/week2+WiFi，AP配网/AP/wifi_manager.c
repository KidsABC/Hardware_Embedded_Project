#include "wifi_manager.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include "lwip/ip4_addr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define MAX_CONNECT_RETRY   10//可重连最多次数
static int sta_connect_cnt = 0;//重连次数

#define TAG  "wifi_manager"

static const char* ap_ssid_name = "ESP32_AP";
static const char* ap_password = "12345678";

//当前STA连接状态
static bool is_sta_connected = false;

//wifi状态回调函数
static p_wifi_state_cb wifi_callback = NULL;

//
static esp_netif_t* esp_netif_ap = NULL;

static SemaphoreHandle_t scan_sem =NULL;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT)
    {
        switch(event_id)
        {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                if(is_sta_connected)
                {
                    is_sta_connected = false;
                    if(wifi_callback)
                        wifi_callback(WIFI_STATE_DISCONNECTED);
                }
                if(sta_connect_cnt < MAX_CONNECT_RETRY)
                {
                    esp_wifi_connect();
                    sta_connect_cnt++;
                }
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG,"Connected to app");
                break;
            case WIFI_EVENT_AP_STACONNECTED:
                ESP_LOGI(TAG,"sta device connected");
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI(TAG,"sta device disconnected");
                break;
            default:break;
        }
    }
    if(event_base == IP_EVENT)
    {
        if(event_id == IP_EVENT_STA_GOT_IP)
        {
            ESP_LOGI(TAG,"Get ip addr");
            is_sta_connected = true;
            if(wifi_callback)
                wifi_callback(WIFI_STATE_CONNECTED);
        }
    }
}


//初始化
void wifi_manager_init(p_wifi_state_cb f)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_ap = esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    wifi_callback = f;
    scan_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(scan_sem);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_start() );
}

void wifi_manager_connect(const char* ssid,const char* password)
{
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    snprintf((char*)wifi_config.sta.ssid,31,"%s",ssid);
    snprintf((char*)wifi_config.sta.password,63,"%s",password);

    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if(mode != WIFI_MODE_STA)
    {
        esp_wifi_stop();
        esp_wifi_set_mode(WIFI_MODE_STA);
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        esp_wifi_start();
    }
    sta_connect_cnt = 0;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    esp_wifi_start();
}

//进入AP模式
esp_err_t wifi_manager_ap(void)
{
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if(mode == WIFI_MODE_APSTA)
    {
        return ESP_OK;
    }
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    wifi_config_t wifi_config =
    {
        .ap =
        {
            .channel = 5,
            .max_connection = 2,
            .authmode = WIFI_AUTH_WPA2_PSK,
        }
    };
    snprintf((char*)wifi_config.ap.ssid,32,"%s",ap_ssid_name);
    wifi_config.ap.ssid_len = strlen(ap_ssid_name);
    snprintf((char*)wifi_config.ap.password,64,"%s",ap_password);
    esp_wifi_set_config(WIFI_IF_AP,&wifi_config);

    esp_netif_ip_info_t ipInfo;
    IP4_ADDR(&ipInfo.ip,192,168,100,1);      //IP地址
    IP4_ADDR(&ipInfo.gw,192,168,100,1);        //网关
    IP4_ADDR(&ipInfo.netmask,255,255,255,0);   //子网掩码

    esp_netif_dhcps_stop(esp_netif_ap);              //dhcp停止后才可以设置IP
    esp_netif_set_ip_info(esp_netif_ap,&ipInfo);
    esp_netif_dhcps_start(esp_netif_ap);
    
    return esp_wifi_start();
}

static void scan_task(void* param)
{
    p_wifi_scan_cb callback = (p_wifi_scan_cb)param;
    uint16_t ap_count = 0;
    uint16_t ap_num = 20;
    wifi_ap_record_t *ap_list = (wifi_ap_record_t*)malloc(sizeof(wifi_ap_record_t)*ap_num);
    esp_wifi_scan_start(NULL,true);
    esp_wifi_scan_get_ap_num(ap_count);
    esp_wifi_scan_get_ap_records(&ap_num,ap_list);
    ESP_LOGI(TAG,"Total ap count:%d,actual ap number:%d",ap_count,ap_num);
    if(callback)
        callback(ap_num,ap_list);
    free(ap_list);
    xSemaphoreGive(scan_sem);
    vTaskDelete(NULL);
}

esp_err_t wifi_manager_scanf(p_wifi_scan_cb f)
{
    if(xSemaphoreTake(scan_sem,0))           //尝试获取信号量
    {
        esp_wifi_clear_ap_list();           //清除上次保存在底部的AP热点列表
        return xTaskCreatePinnedToCore(scan_task,"scan",81902,f,3,NULL,1);
    }
    return ESP_OK;
}