#include "binaryinteger.h"

using namespace std;

size_t BinaryInteger::number_of_blocks = 0;

void BinaryInteger::set_number_of_blocks(size_t number_of_bits)     // Static
{
    number_of_blocks = (number_of_bits + 63)/64;
}
    
BinaryInteger::BinaryInteger() 
    : blocks(number_of_blocks, 0)
{

}

BinaryInteger::BinaryInteger(uint64_t value)
    : blocks(number_of_blocks, 0)
{
    if (!blocks.empty()) {
        blocks[0] = value;
    }
}
   
void BinaryInteger::set_bit(int i)
{
    blocks[i/64] |= uint64_t(1) << (i % 64);
}
   
BinaryInteger& BinaryInteger::operator|=(const BinaryInteger& other)
{
    for(size_t i = 0; i < blocks.size(); ++i)
    {
        blocks[i] |= other.blocks[i];
    }
    return *this;
}
   
bool BinaryInteger::operator==(const BinaryInteger& other) const
{
    return blocks == other.blocks;
}
   
BinaryInteger BinaryInteger::operator|(const BinaryInteger& other) const
{
    BinaryInteger result = *this;
    result |= other;
    return result;
}

bool BinaryInteger::is_subset_of(const BinaryInteger& other) const
{
    for(size_t i = 0; i < blocks.size(); ++i)
    {
        if((blocks[i] | other.blocks[i]) != other.blocks[i])
            return false;
    }
    return true;
}

BinaryInteger& BinaryInteger::operator&=(const BinaryInteger& other)
{
    for (size_t i = 0; i < blocks.size(); ++i) {
        blocks[i] &= other.blocks[i];
    }
    return *this;
}

BinaryInteger BinaryInteger::operator&(const BinaryInteger& other) const
{
    BinaryInteger result = *this;
    result &= other;
    return result;
}

BinaryInteger BinaryInteger::operator~() const
{
    BinaryInteger result = *this;
    for (auto& block : result.blocks) {
        block = ~block;
    }
    return result;
}

BinaryInteger BinaryInteger::operator<<(int shift) const {
    BinaryInteger result;

    int block_shift = shift / 64;
    int bit_shift = shift % 64;

    if (block_shift >= number_of_blocks)
        return result;  // shifted out of range, all 0

    for (std::size_t i = 0; i + block_shift < number_of_blocks; ++i) {
        uint64_t lower = blocks[i] << bit_shift;
        uint64_t carry = 0;
        if (bit_shift != 0 && i + 1 < blocks.size()) {
            carry = blocks[i] >> (64 - bit_shift);
        }

        result.blocks[i + block_shift] |= lower;
        if (i + block_shift + 1 < number_of_blocks)
            result.blocks[i + block_shift + 1] |= carry;
    }

    return result;
}

int BinaryInteger::countr_zero() const
{
    for (std::size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i] != 0) {
            return std::countr_zero(blocks[i]) + i * 64;
        }
    }
    return blocks.size() * 64; // or -1 if you prefer an error signal
}

BinaryInteger::operator bool() const {
    for (auto block : blocks) {
        if (block != 0) return true;
    }
    return false;
}

BinaryInteger& BinaryInteger::operator>>=(int shift) {
    int blockShift = shift / 64;
    int bitShift = shift % 64;
    int n = static_cast<int>(blocks.size());

    // Shift whole blocks down
    for (int i = 0; i < n - blockShift; ++i) {
        blocks[i] = blocks[i + blockShift];
    }
    // Zero out the top blocks
    for (int i = n - blockShift; i < n; ++i) {
        blocks[i] = 0;
    }

    if (bitShift > 0) {
        uint64_t carry = 0;
        for (int i = n - 1; i >= 0; --i) {
            uint64_t new_carry = blocks[i] << (64 - bitShift);
            blocks[i] = (blocks[i] >> bitShift) | carry;
            carry = new_carry;
        }
    }
    return *this;
}

BinaryInteger BinaryInteger::operator>>(int shift) const {
    BinaryInteger result = *this;
    result >>= shift;
    return result;
}

bool BinaryInteger::operator<(const BinaryInteger& other) const {
    if (blocks.size() != other.blocks.size()) {
        return blocks.size() < other.blocks.size();
    }
    for (int i = static_cast<int>(blocks.size()) - 1; i >= 0; --i) {
        if (blocks[i] < other.blocks[i]) return true;
        if (blocks[i] > other.blocks[i]) return false;
    }
    return false; // they are equal
}