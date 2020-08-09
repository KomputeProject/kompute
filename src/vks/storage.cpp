//
//  Created by Bradley Austin Davis on 2016/02/17
//  Copyright 2013-2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "storage.hpp"
#include <string>
#include <cstring>
#include <istream>
#include <fstream>
#include <iterator>


#if defined(WIN32)
#include <Windows.h>
#endif

namespace vks { namespace storage {

#if defined(__ANDROID__)
AAssetManager* assetManager = nullptr;
void setAssetManager(AAssetManager* assetManager) {
    vks::storage::assetManager = assetManager;
}
#endif

class ViewStorage : public Storage {
public:
    ViewStorage(const StoragePointer& owner, size_t size, const uint8_t* data)
        : _owner(owner)
        , _size(size)
        , _data(data) {}
    const uint8_t* data() const override { return _data; }
    size_t size() const override { return _size; }
    bool isFast() const override { return _owner->isFast(); }

private:
    const StoragePointer _owner;
    const size_t _size;
    const uint8_t* _data;
};

StoragePointer Storage::createView(size_t viewSize, size_t offset) const {
    auto selfSize = size();
    if (0 == viewSize) {
        viewSize = selfSize;
    }
    if ((viewSize + offset) > selfSize) {
        return StoragePointer();
        //TODO: Disable te exception for now and return an empty storage instead.
        //throw std::runtime_error("Invalid mapping range");
    }
    return std::make_shared<ViewStorage>(shared_from_this(), viewSize, data() + offset);
}

class MemoryStorage : public Storage {
public:
    MemoryStorage(size_t size, const uint8_t* data = nullptr) {
        _data.resize(size);
        if (data) {
            memcpy(_data.data(), data, size);
        }
    }
    const uint8_t* data() const override { return _data.data(); }
    size_t size() const override { return _data.size(); }
    bool isFast() const override { return true; }

private:
    std::vector<uint8_t> _data;
};

#if defined(__ANDROID__) || defined(WIN32)
#define MAPPED_FILES 1
#else
#define MAPPED_FILES 0
#endif

#if MAPPED_FILES

class FileStorage : public Storage {
public:
    static StoragePointer create(const std::string& filename, size_t size, const uint8_t* data);
    FileStorage(const std::string& filename);
    ~FileStorage();
    // Prevent copying
    FileStorage(const FileStorage& other) = delete;
    FileStorage& operator=(const FileStorage& other) = delete;

    const uint8_t* data() const override { return _mapped; }
    size_t size() const override { return _size; }
    bool isFast() const override { return false; }

private:
    size_t _size{ 0 };
    uint8_t* _mapped{ nullptr };
#if defined(__ANDROID__)
    AAsset* _asset{ nullptr };
#elif (WIN32)
    HANDLE _file{ INVALID_HANDLE_VALUE };
    HANDLE _mapFile{ INVALID_HANDLE_VALUE };
#else
    std::vector<uint8_t> _data;
#endif
};

FileStorage::FileStorage(const std::string& filename) {
#if defined(__ANDROID__)
    // Load shader from compressed asset
    _asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_BUFFER);
    assert(_asset);
    _size = AAsset_getLength(_asset);
    assert(_size > 0);
    _mapped = (uint8_t*)(AAsset_getBuffer(_asset));
#elif (WIN32)
    _file = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (_file == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open file");
    }
    {
        DWORD dwFileSizeHigh;
        _size = GetFileSize(_file, &dwFileSizeHigh);
        _size += (((size_t)dwFileSizeHigh) << 32);
    }
    _mapFile = CreateFileMappingA(_file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (_mapFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to create mapping");
    }
    _mapped = (uint8_t*)MapViewOfFile(_mapFile, FILE_MAP_READ, 0, 0, 0);
#endif
}

FileStorage::~FileStorage() {
#if defined(__ANDROID__)
    AAsset_close(_asset);
#elif (WIN32)
    UnmapViewOfFile(_mapped);
    CloseHandle(_mapFile);
    CloseHandle(_file);
#endif
}

#endif

StoragePointer Storage::create(size_t size, uint8_t* data) {
    return std::make_shared<MemoryStorage>(size, data);
}
StoragePointer Storage::readFile(const std::string& filename) {
#if MAPPED_FILES
    return std::make_shared<FileStorage>(filename);
#else
    // FIXME move to posix memory mapped files
    // open the file:
    std::ifstream file(filename, std::ios::binary);
    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);


	std::vector<uint8_t> fileData;
    // reserve capacity
    fileData.reserve(fileSize);
    // read the data:
    fileData.insert(fileData.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
    file.close();
	return std::make_shared<MemoryStorage>(fileData.size(), fileData.data());
#endif
}

}}  // namespace vks::storage
