# third_party/CMakeLists.txt

include(FetchContent)

# GLFW (FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

# Add spdlog and use its bundled fmt
set(SPDLOG_FMT_EXTERNAL OFF)  # Use bundled fmt to avoid conflicts
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

# Add vk-bootstrap
add_subdirectory(vk-bootstrap)

# Add VulkanMemoryAllocator
add_subdirectory(VulkanMemoryAllocator)

# Add yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(yaml-cpp)

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
    vk-bootstrap
    Vulkan::Vulkan
    yaml-cpp::yaml-cpp
)

