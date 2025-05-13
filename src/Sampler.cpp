// SPDX-License-Identifier: Apache-2.0

#include "kompute/Sampler.hpp"
#include <fmt/core.h>

namespace kp {

Sampler::Sampler(std::shared_ptr<vk::Device> device) :
    mDevice(device)
{
    createSampler();
}

Sampler::~Sampler()
{
    if(mSampler)
        mDevice->destroySampler(*mSampler);
}

void Sampler::createSampler() {
    vk::SamplerCreateInfo samplerInfo;

    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.anisotropyEnable = false;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = false;
    samplerInfo.compareEnable = false;
    samplerInfo.compareOp = vk::CompareOp::eNever;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

    // Create the sampler
    mSampler = std::make_shared<vk::Sampler>();

    auto result = mDevice->createSampler(&samplerInfo, NULL, mSampler.get());
    if (result != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create sampler: " + vk::to_string(result));

}

} // end namespace kp
