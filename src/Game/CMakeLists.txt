# src/Game/CMakeLists.txt

# Add the engine source files
set(GAME_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderGraphSetup.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Input/Input.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Input)

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${GAME_SOURCES}
    PARENT_SCOPE
)
