#pragma once

#include <cstdint>

namespace sdk {
namespace core {
namespace rhi {

enum class GraphicsAPI {
    NONE,
    OPENGL,
    VULKAN,
    METAL
};

enum class TextureFormat {
    RGBA8,
    BGRA8,
    YUV420P,
    NV12,
    NV21
};

class RHITexture {
public:
    virtual ~RHITexture() = default;

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual TextureFormat getFormat() const = 0;

    // Identifies which backend created this texture
    virtual GraphicsAPI getBackendAPI() const = 0;

    // Platform-specific internal handles (VkImage, id<MTLTexture>, GLuint)
    virtual void* getNativeHandle() const = 0;
};

} // namespace rhi
} // namespace core
} // namespace sdk
