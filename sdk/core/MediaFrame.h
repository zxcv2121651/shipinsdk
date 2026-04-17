#pragma once

#include <cstdint>
#include <vector>

enum class FrameType {
    VIDEO,
    AUDIO
};

struct MediaFrame {
    FrameType type;

    // Base synchronization clock
    int64_t pts; // Presentation Timestamp

    // ANGLE abstracts everything back to OpenGL ES textures
    uint32_t texture_id = 0;
    void* hw_buffer = nullptr; // For platform-specific opaque references (CVPixelBufferRef, HardwareBuffer)
    float transform_matrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    int width = 0;
    int height = 0;

    // Audio specific
    std::vector<uint8_t> audio_data;
    int sample_rate = 0;
    int channels = 0;

    // Optional unified constructor for easier initialization
    MediaFrame() : type(FrameType::VIDEO), pts(0) {}
    MediaFrame(FrameType t, int64_t pts) : type(t), pts(pts) {}

    // Prevent default copying to ensure strictly controlled lifecycle if needed,
    // though the requirement says "Use std::shared_ptr<MediaFrame>进行传递",
    // default copy constructor is kept but shouldn't be used across pipeline nodes.
};
