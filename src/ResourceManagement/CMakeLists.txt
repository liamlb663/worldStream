# src/ResourceManagement/CMakeLists.txt

# Add the engine source files
set(RESOURCE_MANAGEMENT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ResourceManager.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/RenderResources/Buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderResources/Image.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${RESOURCE_MANAGEMENT_SOURCES}
    PARENT_SCOPE
)
