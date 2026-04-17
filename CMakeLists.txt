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

# Link Apple-specific frameworks for Codec
if(APPLE)
    target_link_libraries(sdk_codec INTERFACE "-framework VideoToolbox" "-framework CoreMedia")
endif()

# ANGLE Integration: Instead of raw Vulkan/Metal, we link ANGLE's EGL and GLESv2
# Note: In a real project, ANGLE static/shared libs would be precompiled and linked here.
# For the MVP skeleton, we mock the find_library for EGL and GLESv2.
find_library(EGL_LIB NAMES EGL libEGL)
find_library(GLESV2_LIB NAMES GLESv2 libGLESv2)

if(EGL_LIB AND GLESV2_LIB)
    target_link_libraries(sdk_render INTERFACE ${EGL_LIB} ${GLESV2_LIB})
else()
    # Mocking linkage for environments without system ANGLE/EGL installed yet
    target_link_libraries(sdk_render INTERFACE "EGL" "GLESv2")
endif()

# Dependencies between modules
target_link_libraries(sdk_core INTERFACE sdk_base)
target_link_libraries(sdk_platform INTERFACE sdk_core)
target_link_libraries(sdk_render INTERFACE sdk_core)
target_link_libraries(sdk_codec INTERFACE sdk_core)
