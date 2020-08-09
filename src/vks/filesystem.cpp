#include "filesystem.hpp"

#include <cstring>
#include <fstream>
#include <istream>
#include <iterator>
#include <functional>

#include "storage.hpp"

namespace vks { namespace file {

void withBinaryFileContents(const std::string& filename, std::function<void(size_t size, const void* data)> handler) {
    withBinaryFileContents(filename, [&](const char* filename, size_t size, const void* data) { handler(size, data); });
}

void withBinaryFileContents(const std::string& filename, std::function<void(const char* filename, size_t size, const void* data)> handler) {
    auto storage = storage::Storage::readFile(filename);
    handler(filename.c_str(), storage->size(), storage->data());
}

std::vector<uint8_t> readBinaryFile(const std::string& filename) {
    std::vector<uint8_t> result;
    withBinaryFileContents(filename, [&](size_t size, const void* data) {
        result.resize(size);
        memcpy(result.data(), data, size);
    });
    return result;
}

std::string readTextFile(const std::string& fileName) {
    std::string fileContent;
    std::ifstream fileStream(fileName, std::ios::in);

    if (!fileStream.is_open()) {
        throw std::runtime_error("File " + fileName + " not found");
    }
    std::string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        fileContent.append(line + "\n");
    }
    fileStream.close();
    return fileContent;
}

}}  // namespace vks::file
