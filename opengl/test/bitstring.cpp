#include "catch.hpp"
#include "bitstring.hpp"
#include <string>
#include <climits>
#include <iostream>

template<typename T>
void print_bin(const T& t){
	size_t nBytes=sizeof(T);
	char* rawPtr((char*)(&t));
	for(size_t byte=0; byte<nBytes; byte++) {
		for(size_t bit=0; bit<CHAR_BIT; bit++) {
			std::cout << (((rawPtr[byte])>>bit)&1);
		}
	}
	std::cout << std::endl;
};

TEST_CASE("set/clear/flip individual bits", "[bitstring]") {
    bitstring bs(65);
	for (size_t i = 0; i < 65; ++i) {
		bs.setbit(i);
		REQUIRE(bs.bit(i));
		bs.clearbit(i);
		REQUIRE_FALSE(bs.bit(i));
		bs.flipbit(i);
		REQUIRE(bs.bit(i));
	}
}

TEST_CASE("create bitstring from string made of 0s and 1s", "[bitstring]") {
	std::string s("1101010111010110001010010100010111101010111010110001010010100010111010101110101100010100101000101111010101110101100010100101000101110101011101011000101001010001011110101011101011000101001010001011101010111010110001010010100010111101010111010110001010010100010111010101110101100010100101000101111010101110101100010100101000101");
	bitstring bs(s.c_str());

	for (int i = 0; i < s.length(); i++) {
		char c = s[s.length()-1-i];
		if (c == '0') {
			REQUIRE_FALSE(bs.bit(i));
		} else if (c == '1') {
			REQUIRE(bs.bit(i));
		} else {
			INFO("char " << i << " is " << c);
			FAIL(c);
		}
	}
}

TEST_CASE("fails when bitstring from string made of 0s and 1s", "[bitstring]") {
	REQUIRE_THROWS(bitstring("101b"));
}

TEST_CASE("return string made of 0s and 1s from bitstring", "[bitstring]") {
	std::string s("1101010111010110001010010100010111101010111010110001010010100010111010101110101100010100101000101111010101110101100010100101000101110101011101011000101001010001011110101011101011000101001010001011101010111010110001010010100010111101010111010110001010010100010111010101110101100010100101000101111010101110101100010100101000101");
	bitstring bs(s.c_str());
	char buf[s.length()];
	bs.string(buf);
	REQUIRE(std::string(buf) == s);
}

TEST_CASE("count ones (i.e set bits) in a bitstring", "[bitstring]") {
	bitstring bs(64);
	REQUIRE(bs.count_ones() == 0);

	// Chosen bits deal with boundary cases.
	bs.setbit(0);
	bs.setbit(31);
	bs.setbit(32);
	bs.setbit(33);
	bs.setbit(63);
	REQUIRE(bs.count_ones() == 5);
}

TEST_CASE("count zeroes (i.e unset bits) in a bitstring", "[bitstring]") {
	bitstring bs(12);
	REQUIRE(bs.count_zeroes() == 12);

	bs.setbit(0);
	bs.setbit(5);
	bs.setbit(6);
	bs.setbit(9);
	bs.setbit(10);
	REQUIRE(bs.count_zeroes() == 7);
}

