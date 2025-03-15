#include "h.h"

void r_select_task(void *pvParameters) {

    int b;                                      // return value

    struct h *h = (struct h*)pvParameters;      // host
    struct r *r;                                // remote

    struct fd_set sss;                          // selected socket set
    struct rt *c;                               // current socket table entry
    struct timeval r_sel_timeo = {R_SEL_TIMEO_S,R_SEL_TIMEO_US};

    for (;;){

        // copy the host socket set into the socket select set and begin select()
        sss = h->ss;
        select(rt_max()+1,&sss,NULL,NULL,&r_sel_timeo);

        // avoid data races for rt and initialize the current entry at the base of the list
        xSemaphoreTake(h->re,portMAX_DELAY);
        c = h->rt;
        xSemaphoreGive(h->re);

        while (1) {

            // avoid data races for rt
            xSemaphoreTake(h->re,portMAX_DELAY);
            if (c->n == NULL) {

                // if we are at the end of the list, break out of the while loop
                xSemaphoreGive(h->re);
                break;
            }

            // otherwise, continue after copying remote from table and immediately incrementing the current entry
            r = c->r;
            c = c->n;
            xSemaphoreGive(h->re);

            // only continue if this socket has been set by the select() function
            if (FD_ISSET(r->s,&sss)) {

                // clear the socket from the selected socket set
                FD_CLR(r->s,&sss);

                if (r->ba[0] == 16 && r->ca[0] == 0) {

                    // listening socket has a connection that is ready to be received
                    if ((b=r_accept(r)) > 0) {

                        struct r* nr;      // new remote

                        nr = r_create(NULL,0,NULL,0);
                        nr->s = b;
                        r_conn(nr);
                    }
                } else {

                    // socket associated with an external slave/master has received a message
                    if (r_recv(r) == 0) {

                        // message was received
                        r_proc(r);

                        // message was processed
                        if (r->ca[0] == 0) {

                            // message was a request, send response
                            r_send(r);
                        }
                    }
                }
            }
        }
    }
}


