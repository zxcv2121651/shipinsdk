#pragma once

#include <cstdint>
#include <memory>
#include "RHITexture.h"

namespace sdk {
namespace core {
namespace rhi {

/**
 * @class RHISwapchain
 * @brief Abstract representation of a presentation swapchain connected to a native window surface.
 */
class RHISwapchain {
public:
    virtual ~RHISwapchain() = default;

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;

    /**
     * @brief Acquires the next drawable texture from the swapchain.
     * @return A texture that can be rendered into, or nullptr if acquisition failed.
     */
    virtual std::shared_ptr<RHITexture> acquireNextTexture() = 0;

    /**
     * @brief Presents the acquired texture to the screen.
     */
    virtual void present() = 0;

    /**
     * @brief Recreates the swapchain (usually required when the window resizes).
     */
    virtual void resize(uint32_t new_width, uint32_t new_height) = 0;
};

} // namespace rhi
} // namespace core
} // namespace sdk
