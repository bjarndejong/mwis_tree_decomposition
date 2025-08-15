#ifndef GENERAL_H_INCLUDED
#define GENERAL_H_INCLUDED

#include <vector>
#include <stdint.h>
#include <stdio.h>

//std::vector<int> bitmask_filter(const unsigned int bitmask, const std::vector<int>& v);

void print_vector(const std::vector<int>& v);

void print_vector(const std::vector<unsigned int>& v);

void print_vector(const std::vector<unsigned long long>& v);

int countr_zero(__uint128_t x);

#endif // GENERAL_H_INCLUDED
