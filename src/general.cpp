#include "general.h"

#include <vector>
#include <iostream>
#include <zstd.h>
#include <cassert>
#include <fstream>
#include <string>
#include <cstring>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

void remove_c_file(const int& current)
{
    string filename = "c_" + to_string(current) + "_.zstd";
    fs::remove(filename);  // Succeeds if file exists, does nothing if not
}

/*
vector<int> bitmask_filter(const unsigned int bitmask, const vector<int>& v)
{
    vector<int> filtered_vector;
    for(int i = 0; i<v.size(); i++)
        if(bitmask & (1<<i))
            filtered_vector.push_back(v[i]);
    return filtered_vector;
}
    */

void print_vector(const std::vector<int>& v)
{
    for(vector<int>::const_iterator it = v.begin(); it != v.end(); it++)
        cout << *it << ' ';
    cout << endl;
}

void print_vector(const std::vector<unsigned int>& v)
{
    for(vector<unsigned int>::const_iterator it = v.begin(); it != v.end(); it++)
        cout << *it << ' ';
    cout << endl;
}

int countr_zero(__uint128_t x) {
    if (x == 0) return 128;

    uint64_t low = (uint64_t)x;
    if (low != 0) {
        return __builtin_ctzll(low);
    } else {
        uint64_t high = (uint64_t)(x >> 64);
        return 64 + __builtin_ctzll(high);
    }
}

void compress_c_to_file(const int& current, const vector<int>& v)
{
    string filename("c_"+to_string(current)+"_.zstd");

    // Raw binary pointer and size
    const void* input_data = static_cast<const void*>(v.data());
    size_t input_size = v.size() * sizeof(int);

    // Estimate maximum compressed size
    size_t max_compressed_size = ZSTD_compressBound(input_size);
    vector<char> compressed_data(max_compressed_size);

    // Compress the data into a single frame
    size_t compressed_size = ZSTD_compress(compressed_data.data(), max_compressed_size,
                                           input_data, input_size, 1); // compression level 1

    assert(!ZSTD_isError(compressed_size)); // Ensure compression succeeded

    // Write the compressed data (single frame) to file
    ofstream out_file(filename, ios::binary);
    out_file.write(compressed_data.data(), compressed_size);
}

// Decompress the Zstd file and return the vector of integers
void decompress_c_from_file_and_add(const int& current, vector<int>& v)
{
    string filename = "c_" + to_string(current) + "_.zstd";

  // Open the compressed file
    ifstream in_file(filename, ios::binary);
    if (!in_file) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }

    // Read the compressed data
    in_file.seekg(0, ios::end);
    size_t compressed_size = in_file.tellg();
    in_file.seekg(0, ios::beg);

    vector<char> compressed_data(compressed_size);
    in_file.read(compressed_data.data(), compressed_size);

    // Prepare buffer for decompression
    size_t decompressed_size = v.size() * sizeof(int);
    vector<char> decompressed_data(decompressed_size);

    size_t actual_size = ZSTD_decompress(
        decompressed_data.data(), decompressed_size,
        compressed_data.data(), compressed_size);

    if (ZSTD_isError(actual_size)) {
        cerr << "Decompression failed: " << ZSTD_getErrorName(actual_size) << endl;
        return;
    }

    if (actual_size != decompressed_size) {
        cerr << "Warning: Expected " << decompressed_size
             << " bytes, got " << actual_size << " bytes." << endl;
    }

    // Add decompressed values to v
    const int* decompressed_ints = reinterpret_cast<const int*>(decompressed_data.data());
    for (size_t i = 0; i < v.size(); ++i) {
        v[i] += decompressed_ints[i];
    }
}

void compress_p_to_file(const int& current, const int& neighbourposition, const vector<int>& v)
{
    string filename("p_"+to_string(current)+"_"+to_string(neighbourposition)+"_.zstd");

    // Raw binary pointer and size
    const void* input_data = static_cast<const void*>(v.data());
    size_t input_size = v.size() * sizeof(int);

    // Estimate maximum compressed size
    size_t max_compressed_size = ZSTD_compressBound(input_size);
    vector<char> compressed_data(max_compressed_size);

    // Compress the data into a single frame
    size_t compressed_size = ZSTD_compress(compressed_data.data(), max_compressed_size,
                                           input_data, input_size, 1); // compression level 1

    assert(!ZSTD_isError(compressed_size)); // Ensure compression succeeded

    // Write the compressed data (single frame) to file
    ofstream out_file(filename, ios::binary);
    out_file.write(compressed_data.data(), compressed_size);
}

void decompress_p_from_file(const int& current, const int& neighbourposition,vector<int>& v)
{
    string filename = "p_"+to_string(current)+"_"+to_string(neighbourposition)+"_.zstd";

  // Open the compressed file
    ifstream in_file(filename, ios::binary);
    if (!in_file) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }

    // Read the compressed data
    in_file.seekg(0, ios::end);
    size_t compressed_size = in_file.tellg();
    in_file.seekg(0, ios::beg);

    vector<char> compressed_data(compressed_size);
    in_file.read(compressed_data.data(), compressed_size);

    // Prepare buffer for decompression
    size_t decompressed_size = v.size() * sizeof(int);
    vector<char> decompressed_data(decompressed_size);

    size_t actual_size = ZSTD_decompress(
        decompressed_data.data(), decompressed_size,
        compressed_data.data(), compressed_size);

    if (ZSTD_isError(actual_size)) {
        cerr << "Decompression failed: " << ZSTD_getErrorName(actual_size) << endl;
        return;
    }

    if (actual_size != decompressed_size) {
        cerr << "Warning: Expected " << decompressed_size
             << " bytes, got " << actual_size << " bytes." << endl;
    }

    // Overwrite v directly with decompressed values
    const int* decompressed_ints = reinterpret_cast<const int*>(decompressed_data.data());
    std::memcpy(v.data(), decompressed_ints, decompressed_size);
}

void remove_p_file(const int& current, const int& neighbourposition)
{
    string filename = "p_" +to_string(current)+"_"+to_string(neighbourposition)+ "_.zstd";
    fs::remove(filename);  // Succeeds if file exists, does nothing if not
}