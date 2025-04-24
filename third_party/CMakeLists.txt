# third_party/CMakeLists.txt

include(FetchContent)

# GLFW (FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build the GLFW examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build the GLFW tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Don't install GLFW targets")

FetchContent_MakeAvailable(glfw)

# Add spdlog and use its bundled fmt
set(SPDLOG_FMT_EXTERNAL OFF)  # Use bundled fmt to avoid conflicts
set(SPDLOG_BUILD_EXAMPLES OFF)
set(SPDLOG_BUILD_TESTS OFF)
set(SPDLOG_BUILD_BENCH OFF)

add_subdirectory(spdlog)

# ImGui
add_library(ImGui STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_vulkan.cpp
    imgui/backends/imgui_impl_glfw.cpp
)
target_include_directories(ImGui PUBLIC imgui imgui/backends)
target_link_libraries(ImGui PUBLIC glfw Vulkan::Vulkan)

# Add glm (header-only)
add_subdirectory(glm)

# stb (FetchContent)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Add tinygltf
add_subdirectory(tinygltf)

# Add VulkanMemoryAllocator
add_subdirectory(VulkanMemoryAllocator)

# Add yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master
)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Don't build yaml-cpp tests")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Don't build yaml-cpp tools/examples")

FetchContent_MakeAvailable(yaml-cpp)

# Add gslang for shader compilation
find_package(glslang REQUIRED)

# Create an INTERFACE target for third-party libraries
add_library(third_party INTERFACE)

# Include directories
target_include_directories(third_party INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/tinygltf
    ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/include
    ${stb_SOURCE_DIR}
)

# Link libraries
target_link_libraries(third_party INTERFACE
    spdlog
    ImGui
    glfw
    Vulkan::Vulkan
    yaml-cpp::yaml-cpp
    glslang
)

