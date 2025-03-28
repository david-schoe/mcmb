#include "eth.h"
#include "wifi.h"
#include "mb.h"

// all these variables need to be declared at file scope for use by other functions/tasks
struct h *h;
struct r *ra; // access point
struct r *re; // ethernet
struct r *rl; // loopback
struct r *rr; // read
struct r *rw; // write

uint8_t r_pdu[5];
uint8_t w_pdu[5];

// these are stored in network byte order
uint16_t r_reg0;
uint16_t r_reg1;
uint16_t r_reg2;
uint16_t r_reg3;

// these are stored in network byte order
uint16_t w_reg0;
uint16_t w_reg1;
uint16_t w_reg2;
uint16_t w_reg3;

TickType_t time_elapsed;

void recvd_resp_cb(struct a *ia) {
    if (ntohs(ia->tid)%1000 == 0) {
        // stop timer
        time_elapsed = pdTICKS_TO_MS(xTaskGetTickCount())-time_elapsed;
        ESP_LOGI(TAG,"time_elapsed=%d [ms]",time_elapsed);
        time_elapsed = pdTICKS_TO_MS(xTaskGetTickCount());
    }

    // verify the necessary connections
    if (!rw->nc || (rw->tc < 1)) {
        a_dump(ia);
        free(ia);
        return;
    }
    switch(ntohs(ia->tid)) {
        case 0:
            memcpy(w_pdu+1,(uint8_t*)&w_reg0,2);
            memcpy(w_pdu+3,ia->p+2,2);
            r_send_req(rw,ntohs(ia->tid)+0x10,w_pdu,5);
            break;
        case 1:
            memcpy(w_pdu+1,(uint8_t*)&w_reg1,2);
            memcpy(w_pdu+3,ia->p+2,2);
            r_send_req(rw,ntohs(ia->tid)+0x10,w_pdu,5);
            break;
        case 2:
            memcpy(w_pdu+1,(uint8_t*)&w_reg2,2);
            memcpy(w_pdu+3,ia->p+2,2);
            r_send_req(rw,ntohs(ia->tid)+0x10,w_pdu,5);
            break;
        case 3:
            memcpy(w_pdu+1,(uint8_t*)&w_reg3,2);
            memcpy(w_pdu+3,ia->p+2,2);
            r_send_req(rw,ntohs(ia->tid)+0x10,w_pdu,5);
            break;
        default:
            break;
    }
    a_dump(ia);
    free(ia);
}


void recvd_error_cb(struct a *ia) {
    a_dump(ia);
    free(ia);
}


void set_mb_reg(void) {
    r_reg0 = R_REG(0);
    r_reg1 = R_REG(1);
    r_reg2 = R_REG(2);
    r_reg3 = R_REG(3);

    w_reg0 = W_REG(0);
    w_reg1 = W_REG(1);
    w_reg2 = W_REG(2);
    w_reg3 = W_REG(3);
}


void mb_send_task(void *pvParameters) {
    for (;;) {
        // verify the necessary connections
        if (!rr->nc || (rr->tc < 1)) {
            ESP_LOGI(TAG,"idle");
            vTaskDelay(100);
            continue;
        }

        // check state of heap and task
        ESP_LOGI(TAG,"hw: %d\n",uxTaskGetStackHighWaterMark(NULL));
        ESP_LOGI(TAG, "Largest free block: %u\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
        ESP_LOGI(TAG, "Free heap: %u\n", esp_get_free_heap_size());

        memcpy(r_pdu+1,(uint8_t*)&r_reg0,2);
        r_send_req(rr,0,r_pdu,5);
        vTaskDelay(10);
        memcpy(r_pdu+1,(uint8_t*)&r_reg1,2);
        r_send_req(rr,1,r_pdu,5);
        vTaskDelay(10);
        memcpy(r_pdu+1,(uint8_t*)&r_reg2,2);
        r_send_req(rr,2,r_pdu,5);
        vTaskDelay(10);
        memcpy(r_pdu+1,(uint8_t*)&r_reg3,2);
        r_send_req(rr,3,r_pdu,5);
        vTaskDelay(10);
    }
}


void start_mb(void) {

    // create a host, allocate 4 holding registers (generally used for debugging)
    h = NULL;
    h_create(&h);
    h_alloc_reg_h(h,4);

    *(h->hr+0) = 0x0123;
    *(h->hr+1) = 0xabcd;
    *(h->hr+2) = 0x4567;
    *(h->hr+3) = 0xef00;

    // attach a listening remote on wlan0 interface and bind to port 502
    ra = r_create(wlan0_ip4_str,502,NULL,0);

    // attach a listening remote on ethernet interface and bind to port 502
    // re = r_create(eth_ip4_str,502,NULL,0);

    // attach a listening remote on loopback interface and bind to port 502
    rl = r_create("127.0.0.1",502,NULL,0);

    if (strcmp(rb_ip4_str,"") && strcmp(rc_ip4_str,"")) {
        // create a remote that will request a connection to rc_ip4_addr, port 502
        rr = r_create(rb_ip4_str,1024,rc_ip4_str,502);
    } else {
        h_start(h);
        return;
    }
    if (strcmp(wb_ip4_str,"") && strcmp(wc_ip4_str,"")) {
        // create a remote that will request a connection to wc_ip4_addr, port 502
        rw = r_create(wb_ip4_str,1025,wc_ip4_str,502);
    }

    // register recvd_resp_cb and recvd_error_cb
    h->recvd_resp_cb = recvd_resp_cb;
    h->recvd_error_cb = recvd_error_cb;
    h_start(h);




    // start timer
    time_elapsed = pdTICKS_TO_MS(xTaskGetTickCount());

    set_mb_reg();
    r_pdu[0] = 3;
    r_pdu[4] = 1;
    w_pdu[0] = 6;

    xTaskCreate(mb_send_task,"mb_send_task",2048*2,NULL,1,NULL);
}
