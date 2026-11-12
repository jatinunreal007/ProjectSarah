cmake_minimum_required(VERSION 3.20)
project(GpuRayTracer CXX)

set(CMAKE_CXX_STANDARD 20)          # your Utilities.h uses <numbers>
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg install glfw3 glad   (then configure with -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_executable(gpu_renderer GpuRenderer.cpp)
target_include_directories(gpu_renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# the exe loads the shader straight from your source folder, so F5 hot-reload works while you edit it
target_compile_definitions(gpu_renderer PRIVATE SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/raytrace.comp")
target_link_libraries(gpu_renderer PRIVATE glfw glad::glad)
