# src/Render/CMakeLists.txt

# Add the engine source files
set(RENDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Render.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${RENDER_SOURCES}
    PARENT_SCOPE
)
