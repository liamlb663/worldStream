# src/RenderEngine/CMakeLists.txt

# Add the engine source files
set(RENDERENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VkUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CommandSubmitter.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/FrameManagement/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FrameManagement/FrameData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FrameManagement/SwapchainManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FrameManagement/FrameManager.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/InternalResources/CommandPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InternalResources/Fence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InternalResources/Semaphore.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/FrameResources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/InternalResources)

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${RENDERENGINE_SOURCES}
    PARENT_SCOPE
)
