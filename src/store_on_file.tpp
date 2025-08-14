#include <vector>
#include <string>
#include <zstd.h>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <cstddef>
#include <zstd_errors.h>

using namespace std;

template<typename T>
std::vector<T> decompress_from_file(const string& filename)
{
    std::ifstream in_file(filename, std::ios::binary);
    if (!in_file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Read metadata
    size_t element_size = 0;
    size_t number_of_elements = 0;

    in_file.read(reinterpret_cast<char*>(&element_size), sizeof(size_t));
    in_file.read(reinterpret_cast<char*>(&number_of_elements), sizeof(size_t));

    if (element_size != sizeof(T)) {
        throw std::runtime_error("Element size mismatch when decompressing file: " + filename);
    }
    // Read the rest of the file (compressed data)
    in_file.seekg(0, std::ios::end);
    std::streampos file_end = in_file.tellg();

    // Position after reading metadata
    const size_t compressed_size = static_cast<size_t>(file_end - sizeof(size_t) * 2);

    in_file.seekg(sizeof(size_t) * 2, std::ios::beg);

    std::vector<char> compressed_data(compressed_size);
    in_file.read(compressed_data.data(), compressed_size);

    // Prepare buffer for decompressed data
    std::vector<T> decompressed_data(number_of_elements);

    size_t decompressed_size = number_of_elements * sizeof(T);
    size_t actual_decompressed_size = ZSTD_decompress(
        decompressed_data.data(), decompressed_size,
        compressed_data.data(), compressed_size);

    if (ZSTD_isError(actual_decompressed_size)) {
        throw std::runtime_error("Decompression failed: " + std::string(ZSTD_getErrorName(actual_decompressed_size)));
    }

    if (actual_decompressed_size != decompressed_size) {
        throw std::runtime_error("Decompressed size mismatch");
    }

    return std::move(decompressed_data);
}

template<typename T>
void compress_to_file(const std::vector<T>& v, const string& filename)
{
    // Raw meta data:
    const size_t element_size = sizeof(T);
    const size_t number_of_elements = v.size();

    std::ofstream out_file(filename, std::ios::binary);

    if (!out_file) throw std::runtime_error("Failed to open file: " + filename);


    out_file.write(reinterpret_cast<const char*>(&element_size), sizeof(size_t));
    out_file.write(reinterpret_cast<const char*>(&number_of_elements), sizeof(size_t));

    // Compressed frame:
    size_t max_compressed_size = ZSTD_compressBound(v.size() * sizeof(T));

    std::vector<char> compressed_data(max_compressed_size);

    size_t compressed_size = ZSTD_compress(
        compressed_data.data(), max_compressed_size,
        v.data(), v.size() * sizeof(T),
        1 // compression level
    );

    if (ZSTD_isError(compressed_size)) throw std::runtime_error("Compression failed: " + std::string(ZSTD_getErrorName(compressed_size)));

    out_file.write(compressed_data.data(), compressed_size);
}