void r_proc(struct r *r) {

    struct a    *ia;        // inbound modbus adu
    struct a    *oa;        // outbound modbus adu

    int         ii=0;         // inbound message index
    int         oi=0;         // outbound message index
    int         la=0;         // last adu flag

    while (!la) {

        // cast message data at index xi into a modubs adu struct
        ia = (struct a*)(r->im+ii);
        oa = (struct a*)(r->om+oi);

        // increment inbound message index
        ii += ntohs(ia->l)+6;
        if (ii > r->ib) {

            // inbound adu is reported to have a byte cout that exceeds the message byte count
            ESP_LOGE(TAG,"r_proc: ii=%d, r->ib=%d",ii,r->ib);
            break;
        } else if (ii == r->ib) {

            // this is the last adu in the message
            la = 1;
        }

        if (r->ca[0] == 0) {

            // inbound adu is a request

            uint8_t b;          // byte count
            uint8_t j;          // iterative number
            uint8_t m;          // bit mask
            uint8_t *p8;        // host coil pointer
            uint16_t *p16;      // host register pointer
            uint8_t n;          // number of coil/register

            uint16_t l;         // length as given in adu0
            uint8_t f;          // function code
            uint16_t sa;        // starting address
            uint16_t q;         // quantity of inputs except for w/ function codes 5 and 6

            uint8_t *v8;        // pointer to uint8_t
            uint16_t *v16;      // pointer to uint16_t

            // avoid data races for stack of associated device
            xSemaphoreTake(h->he,portMAX_DELAY);

            // assign frequently used portions of adu0
            l = ntohs(ia->l);
            f = ia->p[0];
            sa = ntohs(*((uint16_t*)(ia->p+1)));
            q = ntohs(*((uint16_t*)(ia->p+3)));

            // determine which portion of stack needs to be accessed and access it
            switch(f){
                case 1:
                case 2:

                    // assign pointer to the requested data location on host
                    p8 = *(&h->hc+f-1);
                    n = *(&h->nhc+f-1);

                    // verify that the requested data location/s exist/s
                    if(p8 == NULL) {
                        A_ERROR(2);
                        break;
                    } else if((q < 1) || (q > 0x07d0)){
                        A_ERROR(3);
                        break;
                    } else if(n < sa+q) {
                        A_ERROR(2);
                        break;
                    }

                    // get the byte count given in pdu (not adu)
                    b = (q+7)/8;

                    // increment the outbound message index
                    oi += 7+2+b;

                    // copy first 8 bytes from request adu into response adu
                    memcpy(oa,ia,8);

                    // set the pdu byte count
                    oa->p[1] = b;

                    // set the adu length
                    oa->l = htons(b+3);

                    // ensure the bit requested by the starting address is the first bit in response pdu1
                    for(j=0;j<b;j++){
                        oa->p[2+j] = (uint8_t)(*((uint16_t*)(p8+j+(sa)/8)) >> ((sa)%8));
                    }

                    // zero padd the last byte
                    oa->p[2+b-1] = oa->p[2+b-1] & (0xff >> (8-(q%8)));
                    break;
                case 3:
                case 4:

                    // assign pointer to the requested data location on host
                    p16 = *(&h->hr+f-3);
                    n = *(&h->nhr+f-3);

                    // verify that the requested data location/s exist/s
                    if(p16 == NULL) {
                        A_ERROR(2);
                        break;
                    } else if((q < 1) || (q > 0x7d)){
                        A_ERROR(3);
                        break;
                    } else if(n < sa+q) {
                        A_ERROR(2);
                        break;
                    }

                    // get the byte count
                    b = q*2;

                    // increment the outbound message index
                    oi += 7+2+b;

                    // copy first 8 bytes from request adu
                    memcpy(oa,ia,8);

                    // set the pdu byte count
                    oa->p[1] = b;

                    // set the adu length
                    oa->l = htons(b+3);

                    // ensure the registers are in network byte order
                    v16 = (uint16_t*)(oa->p+2);
                    for(j=0;j<q;j++){
                        v16[j] = htons(p16[sa]);
                    }
                    break;
                case 5:

                    // assign pointer to the requested data location on host
                    p8 = h->hc;
                    n = h->nhc;

                    // verify that the requested data location exists
                    if(p8 == NULL) {
                        A_ERROR(2);
                        break;
                    } else if(n < sa) {
                        A_ERROR(2);
                        break;
                    }

                    // prepare the bit mask corresponding to the starting address
                    m = 0x01 << (sa)%8;

                    // write the requested data
                    if(q == 0xff00){
                    *(p8+(sa)/8) = *(p8+(sa)/8) | m;
                    } else if(q == 0x0000){
                        *(p8+(sa)/8) = *(p8+(sa)/8) | m;
                    } else {
                        A_ERROR(3);
                        break;
                    }

                    // increment the outbound message index
                    oi += 7+5;

                    // copy the first 12 bytes of request adu (entire adu) into response adu
                    memcpy(oa,ia,12);

                    // set the adu length
                    oa->l = htons(6);
                    break;
                case 15:

                    // assign pointer to the requested data location on host
                    p8 = h->hc;
                    n = h->nhc;

                    // verify that the requested data locations exist
                    if(p8 == NULL) {
                        A_ERROR(2);
                        break;
                    } else if(n < sa+q){
                        A_ERROR(2);
                        break;
                    }

                    // get the byte count
                    b = ia->p[5];

                    if ((q < 1) || (q > 0x07b0) || (b != (q+7)/8) || (b != l-7)) {
                        A_ERROR(3);
                        break;
                    }

                    // pad the values sent in adu0 with zero on either side   00101100
                    v8 = (uint8_t*) calloc(b+2,1);
                    ESP_LOGD(TAG,"r_proc: calloc(%p)",v8);
                    memcpy(v8+1,ia->p+6,b);

                    // or the last byte in the data set
                    *(v8+1+b) = *(p8+(sa)/8+b) | ((uint8_t)(*((uint16_t*)(v8+b)) >> (8-(sa)%8)));

                    // write the requested data
                    for(j=0;j<(1+(sa+q-1)/8-sa/8);j++){

                        // let the mask default such that every coil within a storage byte may be overwritten
                        m = 0x00;

                        // if working on the first byte, make a mask which ensures the coil values that precede
                        // the starting bit are preserved when overwritting the requested coils
                        if(j == 0){
                            m = (uint8_t) 0xff >> (8-(sa)%8);
                        }

                        // if working on the last byte, make a mask which ensures the coil values that succede
                        // the final bit are preserved when overwritting the requested coils
                        if(j == ((sa+q-1)/8-sa/8)){
                            if((sa+q)%8!=0){
                                m = ((uint8_t) (0xff << (sa+q)%8)) | (m);
                            }
                        }
                        *(p8+(sa)/8+j) = (*(p8+(sa)/8+j) & m) | ((uint8_t)(*((uint16_t*)(v8+j)) >> (8-(sa)%8)));
                    }
                    ESP_LOGD(TAG,"r_proc: free(%p)",v8);
                    free(v8);

                    // increment the outbound message index
                    oi += 7+5;

                    // copy first 12 bytes of request adu into outbound adu
                    memcpy(oa,ia,12);

                    // set the adu length
                    oa->l = htons(6);
                    break;
                case 6:

                    // assign pointer to the requested data location on host
                    p16 = h->hr;
                    n = h->nhr;

                    // verify that the requested data location/s exist/s
                    if(p16 == NULL) {
                        A_ERROR(2);
                        break;
                    } else if(n < sa+1) {
                        A_ERROR(2);
                        break;
                    }

                    // write the requested data
                    memcpy(p16+2+sa,&q,2);

                    // increment the outbound message index
                    oi += 7+5;

                    // copy first 12 bytes from request adu into outbound adu
                    memcpy(oa,ia,12);

                    // set the adu length
                    oa->l = htons(6);
                    break;
                case 16:

                    // assign pointer to the requested data location on host
                    p16 = h->hr;
                    n = h->nhr;

                    // verify that the requested data location/s exist/s
                    if(p16 == NULL) {
                        A_ERROR(2);
                        break;
                    } else if(n < sa+q) {
                        A_ERROR(2);
                        break;
                    }

                    // get the byte count
                    b = ia->p[5];

                    if ((q < 1) || (q > 0x7b) || (b != q*2) || (b != l-7)) {
                        A_ERROR(3);
                        break;
                    }

                    // write the requested data
                    v16 = (uint16_t*)(ia->p+6);
                    for(j=0;j<q;j++){
                        p16[sa+j] = ntohs(v16[j]);
                    }

                    // increment the outbound message index
                    oi += 7+5;

                    // copy first 12 bytes from request adu into outbound adu
                    memcpy(oa,ia,12);

                    // set the adu length
                    oa->l = htons(6);
                    break;
                default:
                    A_ERROR(1);
                    break;
            }
            xSemaphoreGive(h->he);
            r->ob = oi;
        } else {

            // check if error was returned
            if (ia->p[0] < 0x80) {

                // no error was returned
                if (h->recvd_resp_cb != NULL){

                    // execute callback (callback must free oa)
                    oa = malloc(ntohs(ia->l)+6);

                    ESP_LOGD(TAG,"r_proc: malloc(%p), ntohs(ia->l)=%d",oa,ntohs(ia->l));
                    memcpy(oa,ia,ntohs(ia->l)+6);
                    h->recvd_resp_cb(oa);
                }
            } else {

                // modbus error was returned
                if (h->recvd_error_cb != NULL) {

                    // execute callback (callback must free oa)
                    oa = malloc(ntohs(ia->l)+6);
                    ESP_LOGD(TAG,"r_proc: malloc(%p)",oa);
                    memcpy(oa,ia,ntohs(ia->l)+6);
                    h->recvd_error_cb(oa);
                }
            }
        }
    }
}


