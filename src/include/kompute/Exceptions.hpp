// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <exception>
#include <string>
#include <vulkan/vulkan.hpp>

namespace kp {

/**
 * Exception thrown when the GPU device is lost due to a GPU reset,
 * timeout, or other driver-level failure.
 *
 * When VK_ERROR_DEVICE_LOST is returned from a Vulkan call (vkQueueSubmit,
 * vkWaitForFences, vkDeviceWaitIdle, etc.), Kompute throws this exception.
 * The Manager and Sequence objects associated with the lost device are no
 * longer usable and must be destroyed.
 *
 * Common causes:
 * - GPU TDR (Timeout Detection and Recovery) due to long-running compute
 * - GPU memory allocation failure during dispatch
 * - Shader infinite loop or hardware stall
 * - Thermal shutdown or unstable overclocking
 * - Out-of-memory condition during kernel execution
 */
class DeviceLostError : public std::exception {
  public:
    /**
     * Construct a DeviceLostError with a context message.
     *
     * @param message Human-readable description of what was happening when
     *                the device was lost (e.g., "during Sequence::eval()")
     */
    explicit DeviceLostError(const std::string& message)
      : mMessage("GPU device lost: " + message)
    {
    }

    /**
     * Return the error message.
     */
    const char* what() const noexcept override { return mMessage.c_str(); }

  private:
    std::string mMessage;
};

} // namespace kp
