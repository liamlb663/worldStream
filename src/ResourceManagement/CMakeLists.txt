# src/ResourceManagement/CMakeLists.txt

# Add the engine source files
set(RESOURCE_MANAGEMENT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MaterialManager.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/RenderResources/Buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderResources/Image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderResources/SparseBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderResources/DescriptorSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderResources/DescriptorPool.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/MaterialManagerUtils/yamlParsers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MaterialManagerUtils/stringParsers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MaterialManagerUtils/ShaderLoading.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/RenderResources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/MaterialManagerUtils)

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${RESOURCE_MANAGEMENT_SOURCES}
    PARENT_SCOPE
)