struct r* r_create(char *b_ip4_str, uint16_t bp, char *c_ip4_str, uint16_t cp) {

    ip4_addr_t ip4;          // ip4 address to be used in sockaddr

    // ensure the binding address and connecting address are alligned to 4 bytes
    struct r *r = (struct r*)aligned_alloc(4,sizeof(struct r));
    memset(r,0,sizeof(struct r));

    if ((b_ip4_str != NULL) && (bp != 0)) {

        // ensure network byte ordering
        bp = htons(bp);

        // convert ip4 string to ip4 struct
        ip4addr_aton(b_ip4_str,&ip4);

        // save the ip4 address of the netif to the binding address
        r->ba[0] = 16;
        r->ba[1] = AF_INET;
        memcpy(r->ba+2,&bp,2);
        memcpy(r->ba+4,&ip4,4);
    }

    if ((c_ip4_str != NULL) && (cp != 0)) {

        // ensure network byte ordering
        cp = htons(cp);

        // convert ip4 string to ip4 struct
        ip4addr_aton(c_ip4_str,&ip4);

        // save the ip4 address given by ip4_str to the connecting address
        r->ca[0] = 16;
        r->ca[1] = AF_INET;
        memcpy(r->ca+2,&cp,2);
        memcpy(r->ca+4,&ip4,4);
    }

