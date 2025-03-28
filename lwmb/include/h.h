#ifndef LWMB_H_H
#define LWMB_H_H

#ifndef H_TASK_STACK_SIZE
#define H_TASK_STACK_SIZE (2048)
#endif

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "lwip/ip4_addr.h"
#include "lwip/sockets.h"
#include "a.h"
#include "r.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "lwmb"


struct h {
    struct rt           *rt;                                // remote table
    struct fd_set       ss;                                 // socket set

    SemaphoreHandle_t   he;                                 // host semaphore
    SemaphoreHandle_t   re;                                 // remote semaphore

    uint8_t             nhc;                                // number of holding coils
    uint8_t             nic;                                // number of input coils
    uint8_t             nhr;                                // number of holding registers
    uint8_t             nir;                                // number of input registers

    uint8_t             *hc;                                // holding coils
    uint8_t             *ic;                                // input coils
    uint16_t            *hr;                                // holding registers
    uint16_t            *ir;                                // input registers

    void             (*recvd_resp_cb)(struct a *ia);         // successful response callback function (user defined)
    void             (*recvd_error_cb)(struct a *ia);        // failed response callback function (user defined)


};

extern struct h *h;

void h_create(struct h **ah);
void h_start(struct h *h);
void h_alloc_coil_h(struct h *h, uint8_t n);
void h_alloc_coil_i(struct h *h, uint8_t n);
void h_alloc_reg_h(struct h *h, uint8_t n);
void h_alloc_reg_i(struct h *h, uint8_t n);

#endif
