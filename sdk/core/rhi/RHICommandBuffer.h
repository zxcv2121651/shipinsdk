#pragma once

namespace sdk {
namespace core {
namespace rhi {

class RHICommandBuffer {
public:
    virtual ~RHICommandBuffer() = default;

    virtual void begin() = 0;
    virtual void end() = 0;

    // In the future, methods like bindPipeline, draw, bindTexture will go here.
};

} // namespace rhi
} // namespace core
} // namespace sdk
