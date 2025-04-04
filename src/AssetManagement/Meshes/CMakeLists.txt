# src/AssetManagement/Meshes/CMakeLists.txt

# Add the engine source files
set(MESH_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlaneGenerator.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${MESH_SOURCES}
    PARENT_SCOPE
)
