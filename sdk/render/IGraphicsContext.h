#pragma once

#include "../core/rhi/IRHI.h"
#include <memory>
#include <string>

namespace sdk {
namespace render {

class IGraphicsContext {
public:
    virtual ~IGraphicsContext() = default;

    // Identify which graphics API this context encapsulates
    virtual core::rhi::GraphicsAPI getAPI() const = 0;

    // Retrieve the active Render Hardware Interface driven by this context
    virtual core::rhi::IRHI* getRHI() const = 0;

    // Descriptive name (e.g., "Vulkan Context", "Metal Context")
    virtual std::string getName() const = 0;

    // Lifecycle management
    virtual bool initialize() = 0;
    virtual void destroy() = 0;

    // Platform-specific internal handles (e.g., VkDevice, MTLCommandQueue)
    // Kept opaque here so clients can cast based on getAPI() if absolutely necessary
    virtual void* getNativeHandle() = 0;
};

} // namespace render
} // namespace sdk
