#pragma once

#include "../../core/rhi/IRHI.h"
#include <memory>

namespace sdk {
namespace render {
namespace rhi {

/**
 * @class RHIFactory
 * @brief Factory class to dynamically instantiate the best available RHI backend
 *        based on the compiled platform and runtime capabilities.
 */
class RHIFactory {
public:
    /**
     * @brief Creates and initializes the primary hardware-accelerated RHI context.
     *        Returns Metal on Apple platforms, Vulkan on Android/Linux.
     */
    static std::shared_ptr<core::rhi::IRHI> createBestAvailableRHI();
};

} // namespace rhi
} // namespace render
} // namespace sdk
