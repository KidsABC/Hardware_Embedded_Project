#include <stdio.h>
#include "onenet_mqtt.h"
#include "wifi_manager.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "onenet_dm.h"

static EventGroupHandle_t   wifi_ev = NULL;
#define WIFI_CONNECT_BIT   BIT0

static void wifi_state_callback(WIFI_STATE state)
{
    if(state == WIFI_STATE_CONNECTED)
    {
        xEventGroupSetBits(wifi_ev,WIFI_CONNECT_BIT);
    }
}

void app_main(void)
{
    nvs_flash_init();
    wifi_ev = xEventGroupCreate();
    onenet_dm_init();
    wifi_manager_init(wifi_state_callback);
    wifi_manager_connect("testwifi","12345678");
    EventBits_t ev;
    while(1)
    {
        ev = xEventGroupWaitBits(wifi_ev,WIFI_CONNECT_BIT,pdTRUE,pdFALSE,pdMS_TO_TICKS(10*1000));
        if(ev & WIFI_CONNECT_BIT)
        {
            onenet_start();
        }
    }

}
