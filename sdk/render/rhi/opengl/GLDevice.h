#pragma once

#include "../../../core/rhi/IRHI.h"
#include "../../../base/error/SDKError.h"
#include <memory>
#include <string>

namespace sdk {
namespace render {
namespace rhi {

/**
 * @class GLDevice
 * @brief OpenGL ES 3.0 implementation of the Render Hardware Interface.
 *        Serves as the ultimate fallback for older Android devices or devices
 *        with broken Vulkan drivers. Utilizes PIMPL to isolate <EGL/egl.h>.
 */
class GLDevice : public core::rhi::IRHI {
public:
    GLDevice();
    ~GLDevice() override;

    // Disallow copy/move
    GLDevice(const GLDevice&) = delete;
    GLDevice& operator=(const GLDevice&) = delete;

    /**
     * @brief Initializes the EGL Display, Surface, and Context.
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
