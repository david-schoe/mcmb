#include "mb.h"
#include "wifi.h"

// declare the netifs
esp_netif_t *ap;
esp_netif_t *sta;


void init_wifi(void) {

	// in order to set a static ip address for the sta netif, Dynamic Host Configuration Protocol Client (dhcpc) must be stopped
	esp_netif_dhcpc_stop(sta);

	// set a static ip address for the sta netif
	esp_netif_ip_info_t sta_ip_info = { 0 };
    ip4addr_aton(sta_ip4_str,(ip4_addr_t*)&sta_ip_info.ip);
	ip4addr_aton(sta_gw_str,(ip4_addr_t*)&sta_ip_info.gw);
    ip4addr_aton(sta_nm_str,(ip4_addr_t*)&sta_ip_info.netmask);
    esp_netif_set_ip_info(sta,&sta_ip_info);
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

	// initialize ap and sta
	ap = esp_netif_create_default_wifi_ap();
	sta = esp_netif_create_default_wifi_sta();
	wifi_config_t ap_cfg = {
		.ap = {
			.ssid_len = strlen(ap_ssid_str),
			.channel = AP_CHAN,
			.password = AP_PASS,
			.max_connection = AP_STA,
			.ssid_hidden = AP_HID
		}
	};
	wifi_config_t sta_cfg = {
		.sta = {
				.password = STA_PASS,
				.channel = STA_CHAN
		}
	};
	strcpy((char*)ap_cfg.ap.ssid,ap_ssid_str);
	strcpy((char*)sta_cfg.sta.ssid,sta_ssid_str);

	// in order to set a static ip address for the ap netif, Dynamic Host Configuration Protocol Server (dhcps) must be stopped
    esp_netif_dhcps_stop(ap);

    // set a static ip address for the ap netif
    esp_netif_ip_info_t ap_ip_info = { 0 };
    ip4addr_aton(ap_ip4_str,(ip4_addr_t*)&ap_ip_info.ip);
	ip4addr_aton(ap_gw_str,(ip4_addr_t*)&ap_ip_info.gw);
    ip4addr_aton(ap_nm_str,(ip4_addr_t*)&ap_ip_info.netmask);
    esp_netif_set_ip_info(ap,&ap_ip_info);

	// resume dhcps
	esp_netif_dhcps_start(ap);

	if (!strcmp(sta_ssid_str,"")) {
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&ap_cfg));
		ESP_LOGI(TAG,"ap_init (ap) finished. SSID:%s password:%s channel:%d",ap_cfg.ap.ssid,ap_cfg.ap.password,ap_cfg.ap.channel);

		// start ap
		esp_wifi_start();
	} else {

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&ap_cfg));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&sta_cfg));
		ESP_LOGI(TAG,"ap_init (ap) finished. SSID:%s password:%s channel:%d",ap_cfg.ap.ssid,ap_cfg.ap.password,ap_cfg.ap.channel);
		ESP_LOGI(TAG,"sta_init (sta) finished. SSID:%s password:%s channel:%d",sta_cfg.sta.ssid,sta_cfg.sta.password,sta_cfg.sta.channel);

		// start ap
		esp_wifi_start();

		// start sta
		connect_wifi();
	}
}
