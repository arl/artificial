#include <stdio.h>
#include <string.h>
#include "sht.h"
#include "art.h"

struct art_ctx * gctx;

int32_t sht_init(uint32_t w, uint32_t h) {
    int32_t rc = art_create_ctx(&gctx, w, h);
    if (rc != EARTOK) {
        printf("Error %d: can't create art context\n", rc);
        return rc;
    }
    return EARTOK;
}

int32_t sht_render_image(unsigned char * img_data, uint32_t * bitstring) {
    printf("we are in sht_render_image\n");
    return EARTOK;
}

int32_t sht_cleanup() {
    int32_t rc = art_destroy_context(gctx);
    if (rc != EARTOK) {
        printf("Error %d: can't destroy art context\n", rc);
        return rc;
    }
    return EARTOK;
}