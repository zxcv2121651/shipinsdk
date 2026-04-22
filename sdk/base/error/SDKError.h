#pragma once

#include <cstdint>

namespace sdk {
namespace base {

/**
 * @brief Strictly typed error codes to ensure exact failure identification.
 */
enum class SDKError : int32_t {
    OK = 0,
    ERR_UNKNOWN = -1,
    ERR_INVALID_PARAM = -2,
    ERR_INVALID_STATE = -3,
    ERR_OUT_OF_MEMORY = -4,
    ERR_QUEUE_FULL = -5,
    ERR_QUEUE_EMPTY = -6,
    ERR_TIMEOUT = -7
};

} // namespace base
} // namespace sdk
