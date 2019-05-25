#ifndef INCLUDE_BITSTRING_H
#define INCLUDE_BITSTRING_H

#include <string>
#include <cstdint>

class bitstring {
    private:
        size_t _length;  // length in bits of the bit string
        size_t _ndata;   // size of data array;
        size_t *_data;   // bits are packed in an array.

    public:
        // Creates a bitstring of the specified length (in bits) with all bits
        // initially set to zero (off).
        bitstring(size_t length) noexcept(false);
        bitstring(const bitstring & other);

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
        // Both Bitstring may have different length the bit with index start+lenght must
        // be valid on both or SwapRange will panic.
        static void swap_ranges(bitstring& bs1, bitstring& bs2, size_t start, size_t length);

        // Writes a 0 and 1 string representation of the bitstring in s.
        // The behaviour is undefined is the length of s is lower than the 
        // number of bits of the bitstring + 1 (terminal zero).
        void string(char * s) const;

        std::string string() const;

        // Prints the bitstring memory layout in hexadecimal.
        void print_hex() const;

    private:
        void _bit_must_exist(size_t i) const;

        // swaps range of bits from one word to another.
        // w is the index of the word containing the bits to swap, and m is a mask that specifies
        // whilch bits of that word will be swapped.
        static void _swap_bits(bitstring &x, bitstring &y, size_t w, size_t mask);
};

#endif //INCLUDE_BITSTRING_H