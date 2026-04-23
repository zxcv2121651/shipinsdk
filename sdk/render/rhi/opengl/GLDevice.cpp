#include "GLDevice.h"

// OpenGL ES and EGL headers are safely hidden inside this .cpp file
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <iostream>

namespace sdk {
namespace render {
namespace rhi {

struct GLDevice::Impl {
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLContext context = EGL_NO_CONTEXT;
    EGLSurface dummySurface = EGL_NO_SURFACE; // Needed to make context current on some OS
};

GLDevice::GLDevice() : pimpl_(std::make_unique<Impl>()) {}

GLDevice::~GLDevice() {
    // Teardown EGL Context
    if (pimpl_->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(pimpl_->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (pimpl_->context != EGL_NO_CONTEXT) {
            eglDestroyContext(pimpl_->display, pimpl_->context);
        }
        if (pimpl_->dummySurface != EGL_NO_SURFACE) {
            eglDestroySurface(pimpl_->display, pimpl_->dummySurface);
        }
        eglTerminate(pimpl_->display);
    }
}

base::SDKError GLDevice::initialize() {
    pimpl_->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (pimpl_->display == EGL_NO_DISPLAY) {
        std::cerr << "GLDevice: Failed to get EGL Display!" << std::endl;
        return base::SDKError::ERR_UNKNOWN;
    }

    EGLint major, minor;
    if (!eglInitialize(pimpl_->display, &major, &minor)) {
        std::cerr << "GLDevice: Failed to initialize EGL!" << std::endl;
        return base::SDKError::ERR_UNKNOWN;
    }

    // Basic Config for OpenGL ES 3.0 Context
    const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

    EGLint numConfigs = 0;
    EGLConfig config;
    if (!eglChooseConfig(pimpl_->display, configAttribs, &config, 1, &numConfigs) || numConfigs == 0) {
        std::cerr << "GLDevice: Failed to choose EGL Config!" << std::endl;
        return base::SDKError::ERR_UNKNOWN;
    }

    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    pimpl_->context = eglCreateContext(pimpl_->display, config, EGL_NO_CONTEXT, contextAttribs);
    if (pimpl_->context == EGL_NO_CONTEXT) {
        std::cerr << "GLDevice: Failed to create EGL Context!" << std::endl;
        return base::SDKError::ERR_UNKNOWN;
    }

    // Create a 1x1 pbuffer surface so we can make the context current for offscreen rendering
    const EGLint pbufferAttribs[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE,
    };
    pimpl_->dummySurface = eglCreatePbufferSurface(pimpl_->display, config, pbufferAttribs);

    return base::SDKError::OK;
}

core::rhi::GraphicsAPI GLDevice::getBackendAPI() const {
    return core::rhi::GraphicsAPI::OPENGL;
}

// --- Stub Implementations for MVP Skeleton ---

std::shared_ptr<core::rhi::RHITexture> GLDevice::createTexture(uint32_t width, uint32_t height, core::rhi::TextureFormat format) {
    // Uses glGenTextures, glBindTexture, glTexImage2D
    return nullptr;
}

std::shared_ptr<core::rhi::RHIBuffer> GLDevice::createBuffer(uint32_t size, core::rhi::BufferUsage usage) {
    // Uses glGenBuffers, glBindBuffer, glBufferData
    return nullptr;
}

std::shared_ptr<core::rhi::RHIShader> GLDevice::createShader(core::rhi::ShaderStage stage, const std::string& source) {
    // Uses glCreateShader, glCompileShader
    return nullptr;
}

std::shared_ptr<core::rhi::RHIPipelineState> GLDevice::createPipelineState() {
    // Uses glCreateProgram, glAttachShader, glLinkProgram
    return nullptr;
}

std::shared_ptr<core::rhi::RHISwapchain> GLDevice::createSwapchain(void* native_window, uint32_t width, uint32_t height) {
    // Uses eglCreateWindowSurface
    return nullptr;
}

std::shared_ptr<core::rhi::RHICommandBuffer> GLDevice::createCommandBuffer() {
    // OpenGL doesn't have true command buffers.
    // In an RHI wrapper, this creates a mock command buffer that records GL calls
    // and replays them immediately upon `submit`.
    return nullptr;
}

void GLDevice::submit(std::shared_ptr<core::rhi::RHICommandBuffer> commandBuffer) {
    // Replay recorded GL calls
}

} // namespace rhi
} // namespace render
} // namespace sdk
