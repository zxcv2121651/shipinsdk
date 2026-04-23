#pragma once

#include <cstdint>

namespace sdk {
namespace core {
namespace rhi {

/**
 * @class RHIPipelineState
 * @brief Abstract representation of a Graphics Pipeline State Object (PSO).
 *        Modern graphics APIs (Vulkan/Metal) bake shaders, blend states, and vertex layouts
 *        into a single immutable PSO to eliminate driver validation overhead during draw calls.
 */
class RHIPipelineState {
public:
    virtual ~RHIPipelineState() = default;

    // Platform-specific internal handles (e.g. VkPipeline, id<MTLRenderPipelineState>)
    virtual void* getNativeHandle() const = 0;
};

} // namespace rhi
} // namespace core
} // namespace sdk
