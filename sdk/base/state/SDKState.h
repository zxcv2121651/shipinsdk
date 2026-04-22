#pragma once

#include <cstdint>

namespace sdk {
namespace base {

/**
 * @brief Formalized state machine for component lifecycles.
 */
enum class SDKState : uint8_t {
    UNINITIALIZED = 0,
    READY = 1,
    RUNNING = 2,
    STOPPED = 3
};

} // namespace base
} // namespace sdk
