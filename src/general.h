#ifndef GENERAL_H_INCLUDED
#define GENERAL_H_INCLUDED

#include <vector>
#include <stdint.h>
#include <stdio.h>

//std::vector<int> bitmask_filter(const unsigned int bitmask, const std::vector<int>& v);

void print_vector(const std::vector<int>& v);

void print_vector(const std::vector<unsigned int>& v);


int countr_zero(__uint128_t x);

void compress_c_to_file(const int& current, const std::vector<int>& v);
void decompress_c_from_file_and_add(const int& current, std::vector<int>& v);
void remove_c_file(const int& current);

void compress_p_to_file(const int& current, const int& neighbourposition, const std::vector<int>& v);
void decompress_p_from_file(const int& current, const int& neighbourposition, std::vector<int>& v);

void remove_p_file(const int& current, const int& neighbourposition);



#endif // GENERAL_H_INCLUDED
