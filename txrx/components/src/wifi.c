#include "mb.h"
#include "wifi.h"

// declare the netifs
esp_netif_t *wlan0;
esp_netif_t *wlan1;


void init_wifi(void) {

	// in order to set a static ip address for the wlan1 netif, Dynamic Host Configuration Protocol Client (dhcpc) must be stopped
	esp_netif_dhcpc_stop(wlan1);

	// set a static ip address for the wlan1 netif
	esp_netif_ip_info_t wlan1_ip_info = { 0 };
    ip4addr_aton(wlan1_ip4_str,(ip4_addr_t*)&wlan1_ip_info.ip);
	ip4addr_aton(wlan1_gw_str,(ip4_addr_t*)&wlan1_ip_info.gw);
    ip4addr_aton(wlan1_gw_str,(ip4_addr_t*)&wlan1_ip_info.netmask);
    esp_netif_set_ip_info(wlan1,&wlan1_ip_info);
}


void connect_wifi(void) {
	connect_wifi:
	init_wifi();
	if (esp_wifi_connect() != ESP_OK) {
		vTaskDelay(100);
		goto connect_wifi;
	};
}

void start_wifi(void) {

	// initialize the wifi configurations
	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

	// initialize wlan0 and wlan1 (ap and sta)
	wlan0 = esp_netif_create_default_wifi_ap();
	wlan1 = esp_netif_create_default_wifi_sta();
	wifi_config_t wlan0_cfg = {
		.ap = {
			.ssid_len = strlen(wlan0_ssid_str),
			.channel = WLAN0_CHAN,
			.password = WLAN0_PASS,
			.max_connection = WLAN0_STA,
			.ssid_hidden = WLAN0_HID
		}
	};
	wifi_config_t wlan1_cfg = {
		.sta = {
				.password = WLAN1_PASS,
				.channel = WLAN1_CHAN
		}
	};
	strcpy((char*)wlan0_cfg.ap.ssid,wlan0_ssid_str);
	strcpy((char*)wlan1_cfg.sta.ssid,wlan1_ssid_str);

	// in order to set a static ip address for the wlan0 netif, Dynamic Host Configuration Protocol Server (dhcps) must be stopped
    esp_netif_dhcps_stop(wlan0);

    // set a static ip address for the wlan0 netif
    esp_netif_ip_info_t wlan0_ip_info = { 0 };
    ip4addr_aton(wlan0_ip4_str,(ip4_addr_t*)&wlan0_ip_info.ip);
	ip4addr_aton(wlan1_gw_str,(ip4_addr_t*)&wlan0_ip_info.gw);
    ip4addr_aton(wlan0_nm_str,(ip4_addr_t*)&wlan0_ip_info.netmask);
    esp_netif_set_ip_info(wlan0,&wlan0_ip_info);

	// resume dhcps
	esp_netif_dhcps_start(wlan0);

	if (!strcmp(wlan1_ssid_str,"")) {
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&wlan0_cfg));
		ESP_LOGI(TAG,"wlan0_init (ap) finished. SSID:%s password:%s channel:%d",wlan0_cfg.ap.ssid,wlan0_cfg.ap.password,wlan0_cfg.ap.channel);

		// start wlan0 (ap)
		esp_wifi_start();
	} else {

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&wlan0_cfg));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&wlan1_cfg));
		ESP_LOGI(TAG,"wlan0_init (ap) finished. SSID:%s password:%s channel:%d",wlan0_cfg.ap.ssid,wlan0_cfg.ap.password,wlan0_cfg.ap.channel);
		ESP_LOGI(TAG,"wlan1_init (sta) finished. SSID:%s password:%s channel:%d",wlan1_cfg.sta.ssid,wlan1_cfg.sta.password,wlan1_cfg.sta.channel);

		// start wlan0 (ap)
		esp_wifi_start();

		// start wlan1 (sta)
		connect_wifi();
	}
}
