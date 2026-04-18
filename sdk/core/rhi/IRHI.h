#pragma once

#include "RHITexture.h"
#include "RHICommandBuffer.h"
#include <memory>

class IRHI {
public:
    virtual ~IRHI() = default;

    // Identify which graphics backend this RHI drives
    virtual GraphicsAPI getBackendAPI() const = 0;

    // Resource Creation
    virtual std::shared_ptr<RHITexture> createTexture(uint32_t width, uint32_t height, TextureFormat format) = 0;

    // Command Buffer Management
    virtual std::shared_ptr<RHICommandBuffer> createCommandBuffer() = 0;

    // Command Submission
    virtual void submit(std::shared_ptr<RHICommandBuffer> commandBuffer) = 0;
};
