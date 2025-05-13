// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include "logger/Logger.hpp"
#include <memory>
#include <string>

namespace kp {

class Sampler
{
  public:
    Sampler(std::shared_ptr<vk::Device> device);

    /**
     * @brief Make Sampler uncopyable
     *
     */
    Sampler(const Sampler&) = delete;
    Sampler(const Sampler&&) = delete;
    Sampler& operator=(const Sampler&) = delete;
    Sampler& operator=(const Sampler&&) = delete;

    virtual ~Sampler();

    std::shared_ptr<vk::Sampler> getSampler() const {
        return mSampler;
    }

private:
    void createSampler();

protected:
    std::shared_ptr<vk::Device> mDevice;
    std::shared_ptr<vk::Sampler> mSampler;
};


} // End namespace kp
