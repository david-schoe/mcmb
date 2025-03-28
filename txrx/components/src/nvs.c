#include "nvs.h"

#define TAG "nvs"

// wlan0/1 ip4 strings
char wlan0_ip4_str[16];
char wlan1_ip4_str[16];

// wlan0/1 ssid strings
char wlan0_ssid_str[32];
char wlan1_ssid_str[32];

// eth ip4 string
char eth_ip4_str[16];

// read bind/connect ip4 strings
char rb_ip4_str[16];
char rc_ip4_str[16];

// write bind/connect ip4 strings
char wb_ip4_str[16];
char wc_ip4_str[16];

// read register strings
char r_reg0_str[6];
char r_reg1_str[6];
char r_reg2_str[6];
char r_reg3_str[6];

// write register strings
char w_reg0_str[6];
char w_reg1_str[6];
char w_reg2_str[6];
char w_reg3_str[6];

void start_nvs(void) {
    esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ESP_ERROR_CHECK(nvs_flash_init());
	}

	// these should be initialized by nvs

    // tx config

	strcpy(wlan0_ip4_str,"192.168.2.11");
	strcpy(wlan0_ssid_str,"192.168.2.11");
	strcpy(wlan1_ip4_str,"192.168.2.2");
	strcpy(wlan1_ssid_str,"192.168.2.1");

    strcpy(rb_ip4_str,wlan1_ip4_str);
    strcpy(rc_ip4_str,DAT_8024_IP4_STR);
	strcpy(wb_ip4_str,"127.0.0.1");
    strcpy(wc_ip4_str,"127.0.0.1");






	// rx config
	/*
	strcpy(wlan0_ip4_str,"192.168.2.1");
	strcpy(wlan0_ssid_str,"192.168.2.1");
	strcpy(wlan1_ip4_str,"");
	strcpy(wlan1_ssid_str,"");

	strcpy(eth_ip4_str,"192.168.1.1");

    strcpy(rb_ip4_str,"");
    strcpy(rc_ip4_str,"");
    strcpy(wb_ip4_str,"");
    strcpy(wc_ip4_str,"");
	*/



    strcpy(r_reg0_str,"40000");
    strcpy(r_reg1_str,"40001");
    strcpy(r_reg2_str,"40002");
    strcpy(r_reg3_str,"40003");
    strcpy(w_reg0_str,"40000");
    strcpy(w_reg1_str,"40001");
    strcpy(w_reg2_str,"40002");
    strcpy(w_reg3_str,"40003");

}
