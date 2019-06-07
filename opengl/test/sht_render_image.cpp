#include "catch.hpp"
#include "sht.h"

TEST_CASE("can render a triangle bitstring into an image", "[shapify]") {
    int rc;
    const unsigned int w = 64, h = 64;

    // init
    rc = sht_init(w, h);
    REQUIRE(rc == EARTOK);

    // render

// === RUN   Test_draw/one_big_green_triangle
    size_t bits[2] = {0x8000800080000000, 0x800020000};
    size_t nbits = 100;
// === RUN   Test_draw/red_and_green_alpha_triangles
// 0x4000810080808080 0x80800020000 0x8200200004     // 168 bits

    // As the bitstring will be passed from Go to c, we need a way to create a bitstring from a c pointer to the data plus a length (number of bits)
    unsigned char img_data[w * h * 4];
    rc = sht_render_image(&img_data[0], &bits[0], nbits);
    REQUIRE(rc == EARTOK);

    // compare image data

    // cleanup
    rc = sht_cleanup();
    REQUIRE(rc == EARTOK);
    // free(img_data);
}
