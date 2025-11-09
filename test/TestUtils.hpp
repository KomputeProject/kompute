// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "kompute/Core.hpp"
#include <memory>

namespace TestUtils {

/**
 * Utility function to convert raw pointer to shared_ptr with no-op deleter
 * for compatibility during Phase 6 migration. Raw pointer lifetime is managed
 * by Manager, so we use a no-op deleter to avoid double-deletion.
 *
 * @param ptr Raw pointer to Memory object
 * @return shared_ptr with no-op deleter for Sequence API compatibility
 */
template<typename T>
std::shared_ptr<T> toSharedPtr(T* ptr) {
    return std::shared_ptr<T>(ptr, [](T*){});
}

} // namespace TestUtils