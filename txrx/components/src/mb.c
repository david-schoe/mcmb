#include "mb.h"

// all these variables need to be declared at file scope for use by other functions/tasks
struct h *h;
struct r *rl;
struct r *rc;
uint8_t *r_pdu0;
uint8_t *w_pdu1;
TickType_t time_elapsed;

void recvd_resp_cb(struct a *ia) {
    if (ntohs(ia->tid)%1000 == 0) {
        // stop timer
        time_elapsed = pdTICKS_TO_MS(xTaskGetTickCount())-time_elapsed;
        ESP_LOGI(TAG,"time_elapsed=%d [ms]",time_elapsed);
        time_elapsed = pdTICKS_TO_MS(xTaskGetTickCount());
    }
    // r_send_req(rc,ntohs(ia->tid)+1,w_pdu1,10);
    // r_send_req(rc,ntohs(ia->tid)+1,r_pdu0,5);
    a_dump(ia);
    free(ia);
}

void recvd_error_cb(struct a *ia) {
    a_dump(ia);
    free(ia);
}

void start_mb(void) {

    r_pdu0 = (uint8_t*)malloc(5);
    uint8_t f0 = 3;
    uint16_t a0 = 0;
    uint16_t q0 = 4;

    // ensure network byte ordering
    a0 = htons(a0);
    q0 = htons(q0);
    memcpy(r_pdu0,&f0,1);
    memcpy(r_pdu0+1,&a0,2);
    memcpy(r_pdu0+3,&q0,2);

    // create a write request pdu
    w_pdu1 = (uint8_t*)malloc(10);
    uint8_t f1 = 16;
    uint16_t a1 = 2;
    uint16_t q1 = 2;
    uint8_t b1 = 4;
    uint16_t d1[2];
    d1[0] = 0x0000;
    d1[1] = 0xffff;

    // ensure network byte ordering
    q1 = htons(q1);
    a1 = htons(a1);
    memcpy(w_pdu1,&f1,1);
    memcpy(w_pdu1+1,&a1,2);
    memcpy(w_pdu1+3,&q1,2);
    memcpy(w_pdu1+5,&b1,1);
    memcpy(w_pdu1+6,&d1,4);


    // create a host, allocate 4 holding registers (first 2 for reading, second 2 for writing)
    h = NULL;
    h_create(&h);
    h_alloc_reg_h(h,4);

    // set first register to 0xabcd, second to 0x1234
    *(h->hr) = 0xabcd;
    *(h->hr+1) = 0x1234;

    // attach a listening remote on loopback interface and bind to 502
    rl = r_create("127.0.0.1",502,NULL,0);

    // create a remote that will request a connection on loopback interface, port 502
    rc = r_create("127.0.0.1",1024,"127.0.0.1",502);

    // register recvd_resp_cb and recvd_error_cb
    h->recvd_resp_cb = recvd_resp_cb;
    h->recvd_error_cb = recvd_error_cb;
    h_start(h);

    // start timer
    time_elapsed = pdTICKS_TO_MS(xTaskGetTickCount());
    for (int j=0;;j++) {
        r_send_req(rc,j,r_pdu0,5);
        vTaskDelay(1);
    }

    // check state of heap and task
    ESP_LOGI(TAG,"hw: %d\n",uxTaskGetStackHighWaterMark(NULL));
    ESP_LOGI(TAG, "Largest free block: %u\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    ESP_LOGI(TAG, "Free heap: %u\n", esp_get_free_heap_size());
}
