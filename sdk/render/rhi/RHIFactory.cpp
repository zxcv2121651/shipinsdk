#include "RHIFactory.h"
#include "../../base/error/SDKError.h"
#include <iostream>

#if defined(__APPLE__)
    #include "metal/MetalDevice.h"
#else
    #include "vulkan/VulkanDevice.h"
#endif

#include "opengl/GLDevice.h"

namespace sdk {
namespace render {
namespace rhi {

std::shared_ptr<core::rhi::IRHI> RHIFactory::createBestAvailableRHI() {
#if defined(__APPLE__)
    auto metalRhi = std::make_shared<MetalDevice>();
    if (metalRhi->initialize() == base::SDKError::OK) {
        std::cout << "Successfully initialized Metal RHI backend." << std::endl;
        return metalRhi;
    }
    std::cerr << "Failed to initialize Metal RHI backend." << std::endl;
    return nullptr;

#else
    // Android, Linux, Windows default to Vulkan
    auto vulkanRhi = std::make_shared<VulkanDevice>();
    if (vulkanRhi->initialize() == base::SDKError::OK) {
        std::cout << "Successfully initialized Vulkan RHI backend." << std::endl;
        return vulkanRhi;
    }

    std::cerr << "Vulkan initialization failed. Falling back to OpenGL ES 3.0 backend." << std::endl;

    auto glRhi = std::make_shared<GLDevice>();
    if (glRhi->initialize() == base::SDKError::OK) {
        std::cout << "Successfully initialized OpenGL ES RHI backend." << std::endl;
        return glRhi;
    }

    std::cerr << "Failed to initialize ALL graphics backends. Fatal error." << std::endl;
    return nullptr;
#endif
}

} // namespace rhi
} // namespace render
} // namespace sdk
