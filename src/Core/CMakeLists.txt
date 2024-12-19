# src/Game/CMakeLists.txt

# Add the engine source files
set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/DeletionQueue.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${CORE_SOURCES}
    PARENT_SCOPE
)
