#include "nvs.h"

#define TAG "nvs"

// ip4 strings
char eth_ip4_str[16];
char wlan0_ip4_str[16];
char wlan1_ip4_str[16];

// netmask strings
char eth_nm_str[16];
char wlan0_nm_str[16];
char wlan1_nm_str[16];

// gateway strings
char eth_nm_str[16];
char wlan0_gw_str[16];
char wlan1_gw_str[16];

// ssid strings
char wlan0_ssid_str[32];
char wlan1_ssid_str[32];

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

// Load a single field from NVS
void load_from_nvs(const char *key, char *dest) {
    nvs_handle_t nvs;
    size_t required_size;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs);
    if (err == ESP_OK) {
        err = nvs_get_str(nvs, key, NULL, &required_size);
        if (err == ESP_OK && required_size < 33) {
            nvs_get_str(nvs, key, dest, &required_size);
        }
        nvs_close(nvs);
    }
}


void start_nvs(void) {
    esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ESP_ERROR_CHECK(nvs_flash_init());
	}

	//
	load_from_nvs("eth_ip4_str", eth_ip4_str);
    load_from_nvs("wlan0_ip4_str", wlan0_ip4_str);
    load_from_nvs("wlan1_ip4_str", wlan1_ip4_str);

	load_from_nvs("eth_nm_str", eth_ip4_str);
    load_from_nvs("wlan0_nm_str", wlan0_ip4_str);
    load_from_nvs("wlan1_nm_str", wlan1_ip4_str);

	load_from_nvs("eth_gw_str", eth_ip4_str);
    load_from_nvs("wlan0_gw_str", wlan0_ip4_str);
    load_from_nvs("wlan1_gw_str", wlan1_ip4_str);

    load_from_nvs("r_reg0_str", r_reg0_str);
    load_from_nvs("r_reg1_str", r_reg1_str);
    load_from_nvs("r_reg2_str", r_reg2_str);
    load_from_nvs("r_reg3_str", r_reg3_str);

    load_from_nvs("rc_ip4_str", rc_ip4_str);
    load_from_nvs("rb_ip4_str", rb_ip4_str);

    load_from_nvs("w_reg0_str", w_reg0_str);
    load_from_nvs("w_reg1_str", w_reg1_str);
    load_from_nvs("w_reg2_str", w_reg2_str);
    load_from_nvs("w_reg3_str", w_reg3_str);

    load_from_nvs("wc_ip4_str", wc_ip4_str);
    load_from_nvs("wb_ip4_str", wb_ip4_str);

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
