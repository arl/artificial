#include "bitstring.hpp"
#include "bitops.h"

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <stdexcept>

bitstring::bitstring(size_t length) {
	this->_length = length;
	this->_ndata = (this->_length + maxbits - 1) / maxbits;
	this->_data = new size_t[this->_ndata];
	memset(&this->_data[0], 0, sizeof(size_t) * this->_ndata);
}

bitstring::bitstring(const bitstring & other) {
	this->_length = other._length;
	this->_ndata = other._ndata;
	this->_data = new size_t[this->_ndata];
	memcpy(this->_data, other._data, sizeof(size_t) * this->_ndata);
}

bitstring::bitstring(const char * s) : bitstring(strlen(s)) {
	for (size_t i = 0; i < this->_length; ++i){
		switch (s[i]) {
		case '0':
			break;
		case '1':
			this->setbit(this->_length - i - 1);
			break;
		default:
			throw(std::invalid_argument("illegal character"));
		}
	}
}

bitstring::~bitstring() {
	delete(this->_data);
	this->_data = nullptr;
	this->_length = 0;
	this->_ndata = 0;
}

size_t bitstring::length() const {
	return this->_length;
}

void bitstring::string(char * s) const {
	for (size_t i = 0; i < this->_length; ++i)
		s[this->_length-1-i] = this->bit(i) ? '1' : '0';
	s[this->_length] = '\0';
}

std::string bitstring::string() const {
	char buf[this->_length];
	this->string(buf);
	return std::string(buf);
}

// Crashes if i is not a valid bit index for bs, that is if i is
// greater than bs.length.
void bitstring::_bit_must_exist(size_t i) const {
	#ifdef DEBUG
	if (i >= this->_length) {
		throw std::invalid_argument("bitstring index out of range");
	}
	#endif
}

void bitstring::print_hex() const {
	for (size_t i = 0; i < this->_ndata; ++i) {
		std::cout << std::hex << this->_data[i];
	}
	std::cout << std::endl;
}

void bitstring::setbit(size_t i) {
	this->_bit_must_exist(i);

	size_t w = _wordoffset(i);
	size_t off = _bitoffset(i);
	this->_data[w] |= _bitmask(off);
}

bool bitstring::bit(size_t i) const {
	this->_bit_must_exist(i);

	size_t w = _wordoffset(i);
	size_t off = _bitoffset(i);
	size_t mask = _bitmask(off);
	return (this->_data[w] & mask) != 0;
}

void bitstring::clearbit(size_t i) {
	this->_bit_must_exist(i);

	size_t w = _wordoffset(i);
	size_t off = _bitoffset(i);
	this->_data[w] &= ~_bitmask(off);
}

void bitstring::flipbit(uint64_t i) {
	this->_bit_must_exist(i);

	uint64_t w = _wordoffset(i);
	uint64_t off = _bitoffset(i);
	this->_data[w] ^= _bitmask(off);
}

size_t bitstring::count_ones() const {
	size_t n = 0;
	for (size_t i = 0; i < this->_ndata; ++i) {
		size_t x = this->_data[i];
		while (x != 0) {
			x &= (x - 1);	// unset the LSB
			++n;      		// count how many time we need to unset a bit before zeroing x.
		}
	}
	return n;
}

size_t bitstring::count_zeroes() const {
	return this->_length - this->count_ones();
}

void bitstring::swap_ranges(bitstring& bs1, bitstring& bs2, size_t start, size_t length) {
	bs1._bit_must_exist(start + length - 1);
	bs2._bit_must_exist(start + length - 1);

	// swap the required bits of the first word
	size_t i = start / maxbits;
	start = _bitoffset(start);
	size_t end = std::min(start+length, maxbits);
	size_t remain = length - (end - start);
	bitstring::_swap_bits(bs1, bs2, i, _genmask(start, end));
	i++;

	// swap whole words but the last one
	while (remain > maxbits) {
		std::swap(bs1._data[i], bs2._data[i]);
		remain -= maxbits;
		++i;
	}

	// swap the remaining bits of the last word
	if (remain != 0) {
		bitstring::_swap_bits(bs1, bs2, i, _genlomask(remain));
	}
}

// swapBits swaps range of bits from one word to another.
// w is the index of the word containing the bits to swap, and m is a mask that specifies
// whilch bits of that word will be swapped.
void bitstring::_swap_bits(bitstring &x, bitstring &y, size_t w, size_t mask) {
	size_t keep = ~mask;
	size_t xkeep = x._data[w]&keep;
	size_t ykeep = y._data[w]&keep;
	size_t xswap = x._data[w]&mask;
	size_t yswap = y._data[w]&mask;
	x._data[w] = xkeep | yswap;
	y._data[w] = ykeep | xswap;
}

uint8_t bitstring::uint8(size_t i) const {
	this->_bit_must_exist(i + 7);

	size_t off = _bitoffset(i);
	size_t loword = this->_data[_wordoffset(i)] >> off;
	size_t hiword = this->_data[_wordoffset(i+7)] & ((1 << off) - 1);
	return uint8_t(loword | hiword<<(maxbits-off));
}

uint16_t bitstring::uint16(size_t i) const  {
	this->_bit_must_exist(i + 15);

	size_t off = _bitoffset(i);
	size_t loword = this->_data[_wordoffset(i)] >> off;
	size_t hiword = this->_data[_wordoffset(i+15)] & ((1 << off) - 1);
	return uint16_t(loword | hiword<<(maxbits-off));
}

uint32_t bitstring::uint32(size_t i) const  {
	this->_bit_must_exist(i + 31);

	size_t off = _bitoffset(i);
	size_t loword = this->_data[_wordoffset(i)] >> off;
	size_t hiword = this->_data[_wordoffset(i+31)] & ((1 << off) - 1);
	return uint32_t(loword | hiword<<(maxbits-off));
}

uint64_t bitstring::uint64(size_t i) const  {
	this->_bit_must_exist(i + 63);

	// fast path: i is a multiple of 64
	if ((i&((1<<6)-1)) == 0) {
		return uint64_t(this->_data[i>>6]);
	}

	size_t w = _wordoffset(i);
	size_t off = _bitoffset(i);
	size_t loword = this->_data[w] >> off;
	size_t hiword = this->_data[w+1] & ((1UL << off) - 1);
	return uint64_t(loword | (hiword<<(maxbits-off)));
}

size_t bitstring::uintn(size_t i, size_t n) const {
	#ifdef DEBUG
	if ((n > maxbits) || (n == 0)) {		
		throw std::invalid_argument("uintn supports unsigned integers from 1 to 'word size' bits long");
	}
	#endif //DEBUG
	this->_bit_must_exist(i + n - 1);

	size_t j = _wordoffset(i);
	size_t k = _wordoffset(i + n - 1);
	size_t looff = _bitoffset(i);
	size_t loword = this->_data[j];
	if (j == k) {
		// fast path: same word
		return (loword >> looff) & _genlomask(n);
	}
	size_t hiword = this->_data[k] & _genlomask(_bitoffset(i + n));
	loword = _genhimask(looff) & loword >> looff;
	return loword | hiword<<(maxbits-looff);
}

ssize_t bitstring::intn(size_t i, size_t n) const {
	return ssize_t(this->uintn(i, n));
}

int8_t bitstring::int8(size_t i) const {
	return int8_t(this->uint8(i));
}

int16_t bitstring::int16(size_t i) const {
	return int16_t(this->uint16(i));
}

int32_t bitstring::int32(size_t i) const {
	return int32_t(this->uint32(i));
}

int64_t bitstring::int64(size_t i) const {
	return int64_t(this->uint64(i));
}

