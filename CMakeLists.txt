cmake_minimum_required(VERSION 3.10)
project(ShortVideoSDK CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find FFmpeg using PkgConfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil libswresample)

# Define modules
add_library(sdk_base INTERFACE)
add_library(sdk_core INTERFACE)
add_library(sdk_platform INTERFACE)
add_library(sdk_render INTERFACE)
add_library(sdk_codec INTERFACE)

# Target include directories
target_include_directories(sdk_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sdk_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sdk_platform INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sdk_render INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sdk_codec INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Link FFmpeg to codec
target_link_libraries(sdk_codec INTERFACE PkgConfig::FFMPEG)

# Link Apple-specific frameworks
if(APPLE)
    target_link_libraries(sdk_codec INTERFACE "-framework VideoToolbox" "-framework CoreMedia")
    target_link_libraries(sdk_render INTERFACE "-framework Metal" "-framework MetalKit")
else()
    # Find Vulkan (Android/Linux/Windows)
    find_package(Vulkan REQUIRED)
    target_link_libraries(sdk_render INTERFACE Vulkan::Vulkan)
endif()

# Dependencies between modules
target_link_libraries(sdk_core INTERFACE sdk_base)
target_link_libraries(sdk_platform INTERFACE sdk_core)
target_link_libraries(sdk_render INTERFACE sdk_core)
target_link_libraries(sdk_codec INTERFACE sdk_core)
