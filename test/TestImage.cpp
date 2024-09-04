// SPDX-License-Identifier: Apache-2.0

#include "gtest/gtest.h"

#include "kompute/Kompute.hpp"
#include "kompute/logger/Logger.hpp"

TEST(TestImage, ConstructorData)
{
    kp::Manager mgr;
    std::vector<float> vec{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    std::shared_ptr<kp::ImageT<float>> image = mgr.image(vec, 3, 3, 1);
    EXPECT_EQ(image->size(), vec.size());
    EXPECT_EQ(image->dataTypeMemorySize(), sizeof(float));
    EXPECT_EQ(image->vector(), vec);
}

TEST(TestImage, ReserveData)
{
    kp::Manager mgr;

    std::shared_ptr<kp::Image> image =
      mgr.image(nullptr, 0, 3, 3, 1, kp::Memory::DataTypes::eFloat);
    EXPECT_EQ(image->size(), 9);
    EXPECT_EQ(image->dataTypeMemorySize(), sizeof(float));

    std::shared_ptr<kp::Image> image2 =
      mgr.image(3, 3, 1, kp::Memory::DataTypes::eFloat);
    EXPECT_EQ(image2->size(), 9);
    EXPECT_EQ(image2->dataTypeMemorySize(), sizeof(float));

    std::shared_ptr<kp::ImageT<float>> image3 = mgr.imageT<float>(3, 3, 1);
    EXPECT_EQ(image3->size(), 9);
    EXPECT_EQ(image3->dataTypeMemorySize(), sizeof(float));

    std::shared_ptr<kp::ImageT<float>> image4 = mgr.image(3, 3, 1);
    EXPECT_EQ(image4->size(), 9);
    EXPECT_EQ(image4->dataTypeMemorySize(), sizeof(float));
}
TEST(TestImage, DataTypes)
{
    kp::Manager mgr;
    const int width = 3;
    const int height = 3;

    for (int numChannels = 1; numChannels <= 4; numChannels++) {

        // 3-channel images are not supported and should throw an exception.
        // This is tested by a different test.

        // We don't support 3-channel images because they are not supported by
        // Metal or Mesa (llvmpipe) See comment here:
        // https://github.com/KomputeProject/kompute/pull/388#discussion_r1720959531
        if (numChannels == 3) {
            continue;
        }

        {
            std::vector<float> vec(width * height * numChannels);
            std::shared_ptr<kp::ImageT<float>> image =
              mgr.image(vec, width, height, numChannels);
            EXPECT_EQ(image->dataType(), kp::Memory::DataTypes::eFloat);
        }

        {
            std::vector<int32_t> vec(width * height * numChannels);
            std::shared_ptr<kp::ImageT<int32_t>> image =
              mgr.imageT(vec, width, height, numChannels);
            EXPECT_EQ(image->dataType(), kp::Memory::DataTypes::eInt);
        }

        {
            std::vector<uint32_t> vec(width * height * numChannels);
            std::shared_ptr<kp::ImageT<uint32_t>> image =
              mgr.imageT(vec, width, height, numChannels);
            EXPECT_EQ(image->dataType(), kp::Memory::DataTypes::eUnsignedInt);
        }

        {
            std::vector<int16_t> vec(width * height * numChannels);
            std::shared_ptr<kp::ImageT<int16_t>> image =
              mgr.imageT(vec, width, height, numChannels);
            EXPECT_EQ(image->dataType(), kp::Memory::DataTypes::eShort);
        }

        {
            std::vector<uint16_t> vec(width * height * numChannels);
            std::shared_ptr<kp::ImageT<uint16_t>> image =
              mgr.imageT(vec, width, height, numChannels);
            EXPECT_EQ(image->dataType(), kp::Memory::DataTypes::eUnsignedShort);
        }

        {
            std::vector<int8_t> vec(width * height * numChannels);
            std::shared_ptr<kp::ImageT<int8_t>> image =
              mgr.imageT(vec, width, height, numChannels);
            EXPECT_EQ(image->dataType(), kp::Memory::DataTypes::eChar);
        }

        {
            std::vector<uint8_t> vec(width * height * numChannels);
            std::shared_ptr<kp::ImageT<uint8_t>> image =
              mgr.imageT(vec, width, height, numChannels);
            EXPECT_EQ(image->dataType(), kp::Memory::DataTypes::eUnsignedChar);
        }
    }
}

TEST(TestImage, InvalidVectorSize)
{
    kp::Manager mgr;
    std::vector<float> vec{ 0, 1, 2, 3 };

    // The vector is too small to initialise all the data in the image.
    EXPECT_THROW(mgr.image(vec, 3, 3, 1), std::runtime_error);
}

TEST(TestImage, LargeVectorSize)
{
    kp::Manager mgr;
    std::vector<float> vec{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };

    // The same vector but only the first 4 elements
    std::vector<float> result(vec.begin(), vec.begin() + 4);

    // It's OK to initilaise an image from a vector which is larger than the
    // image.
    std::shared_ptr<kp::ImageT<float>> image = mgr.image(vec, 2, 2, 1);
    EXPECT_EQ(image->size(), 2 * 2 * 1);

    // The output should be equal to the result vector, not the input vector.
    EXPECT_EQ(image->vector(), result);
}

TEST(TestImage, InvalidNumberOfChannels)
{
    kp::Manager mgr;

    // Make a vector big enough to store our theoretical 3x3 image with 5
    // channels.
    std::vector<float> vec(3 * 3 * 5);

    // There should be 1,2 or 4 channels.
    EXPECT_THROW(mgr.image(vec, 3, 3, 0), std::runtime_error);
    EXPECT_THROW(mgr.image(vec, 3, 3, 3), std::runtime_error);
    EXPECT_THROW(mgr.image(vec, 3, 3, 5), std::runtime_error);
}

TEST(TestImage, Tiling)
{
    kp::Manager mgr;
    std::vector<float> vec{ 0, 1, 2, 3 };

    // Test explitly setting the tiling
    std::shared_ptr<kp::ImageT<float>> image =
      mgr.imageT<float>(vec, 2, 2, 1, vk::ImageTiling::eOptimal);

    // The output should be equal to the input vector
    EXPECT_EQ(image->vector(), vec);
}

TEST(TestImage, GetNumChannels)
{
    kp::Manager mgr;
    std::vector<float> vec{ 0, 1, 2, 3 };

    // Test explitly setting the tiling
    std::shared_ptr<kp::ImageT<float>> image =
      mgr.imageT<float>(vec, 2, 2, 1, vk::ImageTiling::eOptimal);

    // Check the number of channels matches
    EXPECT_EQ(image->getNumChannels(), 1);
}

TEST(TestImage, InvalidDataType)
{
    kp::Manager mgr;

    // Custom data types are not supported for images.
    EXPECT_THROW(mgr.image(3, 3, 1, kp::Memory::DataTypes::eCustom),
                 std::runtime_error);
    EXPECT_THROW(
      mgr.image(
        3, 3, 1, kp::Memory::DataTypes::eCustom, vk::ImageTiling::eOptimal),
      std::runtime_error);
}
