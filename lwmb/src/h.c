#include "h.h"

void h_create(struct h **ah) {

    // allocate the host struct
    h = (struct h*)aligned_alloc(4,sizeof(struct h));
    memset(h,0,sizeof(struct h));

    // create a host semaphore
    h->he = xSemaphoreCreateBinary();
    xSemaphoreGive(h->he);

    // create a remote semaphore and remote table
    h->re = xSemaphoreCreateBinary();
    xSemaphoreGive(h->re);
    h->rt = NULL;

    // save the host onto the memory location pointed to by **ah
    *ah = h;
}


void h_start(struct h *h) {

#if R_SEL_VERIFY_TC
#elif
    struct rt *c = h->rt;       // current remote table entry
    // listen on and connect to the remotes created by r_create()
    while(1) {
        if (c == NULL) {
            break;
        }
        r_conn(c->r);
        c = c->n;
    }
#endif // elif

    // start the socket selecting task
    xTaskCreate(r_select_task,"r_select_task",H_TASK_STACK_SIZE*2,h,1,NULL);
}


void h_alloc_coil_h(struct h *h, uint8_t n){

    // avoid data races with other potential tasks
    xSemaphoreTake(h->he,portMAX_DELAY);

    // allocate 1 byte of space to hold the coil count
    // allocate 1 byte of space for every 8 coils
    h->hc = (uint8_t*)calloc(n+7/8,1);

    // set the coil count
    h->nhc = n;
    xSemaphoreGive(h->he);
}


void h_alloc_coil_i(struct h *h, uint8_t n){

    // avoid data races with other potential tasks
    xSemaphoreTake(h->he,portMAX_DELAY);

    // allocate 1 byte of space to hold the coil count
    // allocate 1 byte of space for every 8 coils
    h->ic = (uint8_t*)calloc(n+7/8,1);

    // set the coil count
    h->nic = n;
    xSemaphoreGive(h->he);
}


void h_alloc_reg_h(struct h *h, uint8_t n){

    // avoid data races with other potential tasks
    xSemaphoreTake(h->he,portMAX_DELAY);

    // allocate 1 byte of space to hold the coil count
    // allocate 1 byte of space for every 8 coils
    h->hr = (uint16_t*)calloc(2*n,1);

    // set the coil count
    h->nhr = n;
    xSemaphoreGive(h->he);
}


void h_alloc_reg_i(struct h *h, uint8_t n){

    // avoid data races with other potential tasks
    xSemaphoreTake(h->he,portMAX_DELAY);

    // allocate 1 byte of space to hold the coil count
    // allocate 1 byte of space for every 8 coils
    h->ir = (uint16_t*)calloc(2*n,1);

    // set the coil count
    h->nir = n;
    xSemaphoreGive(h->he);
}
