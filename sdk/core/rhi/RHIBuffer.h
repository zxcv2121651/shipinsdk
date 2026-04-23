#pragma once

#include <cstdint>

namespace sdk {
namespace core {
namespace rhi {

enum class BufferUsage {
    VERTEX,
    INDEX,
    UNIFORM,
    STAGING // Used for CPU-to-GPU memory transfers
};

/**
 * @class RHIBuffer
 * @brief Abstract representation of a GPU memory buffer.
 */
class RHIBuffer {
public:
    virtual ~RHIBuffer() = default;

    virtual uint32_t getSize() const = 0;
    virtual BufferUsage getUsage() const = 0;

    /**
     * @brief Maps GPU memory to CPU address space for writing.
     * @return Pointer to mapped memory, or nullptr if not host-visible.
     */
    virtual void* map() = 0;

    /**
     * @brief Unmaps previously mapped memory.
     */
    virtual void unmap() = 0;

    // Platform-specific internal handles (e.g. VkBuffer, id<MTLBuffer>)
    virtual void* getNativeHandle() const = 0;
};

} // namespace rhi
} // namespace core
} // namespace sdk
