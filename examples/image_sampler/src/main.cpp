#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include <kompute/Kompute.hpp>
#include <shader/upsample.hpp>

int main()
{
    kp::Manager mgr;

    // A tiny 4x4 single-channel "image" (a coarse 2x2 checkerboard).
    const uint32_t inWidth = 4;
    const uint32_t inHeight = 4;
    // clang-format off
    std::vector<float> inputData = {
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f,
    };
    // clang-format on

    std::shared_ptr<kp::ImageT<float>> inputImage =
      mgr.image(inputData, inWidth, inHeight, 1);

    // Attaching a sampler switches this image's descriptor type from a
    // storage image (`image2D`, raw imageLoad/imageStore) to a combined
    // image sampler (`sampler2D`, hardware-filtered `texture()` reads).
    // createSampler() with no arguments uses Kompute's default sampler
    // (linear filtering, clamp-to-edge addressing).
    inputImage->createSampler();

    // A larger 16x16 output image that we fill by *sampling* (not just
    // copying) the small input image, letting the GPU's bilinear filter
    // smoothly interpolate between input texels.
    const uint32_t outWidth = 16;
    const uint32_t outHeight = 16;
    std::vector<float> outputData(outWidth * outHeight, 0.0f);
    std::shared_ptr<kp::ImageT<float>> outputImage =
      mgr.image(outputData, outWidth, outHeight, 1);

    const std::vector<std::shared_ptr<kp::Memory>> params = { inputImage,
                                                               outputImage };

    const std::vector<uint32_t> shader = std::vector<uint32_t>(
      shader::UPSAMPLE_COMP_SPV.begin(), shader::UPSAMPLE_COMP_SPV.end());

    std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(
      params, shader, kp::Workgroup{ outWidth, outHeight, 1 });

    mgr.sequence()
      ->record<kp::OpSyncDevice>(params)
      ->record<kp::OpAlgoDispatch>(algo)
      ->record<kp::OpSyncLocal>(params)
      ->eval();

    std::cout << "Upsampled output (" << outWidth << "x" << outHeight
              << ") sampled from a " << inWidth << "x" << inHeight
              << " source image:\n\n";

    const std::vector<float> result = outputImage->vector();
    for (uint32_t y = 0; y < outHeight; y++) {
        for (uint32_t x = 0; x < outWidth; x++) {
            std::cout << std::fixed << std::setprecision(2)
                      << result[y * outWidth + x] << " ";
        }
        std::cout << "\n";
    }
}
