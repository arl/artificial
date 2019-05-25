#include "catch.hpp"
#include "sht.h"

TEST_CASE("can render a triangle bitstring into an image", "[shapify]") {
    int rc;
    unsigned int w = 256, h = 256;

    // init
    rc = sht_init(w, h);
    REQUIRE(rc == EARTOK);

    // render
    unsigned char * img_data;
    uint32_t * bitstring;
    rc = sht_render_image(img_data, bitstring);
    REQUIRE(rc == EARTOK);

    // compare image data

    // cleanup
    rc = sht_cleanup();
    REQUIRE(rc == EARTOK);
    // free(img_data);
}