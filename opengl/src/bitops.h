#ifndef INCLUDE_BITOPS_H
#define INCLUDE_BITOPS_H

#include <cstdio>
#include <cstdint>
#include <limits>

const size_t maxbits = (size_t)std::numeric_limits<size_t>::digits;
const size_t maxval = (size_t)std::numeric_limits<size_t>::max();

// Returns, for a given bit n of a bit string, the offset
// of the uint that contains bit n.
size_t _wordoffset(size_t n) {
	return n / maxbits;
}

// Returns, for a given bit n of a bit string, the offset of
// that bit with regards to the first bit of the uint that contains it.
size_t _bitoffset(size_t n) {
	return n & (maxbits - 1LU);
}

// bitmask returns a mask where only the nth bit of a uint is set.
size_t _bitmask(size_t n) {
	return 1LU << n;
}

// genlomask returns a mask to keep the n LSB (least significant bits).
// Undefined behaviour if n is greater than uintsize.
size_t _genlomask(size_t n) {
	return maxval >> (maxbits - n);
}

// genhimask returns a mask to keep the n MSB (most significant bits).
// Undefined behaviour if n is greater than uintsize.
size_t _genhimask(size_t n) {
	return maxval << n;
}

// genmask returns a mask that keeps the bits in the range [l, h)
// behaviour undefined if any argument is greater than the size of
// a machine word.
size_t _genmask(size_t l, size_t h) {
	return _genlomask(h) & _genhimask(l);
}

#endif //INCLUDE_BITOPS_H
