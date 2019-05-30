#ifndef INCLUDE_BITSTRING_H
#define INCLUDE_BITSTRING_H

#include <string>
#include <cstdint>

#ifdef DEBUG
# define _bitcheck_ noexcept(false)
#else
# define _bitcheck_
#endif

class bitstring {
    private:
        size_t _length;  // length in bits of the bit string
        size_t _ndata;   // size of data array.
        size_t *_data;   // bits are packed in an array.
        bool _owned;     // wether we own _data.

    public:
        // Creates a bitstring of the specified length (in bits) with all bits
        // initially set to zero (off).
        bitstring(size_t length) noexcept(false);
        // Creates a bitstring from another one (copy);
        bitstring(const bitstring & other) noexcept(false);
        // Creates a bitstring from external memory. 
        // Undefined behaviour if data is not large enough to contain n bits.
        // Bits of data having an offset >= nbits will be zeroed out, so some 
        // methods will continue to function properly.
        bitstring(size_t *data, size_t nbits);
        // Creates a bitstring from a 0 and 1's string.
        bitstring(const char * s) noexcept(false);

        ~bitstring();

        // Returns the number of bits defining the bitstring.
        size_t length() const;

        // Sets the bit at index i.
        void setbit(size_t i);

        // Returns a boolean indicating wether the bit at index i is set.
        bool bit(size_t i) const;

        // Clears the bit at index i.
        void clearbit(size_t i);

        // Flips (i.e toggles) the bit at index i.
        void flipbit(size_t i);

        // Counts the number of set bits.
        size_t count_ones() const;

        // Counts the number of unset bits.
        size_t count_zeroes() const;

        // Swaps the same range of bits between 2 bitstrings.
        //
        // bs1 and bs2 may have different length but if the bit at offset index
        // 'start + lenght' must be valid on both bs1 and bs2, or it will throw
        // an expcetion (in debug build) or behaviour is undefined..
        static void swap_ranges(bitstring& bs1, bitstring& bs2, size_t start, size_t length) _bitcheck_;

        // Writes a 0 and 1 string representation of the bitstring in s.
        // The behaviour is undefined is the length of s is lower than the 
        // number of bits of the bitstring + 1 (terminal zero).
        void string(char * s) const;

        // Returns a 0 and 1 string representation of the bitstring.
        std::string string() const;

        // Prints the bitstring memory layout in hexadecimal.
        void print_hex() const;

        // Returns the unsigned value represented by the N bits starting at 
        // offset i.
        //
        // Throws an exception in debug builds if there aren't enough bits.
        uint8_t uint8(size_t i) const _bitcheck_;
        uint16_t uint16(size_t i) const _bitcheck_;
        uint32_t uint32(size_t i) const _bitcheck_;
        uint64_t uint64(size_t i) const _bitcheck_;

        // Returns the n bits unsigned integer value represented by the n bits
        // starting at offset i.
        //
        // Throws an exception in debug builds if there aren't enough bits or
        // if n is greater than the number of bits in size_t (machine word).
        size_t uintn(size_t i, size_t n) const _bitcheck_;

        // Returns the signed value represented by the n bits starting at 
        // offset i.
        //
        // Throws an exception in debug builds if there aren't enough bits.
        int8_t int8(size_t i) const _bitcheck_;
        int16_t int16(size_t i) const _bitcheck_;
        int32_t int32(size_t i) const _bitcheck_;
        int64_t int64(size_t i) const _bitcheck_;

        // Returns the n bits signed integer value represented by the n bits
        // starting at offset i.
        //
        // Throws an exception in debug builds if there aren't enough bits or
        // if n is greater than the number of bits in size_t (machine word).
        ssize_t intn(size_t i, size_t n) const _bitcheck_;

        // Returns the unsigned value represented by the n gray-coded bits
        // starting at offset i.
        //
        // Throws an exception in debug builds if there aren't enough bits.
        uint8_t gray8(size_t i) const _bitcheck_;
        uint16_t gray16(size_t i) const _bitcheck_;
        uint32_t gray32(size_t i) const _bitcheck_;
        uint64_t gray64(size_t i) const _bitcheck_;

        // Sets the bits starting at offset i to be the representation of the
        // unsigned value x.
        // Throws an exception in debug builds if there aren't enough bits.
        void set_uint8(size_t i, uint8_t x) _bitcheck_;
        void set_uint16(size_t i, uint16_t x) _bitcheck_;
        void set_uint32(size_t i, uint32_t x) _bitcheck_;
        void set_uint64(size_t i, uint64_t x) _bitcheck_;

        // Sets the n bits starting at offset i with n LSB of x.
        // Throws an exception in debug builds if there aren't enough bits.
        void set_uintn(size_t i, size_t n, size_t x) _bitcheck_;

        bool operator == (const bitstring & other) const;
        bool operator != (const bitstring & other) const;

    private:
        void _bit_must_exist(size_t i) const _bitcheck_;

        // swaps range of bits from one word to another.
        // w is the index of the word containing the bits to swap, and m is a mask that specifies
        // whilch bits of that word will be swapped.
        static void _swap_bits(bitstring &x, bitstring &y, size_t w, size_t mask);
};

#endif //INCLUDE_BITSTRING_H
