#pragma once

#include <cstdint>
#include <string>

namespace sdk {
namespace core {
namespace rhi {

enum class ShaderStage {
    VERTEX,
    FRAGMENT,
    COMPUTE
};

/**
 * @class RHIShader
 * @brief Abstract representation of a compiled shader module.
 */
class RHIShader {
public:
    virtual ~RHIShader() = default;

    virtual ShaderStage getStage() const = 0;
    virtual std::string getEntryPoint() const = 0;

    // Platform-specific internal handles (e.g. VkShaderModule, id<MTLLibrary>)
    virtual void* getNativeHandle() const = 0;
};

} // namespace rhi
} // namespace core
} // namespace sdk
