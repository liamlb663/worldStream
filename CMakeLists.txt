# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

project(worldStream LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # For clangd

# Find Vulkan
find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    message(STATUS "Vulkan found: ${Vulkan_LIBRARY}")
else()
    message(FATAL_ERROR "Vulkan not found!")
endif()

add_subdirectory(third_party) # Third-party dependencies
add_subdirectory(src)         # Application source code
#add_subdirectory(shaders)     # Shader compilation pipeline (if applicable)

# Assets Symlink
set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_LINK_NAME "${CMAKE_BINARY_DIR}/assets")

if(NOT EXISTS "${ASSETS_LINK_NAME}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${ASSETS_SOURCE_DIR}" "${ASSETS_LINK_NAME}")
    message(STATUS "Created symbolic link: ${ASSETS_LINK_NAME} -> ${ASSETS_SOURCE_DIR}")
else()
    message(STATUS "Symbolic link already exists: ${ASSETS_LINK_NAME}")
endif()
