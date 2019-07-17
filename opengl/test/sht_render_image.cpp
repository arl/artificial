#include "catch.hpp"
#include "sht.h"

TEST_CASE("can render a triangle bitstring into an image", "[shapify]") {
	struct {
        std::string name;
		size_t wbits, hbits;
        size_t *bits;
		size_t nbits;
	} testcase;

	std::array<decltype(testcase), 2> tests = {{
		{
            name: "one big green triangle",
			wbits: 6, hbits: 6,
            bits: new size_t[2]{0x8000800080000000, 0x800020000},
            nbits: 100,
		},
		{
            name: "red and green alpha triangles",
			wbits: 6, hbits: 6,
            bits: new size_t[3]{0x4000810080808080, 0x80800020000, 0x8200200004},
            nbits: 168,
		},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		INFO("\n<test case>\n" <<"\name:" << tt->name);

        const unsigned int w = 1 << tt->wbits;
        const unsigned int h = 1 << tt->hbits;
        int rc;

        // init
        rc = sht_init(w, h);
        REQUIRE(rc == EARTOK);

        // As the bitstring will be passed from Go to c, we need a way to create a bitstring from a c pointer to the data plus a length (number of bits)
        unsigned char img_data[w * h * 4];
        #include <stdio.h>
        printf("bits[0]: 0x%lx\n", tt->bits[0]);
        printf("bits[1]: 0x%lx\n", tt->bits[1]);
        rc = sht_render_image(&img_data[0], &tt->bits[0], tt->nbits);
        REQUIRE(rc == EARTOK);

        // compare image data

        // cleanup
        rc = sht_cleanup();
        REQUIRE(rc == EARTOK);
        // free(img_data);
        delete[] tt->bits;
	}
}