TEST_CASE("swap ranges from 2 bitstrings", "[bitstring]") {
	struct {
		std::string x, y;
		size_t start, length;
		std::string wantx, wanty;
	} testcase;

	std::array<decltype(testcase), 15> tests = {{
		{
			x:     "1",
			y:     "0",
			start: 0, length: 1,
			wantx: "0",
			wanty: "1",
		},
		{
			x:     "1111111111111111111111111111111111111111111111111111111111111111",
			y:     "0000000000000000000000000000000000000000000000000000000000000000",
			start: 0, length: 32,
			wantx: "1111111111111111111111111111111100000000000000000000000000000000",
			wanty: "0000000000000000000000000000000011111111111111111111111111111111",
		},
		{
			x:     "1111111111111111111111111111111111111111",
			y:     "0000000000000000000000000000000000000000",
			start: 2, length: 30,
			wantx: "1111111100000000000000000000000000000011",
			wanty: "0000000011111111111111111111111111111100",
		},
		{
			x:     "1111111111",
			y:     "0000000000",
			start: 0, length: 3,
			wantx: "1111111000",
			wanty: "0000000111",
		},
		{
			x:     "111",
			y:     "000",
			start: 1, length: 2,
			wantx: "001",
			wanty: "110",
		},
		{
			x:     "111",
			y:     "000",
			start: 0, length: 3,
			wantx: "000",
			wanty: "111",
		},
		{
			x:     "11111111111111111111111111111111",
			y:     "00000000000000000000000000000000",
			start: 0, length: 32,
			wantx: "00000000000000000000000000000000",
			wanty: "11111111111111111111111111111111",
		},
		{
			x:     "111111111111111111111111111111111",
			y:     "000000000000000000000000000000000",
			start: 0, length: 33,
			wantx: "000000000000000000000000000000000",
			wanty: "111111111111111111111111111111111",
		},
		{
			x:     "111111111111111111111111111111111111111111111111111111111111111",
			y:     "000000000000000000000000000000000000000000000000000000000000000",
			start: 0, length: 63,
			wantx: "000000000000000000000000000000000000000000000000000000000000000",
			wanty: "111111111111111111111111111111111111111111111111111111111111111",
		},
		{
			x:     "1111111111111111111111111111111111111111111111111111111111111111",
			y:     "0000000000000000000000000000000000000000000000000000000000000000",
			start: 0, length: 64,
			wantx: "0000000000000000000000000000000000000000000000000000000000000000",
			wanty: "1111111111111111111111111111111111111111111111111111111111111111",
		},
		{
			x:     "11111111111111111111111111111111111111111111111111111111111111111",
			y:     "00000000000000000000000000000000000000000000000000000000000000000",
			start: 0, length: 65,
			wantx: "00000000000000000000000000000000000000000000000000000000000000000",
			wanty: "11111111111111111111111111111111111111111111111111111111111111111",
		},
		{
			x:     "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
			y:     "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
			start: 94, length: 1,
			wantx: "1101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
			wanty: "0010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
		},
		{
			x:     "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
			y:     "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
			start: 1, length: 256,
			wantx: "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
			wanty: "011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110",
		},
		{
			x:     "1111111111111111111111111111111111111111111111111111111111111111111",
			y:     "0000000000000000000000000000000000000000000000000000000000000000000",
			start: 64, length: 2,
			wantx: "1001111111111111111111111111111111111111111111111111111111111111111",
			wanty: "0110000000000000000000000000000000000000000000000000000000000000000",
		},
		{
			x:     "1111111111111111111111111111111111111111111111111111111111111111111",
			y:     "0000000000000000000000000000000000000000000000000000000000000000000",
			start: 65, length: 1,
			wantx: "1011111111111111111111111111111111111111111111111111111111111111111",
			wanty: "0100000000000000000000000000000000000000000000000000000000000000000",
		},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		INFO("\n<test case>\n"
		   <<"\nstart:" << tt->start << "\nlength:" << tt->length
		   <<"\n    x:" << tt->x <<     "\n    y:" << tt->y
		   <<"\nwantx:" << tt->wantx << "\nwanty:" << tt->wanty);
		bitstring x(tt->x.c_str());
		bitstring y(tt->y.c_str());

		bitstring::swap_ranges(x, y, tt->start, tt->length);
		REQUIRE(tt->wantx == x.string());
		REQUIRE(tt->wanty == y.string());
	}
}

TEST_CASE("convert bits to uint8_t", "[bitstring]") {
	struct {
		std::string input;
		size_t i;
		uint8_t want;
	} testcase;

	std::array<decltype(testcase), 9> tests = {{
		// LSB and MSB are both on the same word
		{input: "00000000000000000000000000000001",
			i: 0, want: 1},
		{input: "00000000000000000000000000000010",
			i: 0, want: 2},
		{input: "000000000000000001000010",
			i: 0, want: uint8_t(1<<6) + 2},
		{input: "111111111111111101000010",
			i: 0, want: uint8_t(1<<6) + 2},
		{input: "0000000000000000000000000000000111111111111111111111111111111111",
			i: 32, want: 1},
		{input: "00000000000000001000000111111111111111111111111111111111",
			i: 32, want: uint8_t(1<<7) + 1},
		{input: "100000000",
			i: 1, want: uint8_t(1 << 7)},

		// LSB and MSB are on separate words
		{input: "11111111111111111111111010000101111111111111111111111111111111",
			i: 31, want: uint8_t(1<<6) + 2},
		{input: "00000000000000000000000111111100000000000000000000000000000000",
			i: 31, want: std::numeric_limits<uint8_t>::max() - 1},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		uint8_t got = bs.uint8(tt->i);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\ni:" << tt->i
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert bits to uint16_t", "[bitstring]") {
	struct {
		std::string input;
		size_t i;
		uint16_t want;
	} testcase;

	std::array<decltype(testcase), 9> tests = {{
		// LSB and MSB are both on the same word
		{input: "00000000000000000000000000000001",
			i: 0, want: 1},
		{input: "00000000000000000000000000000010",
			i: 0, want: 2},
		{input: "00000000000000000100000000000010",
			i: 0, want: uint16_t(1<<14) + 2},
		{input: "11111111111111110100000000000010",
			i: 0, want: uint16_t(1<<14) + 2},
		{input: "0000000000000000000000000000000111111111111111111111111111111111",
			i: 32, want: 1},
		{input: "0000000000000000100000000000000111111111111111111111111111111111",
			i: 32, want: uint16_t(1<<15) + 1},
		{input: "10000000000000000",
			i: 1, want: uint16_t(1<<15)},

		// LSB and MSB are on 2 separate words
		{input: "111111111111111111111110100000000000010111111111111111111111111",
			i: 24, want: uint16_t(1<<14) + 2},
		{input: "000000000000000000000001111111111111110000000000000000000000000",
			i: 24, want: std::numeric_limits<uint16_t>::max() - 1},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		uint16_t got = bs.uint16(tt->i);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\ni:" << tt->i
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert bits to uint32_t", "[bitstring]") {
	struct {
		std::string input;
		size_t i;
		uint32_t want;
	} testcase;

	std::array<decltype(testcase), 9> tests = {{
		// LSB and MSB are both on the same word
		{input: "00000000000000000000000000000001",
			i: 0, want: 1},
		{input: "00000000000000000000000000000010",
			i: 0, want: 2},
		{input: "01000000000000000000000000000010",
			i: 0, want: uint32_t(1<<30) + 2},
		{input: "1111111111111111111111111111111101000000000000000000000000000010",
			i: 0, want: uint32_t(1<<30) + 2},
		{input: "0000000000000000000000000000000111111111111111111111111111111111",
			i: 32, want: 1},
		{input: "1000000000000000000000000000000111111111111111111111111111111111",
			i: 32, want: uint32_t(1<<31) + 1},

		// LSB and MSB are on 2 separate words
		{input: "100000000000000000000000000000000",
			i: 1, want: uint32_t(1 << 31)},
		{input: "1111111111111111111101000000000000000000000000000010111111111111",
			i: 12, want: uint32_t(1<<30) + 2},
		{input: "0000111111111111111111111111111111100000000000000000000000000000",
			i: 28, want: std::numeric_limits<uint32_t>::max() - 1},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		uint32_t got = bs.uint32(tt->i);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\ni:" << tt->i
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert bits to uint64_t", "[bitstring]") {
	struct {
		std::string input;
		size_t i;
		uint64_t want;
	} testcase;

	std::array<decltype(testcase), 9> tests = {{
		// LSB and MSB are both on the same word
		{input: "0000000000000000000000000000000000000000000000000000000000000001",
			i: 0, want: 1},
		{input: "0000000000000000000000000000000000000000000000000000000000000010",
			i: 0, want: 2},
		{input: "0100000000000000000000000000000000000000000000000000000000000010",
			i: 0, want: (uint64_t(1)<<62) + 2},
		{input: "11111111111111111111111111111111111111111111111111111111111111110100000000000000000000000000000000000000000000000000000000000010",
			i: 0, want: (uint64_t(1)<<62) + 2},
		{input: "00000000000000000000000000000000000000000000000000000000000000011111111111111111111111111111111111111111111111111111111111111111",
			i: 64, want: 1},
		{input: "10000000000000000000000000000000000000000000000000000000000000011111111111111111111111111111111111111111111111111111111111111111",
			i: 64, want: (uint64_t(1)<<63) + 1},

		// LSB and MSB are on 2 separate words
		{input: "10000000000000000000000000000000000000000000000000000000000000000",
			i: 1, want: uint64_t(1) << 63},
		{input: "1111111111111111111111111110100000000000000000000000000000000000000000000000000000000000010111111111111111111111111111111111111111111111111111111111111",
			i: 60, want: (uint64_t(1)<<62) + 2},
		{input: "000011111111111111111111111111111111111111111111111111111111111111100000000000000000000000000000000000000000000000000000000000",
			i: 58, want: std::numeric_limits<uint64_t>::max() - 1},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		uint64_t got = bs.uint64(tt->i);
		CHECK(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\ni:" << tt->i
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert bits with uintn", "[bitstring]") {
	struct {
		std::string input;
		size_t nbits, i;
		size_t want;
	} testcase;

	std::array<decltype(testcase), 12> tests = {{
		// LSB and MSB are both on the same word
		{input: "10",
			nbits: 1, i: 0, want: 0},
		{input: "111",
			nbits: 1, i: 0, want: 1},
		{input: "101",
			nbits: 1, i: 1, want: 0},
		{input: "010",
			nbits: 1, i: 1, want: 1},
		{input: "100",
			nbits: 2, i: 0, want: 0},
		{input: "1101",
			nbits: 2, i: 1, want: 2},
		{input: "10100000000000000000000000000000",
			nbits: 3, i: 29, want: 5},
		{input: "10000000000000000000000000000000",
			nbits: 1, i: 31, want: 1},

		// // LSB and MSB are on 2 separate words
		{input: "1111111111111111111111111111111111111111111111111111111111111111",
			nbits: 3, i: 31, want: 7},
		{input: "1111111111111111111111111111111111111111111111111111111111111111",
			nbits: 3, i: 30, want: 7},
		{input: "0000000000000000000000000000001010000000000000000000000000000000",
			nbits: 3, i: 31, want: 5},
		{input: "0000000000000000000000000000000101000000000000000000000000000000",
			nbits: 3, i: 30, want: 5},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		size_t got = bs.uintn(tt->i, tt->nbits);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\ni:" << tt->i << " nbits:" << tt->nbits
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert bits to int8_t", "[bitstring]") {
	struct {
		std::string input;
		size_t i;
		int8_t want;
	} testcase;

	std::array<decltype(testcase), 6> tests = {{
		// LSB and MSB are both on the same word
		{input: "11111111",
			i: 0, want: -1},
		{input: "01111111",
			i: 0, want: std::numeric_limits<int8_t>::max()},
		{input: "10000000",
			i: 0, want: std::numeric_limits<int8_t>::min()},
		// LSB and MSB are on 2 separate words
		{input: "111111110000000000000000000000000000000",
			i: 31, want: -1},
		{input: "011111110000000000000000000000000000000",
			i: 31, want: std::numeric_limits<int8_t>::max()},
		{input: "100000001111111111111111111111111111111",
			i: 31, want: std::numeric_limits<int8_t>::min()},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		int8_t got = bs.int8(tt->i);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\ni:" << tt->i
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert bits to int16_t", "[bitstring]") {
	struct {
		std::string input;
		size_t i;
		int16_t want;
	} testcase;

	std::array<decltype(testcase), 6> tests = {{
		// LSB and MSB are both on the same word
		{input: "1111111111111111",
			i: 0, want: -1},
		{input: "0111111111111111",
			i: 0, want: std::numeric_limits<int16_t>::max()},
		{input: "1000000000000000",
			i: 0, want: std::numeric_limits<int16_t>::min()},
		// LSB and MSB are on 2 separate words
		{input: "11111111111111110000000000000000000000000000000",
			i: 31, want: -1},
		{input: "01111111111111110000000000000000000000000000000",
			i: 31, want: std::numeric_limits<int16_t>::max()},
		{input: "10000000000000001111111111111111111111111111111",
			i: 31, want: std::numeric_limits<int16_t>::min()},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		int16_t got = bs.int16(tt->i);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\ni:" << tt->i
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert bits to int32_t", "[bitstring]") {
	struct {
		std::string input;
		size_t i;
		int32_t want;
	} testcase;

	std::array<decltype(testcase), 6> tests = {{
		// LSB and MSB are both on the same word
		{input: "11111111111111111111111111111111",
			i: 0, want: -1},
		{input: "01111111111111111111111111111111",
			i: 0, want: std::numeric_limits<int32_t>::max()},
		{input: "10000000000000000000000000000000",
			i: 0, want: std::numeric_limits<int32_t>::min()},
		// LSB and MSB are on 2 separate words
		{input: "111111111111111111111111111111110000000000000000000000000000000",
			i: 31, want: -1},
		{input: "011111111111111111111111111111110000000000000000000000000000000",
			i: 31, want: std::numeric_limits<int32_t>::max()},
		{input: "100000000000000000000000000000001111111111111111111111111111111",
			i: 31, want: std::numeric_limits<int32_t>::min()},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		int32_t got = bs.int32(tt->i);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\ni:" << tt->i
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert uint8_t to bits", "[bitstring]") {
	struct {
		std::string bs;
		uint8_t x;
		size_t i;
		std::string want;
	} testcase;

	std::array<decltype(testcase), 9> tests = {{
		// LSB and MSB are both on the same word
		{
			bs: "1111111111111111",
			x:  2, i: 0,
			want: "1111111100000010"},
		{
			bs: "1111111111111111",
			x:  2, i: 8,
			want: "0000001011111111"},
		{
			bs: "11111111111111111111111111111111",
			x:  2, i: 16,
			want: "11111111000000101111111111111111"},
		{
			bs: "11111111111111111111111111111111",
			x:  2, i: 24,
			want: "00000010111111111111111111111111"},
		{
			bs: "11111111111111111111111111111111",
			x:  2, i: 22,
			want: "11000000101111111111111111111111"},
		// LSB and MSB are on separate words
		{
			bs: "111111111111111111111111111111111111111111111111111111111111111111111",
			x:  2, i: 61,
			want: "000000101111111111111111111111111111111111111111111111111111111111111"},
		{
			bs: "11111111111111111111111111111111111111111111111111111111111111111111111",
			x:  15, i: 63,
			want: "00001111111111111111111111111111111111111111111111111111111111111111111"},
		{
			bs: "0000000000000000000000000000000000000000000000000000000000000000000",
			x:  std::numeric_limits<uint8_t>::max(), i: 59,
			want: "1111111100000000000000000000000000000000000000000000000000000000000"},
		{
			bs: "0011101010101010101010101010101010101010101010101010101010101010101010101010101010",
			x:  0xaa, i: 63,
			want: "0011101010110101010010101010101010101010101010101010101010101010101010101010101010"},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
			bitstring bs(tt->bs.c_str());
			bs.set_uint8(tt->i, tt->x);
			REQUIRE(tt->want == bs.string());
			INFO("\n<test case>\n"
			<<"\ni:" << tt->i << " x:" << tt->x
			<<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert uint16_t to bits", "[bitstring]") {
	struct {
		std::string bs;
		uint16_t x;
		size_t i;
		std::string want;
	} testcase;

	std::array<decltype(testcase), 9> tests = {{
		// LSB and MSB are both on the same word
		{
			bs: "1111111111111111",
			x:  2, i: 0,
			want: "0000000000000010"},
		{
			bs: "111111111111111111111111",
			x:  2, i: 8,
			want: "000000000000001011111111"},
		{
			bs: "11111111111111111111111111111111",
			x:  2, i: 16,
			want: "00000000000000101111111111111111"},
		{
			bs: "1111111111111111111111111111111111111111",
			x:  2, i: 24,
			want: "0000000000000010111111111111111111111111"},
		{
			bs: "1111111111111111111111111111111111111111",
			x:  2, i: 22,
			want: "1100000000000000101111111111111111111111"},
		// LSB and MSB are on separate words
		{
			bs: "11111111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  2, i: 61,
			want: "00000000000000101111111111111111111111111111111111111111111111111111111111111"},
		{
			bs: "1111111111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  15, i: 63,
			want: "0000000000001111111111111111111111111111111111111111111111111111111111111111111"},
		{
			bs: "000000000000000000000000000000000000000000000000000000000000000000000000000",
			x:  std::numeric_limits<uint16_t>::max(), i: 59,
			want: "111111111111111100000000000000000000000000000000000000000000000000000000000"},
		{
			bs: "001110101010101010101010101010101010101010101010101010101010101010101010101010101010101010",
			x:  0xaaaa, i: 63,
			want: "001110101011010101010101010010101010101010101010101010101010101010101010101010101010101010"},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
			bitstring bs(tt->bs.c_str());
			bs.set_uint16(tt->i, tt->x);
			REQUIRE(tt->want == bs.string());
			INFO("\n<test case>\n"
			<<"\ni:" << tt->i << " x:" << tt->x
			<<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert uint32_t to bits", "[bitstring]") {
	struct {
		std::string bs;
		uint32_t x;
		size_t i;
		std::string want;
	} testcase;

	std::array<decltype(testcase), 9> tests = {{
		// LSB and MSB are both on the same word
		{
			bs: "11111111111111111111111111111111",
			x:  2, i: 0,
			want: "00000000000000000000000000000010"},
		{
			bs: "1111111111111111111111111111111111111111",
			x:  2, i: 8,
			want: "0000000000000000000000000000001011111111"},
		{
			bs: "111111111111111111111111111111111111111111111111",
			x:  2, i: 16,
			want: "000000000000000000000000000000101111111111111111"},
		{
			bs: "11111111111111111111111111111111111111111111111111111111",
			x:  2, i: 24,
			want: "00000000000000000000000000000010111111111111111111111111"},
		{
			bs: "11111111111111111111111111111111111111111111111111111111",
			x:  2, i: 22,
			want: "11000000000000000000000000000000101111111111111111111111"},
		// LSB and MSB are on separate words
		{
			bs: "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  2, i: 61,
			want: "000000000000000000000000000000101111111111111111111111111111111111111111111111111111111111111"},
		{
			bs: "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  15, i: 63,
			want: "00000000000000000000000000001111111111111111111111111111111111111111111111111111111111111111111"},
		{
			bs: "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
			x:  std::numeric_limits<uint32_t>::max(), i: 59,
			want: "1111111111111111111111111111111100000000000000000000000000000000000000000000000000000000000"},
		{
			bs: "0011101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010",
			x:  0xaaaaaaaa, i: 63,
			want: "0011101010110101010101010101010101010101010010101010101010101010101010101010101010101010101010101010101010"},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
			bitstring bs(tt->bs.c_str());
			bs.set_uint32(tt->i, tt->x);
			REQUIRE(tt->want == bs.string());
			INFO("\n<test case>\n"
			<<"\ni:" << tt->i << " x:" << tt->x
			<<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert uint64_t to bits", "[bitstring]") {
	struct {
		std::string bs;
		uint64_t x;
		size_t i;
		std::string want;
	} testcase;

	std::array<decltype(testcase), 9> tests = {{
		// LSB and MSB are both on the same word
		{
			bs: "1111111111111111111111111111111111111111111111111111111111111111",
			x:  2, i: 0,
			want: "0000000000000000000000000000000000000000000000000000000000000010"},
		// LSB and MSB are on separate words
		{
			bs: "111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  2, i: 8,
			want: "000000000000000000000000000000000000000000000000000000000000001011111111"},
		{
			bs: "11111111111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  2, i: 16,
			want: "00000000000000000000000000000000000000000000000000000000000000101111111111111111"},
		{
			bs: "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  2, i: 24,
			want: "0000000000000000000000000000000000000000000000000000000000000010111111111111111111111111"},
		{
			bs: "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  2, i: 22,
			want: "1100000000000000000000000000000000000000000000000000000000000000101111111111111111111111"},
		{
			bs: "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  2, i: 61,
			want: "00000000000000000000000000000000000000000000000000000000000000101111111111111111111111111111111111111111111111111111111111111"},
		{
			bs: "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
			x:  15, i: 63,
			want: "0000000000000000000000000000000000000000000000000000000000001111111111111111111111111111111111111111111111111111111111111111111"},
		{
			bs: "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
			x:  std::numeric_limits<uint64_t>::max(), i: 59,
			want: "111111111111111111111111111111111111111111111111111111111111111100000000000000000000000000000000000000000000000000000000000"},
		{
			bs: "001110101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010",
			x:  0xaaaaaaaaaaaaaaaa, i: 63,
			want: "001110101011010101010101010101010101010101010101010101010101010101010101010010101010101010101010101010101010101010101010101010101010101010"},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
			bitstring bs(tt->bs.c_str());
			bs.set_uint64(tt->i, tt->x);
			REQUIRE(tt->want == bs.string());
			INFO("\n<test case>\n"
			<<"\ni:" << tt->i << " x:" << tt->x
			<<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert uintn to bits", "[bitstring]") {
	struct {
		std::string bs;
		size_t x;
		size_t nbits, i;
		std::string want;
	} testcase;

	std::array<decltype(testcase), 10> tests = {{
		{
			bs:    "000",
			x:2, nbits: 2, i: 0,
			want: "010"},
		{
			bs:    "000",
			x: 2, nbits: 2, i: 1,
			want: "100"},
		{
			bs:    "1111",
			x: 2, nbits: 2, i: 1,
			want: "1101"},
		{
			bs:    "00000000000000000000000000000000",
			x: 1, nbits: 1, i: 19,
			want: "00000000000010000000000000000000"},
		{
			bs:    "00000000000000000000000000000000",
			x: 3, nbits: 1, i: 19,
			want: "00000000000010000000000000000000"},
		{
			bs:    "00000000011110000000000000000000",
			x: 8, nbits: 4, i: 19,
			want: "00000000010000000000000000000000"},
		{
			bs:    "0101010101010101010101010101010101010101",
			x: 0x80000002, nbits: 32, i: 4,
			want: "0101100000000000000000000000000000100101"},
		// LSB and MSB are on separate words
		{
			bs:    "0000000000000000000000000000000000000000000000000000000000000000000",
			x: 3, nbits: 2, i: 63,
			want: "0011000000000000000000000000000000000000000000000000000000000000000"},
		{
			bs:    "00000000000000000000000000000000000000000000000000000000000000000000",
			x: 9, nbits: 4, i: 63,
			want: "01001000000000000000000000000000000000000000000000000000000000000000"},
		{
			bs:    "000000000000000000001101000011010011000001010011010101010101000100101000111101010100000000000000000000000000000000000",
			x: 0x9cfbeb71ee3fcf5f, nbits: 64, i: 35,
			want: "000000000000000000100111001111101111101011011100011110111000111111110011110101111100000000000000000000000000000000000"},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
			bitstring bs(tt->bs.c_str());
			bs.set_uintn(tt->i, tt->nbits, tt->x);
			REQUIRE(bitstring(tt->want.c_str()) == bs);
			INFO("\n<test case>\n"
			<<"\ni:" << tt->i << " nbits:" << tt->nbits << " x:" << tt->x
			<<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert gray-coded bits to uint8_t", "[bitstring]") {
	struct {
		std::string input;
		uint8_t want;
	} testcase;

	std::array<decltype(testcase), 3> tests = {{
		{input: "00000000",
			want: 0},
		{input: "00000111",
			want: 5},
		{input: "10000000",
			want: std::numeric_limits<uint8_t>::max()},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		uint8_t got = bs.gray8(0);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert gray-coded bits to uint16_t", "[bitstring]") {
	struct {
		std::string input;
		uint16_t want;
	} testcase;

	std::array<decltype(testcase), 3> tests = {{
		{input: "0000000000000000",
			want: 0},
		{input: "0000000000000111",
			want: 5},
		{input: "1000000000000000",
			want: std::numeric_limits<uint16_t>::max()},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		uint16_t got = bs.gray16(0);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert gray-coded bits to uint32_t", "[bitstring]") {
	struct {
		std::string input;
		uint32_t want;
	} testcase;

	std::array<decltype(testcase), 3> tests = {{
		{input: "00000000000000000000000000000000",
			want: 0},
		{input: "00000000000000000000000000000111",
			want: 5},
		{input: "10000000000000000000000000000000",
			want: std::numeric_limits<uint32_t>::max()},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		uint32_t got = bs.gray32(0);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\nwant:" << tt->want);
	}
}

TEST_CASE("convert gray-coded bits to uint64_t", "[bitstring]") {
	struct {
		std::string input;
		uint64_t want;
	} testcase;

	std::array<decltype(testcase), 3> tests = {{
		{input: "0000000000000000000000000000000000000000000000000000000000000000",
			want: 0},
		{input: "0000000000000000000000000000000000000000000000000000000000000111",
			want: 5},
		{input: "1000000000000000000000000000000000000000000000000000000000000000",
			want: std::numeric_limits<uint64_t>::max()},
	}};

	for (auto tt = tests.begin(); tt < tests.end(); ++tt) {
		bitstring bs(tt->input.c_str());
		uint64_t got = bs.gray64(0);
		REQUIRE(tt->want == got);
		INFO("\n<test case>\n"
		   <<"\ninput:" << tt->input
		   <<"\nwant:" << tt->want);
	}
}
