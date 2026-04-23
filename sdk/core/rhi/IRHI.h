#pragma once

#include "RHITexture.h"
#include "RHICommandBuffer.h"
#include "RHIBuffer.h"
#include "RHIShader.h"
#include "RHIPipelineState.h"
#include "RHISwapchain.h"
#include <memory>

namespace sdk {
namespace core {
namespace rhi {

class IRHI {
public:
    virtual ~IRHI() = default;

    // Identify which graphics backend this RHI drives
    virtual GraphicsAPI getBackendAPI() const = 0;

    // Resource Creation
    virtual std::shared_ptr<RHITexture> createTexture(uint32_t width, uint32_t height, TextureFormat format) = 0;
    virtual std::shared_ptr<RHIBuffer> createBuffer(uint32_t size, BufferUsage usage) = 0;
    virtual std::shared_ptr<RHIShader> createShader(ShaderStage stage, const std::string& source) = 0;
    virtual std::shared_ptr<RHIPipelineState> createPipelineState() = 0; // Simplified for MVP

    // Swapchain Creation
    virtual std::shared_ptr<RHISwapchain> createSwapchain(void* native_window, uint32_t width, uint32_t height) = 0;

    // Command Buffer Management
    virtual std::shared_ptr<RHICommandBuffer> createCommandBuffer() = 0;

    // Command Submission
    virtual void submit(std::shared_ptr<RHICommandBuffer> commandBuffer) = 0;
};

} // namespace rhi
} // namespace core
} // namespace sdk
