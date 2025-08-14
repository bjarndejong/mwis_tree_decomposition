#ifndef STORE_ON_FILE_H_INCLUDED
#define STORE_ON_FILE_H_INCLUDED

#include <vector>
#include <string>

template<typename T>
void compress_to_file(const std::vector<T>& v, const std::string& file_name);

template<typename T>
std::vector<T> decompress_from_file(const std::string& filename);

#include "store_on_file.tpp"

#endif // STORE_ON_FILE_INCLUDED