    rt_push(r);
    return r;
}


void r_send_req(struct r *r, uint16_t tid, uint8_t *pdu, size_t sz) {
    struct a *oa = (struct a*)r->om;
    r->ob = sz+7;
    oa->tid = htons(tid);
    oa->pid = 0;
    oa->l = htons(sz+1);
    oa->uid = 0xff;
    memcpy(oa->p,pdu,sz);
    r_send(r);
}


void r_dump(struct r *r) {

    int j;          // iterator

    printf("r->s=%d, r->ib=%d, r->ob=%d\n",r->s,r->ib,r->ob);
    printf("im: ");
    for (j=0;j<r->ib;j++) {
        printf("0x%02x |",r->im[j]);
    }
    printf("\nom: ");
    for (j=0;j<r->ib;j++) {
        printf("0x%02x |",r->om[j]);
    }
    printf("\n");
    fflush(stdout);
}


int r_conn(struct r *r) {

    conn:

    if ((r->ba[0] == 16) || (r->ca[0] == 16)) {

        // remote is user created
        if (r_socket(r) < 0) {
            vTaskDelay(100);
            goto conn;
        }
    }
    if (r->ba[0] == 16) {

        // remote has a binding address
        if (r_bind(r) < 0) {
            vTaskDelay(100);
            goto conn;
        }
    }
    if (r->ca[0] == 16) {

        // remote has a connecting address
        if (r_connect(r) < 0) {
            vTaskDelay(100);
            goto conn;
        }
    }
    if ((r->ba[0] == 16) && (r->ca[0] == 0)) {

        // remote is for listening
        if (r_listen(r) < 0) {
            vTaskDelay(100);
            goto conn;
        }
    }

    // change the connection state
    r->c = 1;

    // add the socket to the socket set
    FD_SET(r->s,&h->ss);
    return 0;
}


int r_dconn(struct r *r) {

    if (!r->c) {
        ESP_LOGD(TAG,"it's working");
        return -1;
    }

    // change the connection state
    r->c = 0;

    // remove the socket from the socket set
    FD_CLR(r->s,&h->ss);

    // force shut and close the socket (not always necessary, but doesn't hurt on failure, and simplifies synchronization procedure)
    r_shutdown(r);
    r_close(r);


    if (r->ba[0] == 0 && r->ca[0] == 0) {

        // remote is associated with an external master
        if (rt_remove(r) == 0) {
            ESP_LOGD(TAG,"r_dconn(%p): free(%p)",r,r);
            free(r);
        }
    } else {

        // remote is associated with an external slave or is listening
        r->s = 0;
        #ifdef R_RECONN
        r_conn(r);
        #endif
    }
    return 0;
}


