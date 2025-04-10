#ifndef LWMB_R_H
#define LWMB_R_H

#define R_SEL_VERIFY_NC 1       // if this is enabled, the select task will ensure that each remote in the remote table has a netif connection established
#define R_SEL_VERIFY_TC 1       // if this is enabled, the select task will ensure that each remote in the remote table has a tcp connection established
#define R_SEL_TIMEO_S 0
#define R_SEL_TIMEO_US 10000
#define R_IM_SIZE 256
#define R_OM_SIZE 256



#include "h.h"


// remote
struct r {
    uint8_t     ba[16];             // binding address
    char        b_ip4_str[16];      // binding ip4 string
    uint8_t     ca[16];             // connecting address
    char        c_ip4_str[16];      // connecting ip4_string
    int         s;                  // socket
    int         nc;                 // network interface connection state (user must alter this state)
    int         tc;                 // tcp connection state

    int         ib;                 // inbound message byte count
    uint8_t     im[R_IM_SIZE];     // inbound message

    int         ob;                 // outbound message byte count
    uint8_t     om[R_OM_SIZE];     // oubound message
};

struct rt {
    struct r            *r;     // remote entry
    struct rt           *n;     // next remote entry
};


extern TickType_t time_elapsed;

void r_select_task(void *pvParameters);
struct r* r_create(char *b_ip4_str, uint16_t bp, char *c_ip4_str, uint16_t cp);
void r_recv_req(struct r *r);
void r_send_req(struct r *r, uint16_t tid, uint8_t *pdu, size_t sz);
void r_dump(struct r *r);

int r_conn(struct r *r);
int r_dconn(struct r *r);
int r_socket(struct r *r);
int r_bind(struct r *r);
int r_listen(struct r *r);
int r_connect(struct r *r);
int r_accept(struct r *r);
int r_recv(struct r *r);
int r_send(struct r *r);
int r_shutdown(struct r *r);
int r_close(struct r *r);

void rt_push(struct r *r);
struct r* rt_peek(int s);
struct r* rt_pop(struct r *r);
int rt_remove(struct r *r);
void rt_clean(void);
int rt_max(void);




#endif // DVMB_R_H
