#include "nvs.h"

#define TAG "nvs"

// ip4 strings
char eth_ip4_str[16];
char ap_ip4_str[16];
char sta_ip4_str[16];

// gateway strings
char eth_gw_str[16];
char ap_gw_str[16];
char sta_gw_str[16];

// netmask strings
char eth_nm_str[16];
char ap_nm_str[16];
char sta_nm_str[16];

// ssid strings
char ap_ssid_str[32];
char sta_ssid_str[32];

// read bind/connect ip4 strings
char rb_ip4_str[16];
char rc_ip4_str[16];

// write bind/connect ip4 strings
char wb_ip4_str[16];
char wc_ip4_str[16];

// read register strings
char r_reg_str[4][6];
char r_reg_lo_str[4][7];
char r_reg_hi_str[4][7];

// write register strings
char w_reg_str[4][6];
char w_reg_lo_str[4][7];
char w_reg_hi_str[4][7];

// Load a single field from NVS
int load_from_nvs(const char *key, char *dest) {
    nvs_handle_t nvs;
    size_t required_size;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs);
    if (err == ESP_OK) {
        err = nvs_get_str(nvs, key, NULL, &required_size);
        if (err == ESP_OK && required_size < 33) {
            nvs_get_str(nvs, key, dest, &required_size);
        }
        nvs_close(nvs);
        return 0;
    }
    return -1;
}


void start_nvs(void) {
    esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ESP_ERROR_CHECK(nvs_flash_init());
	}

	// ip4
	if (load_from_nvs("eth_ip4_str",eth_ip4_str) < 0) {
        strcpy(eth_ip4_str,"");
    }
    if (load_from_nvs("ap_ip4_str",ap_ip4_str) < 0) {
        strcpy(ap_ip4_str,"192.168.2.1");
    }
    if (load_from_nvs("sta_ip4_str",sta_ip4_str) < 0) {
        strcpy(sta_ip4_str,"");
    }

    // gw
	if (load_from_nvs("eth_gw_str",eth_gw_str) < 0) {
        strcpy(eth_gw_str,"");
    }
    if (load_from_nvs("ap_gw_str",ap_gw_str) < 0) {
        strcpy(ap_gw_str,"192.168.2.1");
    }
    if (load_from_nvs("sta_gw_str",sta_gw_str) < 0) {
        strcpy(sta_gw_str,"");
    }

    // nm
	if (load_from_nvs("eth_nm_str",eth_nm_str) < 0) {
        strcpy(eth_nm_str,"255.255.255.0");
    }
    if (load_from_nvs("ap_nm_str",ap_nm_str) < 0) {
        strcpy(ap_nm_str,"255.255.255.0");
    }
    if (load_from_nvs("sta_nm_str",sta_nm_str) < 0) {
        strcpy(sta_nm_str,"255.255.255.0");
    }

    // ssid
    if (load_from_nvs("ap_ssid_str",ap_ssid_str) < 0) {
        strcpy(ap_ssid_str,"192.168.2.1");
    }
    if (load_from_nvs("sta_ssid_str",sta_ssid_str) < 0) {
        strcpy(sta_ssid_str,"");
    }

    // r
    if (load_from_nvs("r_reg_str0",r_reg_str[0]) < 0) {
        strcpy(r_reg_str[0],"");
    }
    if (load_from_nvs("r_reg_lo_str0",r_reg_lo_str[0]) < 0) {
        strcpy(r_reg_lo_str[0],"");
    }
    if (load_from_nvs("r_reg_hi_str0",r_reg_hi_str[0]) < 0) {
        strcpy(r_reg_hi_str[0],"");
    }
    if (load_from_nvs("r_reg_str1",r_reg_str[1]) < 0) {
        strcpy(r_reg_str[1],"");
    }
    if (load_from_nvs("r_reg_lo_str1",r_reg_lo_str[1]) < 0) {
        strcpy(r_reg_lo_str[1],"");
    }
    if (load_from_nvs("r_reg_hi_str1",r_reg_hi_str[1]) < 0) {
        strcpy(r_reg_hi_str[1],"");
    }
    if (load_from_nvs("r_reg_str2",r_reg_str[2]) < 0) {
        strcpy(r_reg_str[2],"");
    }
    if (load_from_nvs("r_reg_lo_str2",r_reg_lo_str[2]) < 0) {
        strcpy(r_reg_lo_str[2],"");
    }
    if (load_from_nvs("r_reg_hi_str2",r_reg_hi_str[2]) < 0) {
        strcpy(r_reg_hi_str[2],"");
    }
    if (load_from_nvs("r_reg_str3",r_reg_str[3]) < 0) {
        strcpy(r_reg_str[3],"");
    }
    if (load_from_nvs("r_reg_lo_str3",r_reg_lo_str[3]) < 0) {
        strcpy(r_reg_lo_str[3],"");
    }
    if (load_from_nvs("r_reg_hi_str3",r_reg_hi_str[3]) < 0) {
        strcpy(r_reg_hi_str[3],"");
    }
    if (load_from_nvs("rb_ip4_str",rb_ip4_str) < 0) {
        strcpy(rb_ip4_str,"");
    }
    if (load_from_nvs("rc_ip4_str",rc_ip4_str) < 0) {
        strcpy(rc_ip4_str,"");
    }

    // w
    if (load_from_nvs("w_reg_str0",w_reg_str[0]) < 0) {
        strcpy(w_reg_str[0],"");
    }
    if (load_from_nvs("w_reg_lo_str0",w_reg_lo_str[0]) < 0) {
        strcpy(w_reg_lo_str[0],"");
    }
    if (load_from_nvs("w_reg_hi_str0",w_reg_hi_str[0]) < 0) {
        strcpy(w_reg_hi_str[0],"");
    }
    if (load_from_nvs("w_reg_str1",w_reg_str[1]) < 0) {
        strcpy(w_reg_str[1],"");
    }
    if (load_from_nvs("w_reg_lo_str1",w_reg_lo_str[1]) < 0) {
        strcpy(w_reg_lo_str[1],"");
    }
    if (load_from_nvs("w_reg_hi_str1",w_reg_hi_str[1]) < 0) {
        strcpy(w_reg_hi_str[1],"");
    }
    if (load_from_nvs("w_reg_str2",w_reg_str[2]) < 0) {
        strcpy(w_reg_str[2],"");
    }
    if (load_from_nvs("w_reg_lo_str2",w_reg_lo_str[2]) < 0) {
        strcpy(w_reg_lo_str[2],"");
    }
    if (load_from_nvs("w_reg_hi_str2",w_reg_hi_str[2]) < 0) {
        strcpy(w_reg_hi_str[2],"");
    }
    if (load_from_nvs("w_reg_str3",w_reg_str[3]) < 0) {
        strcpy(w_reg_str[3],"");
    }
    if (load_from_nvs("w_reg_lo_str3",w_reg_lo_str[3]) < 0) {
        strcpy(w_reg_lo_str[3],"");
    }
    if (load_from_nvs("w_reg_hi_str3",w_reg_hi_str[3]) < 0) {
        strcpy(w_reg_hi_str[3],"");
    }
    if (load_from_nvs("wb_ip4_str",wb_ip4_str) < 0) {
        strcpy(wb_ip4_str,"");
    }
    if (load_from_nvs("wc_ip4_str",wc_ip4_str) < 0) {
        strcpy(wc_ip4_str,"");
    }
}
