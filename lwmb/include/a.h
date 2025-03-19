#ifndef LWMB_A_H
#define LWMB_A_H


#define A_ERROR(N) \
    oi = 9; \
    memcpy(oa,ia,9); \
    oa->l = htons(3); \
    oa->p[0] += 0x80; \
    oa->p[1] = N


// adu structure
struct a {

    // head
    uint16_t        tid;        // transaction identifier
    uint16_t        pid;        // protocol identifier (unused)
    uint16_t        l;          // length field
    uint8_t         uid;        // unit identifier (always set to 0xff for modbus tcp/ip)

    //pdu
    uint8_t         p[];
};

void a_dump(struct a *a);

#endif
