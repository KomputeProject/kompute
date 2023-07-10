// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <vector>

class Buffer
{
  public:
    Buffer(void* data, size_t length, size_t element_size)
      : data_(data)
      , length_(length)
      , element_size_(element_size)
      , buff_end_(static_cast<char*>(data_) + length_ * element_size_)
    {}

    size_t element_size() const { return element_size_; }

    size_t size() const { return length_; }

    const void* begin() const {
        return data_;
    }

    const void* end() const
    { 
        return buff_end_;
    }

    const std::vector<unsigned char> to_vector() const
    {
        return std::vector<unsigned char>(static_cast<unsigned char*>(data_),
                                          static_cast<unsigned char*>(buff_end_));
    }

    template<typename T>
    static const Buffer from_vector(std::vector<T> vec)
    {
        return Buffer(vec.data(), vec.size(), sizeof(T));
    }

  private:
    void* data_;
    size_t length_;
    size_t element_size_;
    void* buff_end_;
};