cmake_minimum_required(VERSION 3.10)
project(ShortVideoSDK CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find PkgConfig for FFmpeg
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil libswresample)

# Define modules
add_library(sdk_base sdk/base/queue/LockFreeQueue.cpp)
add_library(sdk_pipeline sdk/pipeline/node/NodeBase.cpp)
add_library(sdk_core INTERFACE)
add_library(sdk_platform INTERFACE)

if(APPLE)
    add_library(sdk_render sdk/render/rhi/RHIFactory.cpp sdk/render/rhi/metal/MetalDevice.mm)
else()
    add_library(sdk_render sdk/render/rhi/RHIFactory.cpp sdk/render/rhi/vulkan/VulkanDevice.cpp sdk/render/rhi/opengl/GLDevice.cpp)
endif()

add_library(sdk_codec INTERFACE)

# Enable ASan for Memory Leak detection in tests (MEM-10)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

# Target include directories
target_include_directories(sdk_base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sdk_pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sdk_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sdk_platform INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sdk_render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sdk_codec INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Find GTest/GMock for testing
find_package(GTest REQUIRED)

# Test Executable
add_executable(sdk_base_tests test/sdk_base/queue/LockFreeQueueTest.cpp)
target_link_libraries(sdk_base_tests sdk_base GTest::gtest_main)

add_executable(sdk_pipeline_tests test/sdk_pipeline/node/DAGTopologyTest.cpp)
target_link_libraries(sdk_pipeline_tests sdk_pipeline sdk_base GTest::gtest_main)

# Link Apple-specific frameworks
if(APPLE)
    target_link_libraries(sdk_codec INTERFACE "-framework VideoToolbox" "-framework CoreMedia")
    target_link_libraries(sdk_render PUBLIC "-framework Metal" "-framework MetalKit" "-framework QuartzCore")
else()
    # Find Vulkan (Android/Linux/Windows)
    find_package(Vulkan REQUIRED)

    # Mocking EGL and GLESv2 linkage for the OpenGL Fallback backend
    target_link_libraries(sdk_render PUBLIC Vulkan::Vulkan "EGL" "GLESv2")
endif()

# Dependencies between modules
target_link_libraries(sdk_core INTERFACE sdk_base)
target_link_libraries(sdk_pipeline PUBLIC sdk_core sdk_base)
target_link_libraries(sdk_platform INTERFACE sdk_pipeline)
target_link_libraries(sdk_render PUBLIC sdk_pipeline)
target_link_libraries(sdk_codec INTERFACE sdk_pipeline PkgConfig::FFMPEG)
