#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace vks { namespace file {

void withBinaryFileContents(const std::string& filename, std::function<void(const char* filename, size_t size, const void* data)> handler);

void withBinaryFileContents(const std::string& filename, std::function<void(size_t size, const void* data)> handler);

std::string readTextFile(const std::string& fileName);

}}  // namespace vks::file
