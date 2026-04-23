#pragma once

#include "../../../core/rhi/IRHI.h"
#include "../../../base/error/SDKError.h"
#include <memory>
#include <string>

namespace sdk {
namespace render {
namespace rhi {

/**
 * @class VulkanDevice
 * @brief Vulkan implementation of the Render Hardware Interface.
 *        Utilizes the PIMPL idiom to absolutely prevent <vulkan/vulkan.h>
 *        from leaking into the public SDK headers, maintaining strict ABI stability
 *        and fast compile times.
 */
class VulkanDevice : public core::rhi::IRHI {
public:
    VulkanDevice();
    ~VulkanDevice() override;

    // Disallow copy/move
    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    /**
     * @brief Initializes the Vulkan Instance, Physical Device, and Logical Device.
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