int r_socket(struct r *r) {

    int b;          // return value (socket)
    int f;          // socket flags

    // open a socket for communication
    if ((b=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0) {
        ESP_LOGE(TAG,"r_conn: socket(), b=%d, errno=%d",b,errno);
        switch (errno) {
            default:
                return -1;
        }
    }
    ESP_LOGD(TAG,"r_conn: socket(), b=%d, errno=%d",b,errno);
    // assign the socket to the remote
    r->s = b;

    // make the socket non-blocking
    f = fcntl(b,F_GETFL,0);
    f |= O_NONBLOCK;
    fcntl(b,F_SETFL,f);

    // disable the Naegle algorithm
    f = 1;
    setsockopt(r->s,IPPROTO_TCP,TCP_NODELAY,(void*)&f,sizeof(int));

    return b;
}


int r_bind(struct r *r) {

    int b;              // return value

    // bind the remote socket to ensure it uses the appropriate network interface
    if ((b=bind(r->s,(struct sockaddr*)r->ba,r->ba[0])) < 0) {
        ESP_LOGE(TAG,"r_bind(%d), b=%d, errno=%d",r->s,b,errno);
        switch (errno) {

            case (EBADF):
                r->s = 0;
                return -1;
            default:
                // assume failure to bind is not detrimental
                return 0;
        }
    }
    ESP_LOGD(TAG,"r_bind(%d), b=%d, errno=%d",r->s,b,errno);
    return 0;
}


int r_listen(struct r *r) {

    int b;              // return value

    // listen for connections on this socket with a queue size of 2
    if ((b=listen(r->s,2)) < 0) {
        ESP_LOGE(TAG,"r_listen(%d), b=%d, errno=%d",r->s,b,errno);
        switch (errno) {
            case (EBADF):
                r->s = 0;
                return -1;
            default:
                r_close(r);
                r->s = 0;
                return -1;
        }
    }
    ESP_LOGD(TAG,"r_listen(%d), b=%d, errno=%d",r->s,b,errno);
    return 0;
}


int r_connect(struct r *r) {

    int b;              // return value

    // connect to the remote address
    if ((b=connect(r->s,(struct sockaddr*)r->ca,r->ca[0])) < 0) {
        ESP_LOGE(TAG,"r_connect(%d), b=%d, errno=%d",r->s,b,errno);
        switch (errno) {
            case (EAGAIN):
            case (EALREADY):
                ESP_LOGD(TAG,"r_connect(%d) pending",r->s);
                return 0;
            case (EBADF):
                r->s = 0;
                return -1;
            case (EINPROGRESS):
                ESP_LOGD(TAG,"r_connect(%d) pending",r->s);
                return 0;
            case (EISCONN):
                ESP_LOGD(TAG,"r_connect(%d) already connected",r->s);
                return 0;
            default:
                r_close(r);
                r->s = 0;
                return -1;
        }
    }
    ESP_LOGD(TAG,"r_connect(%d), b=%d, errno=%d",r->s,b,errno);
    return 0;
}


int r_accept(struct r *r) {

    int b;              // return value (new socket)
    int f;              // socket flags

    // accept the new connection
    if ((b=accept(r->s,NULL,NULL)) < 0) {
        ESP_LOGE(TAG,"r_accept(%d), b=%d, errno=%d",r->s,b,errno);
        switch (errno) {
            case (EWOULDBLOCK):
                break;
            case (EBADF):
                r_dconn(r);
                break;
            case (EINVAL):
            default:
                r_dconn(r);
                return -1;
        }
    }

    // make the socket non-blocking
    f = fcntl(b,F_GETFL,0);
    f |= O_NONBLOCK;
    fcntl(b,F_SETFL,f);
    ESP_LOGD(TAG,"r_accept(%d), b=%d, errno=%d",r->s,b,errno);
    return b;
}


int r_recv(struct r *r) {

    // zero the inbound message
    memset(&r->ib,0,sizeof(int)+r->ib);

    // receive the inbound message
    if ((r->ib=recv(r->s,(void*)r->im,R_IM_SIZE,0)) < 0) {
        ESP_LOGE(TAG,"r_recv(%d), im->b=%d, errno=%d",r->s,r->ib,errno);
        switch (errno) {
            case (EWOULDBLOCK):
                return -1;
            case (EBADF):
                r_dconn(r);
                return -1;
            case (ECONNREFUSED):
            case (ENOTCONN):
                r_dconn(r);
                return -1;
            default:
                return -1;
        }
    } else if (r->ib == 0) {
        ESP_LOGD(TAG,"r_recv(%d), r->ib=%d, errno=%d",r->s,r->ib,errno);
        r_dconn(r);
        return -1;
    }
    ESP_LOGD(TAG,"r_recv(%d), r->ib=%d, errno=%d",r->s,r->ib,errno);
    return 0;
}


int r_send(struct r *r) {

    int b;              // return value

    // send the outbound message
    if ((b=send(r->s,r->om,r->ob,0)) < 0) {
        ESP_LOGE(TAG,"r_send(%d), r->ob=%d, b=%d, errno=%d",r->s,r->ob,b,errno);
        switch (errno) {
            case (EWOULDBLOCK):

                // TO DO: Add support to not drop packets?
                vTaskDelay(10);
                return -1;
            case (EBADF):
                r_dconn(r);
                return -1;
            case (ECONNRESET):
                r_dconn(r);
                return -1;
            case (ENOTCONN):
                r_dconn(r);
                return -1;
            default:
                return -1;
        }
    }
    ESP_LOGD(TAG,"r_send(%d), r->ob=%d, b=%d, errno=%d",r->s,r->ob,b,errno);

    // zero the outbound message
    memset(&r->ob,0,sizeof(int)+R_OM_SIZE);
    return 0;
}


int r_shutdown(struct r *r) {

    int b;          // return value

    // shutdown the socket
    if ((b=shutdown(r->s,SHUT_RDWR)) < 0) {
        ESP_LOGE(TAG,"r_shutdown(%d), b=%d, errno=%d",r->s,b,errno);
        switch (errno) {
            case (EBADF):
            default:
                return -1;
        }
    }
    ESP_LOGD(TAG,"r_shutdown(%d), b=%d, errno=%d",r->s,b,errno);
    return 0;
}


int r_close(struct r *r) {

    int b;              // return value

    // close the socket
    if ((b=close(r->s)) < 0) {
        ESP_LOGE(TAG,"r_close: close(%d), b=%d, errno=%d",r->s,b,errno);
        switch (errno) {
            case (EBADF):
                return -1;
            default:
                return -1;
        }
    }
    ESP_LOGD(TAG,"r_close: close(%d), b=%d, errno=%d",r->s,b,errno);
    return 0;
}


void rt_push(struct r *r) {
    struct rt *c = h->rt;
    xSemaphoreTake(h->re,portMAX_DELAY);

    while(1){
        if (c->n == NULL){
            c->r = r;
            c->n = (struct rt*) calloc(sizeof(struct rt),sizeof(char));
            ESP_LOGD(TAG,"rt_push: calloc(%p)",c->n);
            xSemaphoreGive(h->re);
            break;
        } else {
            c = c->n;
        }
    }
}


struct r* rt_peek(int s) {
    struct rt *c = h->rt;
    xSemaphoreTake(h->re,portMAX_DELAY);

    while(1){
        if (c->r->s == s){
            xSemaphoreGive(h->re);
            return c->r;
        } else if (c->n == NULL) {
            ESP_LOGE(TAG,"rt_peek(%s): no associated remote");
            xSemaphoreGive(h->re);
            return NULL;
        } else {
            c = c->n;
        }
    }
}


int rt_remove(struct r *r) {

    xSemaphoreTake(h->re,portMAX_DELAY);
    struct rt *c = h->rt;          // current entry
    struct rt *p = h->rt;          // previous entry

    while (1) {
        if (c->n == NULL) {
            ESP_LOGE(TAG,"rt_remove(%p): no matching entry found",r);
            xSemaphoreGive(h->re);
            return -1;
        } else if (c->r == r) {
            ESP_LOGD(TAG,"rt_remove(%p): removed remote entry w/ s=%d",r,c->r->s);
            ESP_LOGD(TAG,"st_pop: free(%p)",c);
            if (c == h->rt){
                h->rt = c->n;
                free(c);
                xSemaphoreGive(h->re);
                return 0;
            } else {
                p->n = c->n;
                free(c);
                xSemaphoreGive(h->re);
                return 0;
            }
        } else {
            p = c;
            c = c->n;
        }
    }
}


void rt_clean(void) {
    struct rt *c = h->rt;
    struct rt *p = h->rt;
    xSemaphoreTake(h->re,portMAX_DELAY);

    while(1){
        p = c;
        c = c->n;
        ESP_LOGD(TAG,"rt_clean: free(%p)",p);
        free(p);
        if (c->n == NULL) {
            xSemaphoreGive(h->re);
            return;
        }
    }
}


int rt_max(void) {
    struct rt *c = h->rt;           // current entry in socket table
    int s = 0;                      // highest numbered socket in table
    while (1) {
        if (c->n == NULL)
            return s;
        else {
            if (c->r->s > s) {
                s = c->r->s;
            }
            c = c->n;
        }
    }
}
