#include "wifi.h"


static const char *TAG = "wifi";

// declare the netifs
esp_netif_t *wlan0;
esp_netif_t *wlan1;

// create an event handler for wlan0
static void wlan0_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {

	if (event_id == WIFI_EVENT_STA_CONNECTED) {
		wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t*) event_data;
		ESP_LOGI(TAG,"joined ssid: %s, bssid (mac addr):"MACSTR", AID=%d",event->ssid,MAC2STR(event->bssid),event->aid);
	} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t*) event_data;
		ESP_LOGI(TAG,"left ssid: %s, bssid (mac addr):"MACSTR", reason=%d",event->ssid,MAC2STR(event->bssid),event->reason);
	}
}

// create an event handler for wlan1
static void wlan1_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {

	if (event_id == WIFI_EVENT_AP_STACONNECTED) {
		wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t*) event_data;
		ESP_LOGI(TAG,"station: "MACSTR" joined, AID=%d",MAC2STR(event->mac),event->aid);
	} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t*) event_data;
		ESP_LOGI(TAG,"station: "MACSTR" left, AID=%d, reason=%d",MAC2STR(event->mac),event->aid,event->reason);
	}
}


void start_wifi(void) {

	// initialize the wifi configurations
	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wlan0_event_handler,NULL,NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wlan1_event_handler,NULL,NULL));

	// initialize wlan0 and wlan1 (sta and ap)
	wlan0 = esp_netif_create_default_wifi_sta();
	wlan1 = esp_netif_create_default_wifi_ap();
	wifi_config_t wifi_cfg = {
		.sta = {
			.ssid = WLAN0_SSID,
			.password = WLAN0_PASS,
			.channel = WLAN0_CHAN
		},
		.ap = {
			.ssid = WLAN1_SSID,
			.ssid_len = strlen(WLAN1_SSID),
			.channel = WLAN1_CHAN,
			.password = WLAN1_PASS,
			.max_connection = WLAN1_STA,
			.ssid_hidden = WLAN1_HID
		}
	};

	// in order to set a static ip address for the wlan1 netif, Dynamic Host Configuration Protocol Server (dhcps) must be stopped on the wlan1 netif
    esp_netif_dhcps_stop(wlan1);

    // set a static ip address for the wlan1 netif
    esp_netif_ip_info_t wlan1_ip_info = { 0 };
    ip4addr_aton(WLAN1_IP4_ADDR,(ip4_addr_t*)&wlan1_ip_info.ip);
	ip4addr_aton(WLAN1_GATEWAY,(ip4_addr_t*)&wlan1_ip_info.gw);
    ip4addr_aton(WLAN1_NET_MASK,(ip4_addr_t*)&wlan1_ip_info.netmask);
    esp_netif_set_ip_info(wlan1,&wlan1_ip_info);

	// resume dhcps
	esp_netif_dhcps_start(wlan1);

	ESP_LOGI(TAG,"wlan0_init (sta) finished. SSID:%s password:%s channel:%d",WLAN0_SSID,WLAN0_PASS,WLAN0_CHAN);
	ESP_LOGI(TAG,"wlan1_init (sta) finished. SSID:%s password:%s channel:%d",WLAN1_SSID,WLAN1_PASS,WLAN1_CHAN);

	// start the wifi
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&wifi_cfg));
}
