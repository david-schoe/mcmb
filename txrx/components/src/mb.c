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
uint16_t r_reg[4];
uint16_t w_reg[4];

// these are stored in host byte order
uint16_t r_reg_lo[4];
uint16_t w_reg_lo[4];
uint16_t r_reg_hi[4];
uint16_t w_reg_hi[4];

// this value will be written
uint16_t w_val;

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

    int i = ntohs(ia->tid);
    switch(i) {
        case 0:
        case 1:
        case 2:
        case 3:
            // interpolate
            w_val = htons(((uint16_t)((uint32_t)w_reg_hi[i]-w_reg_lo[i])*(ntohs(*((uint16_t*)(ia->p+2)))-r_reg_lo[i])+w_reg_lo[i]*(r_reg_hi[i]-r_reg_lo[i]))/(r_reg_hi[i]-r_reg_lo[i]));

            // set the register + value
            memcpy(w_pdu+1,(uint8_t*)&w_reg[i],2);
            memcpy(w_pdu+3,&w_val,2);
            r_send_req(rw,ntohs(ia->tid)+0x10,w_pdu,5);
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
    long l_reg;
    long l_reg_lo;
    long l_reg_hi;


    for (int i=0;i<4;i++) {
        l_reg = (r_reg_str[i][1]-0x30)*1000+(r_reg_str[i][2]-0x30)*100+(r_reg_str[i][3]-0x30)*10+r_reg_str[i][4]-0x30;
        l_reg_lo = strtol(r_reg_lo_str[i],NULL,0);
        l_reg_hi = strtol(r_reg_hi_str[i],NULL,0);

        memcpy(r_reg+i,((uint8_t*)&l_reg),2);
        memcpy(r_reg_lo+i,((uint8_t*)&l_reg_lo),2);
        memcpy(r_reg_hi+i,((uint8_t*)&l_reg_hi),2);


        r_reg[i] = htons(r_reg[i]);

        printf("r_reg[%d]=%x\n",i,r_reg[i]);
        printf("r_reg_lo[%d]=%d\n",i,r_reg_lo[i]);
        printf("reg_hi[%d]=%d\n",i,r_reg_hi[i]);


        l_reg = (w_reg_str[i][1]-0x30)*1000+(w_reg_str[i][2]-0x30)*100+(w_reg_str[i][3]-0x30)*10+w_reg_str[i][4]-0x30;
        l_reg_lo = strtol(w_reg_lo_str[i],NULL,0);
        l_reg_hi = strtol(w_reg_hi_str[i],NULL,0);

        memcpy(w_reg+i,((uint8_t*)&l_reg),2);
        memcpy(w_reg_lo+i,((uint8_t*)&l_reg_lo),2);
        memcpy(w_reg_hi+i,((uint8_t*)&l_reg_hi),2);

        w_reg[i] = htons(w_reg[i]);

        printf("w_reg[%d]=%x\n",i,w_reg[i]);
        printf("w_reg_lo[%d]=%d\n",i,w_reg_lo[i]);
        printf("weg_hi[%d]=%d\n",i,w_reg_hi[i]);
    }
}

void mb_send_task(void *pvParameters) {
    for (;;) {
        // verify the necessary connections
        if (!rr->nc || (rr->tc < 1)) {
            ESP_LOGI(TAG,"idle");
            vTaskDelay(200);
            continue;
        }

    if (*(h->hr+0) != 0xffff) {
        *(h->hr+0) += 1;
        *(h->hr+1) += 1;
        *(h->hr+2) += 1;
        *(h->hr+3) += 1;
    } else {
        *(h->hr+0) = 0;
        *(h->hr+1) = 0;
        *(h->hr+2) = 0;
        *(h->hr+3) = 0;
    }

        // check state of heap and task
        ESP_LOGI(TAG,"hw: %d\n",uxTaskGetStackHighWaterMark(NULL));
        ESP_LOGI(TAG, "Largest free block: %u\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
        ESP_LOGI(TAG, "Free heap: %u\n", esp_get_free_heap_size());

        for (int i=0;i<4;i++) {
            if (strcmp(r_reg_str[i],"")) {
                r_pdu[0] = (r_reg_str[i][0]-0x30 == 3) ? 4:3;
                memcpy(r_pdu+1,(uint8_t*)&r_reg[i],2);
                r_send_req(rr,i,r_pdu,5);
                vTaskDelay(30);
            }
        }
    }
}


void start_mb(void) {

    // create a host, allocate 4 holding registers (generally used for debugging)
    h = NULL;
    h_create(&h);
    h_alloc_reg_h(h,4);

    // attach a listening remote on ap interface and bind to port 502
    ra = r_create(ap_ip4_str,502,NULL,0);

    // attach a listening remote on ethernet interface and bind to port 502
    // re = r_create(eth_ip4_str,502,NULL,0);

    // attach a listening remote on loopback interface and bind to port 502
    rl = r_create("127.0.0.1",502,NULL,0);

    if (strcmp(rb_ip4_str,"") && strcmp(rc_ip4_str,"")) {
        // create a remote that will request a connection to rc_ip4_addr, port 502
        rr = r_create(rb_ip4_str,0xf00,rc_ip4_str,502);
    } else {
        h_start(h);
        return;
    }
    if (strcmp(wb_ip4_str,"") && strcmp(wc_ip4_str,"")) {
        // create a remote that will request a connection to wc_ip4_addr, port 502
        rw = r_create(wb_ip4_str,0xf02,wc_ip4_str,502);
    }

    // register recvd_resp_cb and recvd_error_cb
    h->recvd_resp_cb = recvd_resp_cb;
    h->recvd_error_cb = recvd_error_cb;
    h_start(h);

    // start timer
    time_elapsed = pdTICKS_TO_MS(xTaskGetTickCount());

    set_mb_reg();
    r_pdu[4] = 1;
    w_pdu[0] = 6;

    xTaskCreate(mb_send_task,"mb_send_task",2048*2,NULL,1,NULL);
}
