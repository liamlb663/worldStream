# src/RenderEngine/CMakeLists.txt

# Add the engine source files
set(RENDERENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VkUtils.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/FrameResources/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FrameResources/FrameData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FrameResources/SwapchainManager.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/CommandSubmitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/CommandPool.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/FrameResources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Commands)

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${RENDERENGINE_SOURCES}
    PARENT_SCOPE
)
