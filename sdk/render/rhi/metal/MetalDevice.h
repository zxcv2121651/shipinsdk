#pragma once

#include "../../../core/rhi/IRHI.h"
#include "../../../base/error/SDKError.h"
#include <memory>
#include <string>

namespace sdk {
namespace render {
namespace rhi {

/**
 * @class MetalDevice
 * @brief Metal implementation of the Render Hardware Interface.
 *        Utilizes the PIMPL idiom to hide Objective-C++ <Metal/Metal.h>
 *        from the public C++ headers.
 */
class MetalDevice : public core::rhi::IRHI {
public:
    MetalDevice();
    ~MetalDevice() override;

    // Disallow copy/move
    MetalDevice(const MetalDevice&) = delete;
    MetalDevice& operator=(const MetalDevice&) = delete;

    /**
     * @brief Initializes the MTLDevice and default Command Queue.
     */
    base::SDKError initialize();

    // --- IRHI Implementation ---
    core::rhi::GraphicsAPI getBackendAPI() const override;

    std::shared_ptr<core::rhi::RHITexture> createTexture(uint32_t width, uint32_t height, core::rhi::TextureFormat format) override;
    std::shared_ptr<core::rhi::RHIBuffer> createBuffer(uint32_t size, core::rhi::BufferUsage usage) override;
    std::shared_ptr<core::rhi::RHIShader> createShader(core::rhi::ShaderStage stage, const std::string& source) override;
    std::shared_ptr<core::rhi::RHIPipelineState> createPipelineState() override;
    std::shared_ptr<core::rhi::RHISwapchain> createSwapchain(void* native_window, uint32_t width, uint32_t height) override;
    std::shared_ptr<core::rhi::RHICommandBuffer> createCommandBuffer() override;
    void submit(std::shared_ptr<core::rhi::RHICommandBuffer> commandBuffer) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace rhi
} // namespace render
} // namespace sdk
