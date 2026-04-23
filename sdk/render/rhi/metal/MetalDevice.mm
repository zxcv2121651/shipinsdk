#include "MetalDevice.h"

// Metal headers are safely hidden inside this Objective-C++ file
#import <Metal/Metal.h>
#import <QuartzCore/CoreVideo.h>
#include <iostream>

namespace sdk {
namespace render {
namespace rhi {

struct MetalDevice::Impl {
    id<MTLDevice> device = nil;
    id<MTLCommandQueue> commandQueue = nil;
    CVMetalTextureCacheRef textureCache = nullptr;
};

MetalDevice::MetalDevice() : pimpl_(std::make_unique<Impl>()) {}

MetalDevice::~MetalDevice() {
    // Teardown Objective-C and CoreFoundation objects
    if (pimpl_->textureCache) {
        CFRelease(pimpl_->textureCache);
        pimpl_->textureCache = nullptr;
    }

    // ARC (Automatic Reference Counting) handles id<MTLDevice> and id<MTLCommandQueue> cleanup,
    // but we can explicitly nil them out to be safe.
    pimpl_->commandQueue = nil;
    pimpl_->device = nil;
}

base::SDKError MetalDevice::initialize() {
    pimpl_->device = MTLCreateSystemDefaultDevice();
    if (!pimpl_->device) {
        std::cerr << "Metal is not supported on this device!" << std::endl;
        return base::SDKError::ERR_UNKNOWN;
    }

    pimpl_->commandQueue = [pimpl_->device newCommandQueue];
    if (!pimpl_->commandQueue) {
        return base::SDKError::ERR_UNKNOWN;
    }

    // Create the texture cache for zero-copy CVPixelBuffer conversion
    CVReturn err = CVMetalTextureCacheCreate(kCFAllocatorDefault, nullptr, pimpl_->device, nullptr, &pimpl_->textureCache);
    if (err != kCVReturnSuccess) {
        std::cerr << "Failed to create CVMetalTextureCache" << std::endl;
        return base::SDKError::ERR_UNKNOWN;
    }

    return base::SDKError::OK;
}

core::rhi::GraphicsAPI MetalDevice::getBackendAPI() const {
    return core::rhi::GraphicsAPI::METAL;
}

// --- Stub Implementations for MVP Skeleton ---

std::shared_ptr<core::rhi::RHITexture> MetalDevice::createTexture(uint32_t width, uint32_t height, core::rhi::TextureFormat format) {
    return nullptr;
}

std::shared_ptr<core::rhi::RHIBuffer> MetalDevice::createBuffer(uint32_t size, core::rhi::BufferUsage usage) {
    return nullptr;
}

std::shared_ptr<core::rhi::RHIShader> MetalDevice::createShader(core::rhi::ShaderStage stage, const std::string& source) {
    return nullptr;
}

std::shared_ptr<core::rhi::RHIPipelineState> MetalDevice::createPipelineState() {
    return nullptr;
}

std::shared_ptr<core::rhi::RHISwapchain> MetalDevice::createSwapchain(void* native_window, uint32_t width, uint32_t height) {
    return nullptr;
}

std::shared_ptr<core::rhi::RHICommandBuffer> MetalDevice::createCommandBuffer() {
    return nullptr;
}

void MetalDevice::submit(std::shared_ptr<core::rhi::RHICommandBuffer> commandBuffer) {
    // [commandBuffer commit] logic
}

} // namespace rhi
} // namespace render
} // namespace sdk
