#ifndef BITOPS_H_INCLUDED
#define BITOPS_H_INCLUDED

#include <type_traits>
#include "binaryinteger.h"

template<typename T>
struct Bitops
{
    static T bit(int i)
    {
        return T(1) << i;
    }

    static T mask_below(int i)
    {
        return (T(1) << i) - T(1);
    }

    static int countr_zero(const T& x)
    {
        return std::countr_zero(x); // works for built-in unsigned
    }
};

// Specialization for BinaryInteger
template<>
struct Bitops<BinaryInteger>
{
    static BinaryInteger bit(int i)
    {
        BinaryInteger b;
        b.set_bit(i);
        return b;
    }

    static BinaryInteger mask_below(int i)
    {
        BinaryInteger b;
        for (int j = 0; j < i; ++j) {
            b.set_bit(j);
        }
        return b;
    }

    static int countr_zero(const BinaryInteger& x)
    {
        return x.countr_zero();
    }
};

#endif // BITOPS_H_INCLUDED