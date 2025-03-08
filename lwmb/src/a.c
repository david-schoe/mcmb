#include "h.h"

void a_dump(struct a *a) {
    uint8_t *a8 = (uint8_t*)a;
    printf("adu: ");
    for (int j=0;j<(ntohs(a->l)+6);j++) {
        printf("0x%02x |",a8[j]);
    }
    printf("\n");
    fflush(stdout);
}
