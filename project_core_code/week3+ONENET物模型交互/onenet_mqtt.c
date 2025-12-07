#include "onenet_mqtt.h"
#include "onenet_token.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "onenet_dm.h"

#define TAG   "onenet"

static esp_mqtt_client_handle_t mqtt_handle = NULL;

static void onenet_property_ack(const char* id,int code,const char* msg);      //响应下行数据

static void onenet_subscribe(void);                                 //订阅主题

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        onenet_subscribe();
        cJSON* property_js = onenet_porperty_upload();
        char* data = cJSON_PrintUnformatted(property_js);
        onenet_post_property_data(data);
        cJSON_free(data);
        cJSON_Delete(property_js);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if(strstr(event->topic,"property/set"))
        {
            cJSON* property = cJSON_Parse(event->data);
            cJSON* id_js = cJSON_GetObjectItem(property,"id");
            onenet_porperty_handle(property);
            onenet_property_ack(cJSON_GetStringValue(id_js),200,"success");
            cJSON_Delete(property);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}


esp_err_t onenet_start(void)                   //启动MQTT连接至ONENET
{
    esp_mqtt_client_config_t  mqtt_config ;
    memset(&mqtt_config,0,sizeof(esp_mqtt_client_config_t));
    mqtt_config.broker.address.uri = "mqtt://mqtts.heclouds.com";       //uri地址
    mqtt_config.broker.address.port = 1883;                             //端口号

    mqtt_config.credentials.client_id = ONENET_DEVICE_NAME;
    mqtt_config.credentials.username = ONENET_PROJECT_ID;

    static char token[256];

    dev_token_generate(token,SIG_METHOD_SHA256,2080385766,ONENET_PROJECT_ID,ONENET_DEVICE_NAME,ONENET_PROJECT_ACCESS_KEY);  //获取token值
    mqtt_config.credentials.authentication.password = token;

    mqtt_handle = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(mqtt_handle,ESP_EVENT_ANY_ID,mqtt_event_handler,NULL);
    return esp_mqtt_client_start(mqtt_handle);


}


//响应下行数据
static void onenet_property_ack(const char* id,int code,const char* msg)
{
    char topic[128];
    snprintf(topic,128,"$sys/%s/%s/thing/property/set_reply",ONENET_PROJECT_ID,ONENET_DEVICE_NAME);
    cJSON* reply_js =  cJSON_CreateObject();
    cJSON_AddStringToObject(reply_js,"id",id);
    cJSON_AddNumberToObject(reply_js,"code",code);
    cJSON_AddStringToObject(reply_js,"msg",msg);
    char* data = cJSON_PrintUnformatted(reply_js);
    //推送
    esp_mqtt_client_publish(mqtt_handle,topic,data,strlen(data),1,0);
    cJSON_free(data);
    cJSON_Delete(reply_js);
}

//订阅主题
static void onenet_subscribe(void)
{
    char topic[128];
    //订阅上报属性回复主题
    snprintf(topic,128,"$sys/%s/%s/thing/property/post/reply",ONENET_PROJECT_ID,ONENET_DEVICE_NAME);
    esp_mqtt_client_subscribe_single(mqtt_handle,topic,1);
    snprintf(topic,128,"$sys/%s/%s/thing/property/set",ONENET_PROJECT_ID,ONENET_DEVICE_NAME);
    esp_mqtt_client_subscribe_single(mqtt_handle,topic,1);
}

esp_err_t onenet_post_property_data(const char* data)
{
    char topic[128];
    snprintf(topic,128,"$sys/%s/%s/thing/property/post",ONENET_PROJECT_ID,ONENET_DEVICE_NAME);
    ESP_LOGI(TAG,"Upload topic:%s,payload:%s",topic,data);
    return esp_mqtt_client_publish(mqtt_handle,topic,data,strlen(data),1,0);
}
