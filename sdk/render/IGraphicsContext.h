#pragma once

#include "../core/MediaFrame.h"
#include <memory>
#include <string>

class IGraphicsContext {
public:
    virtual ~IGraphicsContext() = default;

    // Descriptive name (e.g., "ANGLE (Metal Backend)", "ANGLE (Vulkan Backend)")
    virtual std::string getName() const = 0;

    // Lifecycle management
    virtual bool initialize() = 0;
    virtual void destroy() = 0;

    // Platform-specific internal handles (e.g., VkDevice, MTLCommandQueue)
    // Kept opaque here so clients can cast based on getAPI() if absolutely necessary
    virtual void* getNativeHandle() = 0;
};
