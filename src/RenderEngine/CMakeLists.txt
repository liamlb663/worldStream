# src/RenderEngine/CMakeLists.txt

# Add the engine source files
set(RENDERENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VkUtils.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${RENDERENGINE_SOURCES}
    PARENT_SCOPE
)
