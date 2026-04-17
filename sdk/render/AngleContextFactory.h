#pragma once

#include "IGraphicsContext.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <iostream>
#include <memory>

// Mock definitions for ANGLE extensions if they are not present in system headers
#ifndef EGL_PLATFORM_ANGLE_ANGLE
#define EGL_PLATFORM_ANGLE_ANGLE          0x3202
#define EGL_PLATFORM_ANGLE_TYPE_ANGLE     0x3203
#define EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE 0x3450
#define EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE  0x3489
#define EGL_PLATFORM_ANGLE_TYPE_OPENGLES_ANGLE 0x320D
#endif

// A unified context implementation over ANGLE
class AngleGraphicsContext : public IGraphicsContext {
public:
    AngleGraphicsContext(EGLint anglePlatformType, const std::string& name)
        : platformType_(anglePlatformType), name_(name) {}

    ~AngleGraphicsContext() override {
        destroy();
    }

    std::string getName() const override {
        return name_;
    }

    // Initialize EGL Context backed by the requested ANGLE platform
    bool initialize() override {
        // eglGetPlatformDisplayEXT is typically used to initialize ANGLE
        PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
            reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(
                eglGetProcAddress("eglGetPlatformDisplayEXT"));

        if (!eglGetPlatformDisplayEXT) {
            std::cerr << "Failed to get eglGetPlatformDisplayEXT. ANGLE might not be linked properly." << std::endl;
            // Fallback to standard EGL for native GLES
            display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            return display_ != EGL_NO_DISPLAY;
        }

        const EGLint displayAttributes[] = {
            EGL_PLATFORM_ANGLE_TYPE_ANGLE, platformType_,
            EGL_NONE,
        };

        display_ = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE,
                                            reinterpret_cast<void*>(EGL_DEFAULT_DISPLAY),
                                            displayAttributes);

        if (display_ == EGL_NO_DISPLAY) {
            std::cerr << "Failed to create ANGLE display for " << name_ << std::endl;
            return false;
        }

        // Standard EGL initialization follows...
        // eglInitialize(display_, &major, &minor);
        // eglChooseConfig(...);
        // eglCreateContext(...);

        return true;
    }

    void destroy() override {
        if (display_ != EGL_NO_DISPLAY) {
            eglTerminate(display_);
            display_ = EGL_NO_DISPLAY;
        }
    }

    void* getNativeHandle() override {
        return display_; // Expose EGLDisplay if needed by lower levels
    }

private:
    EGLint platformType_;
    std::string name_;
    EGLDisplay display_ = EGL_NO_DISPLAY;
};

class GraphicsEngineFactory {
public:
    // Tries to create the best available ANGLE context with automatic fallback
    static std::shared_ptr<IGraphicsContext> createBestAvailableContext() {
#ifdef __APPLE__
        // iOS / macOS: Try Metal via ANGLE
        auto metalCtx = std::make_shared<AngleGraphicsContext>(
            EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE, "ANGLE (Metal Backend)");
        if (metalCtx->initialize()) {
            return metalCtx;
        }
#elif defined(__ANDROID__) || defined(__linux__)
        // Android / Linux: Try Vulkan via ANGLE
        auto vulkanCtx = std::make_shared<AngleGraphicsContext>(
            EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE, "ANGLE (Vulkan Backend)");
        if (vulkanCtx->initialize()) {
            return vulkanCtx;
        }
#endif
        // Fallback to Native OpenGL ES (or ANGLE wrapping GLES)
        auto glesCtx = std::make_shared<AngleGraphicsContext>(
            EGL_PLATFORM_ANGLE_TYPE_OPENGLES_ANGLE, "Native / ANGLE (OpenGL ES Backend)");
        glesCtx->initialize(); // Assume this always works as the ultimate fallback
        return glesCtx;
    }
};
