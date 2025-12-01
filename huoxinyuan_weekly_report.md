# 第二周周报：2025.11.24-2025.12.1

## 一、本周项目完成内容和新学知识

### 1.目前的进度(包括实现的功能和环境搭建等)

（1）掌握ESP32的WIFI连接（STA模式，AP模式）的简单实现

​		STA:ESP32作为终端设备，接入到一个无线接入点

​		AP：ESP32作为无线接入点，等待其他设备的接入

（2）学习了ESP32的AP配网

​		配网方式：手动，smartconfig,ble,**AP**

​		协议:http,websockte

WIFI的简单实现：

static void event_handler(void* arg, esp_event_base_t event_base,

​                int32_t event_id, void* event_data)

{

  if(event_base == WIFI_EVENT)

  {

​    switch(event_id)

​    {

​      case WIFI_EVENT_STA_START:

​        esp_wifi_connect();

​        break;

​      case WIFI_EVENT_STA_DISCONNECTED:

​        if(is_sta_connected)

​        {

​          is_sta_connected = false;

​          if(wifi_callback)

​            wifi_callback(WIFI_STATE_DISCONNECTED);

​        }

​        if(sta_connect_cnt < MAX_CONNECT_RETRY)

​        {

​          esp_wifi_connect();

​          sta_connect_cnt++;

​        }

​        break;

​      case WIFI_EVENT_STA_CONNECTED:

​        ESP_LOGI(TAG,"Connected to app");

​        break;

​      default:break;

​    }

  }

  if(event_base == IP_EVENT)

  {

​    if(event_id == IP_EVENT_STA_GOT_IP)

​    {

​      ESP_LOGI(TAG,"Get ip addr");

​      is_sta_connected = true;

​      if(wifi_callback)

​        wifi_callback(WIFI_STATE_CONNECTED);

​    }

  }

}

void wifi_manager_init(p_wifi_state_cb f);		//初始化

void wifi_manager_connect(const char* ssid,const char* password);		//连接

AP配网：

//接收回调函数

static void ws_recieve_handle(uint8_t* payload,int len)

{

  cJSON* root = cJSON_Parse((const char*)payload);

  if(root)

  {

​    cJSON* scan_js = cJSON_GetObjectItem(root,"scan");

​    cJSON* ssid_js = cJSON_GetObjectItem(root,"ssid");

​    cJSON* password_js = cJSON_GetObjectItem(root,"password");

​    if(scan_js)

​    {

​      char* scan_value = cJSON_GetStringValue(scan_js);

​      if(strcmp(scan_value,"start") == 0)

​      {

​        //启动扫描

​      }

​    }

​    if(ssid_js && password_js)

​    {

​      char* ssid_value = cJSON_GetStringValue(ssid_js);

​      char* password_value = cJSON_GetStringValue(password_js);

​      snprintf(current_ssid,sizeof(current_ssid),"%s",ssid_value);

​      snprintf(current_password,sizeof(current_password),"%s",password_value);

​      xEventGroupSetBits(apcfg_ev,APCFG_BIT);

​      //web_ws_stop();

​      //wifi_manager_connect(ssid_value,ssid_password);

​    }

  }

}

void ap_wifi_apcfg()        //进入AP配网

{

  wifi_manager_ap();

  ws_cfg_t ws_cfg = 

  {

​    .html_code = html_code,

​    .receive_fn = ws_recieve_handle,

  };

  web_ws_start(&ws_cfg);

}

esp_err_t web_ws_start(ws_cfg_t* cfg);          //开启服务器

esp_err_t web_ws_stop(void);          //关闭服务器

esp_err_t web_ws_send(uint8_t *data, int len);          //发送数据

### 2.遇到的问题以及解决方法



描述：1.回调函数的应用

解决：

1.在CSDN上找相应的介绍，B站博主的讲解

### 3.新学习到的知识点

wifi连接的实现，AP配网

## 二、下周安排

ONENET,部分音频开发

### 三、项目源码更新说明