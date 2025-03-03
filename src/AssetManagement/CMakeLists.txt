# src/AssetManagement/CMakeLists.txt

# Add the engine source files
set(ASSET_MANAGEMENT_SOURCES
    # ${CMAKE_CURRENT_SOURCE_DIR}/
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Meshes)

set(ALL_SOURCES
    ${ALL_SOURCES}
    ${ASSET_MANAGEMENT_SOURCES}
    PARENT_SCOPE
)
