// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

#include "shaders/Utils.hpp"

TEST(TestImageSampler, NoSamplerByDefault)
{
    kp::Manager mgr;

    std::shared_ptr<kp::ImageT<float>> image = mgr.image({ 0, 1, 2, 3 }, 2, 2, 1);

    EXPECT_FALSE(image->hasSampler());
    EXPECT_EQ(image->getDescriptorType(), vk::DescriptorType::eStorageImage);
}

TEST(TestImageSampler, CreateAndDestroySampler)
{
    kp::Manager mgr;

    std::shared_ptr<kp::ImageT<float>> image = mgr.image({ 0, 1, 2, 3 }, 2, 2, 1);

    image->createSampler();

    EXPECT_TRUE(image->hasSampler());
    EXPECT_EQ(image->getDescriptorType(),
              vk::DescriptorType::eCombinedImageSampler);

    // Calling createSampler() again should replace the existing sampler
    // rather than leaking or throwing.
    vk::SamplerCreateInfo nearestSamplerInfo =
      kp::Image::defaultSamplerCreateInfo();
    nearestSamplerInfo.magFilter = vk::Filter::eNearest;
    nearestSamplerInfo.minFilter = vk::Filter::eNearest;
    image->createSampler(nearestSamplerInfo);

    EXPECT_TRUE(image->hasSampler());
    EXPECT_EQ(image->getDescriptorType(),
              vk::DescriptorType::eCombinedImageSampler);

    image->destroy();

    EXPECT_FALSE(image->hasSampler());
    EXPECT_EQ(image->getDescriptorType(), vk::DescriptorType::eStorageImage);
    EXPECT_FALSE(image->isInit());
}

TEST(TestImageSampler, DefaultSamplerCreateInfo)
{
    vk::SamplerCreateInfo samplerInfo = kp::Image::defaultSamplerCreateInfo();

    EXPECT_EQ(samplerInfo.magFilter, vk::Filter::eLinear);
    EXPECT_EQ(samplerInfo.minFilter, vk::Filter::eLinear);
    EXPECT_EQ(samplerInfo.addressModeU, vk::SamplerAddressMode::eClampToEdge);
    EXPECT_EQ(samplerInfo.addressModeV, vk::SamplerAddressMode::eClampToEdge);
    EXPECT_EQ(samplerInfo.addressModeW, vk::SamplerAddressMode::eClampToEdge);
    EXPECT_EQ(samplerInfo.anisotropyEnable, VK_FALSE);
}

TEST(TestImageSampler, SampleWithBilinearFiltering)
{
    kp::Manager mgr;

    // A 2x1 texture with a left-to-right gradient from 0.0 to 1.0.
    std::shared_ptr<kp::ImageT<float>> inputImage =
      mgr.image({ 0.0, 1.0 }, 2, 1, 1);
    inputImage->createSampler();

    // A 4x1 output that samples across the input via the combined image
    // sampler, exercising the GPU's bilinear filter and clamp-to-edge
    // addressing.
    std::shared_ptr<kp::ImageT<float>> outputImage =
      mgr.image({ 0.0, 0.0, 0.0, 0.0 }, 4, 1, 1);

    const std::vector<std::shared_ptr<kp::Memory>> params = { inputImage,
                                                               outputImage };

    std::string shader = (R"(
        #version 450

        layout (local_size_x = 1, local_size_y = 1) in;

        layout(binding = 0) uniform sampler2D inputImg;
        layout(binding = 1, r32f) writeonly uniform image2D outputImg;

        void main() {
            ivec2 outCoord = ivec2(gl_GlobalInvocationID.xy);
            ivec2 outSize = imageSize(outputImg);
            vec2 uv = (vec2(outCoord) + 0.5) / vec2(outSize);
            float value = texture(inputImg, uv).r;
            imageStore(outputImg, outCoord, vec4(value, 0.0, 0.0, 0.0));
        }
    )");

    std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(
      params, compileSource(shader), kp::Workgroup{ 4, 1, 1 });

    mgr.sequence()
      ->eval<kp::OpSyncDevice>(params)
      ->eval<kp::OpAlgoDispatch>(algo)
      ->eval<kp::OpSyncLocal>({ outputImage });

    // Sample UVs land at 0.125, 0.375, 0.625, 0.875. With texel centers at
    // 0.25 (value 0.0) and 0.75 (value 1.0) and clamp-to-edge addressing,
    // the expected bilinearly filtered values are 0.0, 0.25, 0.75, 1.0.
    const std::vector<float> result = outputImage->vector();
    ASSERT_EQ(result.size(), 4u);
    EXPECT_NEAR(result[0], 0.0, 1e-2);
    EXPECT_NEAR(result[1], 0.25, 1e-2);
    EXPECT_NEAR(result[2], 0.75, 1e-2);
    EXPECT_NEAR(result[3], 1.0, 1e-2);
}

TEST(TestImageSampler, MixedDescriptorTypesInSameAlgorithm)
{
    kp::Manager mgr;

    // A 1x1 sampled image acting as a "uniform" value.
    std::shared_ptr<kp::ImageT<float>> sampledImage = mgr.image({ 10.0 }, 1, 1, 1);
    sampledImage->createSampler();

    // A plain 2x1 storage image.
    std::shared_ptr<kp::ImageT<float>> storageImage =
      mgr.image({ 1.0, 2.0 }, 2, 1, 1);

    // A tensor (storage buffer) output.
    std::shared_ptr<kp::TensorT<float>> tensorOut =
      mgr.tensor({ 0.0, 0.0 });

    const std::vector<std::shared_ptr<kp::Memory>> params = { sampledImage,
                                                               storageImage,
                                                               tensorOut };

    std::string shader = (R"(
        #version 450

        layout (local_size_x = 1) in;

        layout(binding = 0) uniform sampler2D sampledImg;
        layout(binding = 1, r32f) uniform image2D storageImg;
        layout(binding = 2) buffer bufOut { float o[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            float sampledVal = texture(sampledImg, vec2(0.5, 0.5)).r;
            float storageVal = imageLoad(storageImg, ivec2(int(index), 0)).r;
            o[index] = sampledVal + storageVal;
        }
    )");

    std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(
      params, compileSource(shader), kp::Workgroup{ 2, 1, 1 });

    EXPECT_TRUE(algo->isInit());

    mgr.sequence()
      ->eval<kp::OpSyncDevice>(params)
      ->eval<kp::OpAlgoDispatch>(algo)
      ->eval<kp::OpSyncLocal>({ tensorOut });

    EXPECT_EQ(tensorOut->vector(), (std::vector<float>{ 11.0, 12.0 }));
}
